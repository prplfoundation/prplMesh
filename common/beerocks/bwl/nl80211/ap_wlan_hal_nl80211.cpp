/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
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

#include <linux/nl80211.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>

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
    } else if (opcode == "CTRL-EVENT-CHANNEL-SWITCH") {
        return ap_wlan_hal::Event::CTRL_Channel_Switch;
    } else if (opcode == "BSS-TM-RESP") {
        return ap_wlan_hal::Event::BSS_TM_Response;
    } else if (opcode == "DFS-CAC-COMPLETED") {
        return ap_wlan_hal::Event::DFS_CAC_Completed;
    } else if (opcode == "DFS-NOP-FINISHED") {
        return ap_wlan_hal::Event::DFS_NOP_Finished;
    }

    return ap_wlan_hal::Event::Invalid;
}

static uint8_t wpa_bw_to_beerocks_bw(const std::string &chan_width)
{
    // 20 MHz (no HT)
    // 20 MHz
    // 40 MHz
    // 80 MHz
    // 80+80 MHz
    // 160 MHz

    return (chan_width == "80+80") ? 160 : beerocks::string_utils::stoi(chan_width);
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

    // CHAN_SWITCH cs_count freq [center_freq1] [center_freq2] [bandwidth] [sec_channel_offset]
    //             [ht] [vht] [blocktx]
    // cs_count - CSA_BCN_COUNT, beacon count before switch. 5 is default value in
    //            hostapd_dfs_start_channel_switch()
    std::string cmd = "CHAN_SWITCH 5 ";

    if (chan == 0) {
        LOG(ERROR) << "ACS is not supported";
        return false;
    }

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

    cmd += " bandwidth=" +
           std::to_string(beerocks::utils::convert_bandwidth_to_int((beerocks::eWiFiBandwidth)bw));

    // Supported Standard n/ac
    if (bw == beerocks::BANDWIDTH_20 || bw == beerocks::BANDWIDTH_40) {
        cmd += " ht"; //n
    } else if (bw == beerocks::BANDWIDTH_80 || bw == beerocks::BANDWIDTH_160) {
        cmd += " vht"; // ac
    }

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "wpa_ctrl_send_msg() failed!";
        return false;
    }

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
        chan = son::wireless_utils::freq_to_channel(freq);
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

    return read_supported_channels();
}

// based on print_channels_handler() iw/phy.c
bool ap_wlan_hal_nl80211::read_supported_channels()
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

bool ap_wlan_hal_nl80211::set_tx_power_limit(int tx_pow_limit)
{
    return m_nl80211_client->set_tx_power_limit(m_radio_info.iface_name, tx_pow_limit);
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

    case Event::CTRL_Channel_Switch: {
        std::string bandwidth = parsed_obj["ch_width"];
        if (bandwidth.empty()) {
            LOG(ERROR) << "Invalid bandwidth";
            return false;
        }
        m_radio_info.channel =
            son::wireless_utils::freq_to_channel(beerocks::string_utils::stoi(parsed_obj["freq"]));
        m_radio_info.bandwidth          = wpa_bw_to_beerocks_bw(bandwidth);
        m_radio_info.channel_ext_above  = beerocks::string_utils::stoi(parsed_obj["ch_offset"]);
        m_radio_info.vht_center_freq    = beerocks::string_utils::stoi(parsed_obj["cf1"]);
        m_radio_info.is_dfs_channel     = beerocks::string_utils::stoi(parsed_obj["dfs"]);
        m_radio_info.last_csa_sw_reason = ChanSwReason::Unknown;
        if (son::wireless_utils::which_freq(m_radio_info.channel) == beerocks::eFreqType::FREQ_5G) {
            m_radio_info.is_5ghz = true;
        }
    } break;
    // ACS/CSA Completed
    case Event::ACS_Completed:
    case Event::CSA_Finished:
        event_queue_push(event);
        break;
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
