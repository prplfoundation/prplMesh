/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_dwpal.h"

#include <beerocks/bcl/beerocks_defines.h>
#include <beerocks/bcl/beerocks_os_utils.h>
#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/beerocks_version.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>
#include <easylogging++.h>
#include <math.h>

extern "C" {
#include <dwpal/dwpal.h>
}
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

// Temporary storage for station capabilities
struct SRadioCapabilitiesStrings {
    std::string supported_rates;
    std::string ht_cap;
    std::string ht_mcs;
    std::string vht_cap;
    std::string vht_mcs;
    std::string rrm_caps;
};

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

static bool get_sta_caps(SRadioCapabilitiesStrings &caps_strings,
                         beerocks::message::sRadioCapabilities &sta_caps, bool is_5ghz)
{
    bool break_upper_loop = false;
    std::stringstream str_stream;
    std::string item;
    uint8_t i = 0;

    sta_caps        = beerocks::message::sRadioCapabilities();
    sta_caps.ht_bw  = 0xFF;
    sta_caps.vht_bw = 0xFF;

    // HT, VHT caps parsing
    if (!caps_strings.ht_cap.empty()) {
        uint16_t ht_cap = uint16_t(std::stoul(caps_strings.ht_cap, nullptr, 16));
        sta_caps.ht_bw  = (ht_cap & HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET) >> 1; //20 == 0 / 40 == 1
        sta_caps.ht_sm_power_save =
            (ht_cap & HT_CAP_INFO_SMPS_MASK) >> 2; // 0=static, 1=dynamic, 2=reserved, 3=disabled
        sta_caps.ht_low_bw_short_gi =
            (ht_cap & HT_CAP_INFO_SHORT_GI20MHZ) >> 5; // 20 long == 0 / short == 1
        sta_caps.ht_high_bw_short_gi = (ht_cap & HT_CAP_INFO_SHORT_GI40MHZ) >> 6; // 40

        // parsing "AA BB CC DD XX ..." to 0xDDCCBBAA
        uint32_t ht_mcs = 0;
        str_stream.str(caps_strings.ht_mcs);
        for (i = 0; i < 4; i++) {
            getline(str_stream, item, ' ');
            ht_mcs |= (uint8_t)std::stoul(item, nullptr, 16) << (8 * i);
        }

        uint32_t mask = pow(2, 4 * 8 - 1);    // 0x80000000
        for (i = 4; i > 0; i--) {             // 4ss
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

        getline(str_stream, item, ' ');
        vht_mcs_rx = (uint16_t)std::stoul(item, nullptr, 16);

        for (i = 4; i > 0; i--) { // 4ss
            vht_mcs_temp = (vht_mcs_rx >> (2 * (i - 1))) &
                           0x03;       // 0 indicates support for VHT-MCS 0-7 for n spatial streams
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

    if (!caps_strings.rrm_caps.empty()) {
        uint8_t rrm_caps[6];
        str_stream.str(caps_strings.rrm_caps);
        std::string token;

        for (int i = 0; i < 6; i++) {
            std::getline(str_stream, token, ' ');
            rrm_caps[i] = std::stoul(token, nullptr, 16);
        }

        sta_caps.nr_enabled            = ((rrm_caps[0] & WLAN_RRM_CAPS_NEIGHBOR_REPORT) != 0);
        sta_caps.link_meas             = ((rrm_caps[0] & WLAN_RRM_CAPS_LINK_MEASUREMENT) != 0);
        sta_caps.beacon_report_passive = ((rrm_caps[0] & WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE) != 0);
        sta_caps.beacon_report_active  = ((rrm_caps[0] & WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE) != 0);
        sta_caps.beacon_report_table   = ((rrm_caps[0] & WLAN_RRM_CAPS_BEACON_REPORT_TABLE) != 0);
        sta_caps.lci_meas              = ((rrm_caps[1] & WLAN_RRM_CAPS_LCI_MEASUREMENT) != 0);
        sta_caps.fmt_range_report      = ((rrm_caps[4] & WLAN_RRM_CAPS_FTM_RANGE_REPORT) != 0);
    }

    // find deafult mcs
    str_stream.str(caps_strings.supported_rates);
    uint8_t max_num_rates = 16;
    uint16_t temp_rate    = 0;
    uint16_t max_rate     = 0;
    i                     = 0;
    while (getline(str_stream, item, ' ')) {
        temp_rate = (uint8_t)beerocks::string_utils::stoi(item);
        temp_rate = (temp_rate & 0x7F) * 5; // *5 because rate/2 * 10
        if (temp_rate > max_rate) {
            max_rate = temp_rate;
        }
        i++;
        if (i >= max_num_rates) {
            LOG(ERROR) << "Max number of rates reached: " << int(max_num_rates);
            break;
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
        << "ht_sm_power_save = " << (sta_caps.ht_sm_power_save ? "true" : "false")

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
        << "cell_capa = " << int(sta_caps.cell_capa) << std::endl
        << "link_meas = " << int(sta_caps.link_meas) << std::endl
        << "beacon_report_passive = " << int(sta_caps.beacon_report_passive) << std::endl
        << "beacon_report_active = " << int(sta_caps.beacon_report_active) << std::endl
        << "beacon_report_table = " << int(sta_caps.beacon_report_table) << std::endl
        << "lci_meas = " << int(sta_caps.lci_meas) << std::endl
        << "fmt_range_report = " << int(sta_caps.fmt_range_report);

    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_dwpal* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_dwpal::ap_wlan_hal_dwpal(std::string iface_name, bool acs_enabled,
                                     hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, acs_enabled, callback),
      base_wlan_hal_dwpal(bwl::HALType::AccessPoint, iface_name, acs_enabled, callback,
                          HOSTAPD_TO_DWPAL_MSG_LENGTH)
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

bool ap_wlan_hal_dwpal::sta_allow(const std::string &mac)
{
    // Build command string
    std::string cmd = "STA_ALLOW " + mac;

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_allow() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_deny(const std::string &mac, int reject_sta)
{
    // Build command string
    std::string cmd = "DENY_MAC " + mac;

    if (reject_sta) {
        cmd += " reject_sta=" + std::to_string(reject_sta);
    }

    // Send command
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "sta_deny() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_dwpal::sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    // Build command string
    const std::string ifname =
        beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    std::string cmd = "DISASSOCIATE " + ifname + " " + mac + " tx=0";

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

    if (disassoc_timer) {
        cmd += std::string() + " disassoc_imminent=" + "1" +
               " disassoc_timer=" + std::to_string(disassoc_timer);
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

bool ap_wlan_hal_dwpal::sta_unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                     int vht_center_frequency, int delay,
                                                     int window_size)
{
    // Convert values to strings
    std::string chanBandwidth     = std::to_string(bw);
    std::string centerFreq        = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
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

        // int bandwidth_int = utils::convert_bandwidth_to_int(bandwidth);
        std::string bandwidth_str             = std::to_string(bw);
        int freq                              = beerocks::utils::wifi_channel_to_freq(chan);
        std::string freq_str                  = std::to_string(freq);
        std::string wave_vht_center_frequency = std::to_string(vht_center_frequency);

        // Center Freq
        cmd += freq_str; // CenterFrequency

        // Extension Channel
        if (bw != 20) {
            if (freq < vht_center_frequency) {
                cmd += " sec_channel_offset=1";
            } else {
                cmd += " sec_channel_offset=-1";
            }
        }

        // Channel bandwidth
        if (bw == 80) {
            cmd += " center_freq1=" + wave_vht_center_frequency;
        }

        cmd += " bandwidth=" + bandwidth_str;

        // Supported Standard n/ac
        if (bw == 20 || bw == 40) {
            cmd += " ht"; //n
        } else if (bw == 80) {
            cmd += " vht"; // ac
        }
    }

    // Send command
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
        std::string chan_str = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
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

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse) == DWPAL_FAILURE) {
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
        chan = beerocks::utils::wifi_freq_to_channel(beerocks::string_utils::stoi(freq));
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

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse) == DWPAL_FAILURE) {
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

bool ap_wlan_hal_dwpal::set_vap_enable(const std::string &iface_name, const bool enable)
{
    LOG(DEBUG) << "TODO: missing function implementation";
    return true;
}

bool ap_wlan_hal_dwpal::get_vap_enable(const std::string &iface_name, bool &enable)
{
    LOG(DEBUG) << "TODO: missing function implementation";
    return true;
}

std::string ap_wlan_hal_dwpal::get_radio_driver_version()
{
    return beerocks::version::get_wave_version();
}

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

    if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE)
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
        LOG(DEBUG) << opcode << " buffer= \n" << buffer;
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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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
        LOG(DEBUG) << "AP-STA-CONNECTED buffer= \n" << buffer;
        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";
        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        msg->params.capabilities = beerocks::message::sRadioCapabilities();

        char VAP[SSID_MAX_SIZE]        = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0};
        char supportedRates[64]        = {0};
        char HT_CAP[8]                 = {0};
        char HT_MCS[64]                = {0};
        char VHT_CAP[16]               = {0};
        char VHT_MCS[24]               = {0};
        char RRM_CAP[20]               = {0};
        size_t numOfValidArgs[20]      = {0};
        FieldsToParse fieldsToParse[]  = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)VAP, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(VAP)},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)supportedRates, &numOfValidArgs[3], DWPAL_STR_PARAM,
             "SupportedRates=", sizeof(supportedRates)},
            {(void *)HT_CAP, &numOfValidArgs[4], DWPAL_STR_PARAM, "HT_CAP=", sizeof(HT_CAP)},
            {(void *)HT_MCS, &numOfValidArgs[5], DWPAL_STR_PARAM, "HT_MCS=", sizeof(HT_MCS)},
            {(void *)VHT_CAP, &numOfValidArgs[6], DWPAL_STR_PARAM, "VHT_CAP=", sizeof(VHT_CAP)},
            {(void *)VHT_MCS, &numOfValidArgs[7], DWPAL_STR_PARAM, "VHT_MCS=", sizeof(VHT_MCS)},
            {(void *)&msg->params.capabilities.btm_supported, &numOfValidArgs[8], DWPAL_CHAR_PARAM,
             "btm_supported=", 0},
            {(void *)&msg->params.capabilities.cell_capa, &numOfValidArgs[9], DWPAL_CHAR_PARAM,
             "cell_capa=", 0},
            {(void *)&msg->params.capabilities.band_2g_capable, &numOfValidArgs[10],
             DWPAL_CHAR_PARAM, "band_2g_capable=", 0},
            {(void *)&msg->params.capabilities.band_5g_capable, &numOfValidArgs[11],
             DWPAL_CHAR_PARAM, "band_5g_capable=", 0},
            {(void *)&msg->params.capabilities.rrm_supported, &numOfValidArgs[12], DWPAL_CHAR_PARAM,
             "rrm_supported=", 0},
            {(void *)RRM_CAP, &numOfValidArgs[13], DWPAL_STR_PARAM, "rrm_cap=", sizeof(RRM_CAP)},
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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " vap_id= " << VAP;
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " MACAddress= " << MACAddress;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " supportedRates= " << supportedRates;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " HT_CAP= " << HT_CAP;
        LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " HT_MCS= " << HT_MCS;
        LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " VHT_CAP= " << VHT_CAP;
        LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7] << " VHT_MCS= " << VHT_MCS;
        LOG(DEBUG) << "numOfValidArgs[8]= " << numOfValidArgs[8]
                   << " btm_supported= " << (int)msg->params.capabilities.btm_supported;
        LOG(DEBUG) << "numOfValidArgs[9]= " << numOfValidArgs[9]
                   << " cell_capa= " << (int)msg->params.capabilities.cell_capa;
        LOG(DEBUG) << "numOfValidArgs[10]= " << numOfValidArgs[10]
                   << " band_2g_capable= " << (int)msg->params.capabilities.band_2g_capable;
        LOG(DEBUG) << "numOfValidArgs[11]= " << numOfValidArgs[11]
                   << " band_5g_capable= " << (int)msg->params.capabilities.band_5g_capable;
        LOG(DEBUG) << "numOfValidArgs[12]= " << numOfValidArgs[12]
                   << " rrm_supported= " << (int)msg->params.capabilities.rrm_supported;
        LOG(DEBUG) << "numOfValidArgs[13]= " << numOfValidArgs[13] << " RRM_CAP= " << RRM_CAP;
        LOG(DEBUG) << "numOfValidArgs[14]= " << numOfValidArgs[14]
                   << " max_ch_width= " << (int)msg->params.capabilities.max_ch_width;
        LOG(DEBUG) << "numOfValidArgs[15]= " << numOfValidArgs[15]
                   << " max_streams= " << (int)msg->params.capabilities.max_streams;
        LOG(DEBUG) << "numOfValidArgs[16]= " << numOfValidArgs[16]
                   << " phy_mode= " << (int)msg->params.capabilities.phy_mode;
        LOG(DEBUG) << "numOfValidArgs[17]= " << numOfValidArgs[17]
                   << " max_mcs= " << (int)msg->params.capabilities.max_mcs;
        LOG(DEBUG) << "numOfValidArgs[18]= " << numOfValidArgs[18]
                   << " max_tx_power= " << (int)msg->params.capabilities.max_tx_power;
        LOG(DEBUG) << "numOfValidArgs[19]= " << numOfValidArgs[19]
                   << " mumimo_supported= " << (int)msg->params.capabilities.mumimo_supported;
        /* End of TEMP: Traces... */

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i
                           << " ==> Continue with default values";
            }
        }

        msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(VAP).vap_id;
        msg->params.mac    = beerocks::net::network_utils::mac_from_string(MACAddress);

        SRadioCapabilitiesStrings caps_strings;
        caps_strings.supported_rates.assign(supportedRates);
        caps_strings.ht_cap.assign(HT_CAP);
        caps_strings.ht_mcs.assign(HT_MCS);
        caps_strings.vht_cap.assign(VHT_CAP);
        caps_strings.vht_mcs.assign(VHT_MCS);
        caps_strings.rrm_caps.assign(RRM_CAP);
        get_sta_caps(caps_strings, msg->params.capabilities, get_radio_info().is_5ghz);

        // Add the message to the queue
        event_queue_push(Event::STA_Connected, msg_buff);
        break;
    }

    case Event::STA_Disconnected: {
        LOG(DEBUG) << "AP-STA-DISCONNECTED buffer= \n" << buffer;
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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " vap_id= " << VAP;
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " MACAddress= " << MACAddress;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " reason= " << int(msg->params.reason);
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4]
                   << " source= " << int(msg->params.source);
        LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5]
                   << " type= " << int(msg->params.type);

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(VAP).vap_id;
        msg->params.mac    = beerocks::net::network_utils::mac_from_string(MACAddress);

        // Add the message to the queue
        event_queue_push(Event::STA_Disconnected, msg_buff);
        break;
    }

    case Event::STA_Unassoc_RSSI: {
        LOG(DEBUG) << "UNCONNECTED-STA-RSSI buffer= \n" << buffer;
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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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
        LOG(DEBUG) << opcode << " buffer=" << std::endl << buffer;
        // TODO: Change to HAL objects

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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParseForMsgType) ==
            DWPAL_FAILURE) {
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

            if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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

            if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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
        LOG(DEBUG) << "BSS-TM-RESP buffer= \n" << buffer;
        // TODO: Change to HAL objects
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));
        auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";
        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));

        char MACAddress[MAC_ADDR_SIZE] = {0};
        int status_code                = 0;
        size_t numOfValidArgs[4]       = {0};
        FieldsToParse fieldsToParse[]  = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {NULL, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)&status_code, &numOfValidArgs[3], DWPAL_INT_PARAM, "status_code=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
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

        // Add the message to the queue
        event_queue_push(Event::BSS_TM_Response, msg_buff);
        break;
    }

    case Event::DFS_CAC_Completed: {
        LOG(DEBUG) << "DFS-CAC-COMPLETED buffer= \n" << buffer;
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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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

        msg->params.channel   = beerocks::utils::wifi_freq_to_channel(msg->params.frequency);
        msg->params.bandwidth = dwpal_bw_to_beerocks_bw(chan_width);

        // Add the message to the queue
        event_queue_push(Event::DFS_CAC_Completed, msg_buff);
        break;
    }

    case Event::DFS_NOP_Finished: {
        LOG(DEBUG) << "DFS-NOP-FINISHED buffer= \n" << buffer;
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

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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

        msg->params.channel   = beerocks::utils::wifi_freq_to_channel(msg->params.frequency);
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
        LOG(INFO) << "AP_Disabled buffer= \n" << buffer;

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        event_queue_push(Event::AP_Disabled, msg_buff);

    } break;
    case Event::AP_Enabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        auto msg      = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        LOG(DEBUG) << "AP_ENABLED buffer= \n" << buffer;

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse) == DWPAL_FAILURE) {
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
        // Forward to the AP manager
        event_queue_push(event);
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

} // namespace dwpal
} // namespace bwl

// AP dwpal HAL Factory Functions
extern "C" {

bwl::ap_wlan_hal *ap_wlan_hal_create(std::string iface_name, bool acs_enabled,
                                     bwl::base_wlan_hal::hal_event_cb_t callback)
{
    return new bwl::dwpal::ap_wlan_hal_dwpal(iface_name, acs_enabled, callback);
}

void ap_wlan_hal_destroy(bwl::ap_wlan_hal *obj) { delete obj; }
}
