/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_dwpal.h"

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>
#include <math.h>

#ifdef USE_LIBSAFEC
#define restrict __restrict
#include <libsafec/safe_str_lib.h>
#elif USE_SLIBC
#include <slibc/string.h>
#else
#error "No safe C library defined, define either USE_LIBSAFEC or USE_SLIBC"
#endif

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace dwpal {

#define CSA_EVENT_FILTERING_TIMEOUT_MS 1000

// As defined on "ieee802_11_defs.h":
#define WLAN_FC_STYPE_PROBE_REQ 4
#define WLAN_FC_STYPE_AUTH 11

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

// Temporary storage for parsed ACS report
struct DWPAL_acs_report_get {
    int Ch;
    int BW;
    int DFS;
    int bss;
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static ap_wlan_hal::Event dwpal_to_bwl_event(const std::string &opcode)
{
    if (opcode == "AP-ENABLED") {
        return ap_wlan_hal::Event::AP_Enabled;
    } else if (opcode == "AP-DISABLED") {
        return ap_wlan_hal::Event::AP_Disabled;
    } else if (opcode == "AP-STA-CONNECTED") {
        return ap_wlan_hal::Event::STA_Connected;
    } else if (opcode == "AP-STA-DISCONNECTED") {
        return ap_wlan_hal::Event::STA_Disconnected;
    } else if (opcode == "UNCONNECTED-STA-RSSI") {
        return ap_wlan_hal::Event::STA_Unassoc_RSSI;
    } else if (opcode == "INTERFACE-ENABLED") {
        return ap_wlan_hal::Event::Interface_Enabled;
    } else if (opcode == "INTERFACE-DISABLED") {
        return ap_wlan_hal::Event::Interface_Disabled;
    } else if (opcode == "ACS-STARTED") {
        return ap_wlan_hal::Event::ACS_Started;
    } else if (opcode == "ACS-COMPLETED") {
        return ap_wlan_hal::Event::ACS_Completed;
    } else if (opcode == "ACS-FAILED") {
        return ap_wlan_hal::Event::ACS_Failed;
    } else if (opcode == "AP-CSA-FINISHED") {
        return ap_wlan_hal::Event::CSA_Finished;
    } else if (opcode == "BSS-TM-RESP") {
        return ap_wlan_hal::Event::BSS_TM_Response;
    } else if (opcode == "DFS-CAC-COMPLETED") {
        return ap_wlan_hal::Event::DFS_CAC_Completed;
    } else if (opcode == "DFS-NOP-FINISHED") {
        return ap_wlan_hal::Event::DFS_NOP_Finished;
    } else if (opcode == "LTQ-SOFTBLOCK-DROP") {
        return ap_wlan_hal::Event::STA_Softblock_Drop;
    }

    return ap_wlan_hal::Event::Invalid;
}

static uint8_t dwpal_bw_to_beerocks_bw(const uint8_t chan_width)
{

    // 0 => CHAN_WIDTH_20_NOHT
    // 1 => CHAN_WIDTH_20
    // 2 => CHAN_WIDTH_40
    // 3 => CHAN_WIDTH_80
    // 4 => CHAN_WIDTH_80P80
    // 5 => CHAN_WIDTH_160
    // 6 => CHAN_WIDTH_UNKNOWN
    uint8_t bw = chan_width;

    if (chan_width > 6) {
        LOG(ERROR) << "Invalid bandwidth value: " << int(chan_width);
        return 0;
    }

    // Convert to beerocks value (we have only one value for 20Mhz bandwidth)
    if (chan_width > 0) {
        bw -= 1;
    }

    // Treat 80P80 as 160
    if (chan_width >= 3) {
        bw = (uint8_t)BANDWIDTH_160;
    }

    return bw;
}

static void get_ht_mcs_capabilities(int *HT_MCS, std::string &ht_cap_str,
                                    beerocks::message::sRadioCapabilities &sta_caps)
{
    bool break_upper_loop = false;
    sta_caps              = {};
    sta_caps.ht_bw        = 0xFF;

    if (!ht_cap_str.empty() && (HT_MCS != nullptr)) {
        uint16_t ht_cap = uint16_t(std::stoul(ht_cap_str, nullptr, 16));
        sta_caps.ht_bw  = (ht_cap & HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET) != 0; // 20 == 0 / 40 == 1
        sta_caps.ht_sm_power_save = ((ht_cap & HT_CAP_INFO_SMPS_MASK) >> 2) &
                                    0x03; // 0=static, 1=dynamic, 2=reserved, 3=disabled
        sta_caps.ht_low_bw_short_gi =
            (ht_cap & HT_CAP_INFO_SHORT_GI20MHZ) != 0; // 20MHz long == 0 / short == 1
        sta_caps.ht_high_bw_short_gi =
            (ht_cap & HT_CAP_INFO_SHORT_GI40MHZ) != 0; // 40MHz long == 0 / short == 1

        // parsing HT_MCS {AA, BB, CC, DD, XX, ...} to 0xDDCCBBAA
        uint32_t ht_mcs = 0;

        for (uint8_t i = 0; i < 4; i++) {
            ht_mcs |= HT_MCS[i] << (8 * i);
        }

        uint32_t mask = pow(2, 4 * 8 - 1); // 0x80000000

        for (uint8_t i = 4; i > 0; i--) {     // 4ss
            for (int8_t j = 7; j >= 0; j--) { // 8bits
                if ((ht_mcs & mask) > 0) {
                    sta_caps.ht_ss   = i;
                    sta_caps.ant_num = i;
                    sta_caps.ht_mcs  = j;
                    break_upper_loop = true;
                    break;
                }
                mask /= 2;
            }
            if (break_upper_loop) {
                break;
            }
        }
    } else {
        // Use default value
        sta_caps.ant_num = 1;
    }
}

static void get_vht_mcs_capabilities(int16_t *VHT_MCS, std::string &vht_cap_str,
                                     beerocks::message::sRadioCapabilities &sta_caps)
{
    sta_caps.vht_bw = 0xFF;

    if (!vht_cap_str.empty() && (VHT_MCS != nullptr)) {
        uint32_t vht_cap          = uint16_t(std::stoul(vht_cap_str, nullptr, 16));
        uint8_t supported_bw_bits = (vht_cap >> 2) & 0x03;

        if (supported_bw_bits == 0x03) { // reserved mode
            LOG(ERROR) << "INFORMATION ERROR! STA SENT RESERVED BIT COMBINATION";
        }

        // if supported_bw_bits=0 max bw is 80 Mhz, else max bw is 160 Mhz
        sta_caps.vht_bw               = (BANDWIDTH_80 + (supported_bw_bits > 0 ? 1 : 0));
        sta_caps.vht_low_bw_short_gi  = (vht_cap >> 5) & 0x01; // 80 Mhz
        sta_caps.vht_high_bw_short_gi = (vht_cap >> 6) & 0x01; // 160 Mhz

        uint16_t vht_mcs_rx = 0;
        uint16_t vht_mcs_temp;

        vht_mcs_rx = VHT_MCS[0];

        for (uint8_t i = 4; i > 0; i--) { // 4ss
            vht_mcs_temp = (vht_mcs_rx >> (2 * (i - 1))) & 0x03;
            // 0 indicates support for VHT-MCS 0-7 for n spatial streams
            // 1 indicates support for VHT-MCS 0-8 for n spatial streams
            // 2 indicates support for VHT-MCS 0-9 for n spatial streams
            // 3 indicates that n spatial streams is not supported
            if (vht_mcs_temp != 0x3) { //0x3 == not supported
                sta_caps.vht_ss  = i;
                sta_caps.ant_num = i;
                sta_caps.vht_mcs = vht_mcs_temp + 7;
                break;
            }
        }
    } else {
        // Use default value
        sta_caps.ant_num = 1;
    }

    // update standard
    if (sta_caps.vht_ss) {
        sta_caps.wifi_standard = STANDARD_AC;
    } else if (sta_caps.ht_ss) {
        sta_caps.wifi_standard = STANDARD_N;
    } else {
        sta_caps.wifi_standard = STANDARD_A;
    }
}

static void print_sta_capabilities(beerocks::message::sRadioCapabilities &sta_caps)
{
    LOG(DEBUG) << "sta HT_CAPS:" << std::endl
               << "bw20 short gi = " << (sta_caps.ht_low_bw_short_gi ? "true" : "false")
               << std::endl
               << "bw40 short gi = " << (sta_caps.ht_high_bw_short_gi ? "true" : "false")
               << std::endl
               << "ht_mcs = " << ((int(sta_caps.ht_mcs)) ? std::to_string(sta_caps.ht_mcs) : "n/a")
               << std::endl
               << "ht_ss = " << ((int(sta_caps.ht_ss)) ? std::to_string(sta_caps.ht_ss) : "n/a")
               << std::endl
               << "ht_bw = "
               << ((sta_caps.ht_bw != 0xFF)
                       ? std::to_string(beerocks::utils::convert_bandwidth_to_int(
                             beerocks::eWiFiBandwidth(sta_caps.ht_bw)))
                       : "n/a")
               << std::endl
               << "ht_sm_power_save = " << ([](uint8_t n) {
                      switch (n) {
                      case beerocks::HT_SM_POWER_SAVE_MODE_STATIC:
                          return "static";
                      case beerocks::HT_SM_POWER_SAVE_MODE_DYNAMIC:
                          return "dynamic";
                      case beerocks::HT_SM_POWER_SAVE_MODE_RESERVED:
                          return "reserved(ERROR)";
                      case beerocks::HT_SM_POWER_SAVE_MODE_DISABLED:
                          return "disabled";
                      }
                      return "ERROR";
                  })(sta_caps.ht_sm_power_save);
    LOG(DEBUG) << "sta VHT_CAPS:" << std::endl
               << "bw80 short gi = " << (sta_caps.vht_low_bw_short_gi ? "true" : "false")
               << std::endl
               << "bw160 short gi = " << (sta_caps.vht_high_bw_short_gi ? "true" : "false")
               << std::endl
               << "vht_ss = " << ((int(sta_caps.vht_ss)) ? std::to_string(sta_caps.vht_ss) : "n/a")
               << std::endl
               << "vht_bw = "
               << ((sta_caps.vht_bw != 0xFF)
                       ? std::to_string(beerocks::utils::convert_bandwidth_to_int(
                             beerocks::eWiFiBandwidth(sta_caps.vht_bw)))
                       : "n/a");
    LOG(DEBUG) << "sta DEFAULT_CAPS:" << std::endl
               << "default_mcs = " << int(sta_caps.default_mcs) << std::endl
               << "default_short_gi = " << int(sta_caps.default_short_gi);
    LOG(DEBUG) << "sta OTHER_CAPS:" << std::endl
               << "wifi_standard [enum] = " << int(sta_caps.wifi_standard) << std::endl
               << "btm_supported = " << (sta_caps.btm_supported ? "true" : "false") << std::endl
               << "nr_enabled = " << (sta_caps.nr_enabled ? "true" : "false") << std::endl
               << "cell_capa = " << int(sta_caps.cell_capa) << std::endl
               << "link_meas = " << int(sta_caps.link_meas) << std::endl
               << "beacon_report_passive = " << int(sta_caps.beacon_report_passive) << std::endl
               << "beacon_report_active = " << int(sta_caps.beacon_report_active) << std::endl
               << "beacon_report_table = " << int(sta_caps.beacon_report_table) << std::endl
               << "lci_meas = " << int(sta_caps.lci_meas) << std::endl
               << "fmt_range_report = " << int(sta_caps.fmt_range_report);
}

static void get_mcs_from_supported_rates(int *supported_rates,
                                         beerocks::message::sRadioCapabilities &sta_caps)
{
    uint16_t temp_rate = 0;
    uint16_t max_rate  = 0;

    for (int i = 0; i < 16; i++) {
        temp_rate = (supported_rates[i] & 0x7F) * 5; // rate/2 * 10

        if (temp_rate > max_rate) {
            max_rate = temp_rate;
        }
    }

    LOG(DEBUG) << "get_mcs_from_supported_rates() | max rate:" << std::dec << int(max_rate);

    if (son::wireless_utils::get_mcs_from_rate(max_rate, beerocks::ANT_MODE_1X1_SS1,
                                               beerocks::BANDWIDTH_20, sta_caps.default_mcs,
                                               sta_caps.default_short_gi)) {
        LOG(DEBUG) << "get_mcs_from_supported_rates() | MCS rate match";
    } else {
        LOG(DEBUG) << "get_mcs_from_supported_rates() | no MCS rate match --> using nearest value";
    }
}

static void parse_rrm_capabilities(int *RRM_CAPS, beerocks::message::sRadioCapabilities &sta_caps)
{
    sta_caps.nr_enabled            = ((RRM_CAPS[0] & WLAN_RRM_CAPS_NEIGHBOR_REPORT) != 0);
    sta_caps.link_meas             = ((RRM_CAPS[0] & WLAN_RRM_CAPS_LINK_MEASUREMENT) != 0);
    sta_caps.beacon_report_passive = ((RRM_CAPS[0] & WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE) != 0);
    sta_caps.beacon_report_active  = ((RRM_CAPS[0] & WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE) != 0);
    sta_caps.beacon_report_table   = ((RRM_CAPS[0] & WLAN_RRM_CAPS_BEACON_REPORT_TABLE) != 0);
    sta_caps.lci_meas              = ((RRM_CAPS[1] & WLAN_RRM_CAPS_LCI_MEASUREMENT) != 0);
    sta_caps.fmt_range_report      = ((RRM_CAPS[4] & WLAN_RRM_CAPS_FTM_RANGE_REPORT) != 0);
}

static std::shared_ptr<char> generate_client_assoc_event(const std::string &event, int vap_id,
                                                         bool radio_5G)
{
    // TODO: Change to HAL objects
    auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
    auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());

    if (!msg) {
        LOG(FATAL) << "Memory allocation failed";
        return nullptr;
    }

    // Initialize the message
    memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
    memset((char *)&msg->params.capabilities, 0, sizeof(msg->params.capabilities));

    char client_mac[MAC_ADDR_SIZE] = {0};
    int supported_rates[16]        = {0}; // example: supported_rates=8c 12 98 24 b0 48 60 6c
    int HT_MCS[16]                 = {0};
    int16_t VHT_MCS[1]             = {0};
    char ht_cap[8]                 = {0};
    char ht_mcs[64]                = {0};
    char vht_cap[16]               = {0};
    char vht_mcs[24]               = {0};
    size_t numOfValidArgs[7]       = {0};

    FieldsToParse fieldsToParse[] = {
        {(void *)client_mac, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, sizeof(client_mac)},
        {(void *)supported_rates, &numOfValidArgs[1], DWPAL_INT_HEX_ARRAY_PARAM,
         "supported_rates=", sizeof(supported_rates)},
        {(void *)ht_cap, &numOfValidArgs[2], DWPAL_STR_PARAM, "ht_caps_info=", sizeof(ht_cap)},
        {(void *)ht_mcs, &numOfValidArgs[3], DWPAL_STR_PARAM, "ht_mcs_bitmask=", sizeof(ht_mcs)},
        {(void *)vht_cap, &numOfValidArgs[4], DWPAL_STR_PARAM, "vht_caps_info=", sizeof(vht_cap)},
        {(void *)vht_mcs, &numOfValidArgs[5], DWPAL_STR_PARAM, "rx_vht_mcs_map=", sizeof(vht_mcs)},
        {(void *)&msg->params.capabilities.max_tx_power, &numOfValidArgs[6], DWPAL_CHAR_PARAM,
         "max_txpower=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse((char *)event.c_str(), event.length(), fieldsToParse,
                                     sizeof(client_mac)) == DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return nullptr;
    }

    std::string ht_mcs_str("0x00");
    std::string vht_mcs_str("0x0000");

    // convert string to vector: ht_mcs "ffff0000000000000000" -> HT_MCS {0xFF, 0xFF, 0x00, ...}
    for (uint i = 0; (i < 8) && (i < sizeof(ht_mcs)); i++) {
        ht_mcs_str[2] = ht_mcs[2 * i];
        ht_mcs_str[3] = ht_mcs[2 * i + 1];
        HT_MCS[i]     = beerocks::string_utils::stoi(ht_mcs_str);
    }

    vht_mcs_str[2] = vht_mcs[0];
    vht_mcs_str[3] = vht_mcs[1];
    vht_mcs_str[4] = vht_mcs[2];
    vht_mcs_str[5] = vht_mcs[3];
    VHT_MCS[0]     = beerocks::string_utils::stoi(vht_mcs_str);

    LOG(DEBUG) << "client_mac      : " << client_mac;
    LOG(DEBUG) << "supported_rates : " << std::dec << supported_rates[0]
               << " (first  element only)";
    LOG(DEBUG) << "ht_mcs_bitmask  : 0x" << ht_mcs;
    LOG(DEBUG) << "vht_mcs         : 0x" << vht_mcs;
    LOG(DEBUG) << "ht_caps_info    : " << ht_cap;
    LOG(DEBUG) << "vht_cap         : " << vht_cap;
    LOG(DEBUG) << "max_txpower     : " << std::dec << (int)msg->params.capabilities.max_tx_power;

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i
                       << " ==> Continue with default values";
        }
    }

    msg->params.vap_id = vap_id;
    msg->params.mac    = beerocks::net::network_utils::mac_from_string(client_mac);

    msg->params.capabilities = {};
    std::string ht_cap_str(ht_cap);
    get_ht_mcs_capabilities(HT_MCS, ht_cap_str, msg->params.capabilities);

    if (radio_5G) {
        std::string vht_cap_str(vht_cap);
        get_vht_mcs_capabilities(VHT_MCS, vht_cap_str, msg->params.capabilities);
    }

    get_mcs_from_supported_rates(supported_rates, msg->params.capabilities);

    print_sta_capabilities(msg->params.capabilities);

    // return the buffer
    return msg_buff;
}

///
/// @brief Finds value by key in a string vector representing hostapd config
///        (head or VAP) and returns value found in the supplied string object.
///
static bool hostapd_config_get_value(const std::vector<std::string> &hostapd_config,
                                     const std::string &key, std::string &value)
{
    std::string key_eq(key + "=");
    auto it_str = std::find_if(hostapd_config.begin(), hostapd_config.end(),
                               [&key_eq](std::string str) -> bool {
                                   return (str.compare(0, key_eq.length(), key_eq) == 0);
                               });
    if (it_str == hostapd_config.end()) {
        return false;
    }
    value.assign(*it_str, key_eq.length(), std::string::npos);
    return true;
}

///
/// @brief Finds a value by key in a string vector representing hostapd config
///        (head or VAP) and replaces it with the supplied one. If the value
///        does not exist, it's added. If the value is empty string it's deleted.
///
static void hostapd_config_set_value(std::vector<std::string> &hostapd_config,
                                     const std::string &key, const std::string &value)
{
    std::string key_eq(key + "=");
    auto it_str = std::find_if(hostapd_config.begin(), hostapd_config.end(),
                               [&key_eq](std::string str) -> bool {
                                   return (str.compare(0, key_eq.length(), key_eq) == 0);
                               });
    // Delete the key-value if found
    if (it_str != hostapd_config.end()) {
        hostapd_config.erase(it_str);
    }
    // If the new value is provided add the key back with that new value
    if (value.length() != 0) {
        hostapd_config.push_back(key_eq + value);
    }
    return;
}

static void hostapd_config_set_value(std::vector<std::string> &hostapd_config,
                                     const std::string &key, const int value)
{
    return hostapd_config_set_value(hostapd_config, key, std::to_string(value));
}

///
/// @brief Loads hostapd configuration from specified file
///
static bool
load_hostapd_config_file(const std::string &fname, std::vector<std::string> &hostapd_config_head,
                         std::map<std::string, std::vector<std::string>> &hostapd_config_vaps)
{
    std::ifstream ifs(fname);
    std::string line;

    bool parsing_vaps = false;
    std::string cur_vap;
    while (getline(ifs, line)) {
        // Skip empty lines
        if (beerocks::string_utils::is_empty(line)) {
            continue;
        }
        // Check if the string belongs to a VAP config part and
        // capture which one.
        const std::string bss_eq("bss=");
        if (line.compare(0, bss_eq.length(), bss_eq) == 0) {
            parsing_vaps = true;
            cur_vap.assign(line, bss_eq.length(), std::string::npos);
        }
        // If not a VAP line store it in the header part of the config,
        // otherwise add to the currently being parsed VAP storage.
        if (!parsing_vaps) {
            hostapd_config_head.push_back(line);
        } else {
            std::vector<std::string> &hostapd_config_vap = hostapd_config_vaps[cur_vap];
            hostapd_config_vap.push_back(line);
        }
    }

    // Log something if there was an error during the read
    if (ifs.bad()) {
        LOG(ERROR) << "Unable to read " << fname << ": " << strerror(errno);
    }

    // If we've got to parsing VAPs and no read errors, assume all is good
    return parsing_vaps && !ifs.bad();
}

///
/// @brief Figures out hostapd config file name by the interface name and loads
///        its content using load_hostapd_config_file()
///        Warning: the content of the hostapd_config_head and hostapd_config_vaps
///        is destroyed.
///
static bool
load_hostapd_config(const std::string &radio_iface_name, std::string &fname,
                    std::vector<std::string> &hostapd_config_head,
                    std::map<std::string, std::vector<std::string>> &hostapd_config_vaps)
{
    bool loaded                                = false;
    std::vector<std::string> hostapd_cfg_names = {"/var/run/hostapd-phy0.conf",
                                                  "/var/run/hostapd-phy1.conf"};

    for (const auto &try_fname : hostapd_cfg_names) {
        LOG(DEBUG) << "Trying to load " << try_fname << "...";

        hostapd_config_head.clear();
        hostapd_config_vaps.clear();
        if (!beerocks::os_utils::file_exists(try_fname)) {
            continue;
        }
        if (!load_hostapd_config_file(try_fname, hostapd_config_head, hostapd_config_vaps)) {
            LOG(ERROR) << "Failed to load hostapd cofig file: " << try_fname;
            continue;
        }
        // See if it's the right one
        std::string ifname;
        if (!hostapd_config_get_value(hostapd_config_head, "interface", ifname)) {
            LOG(ERROR) << "No interface value in " << try_fname;
            continue;
        }
        if (ifname != radio_iface_name) {
            LOG(DEBUG) << "File " << try_fname << " interface " << radio_iface_name << " no match";
            continue;
        }
        LOG(DEBUG) << "Loaded " << try_fname << " for interface " << radio_iface_name;

        loaded = true;
        fname.assign(try_fname);
        break;
    }
    if (!loaded) {
        hostapd_config_head.clear();
        hostapd_config_vaps.clear();
    }

    return loaded;
}

///
/// @brief Saves hostapd configuration to specified file
///
static bool
save_hostapd_config_file(const std::string &fname, std::vector<std::string> &hostapd_config_head,
                         std::map<std::string, std::vector<std::string>> &hostapd_config_vaps)
{
    std::ofstream out(fname, std::ofstream::out | std::ofstream::trunc);

    for (const auto &line : hostapd_config_head) {
        out << line << "\n";
    }
    for (auto &it : hostapd_config_vaps) {
        out << "\n"; // add empty line for readability
        const std::vector<std::string> &hostapd_config_vap = it.second;
        for (auto &line : hostapd_config_vap) {
            out << line << "\n";
        }
    }

    // Log something if there was an error during the write
    out.close();
    if (out.fail()) {
        LOG(ERROR) << "Unable to write " << fname << ": " << strerror(errno);
    }

    return !out.fail();
}

static bool
update_vap_credentials_configure_wpa(const std::string &vap_if,
                                     std::vector<std::string> &vap_hostapd_config,
                                     const son::wireless_utils::sBssInfoConf &bss_info_conf)
{
    // Hostapd "wpa" field.
    // This field is a bit field that can be used to enable WPA (IEEE 802.11i/D3.0)
    // and/or WPA2 (full IEEE 802.11i/RSN):
    // bit0 = WPA
    // bit1 = IEEE 802.11i/RSN (WPA2) (dot11RSNAEnabled)
    int wpa = 0;

    // Set of accepted key management algorithms (WPA-PSK, WPA-EAP, or both). The
    // entries are separated with a space. WPA-PSK-SHA256 and WPA-EAP-SHA256 can be
    // added to enable SHA256-based stronger algorithms.
    // WPA-PSK = WPA-Personal / WPA2-Personal
    std::string wpa_key_mgmt(""); // Empty -> delete from hostapd config

    // (dot11RSNAConfigPairwiseCiphersTable)
    // Pairwise cipher for WPA (v1) (default: TKIP)
    //  wpa_pairwise=TKIP CCMP
    // Pairwise cipher for RSN/WPA2 (default: use wpa_pairwise value)
    //  rsn_pairwise=CCMP
    std::string wpa_pairwise(""); // Empty -> delete from hostapd config

    // WPA pre-shared keys for WPA-PSK. This can be either entered as a 256-bit
    // secret in hex format (64 hex digits), wpa_psk, or as an ASCII passphrase
    // (8..63 characters), wpa_passphrase.
    std::string wpa_passphrase("");
    std::string wpa_psk("");

    // ieee80211w: Whether management frame protection (MFP) is enabled
    // 0 = disabled (default)
    // 1 = optional
    // 2 = required
    std::string ieee80211w("");

    // This parameter can be used to disable caching of PMKSA created through EAP
    // authentication. RSN preauthentication may still end up using PMKSA caching if
    // it is enabled (rsn_preauth=1).
    // 0 = PMKSA caching enabled (default)
    // 1 = PMKSA caching disabled
    std::string disable_pmksa_caching("");

    // Opportunistic Key Caching (aka Proactive Key Caching)
    // Allow PMK cache to be shared opportunistically among configured interfaces
    // and BSSes (i.e., all configurations within a single hostapd process).
    // 0 = disabled (default)
    // 1 = enabled
    std::string okc("");

    // This parameter can be used to disable retransmission of EAPOL-Key frames that
    // are used to install keys (EAPOL-Key message 3/4 and group message 1/2). This
    // is similar to setting wpa_group_update_count=1 and
    std::string wpa_disable_eapol_key_retries("");

    // EasyMesh R1 only allows Open and WPA2 PSK auth&encryption methods.
    // Quote: A Multi-AP Controller shall set the Authentication Type attribute
    //        in M2 to indicate WPA2-Personal or Open System Authentication.
    // bss_info_conf.authentication_type is a bitfield, but we are not going
    // to accept any combinations due to the above limitation.
    if (bss_info_conf.authentication_type == WSC::eWscAuth::WSC_AUTH_OPEN) {
        wpa = 0x0;
        if (bss_info_conf.encryption_type != WSC::eWscEncr::WSC_ENCR_NONE) {
            LOG(ERROR) << "Autoconfiguration: " << vap_if << " encryption set on open VAP";
            return false;
        }
        if (bss_info_conf.network_key.length() > 0) {
            LOG(ERROR) << "Autoconfiguration: " << vap_if << " network key set for open VAP";
            return false;
        }
    } else if (bss_info_conf.authentication_type == WSC::eWscAuth::WSC_AUTH_WPA2PSK) {
        wpa = 0x2;
        wpa_key_mgmt.assign("WPA-PSK");
        // Cipher must include AES for WPA2, TKIP is optional
        if ((uint16_t(bss_info_conf.encryption_type) & uint16_t(WSC::eWscEncr::WSC_ENCR_AES)) ==
            0) {
            LOG(ERROR) << "Autoconfiguration:  " << vap_if << " CCMP(AES) is required for WPA2";
            return false;
        }
        if ((uint16_t(bss_info_conf.encryption_type) & uint16_t(WSC::eWscEncr::WSC_ENCR_TKIP)) !=
            0) {
            wpa_pairwise.assign("TKIP CCMP");
        } else {
            wpa_pairwise.assign("CCMP");
        }
        if (bss_info_conf.network_key.length() < 8 || bss_info_conf.network_key.length() > 64) {
            LOG(ERROR) << "Autoconfiguration: " << vap_if << " invalid network key length "
                       << bss_info_conf.network_key.length();
            return false;
        }
        if (bss_info_conf.network_key.length() < 64) {
            wpa_passphrase.assign(bss_info_conf.network_key);
        } else {
            wpa_psk.assign(bss_info_conf.network_key);
        }
        ieee80211w.assign("0");
        disable_pmksa_caching.assign("1");
        okc.assign("0");
        wpa_disable_eapol_key_retries.assign("0");
    } else {
        LOG(ERROR) << "Autoconfiguration: " << vap_if << " invalid authentication type";
        return false;
    }

    hostapd_config_set_value(vap_hostapd_config, "wpa", wpa);
    hostapd_config_set_value(vap_hostapd_config, "okc", okc);
    hostapd_config_set_value(vap_hostapd_config, "wpa_key_mgmt", wpa_key_mgmt);
    hostapd_config_set_value(vap_hostapd_config, "wpa_pairwise", wpa_pairwise);
    hostapd_config_set_value(vap_hostapd_config, "wpa_psk", wpa_psk);
    hostapd_config_set_value(vap_hostapd_config, "ieee80211w", ieee80211w);
    hostapd_config_set_value(vap_hostapd_config, "wpa_passphrase", wpa_passphrase);
    hostapd_config_set_value(vap_hostapd_config, "disable_pmksa_caching", disable_pmksa_caching);
    hostapd_config_set_value(vap_hostapd_config, "wpa_disable_eapol_key_retries",
                             wpa_disable_eapol_key_retries);

    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_dwpal* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_dwpal::ap_wlan_hal_dwpal(std::string iface_name, hal_event_cb_t callback,
                                     hal_conf_t hal_conf)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_dwpal(bwl::HALType::AccessPoint, iface_name, callback, hal_conf)
{
}

ap_wlan_hal_dwpal::~ap_wlan_hal_dwpal() {}

HALState ap_wlan_hal_dwpal::attach(bool block)
{
    auto state = base_wlan_hal_dwpal::attach(block);

    // On Operational send the AP_Attached event to the AP Manager
    if (state == HALState::Operational) {
        event_queue_push(Event::AP_Attached);
    }

    return state;
}

bool ap_wlan_hal_dwpal::enable()
{
    if (!dwpal_send_cmd("ENABLE")) {
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::disable()
{
    if (!dwpal_send_cmd("DISABLE")) {
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::set_start_disabled(bool enable, int vap_id)
{
    if (vap_id != beerocks::IFACE_RADIO_ID) {
        return set("start_disabled", std::to_string(enable), vap_id);
    }

    bool ret = true;

    for (auto &vap : m_radio_info.available_vaps) {
        if (!set("start_disabled", std::to_string(enable), vap.first)) {
            LOG(ERROR) << "Failed setting start_disabled on vap=" << vap.first;
            ret = false;
        }
    }

    return ret;
}

bool ap_wlan_hal_dwpal::set_channel(int chan, int bw, int center_channel)
{
    if (chan < 0) {
        LOG(ERROR) << "Invalid input: channel(" << chan << ") < 0";
        return false;
    }

    std::string chan_string = (chan == 0) ? "acs_smart" : std::to_string(chan);

    LOG(DEBUG) << "Set channel to " << chan_string;

    if (!set("channel", chan_string)) {
        LOG(ERROR) << "Failed setting channel";
        return false;
    }

    if (bw > 0) {
        int wifi_bw = 0;
        // based on hostapd.conf @ https://w1.fi/cgit/hostap/plain/hostapd/hostapd.conf
        // # 0 = 20 or 40 MHz operating Channel width
        // # 1 = 80 MHz channel width
        // # 2 = 160 MHz channel width
        // # 3 = 80+80 MHz channel width
        // #vht_oper_chwidth=1

        if (bw == 20 || bw == 40) {
            wifi_bw = 0;
        } else if (bw == 80) {
            wifi_bw = 1;
        } else if (bw == 160) {
            wifi_bw = 2;
        } else {
            LOG(ERROR) << "Unknown BW " << bw;
            return false;
        }

        if (!set("vht_oper_chwidth", std::to_string(wifi_bw))) {
            LOG(ERROR) << "Failed setting vht_oper_chwidth";
            return false;
        }
    }

    if (center_channel > 0) {
        if (!set("vht_oper_centr_freq_seg0_idx=", std::to_string(center_channel))) {
            LOG(ERROR) << "Failed setting vht_oper_centr_freq_seg0_idx";
            return false;
        }
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_allow(const std::string &mac, const std::string &bssid)
{
    // Check if the requested BSSID is part of this radio
    for (const auto &vap : m_radio_info.available_vaps) {
        if (vap.second.mac == bssid) {
            // Send the command
            std::string cmd = "STA_ALLOW " + mac;
            if (!dwpal_send_cmd(cmd, vap.first)) {
                LOG(ERROR) << "sta_allow() failed!";
                return false;
            }

            return true;
        }
    }

    // If the requested BSSID is not part of this radio, return silently
    return true;
}

bool ap_wlan_hal_dwpal::sta_deny(const std::string &mac, const std::string &bssid)
{
    // Check if the requested BSSID is part of this radio
    for (const auto &vap : m_radio_info.available_vaps) {
        if (vap.second.mac == bssid) {
            // Send the command
            std::string cmd = "DENY_MAC " + mac + " reject_sta=33";
            if (!dwpal_send_cmd(cmd, vap.first)) {
                LOG(ERROR) << "sta_allow() failed!";
                return false;
            }

            return true;
        }
    }

    // If the requested BSSID is not part of this radio, return silently
    return true;
}

bool ap_wlan_hal_dwpal::sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    // Build command string
    const std::string ifname =
        beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    std::string cmd = "DISASSOCIATE " + ifname + " " + mac;

    if (reason) {
        cmd += " reason=" + std::to_string(reason);
    }

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_disassoc() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_deauth(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    // Build command string
    const std::string ifname =
        beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    std::string cmd = "DEAUTHENTICATE " + ifname + " " + mac;

    if (reason) {
        cmd += " reason=" + std::to_string(reason);
    }

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_deauth() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_bss_steer(const std::string &mac, const std::string &bssid, int chan,
                                      int disassoc_timer, int valid_int)
{
    // Build command string
    std::string cmd =
        // Set the STA MAC address
        "BSS_TM_REQ " +
        mac

        // Transition management parameters
        + " dialog_token=" + "0" + " Mode=" + "0" + " pref=" + "1" + " abridged=" + "1" +
        " neighbor=" + bssid + ",0,0," + std::to_string(chan) + ",0,255";

    // Divide disassoc_timer by 100, because the hostapd expects it to be in beacon interval
    // which is 100ms.
    if (disassoc_timer) {
        cmd += std::string() + " disassoc_imminent=" + "1" +
               " disassoc_timer=" + std::to_string(disassoc_timer / 100);
    }

    if (valid_int) {
        cmd += " valid_int=" + std::to_string(valid_int);
    }

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_bss_steer() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::update_vap_credentials(
    std::list<son::wireless_utils::sBssInfoConf> &bss_info_conf_list)
{
    // The bss_info_conf_list contains list of all VAPs and STAs that have to be created
    // on the radio. If the list is empty, there should be no VAPs left.
    // At the moment we are taking a shortcut to make this work for certification setup.
    // We are updating the hostapd .conf files with the info from the bss_info_conf_list
    // and sending RELOAD command to hostapd making it update the VAPs.
    // With that approach there are following limitations:
    // - all the VAPs are expected to be always present in the config, but have
    //   start_disabled=1 option if not active
    // - any configuration change causing the hostapd config files to be re-genrated
    //   and the hostapd processes restarted will override the purplMesh configuration

    // The hostapd config is stored here. The items order in the header and VAPs sections
    // is mostly preserved (with the exception of the itemes prplMesh modifies). This helps
    // to keep the configuration the same and easily see what prplMesh is changing.
    std::string fname;
    std::vector<std::string> hostapd_config_head;
    std::map<std::string, std::vector<std::string>> hostapd_config_vaps;

    // Load hostapd config for the radio
    if (!load_hostapd_config(m_radio_info.iface_name, fname, hostapd_config_head,
                             hostapd_config_vaps)) {
        LOG(ERROR) << "Autoconfiguration: no hostapd config to apply configuration!";
        return false;
    }

    // If a Multi-AP Agent receives an AP-Autoconfiguration WSC message containing one or
    // more M2, it shall validate each M2 (based on its 1905 AL MAC address) and configure
    // a BSS on the corresponding radio for each of the M2. If the Multi-AP Agent is currently
    // operating a BSS with operating parameters that do not completely match any of the M2 in
    // the received AP-Autoconfiguration WSC message, it shall tear down that BSS.

    // Find first VAP entry
    auto hostapd_vap_iterator = hostapd_config_vaps.begin();

    // Go through the bss_info_conf_list and change the hostapd config accordingly
    for (const auto &bss_info_conf : bss_info_conf_list) {
        auto auth_type =
            son::wireless_utils::wsc_to_bwl_authentication(bss_info_conf.authentication_type);
        if (auth_type == "INVALID") {
            LOG(ERROR) << "Autoconfiguration: invalid auth_type "
                       << int(bss_info_conf.authentication_type);
            return false;
        }
        auto enc_type = son::wireless_utils::wsc_to_bwl_encryption(bss_info_conf.encryption_type);
        if (enc_type == "INVALID") {
            LOG(ERROR) << "Autoconfiguration: invalid enc_type "
                       << int(bss_info_conf.encryption_type);
            return false;
        }
        auto bss_type = son::wireless_utils::wsc_to_bwl_bss_type(bss_info_conf.bss_type);
        if (bss_type == beerocks::BSS_TYPE_INVALID) {
            LOG(ERROR) << "Autoconfiguration: invalid bss_type";
            return false;
        }
        LOG(DEBUG) << "Autoconfiguration for ssid: " << bss_info_conf.ssid
                   << " auth_type: " << auth_type << " encr_type: " << enc_type
                   << " network_key: " << bss_info_conf.network_key << " bss_type: " << bss_type;

        // We cannot configure STAs here
        if ((bss_info_conf.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA) != 0) {
            LOG(ERROR) << "Autoconfiguration: STA cannot be configured here";
            return false;
        }

        // We only can use AP entries, skip STAs if ended up on one
        std::string entry_mode;
        while (hostapd_vap_iterator != hostapd_config_vaps.end() &&
               hostapd_config_get_value(hostapd_vap_iterator->second, "mode", entry_mode) &&
               entry_mode != "ap") {
            LOG(DEBUG) << "Autoconfiguration: skipping STA entry";
            ++hostapd_vap_iterator;
        }

        // Make sure we still have VAPs available to configure
        if (hostapd_vap_iterator == hostapd_config_vaps.end()) {
            LOG(ERROR) << "Autoconfiguration: not enough VAPs";
            return false;
        }

        // Update VAP settings in hostapd config
        const std::string &vap_if                    = hostapd_vap_iterator->first;
        std::vector<std::string> &vap_hostapd_config = hostapd_vap_iterator->second;

        // SSID
        hostapd_config_set_value(vap_hostapd_config, "ssid", bss_info_conf.ssid);

        // Everything related to switching between open and WPA2
        if (!update_vap_credentials_configure_wpa(vap_if, vap_hostapd_config, bss_info_conf)) {
            // The function prints the error messages itself
            return false;
        }

        // BSS type (backhaul, fronthaul or both)
        // From hostap 2.8
        // Enable Multi-AP functionality
        // 0 = disabled (default)
        // 1 = AP support backhaul BSS
        // 2 = AP support fronthaul BSS
        // 3 = AP supports both backhaul BSS and fronthaul BSS
        int multi_ap = 0;
        if ((bss_info_conf.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS) != 0) {
            multi_ap |= 0x01;
        }
        if ((bss_info_conf.bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS) != 0) {
            multi_ap |= 0x02;
        }
        hostapd_config_set_value(vap_hostapd_config, "multi_ap", multi_ap);

        // Finally enable the VAP
        hostapd_config_set_value(vap_hostapd_config, "start_disabled", "0");

        // Successfully updated the VAP, move on to the the next
        ++hostapd_vap_iterator;
    }

    // Disable the remaining VAPs
    for (; hostapd_vap_iterator != hostapd_config_vaps.end(); ++hostapd_vap_iterator) {
        std::vector<std::string> &vap_hostapd_config = hostapd_vap_iterator->second;
        // Skip STAs
        std::string entry_mode;
        if (hostapd_config_get_value(vap_hostapd_config, "mode", entry_mode) &&
            entry_mode != "ap") {
            continue;
        }
        // SSID
        hostapd_config_set_value(vap_hostapd_config, "ssid", std::string());
        // Disable the VAP
        hostapd_config_set_value(vap_hostapd_config, "start_disabled", "1");
    }

    // If we are still here, then autoconfiguration was successful,
    // update the hostapd configuration files and send RELOAD command.
    if (!save_hostapd_config_file(fname, hostapd_config_head, hostapd_config_vaps)) {
        LOG(ERROR) << "Autoconfiguration: cannot save hostapd config!";
        return false;
    }

    // hostapd help says:
    // RECONF [BSS name] = reconfigure interface (add/remove BSS's while other BSS are unaffected)
    // if BSS name is given, that BSS will be reloaded (main BSS isn't supported)
    // In reality (as of now, Jan 2020) only RECONF with explicitly specified interface name
    // or sending SIGHUP work for making hostapd to reload the configuration.
    size_t vap_ok_count    = 0;
    size_t vap_total_count = 0;
    for (const auto &it : hostapd_config_vaps) {
        // Skip STAs
        std::string entry_mode;
        if (hostapd_config_get_value(it.second, "mode", entry_mode) && entry_mode != "ap") {
            continue;
        }
        // Count the the total of available for reconfiguration VAPs
        ++vap_total_count;
        // Send the command
        std::string cmd("RECONF " + it.first);
        if (!dwpal_send_cmd(cmd)) {
            LOG(ERROR) << "Autoconfiguration: \"" << cmd << "\" command to hostapd has failed!";
            // Keep going and try to complete what we can
            continue;
        }
        ++vap_ok_count;
    }

    LOG(INFO) << "Autoconfiguration: completed successfully for " << vap_ok_count << " out of "
              << vap_total_count << " available hostapd VAP sections";
    return (vap_ok_count == vap_total_count);
}

bool ap_wlan_hal_dwpal::sta_unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                     int vht_center_frequency, int delay,
                                                     int window_size)
{
    // Convert values to strings
    std::string chanBandwidth     = std::to_string(bw);
    std::string centerFreq        = std::to_string(son::wireless_utils::channel_to_freq(chan));
    std::string waveVhtCenterFreq = std::to_string(vht_center_frequency);

    // Build command string
    std::string cmd = "UNCONNECTED_STA_RSSI " + mac + " " + centerFreq +
                      " center_freq1=" + waveVhtCenterFreq + " bandwidth=" + chanBandwidth;

    // Delay the first measurement...
    UTILS_SLEEP_MSEC(delay);

    m_unassoc_measure_start       = std::chrono::steady_clock::now();
    m_unassoc_measure_delay       = delay;
    m_unassoc_measure_window_size = window_size;

    // Trigger a measurement
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_unassoc_rssi_measurement() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_softblock_add(const std::string &vap_name,
                                          const std::string &client_mac, uint8_t reject_error_code,
                                          uint8_t probe_snr_threshold_hi,
                                          uint8_t probe_snr_threshold_lo,
                                          uint8_t authetication_snr_threshold_hi,
                                          uint8_t authetication_snr_threshold_lo)
{
    // Build command string
    std::string cmd = "STA_SOFTBLOCK " + vap_name + " " + client_mac + " remove=0" +
                      " reject_sta=" + std::to_string(reject_error_code) +
                      " snrProbeHWM=" + std::to_string(probe_snr_threshold_hi) +
                      " snrProbeLWM=" + std::to_string(probe_snr_threshold_lo) +
                      " snrAuthHWM=" + std::to_string(authetication_snr_threshold_hi) +
                      " snrAuthLWM=" + std::to_string(authetication_snr_threshold_lo);

    // Trigger a measurement
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_softblock_add() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_softblock_remove(const std::string &vap_name,
                                             const std::string &client_mac)
{
    // Build command string
    std::string cmd = "STA_SOFTBLOCK " + vap_name + " " + client_mac + " remove=1";

    // Trigger a measurement
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_softblock_remove() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::switch_channel(int chan, int bw, int vht_center_frequency)
{
    std::string cmd = "CHAN_SWITCH 5 ";
    // ACS
    if (chan == 0) {
        m_drop_csa = true;
        cmd += "0"; // CenterFrequency
                    // Specific Channel
    } else {
        m_drop_csa = false;

        int freq                              = son::wireless_utils::channel_to_freq(chan);
        std::string freq_str                  = std::to_string(freq);
        std::string wave_vht_center_frequency = std::to_string(vht_center_frequency);

        // Center Freq
        cmd += freq_str; // CenterFrequency

        // Extension Channel
        if (bw != beerocks::BANDWIDTH_20) {
            if (freq < vht_center_frequency) {
                cmd += " sec_channel_offset=1";
            } else {
                cmd += " sec_channel_offset=-1";
            }
        }

        // Channel bandwidth
        if (bw == beerocks::BANDWIDTH_80) {
            cmd += " center_freq1=" + wave_vht_center_frequency;
        }

        cmd += " bandwidth=" + std::to_string(beerocks::utils::convert_bandwidth_to_int(
                                   static_cast<beerocks::eWiFiBandwidth>(bw)));

        // Supported Standard n/ac
        if (bw == beerocks::BANDWIDTH_20 || bw == beerocks::BANDWIDTH_40) {
            cmd += " ht"; //n
        } else if (bw == beerocks::BANDWIDTH_80) {
            cmd += " vht"; // ac
        }
    }

    // Send command
    LOG(DEBUG) << "switch channel command: " << cmd;
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "switch_channel() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::set_antenna_mode(AntMode mode)
{
    std::string cmd = "iwpriv " + get_radio_info().iface_name + " sCoCPower 0 ";

    switch (mode) {
    case AntMode::ANT_2X2: {
        cmd += "2 2";
    } break;
    case AntMode::ANT_4X4: {
        cmd += "4 4";
    } break;
    default: {
        LOG(ERROR) << "Invalid antenna mode: " << int(mode);
        return false;
    }
    }

    LOG(DEBUG) << "Send cmd: " << cmd;
    // Execute the command
    beerocks::os_utils::system_call(cmd, 2);

    return true;
}

bool ap_wlan_hal_dwpal::wds_set_mode(WDSMode mode)
{
    std::string cmd =
        "iwpriv " + get_radio_info().iface_name + " sFourAddrMode " + std::to_string(int(mode));
    LOG(DEBUG) << "Send cmd: " << cmd;
    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_dwpal::wds_add_sta(const std::string &mac)
{
    std::string cmd = "iwpriv " + get_radio_info().iface_name + " sAddFourAddrSta " + mac;
    LOG(DEBUG) << "Send cmd: " << cmd;
    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_dwpal::wds_del_sta(const std::string &mac)
{
    std::string cmd = "iwpriv " + get_radio_info().iface_name + " sDelFourAddrSta " + mac;
    LOG(DEBUG) << "Send cmd: " << cmd;
    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_dwpal::wds_clear_list()
{
    // TODO: Check if works... Line extracted from beerocks_utils.sh script
    std::string cmd = "iwpriv " + get_radio_info().iface_name +
                      " gFourAddrStas | while read -r line; do iwpriv " +
                      get_radio_info().iface_name + " sDelFourAddrSta \"$line\"; done";
    LOG(DEBUG) << "Send cmd: " << cmd;
    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_dwpal::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    // Channel number of the new channel or ‘0’ to trigger low level channel selection algorithm.
    // '0' triggers the same behavior as if the failsafe channel is NOT set.
    std::string cmd = "SET_FAILSAFE_CHAN ";

    // Build command string
    if (chan) {
        std::string bw_str   = std::to_string(bw);
        std::string chan_str = std::to_string(son::wireless_utils::channel_to_freq(chan));
        std::string freq_str = std::to_string(vht_center_frequency);
        LOG(DEBUG) << "chan_str = " << chan_str << " bw_str = " << bw_str
                   << " vht_freq_str = " << freq_str;

        cmd += chan_str + " center_freq1=" + freq_str + " bandwidth=" + bw_str;
    } else {
        cmd += "0";
    }

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "failsafe_channel_set() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::failsafe_channel_get(int &chan, int &bw)
{
    char *reply = nullptr;
    // Build command string
    std::string cmd = "GET_FAILSAFE_CHAN";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "failsafe_channel_get() failed!";
        return false;
    }

    size_t numOfValidArgs[2] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    char freq[8]{0};
    FieldsToParse fieldsToParse[] = {
        {(void *)freq, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, sizeof(freq)},
        {(void *)&bw, &numOfValidArgs[1], DWPAL_INT_PARAM, "bandwidth=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(freq)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    LOG(DEBUG) << "GET_FAILSAFE_CHAN reply= " << reply;
    LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " freq= " << freq;
    LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " bandwidth= " << bw;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    // Custom reply parsing
    if (!strncmp(freq, "UNSPECIFIED", 11)) {
        chan = -1;
        bw   = -1;
    } else if (!strncmp(freq, "ACS", 3)) {
        chan = bw = 0;
    } else {
        chan = son::wireless_utils::freq_to_channel(beerocks::string_utils::stoi(freq));
    }

    return true;
}

bool ap_wlan_hal_dwpal::restricted_channels_set(char *channel_list)
{
    // For example, the channel_list_str: “1 6 11 12 13”
    // *** WARNING: It is possible to set restricted channel only from the supported channels list!
    // *** setting channel that is not in the list, will cause this function to fail!
    std::stringstream channel_list_str;
    for (int i = 0; i < beerocks::message::RESTRICTED_CHANNEL_LENGTH; i++) {
        // Break after the last channel
        if (channel_list[i] == 0)
            break;

        // Convert the byte-long channels into unsigned integers
        if (i > 0)
            channel_list_str << " ";
        channel_list_str << int(uint8_t(channel_list[i]));
    }

    auto temp_channel_list = channel_list_str.rdbuf()->str();
    LOG(DEBUG) << "temp_channel_list = " << temp_channel_list;

    std::string cmd = "RESTRICTED_CHANNELS " + temp_channel_list;

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "restricted_channels_set() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::restricted_channels_get(char *channel_list)
{
    char *reply = nullptr;

    std::string cmd = "GET_RESTRICTED_CHANNELS";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "restricted_channels_get() failed!";
        return false;
    }

    /* TEMP: Traces... */
    LOG(DEBUG) << "GET_RESTRICTED_CHANNELS reply= " << reply;
    /* End of TEMP: Traces... */

    memset(channel_list, 0, beerocks::message::RESTRICTED_CHANNEL_LENGTH);

    // Convert the string to an array of bytes (int values)
    std::stringstream channel_list_stream(reply);
    int idx = 0;
    while (true) {
        int n;
        channel_list_stream >> n;
        channel_list[idx] = n;

        // Break on end-of-stream
        if (!channel_list_stream)
            break;

        idx++;
    }

    return true;
}

bool ap_wlan_hal_dwpal::read_acs_report()
{
    LOG(TRACE) << __func__ << " for interface: " << get_radio_info().iface_name;

    char *reply = nullptr;

    std::string cmd = "GET_ACS_REPORT";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "read_acs_report() failed!";
        return false;
    }

    size_t replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    /* TEMP: Traces... */
    LOG(DEBUG) << "GET_ACS_REPORT replylen=" << (int)replyLen;
    LOG(DEBUG) << "GET_ACS_REPORT reply=\n" << reply;

    // Initialize default values
    m_radio_info.is_5ghz = false;

    // Resize the supported channels vector
    if (MAX_SUPPORTED_20M_CHANNELS >= m_radio_info.supported_channels.size()) {
        LOG(DEBUG) << "Increasing supported channels vector to: " << MAX_SUPPORTED_20M_CHANNELS;
        m_radio_info.supported_channels.resize(MAX_SUPPORTED_20M_CHANNELS);
    }

    // Clear the supported channels vector
    for (auto &chan : m_radio_info.supported_channels) {
        memset(&chan, 0, sizeof(chan));
    }

    size_t numOfValidArgs[4] = {0};
    std::vector<DWPAL_acs_report_get> acs_report(MAX_SUPPORTED_CHANNELS);

    FieldsToParse fieldsToParse[] = {
        {(void *)&acs_report[0].BW, &numOfValidArgs[0], DWPAL_INT_PARAM, "BW=", 0},
        {(void *)&acs_report[0].Ch, &numOfValidArgs[1], DWPAL_INT_PARAM, "Ch=", 0},
        {(void *)&acs_report[0].DFS, &numOfValidArgs[2], DWPAL_INT_PARAM, "DFS=", 0},
        {(void *)&acs_report[0].bss, &numOfValidArgs[3], DWPAL_INT_PARAM, "bss=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse,
                                     sizeof(DWPAL_acs_report_get) * MAX_SUPPORTED_CHANNELS) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    for (uint16_t i = 0; i < sizeof(numOfValidArgs) / sizeof(size_t); i++) {
        if ((numOfValidArgs[i] == 0) || (numOfValidArgs[i] != numOfValidArgs[0])) {
            LOG(ERROR) << "Failed reading parameter " << (int)i << ": " << (int)numOfValidArgs[i];
            LOG(ERROR) << "Should be the same as numOfValidArgs[0]= " << (int)numOfValidArgs[0]
                       << " ==> Abort";
            return false;
        }
    }

    uint8_t channel_idx = 0;
    LOG(DEBUG) << "Parsed ACS report:";
    for (uint16_t i = 0; (i < numOfValidArgs[0]) && (i < MAX_SUPPORTED_CHANNELS); i++) {
        LOG(DEBUG) << "Ch=" << (int)acs_report[i].Ch << " BW=" << (int)acs_report[i].BW
                   << " DFS=" << acs_report[i].DFS << " bss=" << acs_report[i].bss;

        if (acs_report[i].BW == 20) {
            m_radio_info.supported_channels[channel_idx].bandwidth = acs_report[i].BW;
            m_radio_info.supported_channels[channel_idx].channel   = acs_report[i].Ch;
            // Check if channel is 5GHz
            if (son::wireless_utils::which_freq(
                    m_radio_info.supported_channels[channel_idx].channel) ==
                beerocks::eFreqType::FREQ_5G) {
                m_radio_info.is_5ghz = true;
            }
            m_radio_info.supported_channels[channel_idx].bss_overlap = acs_report[i].bss;
            m_radio_info.supported_channels[channel_idx].is_dfs      = acs_report[i].DFS;

            channel_idx++;
            if (channel_idx == MAX_SUPPORTED_20M_CHANNELS) {
                LOG(DEBUG) << "Max supported channels reached ==> break!";
                break;
            }
        }
    }

    return true;
}

bool ap_wlan_hal_dwpal::read_supported_channels()
{
    auto ifname = get_radio_info().iface_name;
    LOG(TRACE) << "for interface: " << ifname;

    bwl::nl80211_client::radio_info radio_info;

    if (!m_nl80211_client->get_radio_info(ifname, radio_info)) {
        LOG(TRACE) << "Failed to get channels info from nl80211";
        return false;
    }
    std::vector<bwl::WiFiChannel> supported_channels;
    for (auto const &band : radio_info.bands) {
        for (auto const &pair : band.supported_channels) {
            auto &channel_info = pair.second;
            for (auto bw : channel_info.supported_bandwidths) {
                bwl::WiFiChannel channel;
                channel.channel   = channel_info.number;
                channel.bandwidth = beerocks::utils::convert_bandwidth_to_int(bw);
                channel.tx_pow    = channel_info.tx_power;
                channel.is_dfs    = channel_info.is_dfs;
                supported_channels.push_back(channel);
            }
        }
    }

    // Clear the supported channels vector
    m_radio_info.supported_channels.clear();
    // Resize the supported channels vector
    m_radio_info.supported_channels.insert(m_radio_info.supported_channels.begin(),
                                           supported_channels.begin(), supported_channels.end());
    return true;
}

bool ap_wlan_hal_dwpal::set_tx_power_limit(int tx_pow_limit)
{
    return m_nl80211_client->set_tx_power_limit(m_radio_info.iface_name, tx_pow_limit);
}

bool ap_wlan_hal_dwpal::set_vap_enable(const std::string &iface_name, const bool enable)
{
    LOG(DEBUG) << "set_vap_enable(): missing function implementation";
    return true;
}

bool ap_wlan_hal_dwpal::get_vap_enable(const std::string &iface_name, bool &enable)
{
    LOG(DEBUG) << "get_vap_enable(): missing function implementation";
    return true;
}

bool ap_wlan_hal_dwpal::generate_connected_clients_events()
{
    bool queried_first = false;
    std::string cmd;
    std::string client_mac;
    bool ret = true;

    for (const auto &vap_element : m_radio_info.available_vaps) {
        char *reply;
        size_t replyLen;

        const int &vap_id = vap_element.first;
        auto vap_iface_name =
            beerocks::utils::get_iface_string_from_iface_vap_ids(get_iface_name(), vap_id);
        LOG(TRACE) << __func__ << " for vap interface: " << vap_iface_name;

        do {
            if (queried_first) {
                cmd = "STA-NEXT " + vap_iface_name + " " + client_mac;
            } else {
                cmd           = "STA-FIRST " + vap_iface_name;
                queried_first = true;
            }

            reply = nullptr;

            // Send command
            if (!dwpal_send_cmd(cmd, &reply)) {
                LOG(ERROR) << __func__ << ": cmd='" << cmd << "' failed!";
                ret = false;
                break;
            }

            replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);

            if (replyLen == 0) {
                LOG(DEBUG) << "cmd:" << cmd << " | reply:EMPTY";
                break;
            } else {
                LOG(DEBUG) << "cmd:" << cmd << " | replylen:" << (int)replyLen
                           << " | reply:" << reply;
            }

            auto msg_buff = generate_client_assoc_event(reply, vap_id, get_radio_info().is_5ghz);

            if (!msg_buff)
                break;

            // update client mac
            auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(
                msg_buff.get());
            client_mac = beerocks::net::network_utils::mac_to_string(msg->params.mac);

            event_queue_push(Event::STA_Connected, msg_buff); // send message to the AP manager

        } while (replyLen > 0);

        if (!ret)
            return ret; // return from lambda function

        queried_first = false;
    }

    return ret;
}

std::string ap_wlan_hal_dwpal::get_radio_driver_version() { return std::string("NA"); }

static bool is_acs_completed_scan(char *buffer, int bufLen)
{
    size_t numOfValidArgs[3]      = {0};
    char scan[32]                 = {0};
    FieldsToParse fieldsToParse[] = {
        {NULL /* opCode */, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
        {NULL /* iface */, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
        {scan /* sub operation */, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(scan)},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(scan)) == DWPAL_FAILURE)
        return false;

    return !strncmp(scan, "SCAN", 4);
}

bool ap_wlan_hal_dwpal::process_dwpal_event(char *buffer, int bufLen, const std::string &opcode)
{
    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = dwpal_to_bwl_event(opcode);

    switch (event) {
    case Event::ACS_Completed:
    case Event::CSA_Finished: {
        LOG(DEBUG) << buffer;

        if (event == Event::CSA_Finished) {
            if (std::chrono::steady_clock::now() <
                    (m_csa_event_filtering_timestamp +
                     std::chrono::milliseconds(CSA_EVENT_FILTERING_TIMEOUT_MS)) &&
                m_drop_csa) {
                LOG(DEBUG) << "CSA_Finished - dump event - arrive on csa filtering timeout window";
                return true;
            }
        } else {
            m_csa_event_filtering_timestamp = std::chrono::steady_clock::now();
            // ignore ACS_COMPLETED <iface> SCAN
            if (is_acs_completed_scan(buffer, bufLen)) {
                LOG(DEBUG) << "Received ACS_COMPLETED SCAN event, ignoring";
                return true;
            }
        }

        char reason[32]               = {0};
        char VAP[SSID_MAX_SIZE]       = {0};
        ChanSwReason chanSwReason     = ChanSwReason::Unknown;
        std::string channelStr        = (event == Event::CSA_Finished) ? "Channel=" : "channel=";
        size_t numOfValidArgs[8]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)VAP, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(VAP)},
            {(void *)&m_radio_info.channel, &numOfValidArgs[2], DWPAL_INT_PARAM, channelStr.c_str(),
             0},
            {(void *)&m_radio_info.bandwidth, &numOfValidArgs[3], DWPAL_INT_PARAM,
             "OperatingChannelBandwidt=", 0},
            {(void *)&m_radio_info.channel_ext_above, &numOfValidArgs[4], DWPAL_INT_PARAM,
             "ExtensionChannel=", 0},
            {(void *)&m_radio_info.vht_center_freq, &numOfValidArgs[5], DWPAL_INT_PARAM, "cf1=", 0},
            {(void *)&m_radio_info.is_dfs_channel, &numOfValidArgs[6], DWPAL_BOOL_PARAM,
             "dfs_chan=", 0},
            {(void *)&reason, &numOfValidArgs[7], DWPAL_STR_PARAM, "reason=", sizeof(reason)},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(m_radio_info)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " " << VAP;
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " " << channelStr
                   << m_radio_info.channel;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " OperatingChannelBandwidt= " << (int)m_radio_info.bandwidth;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4]
                   << " ExtensionChannel= " << (int)m_radio_info.channel_ext_above;
        LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5]
                   << " cf1= " << (int)m_radio_info.vht_center_freq;
        LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6]
                   << " dfs_chan= " << (int)m_radio_info.is_dfs_channel;
        LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7] << " reason= " << reason;
        /* End of TEMP: Traces... */

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        if (beerocks::utils::get_ids_from_iface_string(VAP).vap_id != beerocks::IFACE_RADIO_ID) {
            LOG(INFO) << "Ignoring ACS/CSA events on non Radio interface";
            break;
        }

        // Channel switch reason
        auto tmpStr = std::string(reason);
        if (tmpStr.find("RADAR") != std::string::npos) {
            chanSwReason = ChanSwReason::Radar;
        } else if (tmpStr.find("20_COEXISTANCE") != std::string::npos) {
            chanSwReason = ChanSwReason::CoEx_20;
        } else if (tmpStr.find("40_COEXISTANCE") != std::string::npos) {
            chanSwReason = ChanSwReason::CoEx_40;
        }
        m_radio_info.last_csa_sw_reason = chanSwReason;

        // Update the radio information structure
        if (son::wireless_utils::which_freq(m_radio_info.channel) == beerocks::eFreqType::FREQ_5G) {
            m_radio_info.is_5ghz = true;
        }

        event_queue_push(event);

        // TODO: WHY?!
        if (event == Event::ACS_Completed) {
            LOG(DEBUG) << "ACS_COMPLETED >>> adding CSA_FINISHED event as well";
            event_queue_push(Event::CSA_Finished);
        }
        break;
    }

    case Event::STA_Connected: {

        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        memset((char *)&msg->params.capabilities, 0, sizeof(msg->params.capabilities));

        char VAP[SSID_MAX_SIZE]        = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0};
        int supported_rates[16]        = {0};
        int RRM_CAP[8]                 = {0};
        int HT_MCS[16]                 = {0};
        int16_t VHT_MCS[16]            = {0};
        char ht_cap[8]                 = {0};
        char vht_cap[16]               = {0};
        size_t numOfValidArgs[20]      = {0};

        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)VAP, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(VAP)},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)supported_rates, &numOfValidArgs[3], DWPAL_INT_ARRAY_PARAM,
             "SupportedRates=", sizeof(supported_rates)},
            {(void *)RRM_CAP, &numOfValidArgs[4], DWPAL_INT_HEX_ARRAY_PARAM,
             "rrm_cap=", sizeof(RRM_CAP)},
            {(void *)HT_MCS, &numOfValidArgs[5], DWPAL_INT_HEX_ARRAY_PARAM,
             "HT_MCS=", sizeof(HT_MCS)},
            {(void *)VHT_MCS, &numOfValidArgs[6], DWPAL_INT_HEX_ARRAY_PARAM,
             "VHT_MCS=", sizeof(VHT_MCS)},
            {(void *)ht_cap, &numOfValidArgs[7], DWPAL_STR_PARAM, "HT_CAP=", sizeof(ht_cap)},
            {(void *)vht_cap, &numOfValidArgs[8], DWPAL_STR_PARAM, "VHT_CAP=", sizeof(vht_cap)},
            {(void *)&msg->params.capabilities.btm_supported, &numOfValidArgs[9], DWPAL_CHAR_PARAM,
             "btm_supported=", 0},
            {(void *)&msg->params.capabilities.cell_capa, &numOfValidArgs[10], DWPAL_CHAR_PARAM,
             "cell_capa=", 0},
            {(void *)&msg->params.capabilities.band_2g_capable, &numOfValidArgs[11],
             DWPAL_CHAR_PARAM, "band_2g_capable=", 0},
            {(void *)&msg->params.capabilities.band_5g_capable, &numOfValidArgs[12],
             DWPAL_CHAR_PARAM, "band_5g_capable=", 0},
            {(void *)&msg->params.capabilities.rrm_supported, &numOfValidArgs[13], DWPAL_CHAR_PARAM,
             "rrm_supported=", 0},
            {(void *)&msg->params.capabilities.max_ch_width, &numOfValidArgs[14], DWPAL_CHAR_PARAM,
             "max_ch_width=", 0},
            {(void *)&msg->params.capabilities.max_streams, &numOfValidArgs[15], DWPAL_CHAR_PARAM,
             "max_streams=", 0},
            {(void *)&msg->params.capabilities.phy_mode, &numOfValidArgs[16], DWPAL_CHAR_PARAM,
             "phy_mode=", 0},
            {(void *)&msg->params.capabilities.max_mcs, &numOfValidArgs[17], DWPAL_CHAR_PARAM,
             "max_mcs=", 0},
            {(void *)&msg->params.capabilities.max_tx_power, &numOfValidArgs[18], DWPAL_CHAR_PARAM,
             "max_tx_power=", 0},
            {(void *)&msg->params.capabilities.mumimo_supported, &numOfValidArgs[19],
             DWPAL_CHAR_PARAM, "mu_mimo=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(VAP)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        LOG(DEBUG) << "vap_id           : " << VAP;
        LOG(DEBUG) << "MACAddress       : " << MACAddress;
        LOG(DEBUG) << "supported_rates  : " << std::dec << supported_rates[0]
                   << " (first element only)";
        LOG(DEBUG) << "RRM_CAP          : 0x" << std::hex << RRM_CAP[0] << " (first element only)";
        LOG(DEBUG) << "HT_MCS           : 0x" << std::hex << HT_MCS[0] << " (first element only)";
        LOG(DEBUG) << "VHT_MCS          : 0x" << std::hex << VHT_MCS[0] << " (first element only)";
        LOG(DEBUG) << "HT_CAP           : " << ht_cap;
        LOG(DEBUG) << "VHT_CAP          : " << vht_cap;
        LOG(DEBUG) << "btm_supported    : " << (int)msg->params.capabilities.btm_supported;
        LOG(DEBUG) << "cell_capa        : " << (int)msg->params.capabilities.cell_capa;
        LOG(DEBUG) << "band_2g_capable  : " << (int)msg->params.capabilities.band_2g_capable;
        LOG(DEBUG) << "band_5g_capable  : " << (int)msg->params.capabilities.band_5g_capable;
        LOG(DEBUG) << "rrm_supported    : " << (int)msg->params.capabilities.rrm_supported;
        LOG(DEBUG) << "max_ch_width     : " << (int)msg->params.capabilities.max_ch_width;
        LOG(DEBUG) << "max_streams      : " << (int)msg->params.capabilities.max_streams;
        LOG(DEBUG) << "phy_mode         : " << (int)msg->params.capabilities.phy_mode;
        LOG(DEBUG) << "max_mcs          : " << (int)msg->params.capabilities.max_mcs;
        LOG(DEBUG) << "max_tx_power     : " << (int)msg->params.capabilities.max_tx_power;
        LOG(DEBUG) << "mumimo_supported : " << (int)msg->params.capabilities.mumimo_supported;

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i
                           << " ==> Continue with default values";
            }
        }

        msg->params.vap_id       = beerocks::utils::get_ids_from_iface_string(VAP).vap_id;
        msg->params.mac          = beerocks::net::network_utils::mac_from_string(MACAddress);
        msg->params.capabilities = {};

        std::string ht_cap_str(ht_cap);
        get_ht_mcs_capabilities(HT_MCS, ht_cap_str, msg->params.capabilities);

        if (get_radio_info().is_5ghz) {
            std::string vht_cap_str(vht_cap);
            get_vht_mcs_capabilities(VHT_MCS, vht_cap_str, msg->params.capabilities);
        }

        get_mcs_from_supported_rates(supported_rates, msg->params.capabilities);

        parse_rrm_capabilities(RRM_CAP, msg->params.capabilities);

        print_sta_capabilities(msg->params.capabilities);

        event_queue_push(Event::STA_Connected, msg_buff); // send message to the AP manager

        break;
    }

    case Event::STA_Disconnected: {
        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));

        char VAP[SSID_MAX_SIZE]        = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0};
        size_t numOfValidArgs[6]       = {0};
        FieldsToParse fieldsToParse[]  = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)VAP, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(VAP)},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)&msg->params.reason, &numOfValidArgs[3], DWPAL_CHAR_PARAM, "reason=", 0},
            {(void *)&msg->params.source, &numOfValidArgs[4], DWPAL_CHAR_PARAM, "source=", 0},
            {(void *)&msg->params.type, &numOfValidArgs[5], DWPAL_CHAR_PARAM, "type=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(VAP)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        LOG(DEBUG) << "vap_id     : " << VAP;
        LOG(DEBUG) << "MACAddress : " << MACAddress;
        LOG(DEBUG) << "reason     : " << int(msg->params.reason);
        LOG(DEBUG) << "source     : " << int(msg->params.source);
        LOG(DEBUG) << "type       : " << int(msg->params.type);

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(VAP).vap_id;
        msg->params.mac    = beerocks::net::network_utils::mac_from_string(MACAddress);

        event_queue_push(Event::STA_Disconnected, msg_buff); // send message to the AP manager
        break;
    }

    case Event::STA_Unassoc_RSSI: {
        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
        auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE *>(
            msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));

        char MACAddress[MAC_ADDR_SIZE] = {0};
        char rssi[24]                  = {0};
        uint64_t rx_packets            = 0;
        size_t numOfValidArgs[5]       = {0};
        FieldsToParse fieldsToParse[]  = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {NULL, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)rssi, &numOfValidArgs[3], DWPAL_STR_PARAM, "rssi=", sizeof(rssi)},
            {(void *)&rx_packets, &numOfValidArgs[4], DWPAL_LONG_LONG_INT_PARAM, "rx_packets=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(MACAddress)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " MACAddress= " << MACAddress;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " rssi= " << rssi;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " rx_packets= " << rx_packets;

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->params.rx_rssi    = beerocks::RSSI_INVALID;
        msg->params.rx_snr     = beerocks::SNR_INVALID;
        msg->params.result.mac = beerocks::net::network_utils::mac_from_string(MACAddress);

        // Split the RSSI values
        auto rssiVec = beerocks::string_utils::str_split(rssi, ' ');
        auto snrVec  = beerocks::string_utils::str_split(rssi, ' ');

        // -128 -128 -128 -128
        int rssi_cnt    = 0;
        float rssi_watt = 0;
        float rssi_tmp  = 0;
        for (auto v : rssiVec) {
            rssi_tmp = float(beerocks::string_utils::stoi(v));
            if (rssi_tmp > beerocks::RSSI_MIN) {
                rssi_watt += pow(10, rssi_tmp / float(10));
                rssi_cnt++;
            }
        }

        msg->params.rx_packets = (rx_packets >= 127) ? 127 : rx_packets;

        // Measurement succeeded
        if ((rssi_cnt > 0) && (msg->params.rx_packets > 1)) {
            rssi_watt           = (rssi_watt / float(rssi_cnt));
            msg->params.rx_rssi = int(10 * log10(rssi_watt));
            // TODO: add snr measurements when implementing unconnected station measurement on rdkb
        } else { // Measurement failed
            auto now = std::chrono::steady_clock::now();
            auto delta =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - m_unassoc_measure_start)
                    .count();
            auto temp_sum_delay = (delta - m_unassoc_measure_delay);

            if (temp_sum_delay > m_unassoc_measure_window_size) {
                LOG(ERROR) << "event_obj->params.rx_packets = -2 , delay = " << int(temp_sum_delay);
                msg->params.rx_packets =
                    -2; // means the actual measurment started later then aspected
            }
        }

        // Measurement succeeded
        if ((rssi_cnt > 0) && (msg->params.rx_packets > 1)) {
            rssi_watt           = (rssi_watt / float(rssi_cnt));
            msg->params.rx_rssi = int(10 * log10(rssi_watt));
            // TODO: add snr measurements when implementing unconnected station measurement on rdkb
        } else { // Measurement failed
            auto now = std::chrono::steady_clock::now();
            auto delta =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - m_unassoc_measure_start)
                    .count();
            auto temp_sum_delay = (delta - m_unassoc_measure_delay);

            if (temp_sum_delay > m_unassoc_measure_window_size) {
                LOG(ERROR) << "event_obj->params.rx_packets = -2 , delay = " << int(temp_sum_delay);
                msg->params.rx_packets =
                    -2; // means the actual measurment started later then aspected
            }
        }

        // Add the message to the queue
        event_queue_push(Event::STA_Unassoc_RSSI, msg_buff);
        break;
    }

    case Event::STA_Softblock_Drop: {
        LOG(DEBUG) << buffer;

        char client_mac[MAC_ADDR_SIZE]                      = {0};
        char vap_bssid[MAC_ADDR_SIZE]                       = {0};
        char vap_name[beerocks::message::IFACE_NAME_LENGTH] = {0};
        uint8_t message_type;
        size_t numOfValidArgsForMsgType[5] = {0};

        FieldsToParse fieldsToParseForMsgType[] = {
            {NULL /*opCode*/, &numOfValidArgsForMsgType[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)&message_type, &numOfValidArgsForMsgType[1], DWPAL_UNSIGNED_CHAR_PARAM,
             "msg_type=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParseForMsgType,
                                         sizeof(message_type)) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[1]=" << numOfValidArgsForMsgType[1]
                   << ", message_type=" << (int)message_type;

        if (message_type == WLAN_FC_STYPE_PROBE_REQ) {

            auto msg_buff =
                ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION *>(
                msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            // Initialize the message
            memset(msg_buff.get(), 0,
                   sizeof(sACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION));

            size_t numOfValidArgs[6] = {0};

            FieldsToParse fieldsToParse[] = {
                {(void *)vap_name, &numOfValidArgs[0], DWPAL_STR_PARAM, "VAP=", sizeof(vap_name)},
                {(void *)vap_bssid, &numOfValidArgs[1], DWPAL_STR_PARAM,
                 "VAP_BSSID=", sizeof(vap_bssid)},
                {(void *)client_mac, &numOfValidArgs[2], DWPAL_STR_PARAM,
                 "addr=", sizeof(client_mac)},
                {
                    (void *)&msg->params.rx_snr,
                    &numOfValidArgs[3],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "snr=",
                    0,
                },
                {
                    (void *)&msg->params.blocked,
                    &numOfValidArgs[4],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "blocked=",
                    0,
                },
                {
                    (void *)&msg->params.broadcast,
                    &numOfValidArgs[5],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "broadcast=",
                    0,
                },
                /* Must be at the end */
                {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

            if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(vap_name)) ==
                DWPAL_FAILURE) {
                LOG(ERROR) << "DWPAL parse error ==> Abort";
                return false;
            }

            for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
                if (numOfValidArgs[i] == 0) {
                    LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                    return false;
                }
            }

            LOG(DEBUG) << "numOfValidArgs[0]=" << numOfValidArgs[0] << ", VAP=" << vap_name;
            LOG(DEBUG) << "numOfValidArgs[1]=" << numOfValidArgs[1] << ", VAP_BSSID=" << vap_bssid;
            LOG(DEBUG) << "numOfValidArgs[2]=" << numOfValidArgs[2] << ", addr=" << client_mac;
            LOG(DEBUG) << "numOfValidArgs[3]=" << numOfValidArgs[3]
                       << ", rx_snr=" << (int)msg->params.rx_snr;
            LOG(DEBUG) << "numOfValidArgs[4]=" << numOfValidArgs[4]
                       << ", blocked=" << (int)msg->params.blocked;
            LOG(DEBUG) << "numOfValidArgs[5]=" << numOfValidArgs[5]
                       << ", broadcast=" << (int)msg->params.broadcast;

            msg->params.client_mac = beerocks::net::network_utils::mac_from_string(client_mac);
            //TODO need to add VAP name parsing to  this WLAN_FC_STYPE_PROBE_REQ event - WLANRTSYS-9170
            msg->params.bssid = beerocks::net::network_utils::mac_from_string(vap_bssid);

            // Add the message to the queue
            event_queue_push(Event::STA_Steering_Probe_Req, msg_buff);

        } else if (message_type == WLAN_FC_STYPE_AUTH) {

            auto msg_buff =
                ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION *>(
                msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            // Initialize the message
            memset(msg_buff.get(), 0,
                   sizeof(sACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION));

            size_t numOfValidArgs[7] = {0};

            FieldsToParse fieldsToParse[] = {
                {(void *)vap_name, &numOfValidArgs[0], DWPAL_STR_PARAM, "VAP=", sizeof(vap_name)},
                {(void *)vap_bssid, &numOfValidArgs[1], DWPAL_STR_PARAM,
                 "VAP_BSSID=", sizeof(vap_bssid)},
                {(void *)client_mac, &numOfValidArgs[2], DWPAL_STR_PARAM,
                 "addr=", sizeof(client_mac)},
                {
                    (void *)&msg->params.rx_snr,
                    &numOfValidArgs[3],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "snr=",
                    0,
                },
                {
                    (void *)&msg->params.blocked,
                    &numOfValidArgs[4],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "blocked=",
                    0,
                },
                {
                    (void *)&msg->params.reject,
                    &numOfValidArgs[5],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "rejected=",
                    0,
                },
                {
                    (void *)&msg->params.reason,
                    &numOfValidArgs[6],
                    DWPAL_UNSIGNED_CHAR_PARAM,
                    "reason=",
                    0,
                },
                /* Must be at the end */
                {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

            if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(vap_name)) ==
                DWPAL_FAILURE) {
                LOG(ERROR) << "DWPAL parse error ==> Abort";
                return false;
            }

            for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
                if (numOfValidArgs[i] == 0) {
                    LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                    return false;
                }
            }

            LOG(DEBUG) << "numOfValidArgs[0]=" << numOfValidArgs[0] << ", VAP=" << vap_name;
            LOG(DEBUG) << "numOfValidArgs[1]=" << numOfValidArgs[1] << ", VAP_BSSID=" << vap_bssid;
            LOG(DEBUG) << "numOfValidArgs[2]=" << numOfValidArgs[2]
                       << ", client_mac=" << client_mac;
            LOG(DEBUG) << "numOfValidArgs[3]=" << numOfValidArgs[3]
                       << ", rx_snr=" << (int)msg->params.rx_snr;
            LOG(DEBUG) << "numOfValidArgs[4]=" << numOfValidArgs[4]
                       << ", blocked=" << (int)msg->params.blocked;
            LOG(DEBUG) << "numOfValidArgs[5]=" << numOfValidArgs[5]
                       << ", rejected=" << (int)msg->params.reject;
            LOG(DEBUG) << "numOfValidArgs[6]=" << numOfValidArgs[6]
                       << ", reason=" << (int)msg->params.reason;

            msg->params.client_mac = beerocks::net::network_utils::mac_from_string(client_mac);
            //TODO need to add VAP name parsing to  this WLAN_FC_STYPE_AUTH event - WLANRTSYS-9170
            msg->params.bssid = beerocks::net::network_utils::mac_from_string(vap_bssid);

            // Add the message to the queue
            event_queue_push(Event::STA_Steering_Auth_Fail, msg_buff);

        } else {
            LOG(ERROR) << "Unknown message type!";
            break;
        }

        break;
    }

    case Event::BSS_TM_Response: {
        // TODO: Change to HAL objects
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));
        auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));

        char MACAddress[MAC_ADDR_SIZE]                      = {0};
        int status_code                                     = 0;
        char vap_name[beerocks::message::IFACE_NAME_LENGTH] = {0};
        size_t numOfValidArgs[4]                            = {0};
        FieldsToParse fieldsToParse[]                       = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)vap_name, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL,
             beerocks::message::IFACE_NAME_LENGTH},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)&status_code, &numOfValidArgs[3], DWPAL_INT_PARAM, "status_code=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse,
                                         sizeof(vap_name) + sizeof(MACAddress) +
                                             sizeof(status_code)) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " VAP name= " << vap_name;
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " MACAddress= " << MACAddress;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " status_code= " << int(status_code);

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->params.mac         = beerocks::net::network_utils::mac_from_string(MACAddress);
        msg->params.status_code = status_code;
        std::string bssid;
        beerocks::net::network_utils::linux_iface_get_mac(vap_name, bssid);
        LOG(DEBUG) << "BTM response source BSSID: " << bssid;
        msg->params.source_bssid = beerocks::net::network_utils::mac_from_string(bssid);

        // Add the message to the queue
        event_queue_push(Event::BSS_TM_Response, msg_buff);
        break;
    }

    case Event::DFS_CAC_Completed: {
        LOG(DEBUG) << buffer;

        if (!get_radio_info().is_5ghz) {
            LOG(WARNING) << "DFS event on 2.4Ghz radio. Ignoring...";
            return true;
        }

        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION));
        auto msg = reinterpret_cast<sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION *>(
            msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";
        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION));

        uint8_t chan_width            = 0;
        size_t numOfValidArgs[6]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {NULL, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
            {(void *)&msg->params.success, &numOfValidArgs[2], DWPAL_CHAR_PARAM, "success=", 0},
            {(void *)&msg->params.frequency, &numOfValidArgs[3], DWPAL_INT_PARAM, "freq=", 0},
            {(void *)&msg->params.timeout, &numOfValidArgs[4], DWPAL_INT_PARAM, "timeout=", 0},
            {(void *)&chan_width, &numOfValidArgs[5], DWPAL_CHAR_PARAM, "chan_width=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(int)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2]
                   << " success= " << (int)msg->params.success;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " freq= " << msg->params.frequency;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4]
                   << " timeout= " << msg->params.timeout;
        LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5]
                   << " chan_width= " << (int)chan_width;

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->params.channel   = son::wireless_utils::freq_to_channel(msg->params.frequency);
        msg->params.bandwidth = dwpal_bw_to_beerocks_bw(chan_width);

        // Add the message to the queue
        event_queue_push(Event::DFS_CAC_Completed, msg_buff);
        break;
    }

    case Event::DFS_NOP_Finished: {
        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION));
        auto msg = reinterpret_cast<sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION *>(
            msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0,
               sizeof(sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION));

        uint8_t chan_width            = 0;
        size_t numOfValidArgs[5]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {NULL, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
            {(void *)&msg->params.frequency, &numOfValidArgs[2], DWPAL_INT_PARAM, "freq=", 0},
            {(void *)&msg->params.vht_center_frequency, &numOfValidArgs[3], DWPAL_SHORT_INT_PARAM,
             "cf1=", 0},
            {(void *)&chan_width, &numOfValidArgs[4], DWPAL_CHAR_PARAM, "chan_width=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(int)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2]
                   << " freq= " << (int)msg->params.frequency;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " cf1= " << (int)msg->params.vht_center_frequency;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4]
                   << " chan_width= " << (int)chan_width;

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->params.channel   = son::wireless_utils::freq_to_channel(msg->params.frequency);
        msg->params.bandwidth = dwpal_bw_to_beerocks_bw(chan_width);

        // Add the message to the queue
        event_queue_push(Event::DFS_NOP_Finished, msg_buff);
        break;
    }

    case Event::AP_Disabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
        auto msg      = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_DISABLED_NOTIFICATION));

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(interface)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        event_queue_push(Event::AP_Disabled, msg_buff); // send message to the AP manager

    } break;
    case Event::AP_Enabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        auto msg      = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_ENABLED_NOTIFICATION));

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(interface)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        if (iface_ids.vap_id == beerocks::IFACE_RADIO_ID) {
            // Ignore AP-ENABLED on radio
            return true;
        }

        event_queue_push(Event::AP_Enabled, msg_buff);
    } break;

    case Event::Interface_Disabled:
    case Event::ACS_Failed:
        LOG(DEBUG) << buffer;
        event_queue_push(event); // Forward to the AP manager
        break;

    // Gracefully ignore unhandled events
    // TODO: Probably should be changed to an error once dwpal will stop
    //       sending empty or irrelevant events...
    default:
        LOG(WARNING) << "Unhandled event received: " << opcode;
        return true;
        break;
    }

    return true;
}

bool ap_wlan_hal_dwpal::process_dwpal_nl_event(struct nl_msg *msg)
{
    LOG(ERROR) << __func__ << "isn't implemented by this derived and shouldn't be called";
    return false;
}

} // namespace dwpal

std::shared_ptr<ap_wlan_hal> ap_wlan_hal_create(std::string iface_name, hal_conf_t hal_conf,
                                                base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<dwpal::ap_wlan_hal_dwpal>(iface_name, callback, hal_conf);
}

} // namespace bwl
