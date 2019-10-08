/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_nl80211.h"

#include <beerocks/bcl/beerocks_os_utils.h>
#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/beerocks_version.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

#include <cmath>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace nl80211 {

#define BUFFER_SIZE 4096
#define CSA_EVENT_FILTERING_TIMEOUT_MS 1000

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

// Temporary storage for station capabilities
struct SRadioCapabilitiesStrings {
    std::string supported_rates;
    std::string ht_cap;
    std::string ht_mcs;
    std::string vht_cap;
    std::string vht_mcs;
    std::string btm_supported;
    std::string nr_enabled;
    std::string non_pref_chan;
    std::string cell_capa;
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static ap_wlan_hal::Event nl80211_to_bwl_event(const std::string &opcode)
{
    if (opcode == "AP-ENABLED") {
        return ap_wlan_hal::Event::AP_Enabled;
    } else if (opcode == "AP-DISABLED") {
        return ap_wlan_hal::Event::AP_Disabled;
    } else if (opcode == "AP-STA-CONNECTED") {
        return ap_wlan_hal::Event::STA_Connected;
    } else if (opcode == "AP-STA-DISCONNECTED") {
        return ap_wlan_hal::Event::STA_Disconnected;
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
    }

    return ap_wlan_hal::Event::Invalid;
}

#if 0

static uint8_t wav_bw_to_beerocks_bw(const char *tmpBuff)
{

    // 0 => CHAN_WIDTH_20_NOHT
    // 1 => CHAN_WIDTH_20
    // 2 => CHAN_WIDTH_40
    // 3 => CHAN_WIDTH_80
    // 4 => CHAN_WIDTH_80P80
    // 5 => CHAN_WIDTH_160
    // 6 => CHAN_WIDTH_UNKNOWN
    uint8_t bw = (uint8_t)beerocks::string_utils::stoi(tmpBuff);

    if (bw > 6) {
        LOG(ERROR) << "Invalid bandwidth value: " << int(bw);
        return 0;
    }

    // Convert to beerocks value (we have only one value for 20Mhz bandwidth)
    if (bw > 0) {
        bw -= 1;
    }

    // Treat 80P80 as 160
    if (bw >= 3) {
        bw = (uint8_t)BANDWIDTH_160;
    }

    return bw;
}

static bool get_sta_caps(SRadioCapabilitiesStrings &caps_strings, 
    beerocks::message::sRadioCapabilities &sta_caps, bool is_5ghz)
{
    bool break_upper_loop = false;
    std::stringstream str_stream;
    std::string item;

    memset(&sta_caps, 0, sizeof(sta_caps));
    sta_caps.ht_bw  = 0xFF;
    sta_caps.vht_bw = 0xFF;

    // extract btm_supported, nr_enabled, cell_capa
    sta_caps.btm_supported = ((caps_strings.btm_supported == "true") ? true : false);
    sta_caps.nr_enabled    = ((caps_strings.nr_enabled == "true") ? true : false);
    sta_caps.cell_capa     = (uint8_t)beerocks::string_utils::stoi(caps_strings.cell_capa);

    // HT, VHT caps parsing
    if (!caps_strings.ht_cap.empty()) {
        uint16_t ht_cap              = uint16_t(std::stoul(caps_strings.ht_cap, nullptr, 16));
        sta_caps.ht_bw               = (ht_cap >> 1) & 0x01; //20 == 0 / 40 == 1
        sta_caps.ht_low_bw_short_gi  = (ht_cap >> 5) & 0x01; // 20 long == 0 / short == 1
        sta_caps.ht_high_bw_short_gi = (ht_cap >> 6) & 0x01; // 40

        // parsing "AA BB CC DD XX ..." to 0xDDCCBBAA
        uint32_t ht_mcs = 0;
        str_stream.str(caps_strings.ht_mcs);
        for (int i = 0; i < 4; i++) {
            getline(str_stream, item, ' ');
            ht_mcs |= (uint8_t)std::stoul(item, nullptr, 16) << (8 * i);
        }

        uint32_t mask = pow(2, 4 * 8 - 1);
        for (int8_t i = 4; i > 0; i--) {      // 4ss
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
    }

    if (is_5ghz && (!caps_strings.vht_cap.empty())) {
        uint32_t vht_cap          = uint16_t(std::stoul(caps_strings.vht_cap, nullptr, 16));
        uint8_t supported_bw_bits = (vht_cap >> 2) & 0x03;
        if (supported_bw_bits == 0x03) { // reserved mode
            LOG(ERROR) << "INFORMATION ERROR! STA SENT RESERVED BIT COMBINATION";
        }

        // if supported_bw_bits=0 max bw is 80 Mhz, else max bw is 160 Mhz
        sta_caps.vht_bw               = (BANDWIDTH_80 + (supported_bw_bits > 0 ? 1 : 0));
        sta_caps.vht_low_bw_short_gi  = (vht_cap >> 5) & 0x01; // 80 Mhz
        sta_caps.vht_high_bw_short_gi = (vht_cap >> 6) & 0x01; // 160 Mhz

        str_stream.str(caps_strings.vht_mcs);
        uint16_t vht_mcs_rx = 0;
        uint16_t vht_mcs_temp;

        for (int8_t i = 0; i < 3; i++) {
            getline(str_stream, item, ' ');
            if (i == 0) {
                vht_mcs_rx = (uint16_t)std::stoul(item, nullptr, 16);
                break;
            }
        }

        for (int8_t i = 4; i > 0; i--) // 4ss
        {
            vht_mcs_temp = (vht_mcs_rx >> (2 * (i - 1))) & 0x03;
            if (vht_mcs_temp != 0x3) { //0x3 == not supported
                sta_caps.vht_ss  = i;
                sta_caps.ant_num = i;
                sta_caps.vht_mcs = vht_mcs_temp + 7;
                break;
            }
        }
    }

    // find deafult mcs
    str_stream.str(caps_strings.supported_rates);
    uint16_t temp_rate;
    uint16_t max_rate = 0;
    while (!std::getline(str_stream, item, ' ').eof()) {
        temp_rate = (uint8_t)beerocks::string_utils::stoi(item);
        temp_rate = (temp_rate & 0x7F) * 5; // *5 because rate/2 * 10
        if (temp_rate > max_rate) {
            max_rate = temp_rate;
        }
    }
    LOG(DEBUG) << "deafult mcs max rate: " << int(max_rate) << " [100Kb]";

    if (!son::wireless_utils::get_mcs_from_rate(max_rate, beerocks::ANT_MODE_1X1_SS1,
                                                beerocks::BANDWIDTH_20, sta_caps.default_mcs,
                                                sta_caps.default_short_gi)) {
        LOG(DEBUG) << "No MCS rate match -> USING NEAREST VALUE!";
    }

    // update standard
    if (is_5ghz) {
        if (sta_caps.vht_ss) {
            sta_caps.wifi_standard = STANDARD_AC;
        } else if (sta_caps.ht_ss) {
            sta_caps.wifi_standard = STANDARD_N;
        } else {
            sta_caps.wifi_standard = STANDARD_A;
        }
    } else if (sta_caps.ht_ss) { // 2.4 && HT
        sta_caps.wifi_standard = STANDARD_N;
    } else {
        sta_caps.wifi_standard = STANDARD_B | STANDARD_G;
    }

    LOG(DEBUG)
        << std::endl
        << " sta HT_CAPS:" << std::endl
        << "bw20 short gi = " << (sta_caps.ht_low_bw_short_gi ? "true" : "false") << std::endl
        << "bw40 short gi = " << (sta_caps.ht_high_bw_short_gi ? "true" : "false") << std::endl
        << "ht_mcs = " << ((int(sta_caps.ht_mcs)) ? std::to_string(sta_caps.ht_mcs) : "n/a")
        << std::endl
        << "ht_ss = " << ((int(sta_caps.ht_ss)) ? std::to_string(sta_caps.ht_ss) : "n/a")
        << std::endl
        << "ht_bw = "
        << ((sta_caps.ht_bw != 0xFF) ? std::to_string(beerocks::utils::convert_bandwidth_to_int(
                                           beerocks::eWiFiBandwidth(sta_caps.ht_bw)))
                                     : "n/a")

        << std::endl
        << "\n sta VHT_CAPS:" << std::endl
        << "bw80 short gi = " << (sta_caps.vht_low_bw_short_gi ? "true" : "false") << std::endl
        << "bw160 short gi = " << (sta_caps.vht_high_bw_short_gi ? "true" : "false") << std::endl
        << "vht_mcs = " << ((int(sta_caps.vht_mcs)) ? std::to_string(sta_caps.vht_mcs) : "n/a")
        << std::endl
        << "vht_ss = " << ((int(sta_caps.vht_ss)) ? std::to_string(sta_caps.vht_ss) : "n/a")
        << std::endl
        << "vht_bw = "
        << ((sta_caps.vht_bw != 0xFF) ? std::to_string(beerocks::utils::convert_bandwidth_to_int(
                                            beerocks::eWiFiBandwidth(sta_caps.vht_bw)))
                                      : "n/a")

        << std::endl
        << "\n sta DEFAULT_CAPS:" << std::endl
        << "default_mcs = " << int(sta_caps.default_mcs) << std::endl
        << "default_short_gi = " << int(sta_caps.default_short_gi)

        << std::endl
        << "\n sta OTHER_CAPS:" << std::endl
        << "wifi_standard [enum] = " << int(sta_caps.wifi_standard) << std::endl
        << "btm_supported = " << (sta_caps.btm_supported ? "true" : "false") << std::endl
        << "nr_enabled = " << (sta_caps.nr_enabled ? "true" : "false") << std::endl
        << "cell_capa = " << int(sta_caps.cell_capa);

    return true;
}

#endif

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_nl80211* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_nl80211::ap_wlan_hal_nl80211(std::string iface_name, bool acs_enabled, hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, acs_enabled, callback),
      base_wlan_hal_nl80211(bwl::HALType::AccessPoint, iface_name, acs_enabled, callback, BUFFER_SIZE)
{
}

ap_wlan_hal_nl80211::~ap_wlan_hal_nl80211() {}

HALState ap_wlan_hal_nl80211::attach(bool block)
{
    auto state = base_wlan_hal_nl80211::attach(block);

    // On Operational send the AP_Attached event to the AP Manager
    if (state == HALState::Operational) {
        event_queue_push(Event::AP_Attached);
    }

    return state;
}

bool ap_wlan_hal_nl80211::sta_allow(const std::string &mac)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    // Build command string
    // We use the DENY_ACL list only
    const std::string cmd = "DENY_ACL DEL_MAC " + mac;

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_allow() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_nl80211::sta_deny(const std::string &mac, int reject_sta)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    // Build command string
    // We use the DENY_ACL list only
    const std::string cmd = "DENY_ACL ADD_MAC " + mac;

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_deny() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_nl80211::sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    // Build command string
    const std::string cmd = "DISASSOCIATE " + mac + " reason=" + std::to_string(reason) + " tx=0";

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_disassoc() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_nl80211::sta_deauth(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    // Build command string
    const std::string cmd = "DEAUTHENTICATE " + mac + " reason=" + std::to_string(reason) + " tx=0";

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_disassoc() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_nl80211::sta_bss_steer(const std::string &mac, const std::string &bssid, int chan,
                                    int disassoc_timer, int valid_int)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", BSS: " << bssid << ", channel: " << chan
               << ", disassoc: " << disassoc_timer << ", valid_int: " << valid_int;

    // Build command string
    std::string cmd =
        // Set the STA MAC address
        "BSS_TM_REQ " + mac
        // Transition management parameters
        + " pref=" + "1" + " abridged=" + "1" +
        // Target BSSID
        " neighbor=" + bssid + ",0,0," + std::to_string(chan) + ",0";

    if (disassoc_timer) {
        cmd += std::string() + " disassoc_imminent=" + "1" +
               " disassoc_timer=" + std::to_string(disassoc_timer);
    }
    // " bss_term="  // Unused Param
    // " url="       // Unused Param
    // " mbo="       // Unused Param

    if (valid_int) {
        cmd += " valid_int=" + std::to_string(valid_int);
    }

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_bss_steer() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_nl80211::sta_unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                   int vht_center_frequency, int delay,
                                                   int window_size)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";   
    return false;
}

bool ap_wlan_hal_nl80211::sta_softblock_add(
        const std::string &vap_name, const std::string &client_mac,
        uint8_t reject_error_code, uint8_t probe_snr_threshold_hi,
        uint8_t probe_snr_threshold_lo,
        uint8_t authetication_snr_threshold_hi,
        uint8_t authetication_snr_threshold_lo)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_nl80211::sta_softblock_remove(const std::string &vap_name,
        const std::string &client_mac)
{
    return false;
}

bool ap_wlan_hal_nl80211::switch_channel(int chan, int bw, int vht_center_frequency)
{
    LOG(TRACE) << __func__ << " channel: " << chan << ", bw: " << bw
               << ", vht_center_frequency: " << vht_center_frequency;

    // TODO: IMPLEMENT!

    LOG(DEBUG) << "Got channel switch, simulate ACS-STARTED;ACS-COMPLETED";
    event_queue_push(Event::ACS_Started);
    event_queue_push(Event::ACS_Completed);
    event_queue_push(Event::CSA_Finished);

    return true;
}

bool ap_wlan_hal_nl80211::set_antenna_mode(AntMode mode)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::wds_set_mode(WDSMode mode)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::wds_add_sta(const std::string &mac)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::wds_del_sta(const std::string &mac)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::wds_clear_list()
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::failsafe_channel_get(int &chan, int &bw)
{
    LOG(TRACE) << __func__;

    // Build command string
    std::string cmd = "GET_FAILSAFE_CHAN";
    char *reply;

    // Send command
    if (!wpa_ctrl_send_msg(cmd, &reply)) {
        LOG(ERROR) << "failsafe_channel_get() failed!";
        return false;
    }

    // Custom reply parsing
    if (!strncmp(reply, "UNSPECIFIED", 11)) {
        chan = -1;
        bw   = -1;
    } else if (!strncmp(reply, "ACS", 3)) {
        chan = bw = 0;

    } else {
        int freq;
        sscanf(reply, "%d %*s %*s bandwidth=%d", &freq, &bw);
        chan = beerocks::utils::wifi_freq_to_channel(freq);
    }

    return true;
}

bool ap_wlan_hal_nl80211::restricted_channels_set(char *channel_list)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::restricted_channels_get(char *channel_list)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    memset(channel_list, 0, beerocks::message::RESTRICTED_CHANNEL_LENGTH);
    return true;
}

bool ap_wlan_hal_nl80211::read_acs_report()
{
    LOG(TRACE) << __func__ << " for interface: " << get_radio_info().iface_name;

    // TODO: Get actual list of supported channels

    uint8_t idx = 0;
    if (m_radio_info.is_5ghz == false) {
        // 2.4G simulated report
        for (uint16_t ch = 1; ch <= 11; ch++) {
            m_radio_info.supported_channels[idx].channel     = ch;
            m_radio_info.supported_channels[idx].bandwidth   = 20;
            m_radio_info.supported_channels[idx].bss_overlap = 10;
            m_radio_info.supported_channels[idx].is_dfs      = 0;
            idx++;
        }
    } else {
        // 5G simulated report
        for (uint16_t ch = 36; ch <= 64; ch += 4) {
            for (uint16_t step = 0; step < 3; step++) {
                m_radio_info.supported_channels[idx].channel     = ch;
                m_radio_info.supported_channels[idx].bandwidth   = 20 + step * 20;
                m_radio_info.supported_channels[idx].bss_overlap = 10 + step * 10;
                m_radio_info.supported_channels[idx].is_dfs      = (ch > 48) ? 1 : 0;
                idx++;
            }
        }
        for (uint16_t ch = 100; ch <= 144; ch += 4) {
            for (uint16_t step = 0; step < 3; step++) {
                m_radio_info.supported_channels[idx].channel     = ch;
                m_radio_info.supported_channels[idx].bandwidth   = 20 + step * 20;
                m_radio_info.supported_channels[idx].bss_overlap = 10 + step * 10;
                m_radio_info.supported_channels[idx].is_dfs      = 1;
                idx++;
            }
        }
        for (uint16_t ch = 149; ch <= 165; ch += 4) {
            for (uint16_t step = 0; step < 3; step++) {
                m_radio_info.supported_channels[idx].channel     = ch;
                m_radio_info.supported_channels[idx].bandwidth   = 20 + step * 20;
                m_radio_info.supported_channels[idx].bss_overlap = 10 + step * 10;
                m_radio_info.supported_channels[idx].is_dfs      = 0;
                idx++;
            }
        }
    }

    return true;
}

bool ap_wlan_hal_nl80211::set_vap_enable(const std::string &iface_name, const bool enable)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::get_vap_enable(const std::string &iface_name, bool &enable)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

std::string ap_wlan_hal_nl80211::get_radio_driver_version()
{
    return "nl80211";
}

bool ap_wlan_hal_nl80211::process_nl80211_event(parsed_obj_map_t &parsed_obj)
{
    // Filter out empty events
    std::string opcode;
    if (!(parsed_obj.find("_opcode") != parsed_obj.end() &&
          !(opcode = parsed_obj["_opcode"]).empty())) {
        return true;
    }

    // LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = nl80211_to_bwl_event(opcode);

    switch (event) {

    // STA Connected
    case Event::STA_Connected: {

        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));

        msg->params.vap_id = 0;
        msg->params.mac = beerocks::net::network_utils::mac_from_string(parsed_obj["_mac"]);

        bool caps_valid = true;

        // TODO: Update station capabilities
#if 0

        SRadioCapabilitiesStrings caps_strings;
        if (!wav_obj_read_str("SupportedRates", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading SupportedRates parameter!";
            caps_valid = false;
        } else {
            caps_strings.supported_rates.assign(tmp_str);
        }

        if (!wav_obj_read_str("HT_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.ht_cap.assign(tmp_str);
        }

        if (!wav_obj_read_str("HT_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HT_MCS parameter!";
            caps_valid = false;
        } else {
            caps_strings.ht_mcs.assign(tmp_str);
        }

        if (!wav_obj_read_str("VHT_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading VHT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.vht_cap.assign(tmp_str);
        }

        if (!wav_obj_read_str("VHT_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading VHT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.vht_mcs.assign(tmp_str);
        }

        if (!wav_obj_read_str("btm_supported", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading btm_supported parameter!";
            caps_valid = false;
        } else {
            caps_strings.btm_supported.assign(tmp_str);
        }

        if (!wav_obj_read_str("nr_enabled", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading nr_enabled parameter!";
            caps_valid = false;
        } else {
            caps_strings.nr_enabled.assign(tmp_str);
        }

        if (!wav_obj_read_str("cell_capa", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading cell_capa parameter!";
            caps_valid = false;
        } else {
            caps_strings.cell_capa.assign(tmp_str);
        }

#endif

        if (caps_valid) {
            // get_sta_caps(caps_strings, msg->params.capabilities, get_radio_info().is_5ghz);
        } else {
            LOG(ERROR) << "One or more of required capability strings is missing!";

            // Setting minimum default values
            msg->params.capabilities.ant_num       = 1;
            msg->params.capabilities.wifi_standard = STANDARD_N;
            msg->params.capabilities.default_mcs   = MCS_6;
        }

        // Add the message to the queue
        event_queue_push(Event::STA_Connected, msg_buff);

    } break;

    // STA Disconnected
    case Event::STA_Disconnected: {

        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));

        // Store the MAC address of the disconnected STA
        msg->params.vap_id = 0;
        msg->params.mac = beerocks::net::network_utils::mac_from_string(parsed_obj["_mac"]);

        // Add the message to the queue
        event_queue_push(Event::STA_Disconnected, msg_buff);

    } break;

    // BSS Transition (802.11v)
    case Event::BSS_TM_Response: {

        // TODO: Change to HAL objects
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));
        auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));

        // Client params
        msg->params.mac = beerocks::net::network_utils::mac_from_string(parsed_obj["_mac"]);
        msg->params.status_code = beerocks::string_utils::stoi(parsed_obj["status_code"]);

        // Add the message to the queue
        event_queue_push(Event::BSS_TM_Response, msg_buff);

    } break;

    // ACS/CSA Completed
    case Event::ACS_Completed:
    case Event::CSA_Finished:
    case Event::Interface_Disabled:
    case Event::ACS_Failed: {
        // Forward to the AP manager
        event_queue_push(event);
    } break;

    // Gracefully ignore unhandled events
    // TODO: Probably should be changed to an error once WAV will stop
    //       sending empty or irrelevant events...
    default: {
        LOG(WARNING) << "Unhandled event received: " << opcode;
        return true;

    } break;
    }

    return true;
}

} // namespace nl80211
} // namespace bwl

// AP WAV HAL Factory Functions
extern "C" {

bwl::ap_wlan_hal *ap_wlan_hal_create(std::string iface_name, bool acs_enabled,
                                     bwl::base_wlan_hal::hal_event_cb_t callback)
{
    return new bwl::nl80211::ap_wlan_hal_nl80211(iface_name, acs_enabled, callback);
}

void ap_wlan_hal_destroy(bwl::ap_wlan_hal *obj) { delete obj; }
}
