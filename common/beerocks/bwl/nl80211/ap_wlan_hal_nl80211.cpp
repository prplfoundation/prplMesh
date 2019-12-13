/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_nl80211.h"

#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

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

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_nl80211* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_nl80211::ap_wlan_hal_nl80211(std::string iface_name, hal_event_cb_t callback,
                                         hal_conf_t hal_conf)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_nl80211(bwl::HALType::AccessPoint, iface_name, callback, BUFFER_SIZE)
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

bool ap_wlan_hal_nl80211::enable()
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::disable()
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::set_start_disabled(bool enable, int vap_id)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::set_channel(int chan, int bw, int center_channel)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

bool ap_wlan_hal_nl80211::sta_allow(const std::string &mac, const std::string &bssid)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", bssid: " << bssid;

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

bool ap_wlan_hal_nl80211::sta_deny(const std::string &mac, const std::string &bssid)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", bssid: " << bssid;

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
        "BSS_TM_REQ " +
        mac
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

bool ap_wlan_hal_nl80211::update_vap_credentials(
    std::list<son::wireless_utils::sBssInfoConf> &bss_info_conf_list)
{
    //TODO Implement #346
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
    const std::string &vap_name, const std::string &client_mac, uint8_t reject_error_code,
    uint8_t probe_snr_threshold_hi, uint8_t probe_snr_threshold_lo,
    uint8_t authetication_snr_threshold_hi, uint8_t authetication_snr_threshold_lo)
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

    std::string reply_str(reply);
    if (reply_str.find("UNSPECIFIED", 0, 11) != std::string::npos) {
        chan = -1;
        bw   = -1;
    } else if (reply_str.find("ACS", 0, 3) != std::string::npos) {
        chan = bw = 0;

    } else {
        int freq;
        std::string tmp;
        std::stringstream ss(reply_str);
        // parsing string in form: "%d %*s %*s bandwidth=%d"
        ss >> freq >> tmp >> tmp >> tmp;
        auto tmp_vec = beerocks::string_utils::str_split(tmp, '=');
        if (tmp_vec.size() != 2 || tmp_vec[0] != std::string("bandwidth")) {
            return false;
        }
        bw   = beerocks::string_utils::stoi(tmp_vec[1]);
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
    // See: https://github.com/prplfoundation/prplMesh/issues/457

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

bool ap_wlan_hal_nl80211::read_supported_channels()
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
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

bool ap_wlan_hal_nl80211::generate_connected_clients_events()
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return true;
}

std::string ap_wlan_hal_nl80211::get_radio_driver_version() { return "nl80211"; }

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
        msg->params.mac    = beerocks::net::network_utils::mac_from_string(parsed_obj["_mac"]);

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
        msg->params.mac    = beerocks::net::network_utils::mac_from_string(parsed_obj["_mac"]);

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
        msg->params.mac         = beerocks::net::network_utils::mac_from_string(parsed_obj["_mac"]);
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

std::shared_ptr<ap_wlan_hal> ap_wlan_hal_create(std::string iface_name, hal_conf_t hal_conf,
                                                base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<nl80211::ap_wlan_hal_nl80211>(iface_name, callback, hal_conf);
}

} // namespace bwl
