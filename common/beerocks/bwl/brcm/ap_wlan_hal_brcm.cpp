/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_brcm.h"

#include <network/network_utils.h>

// TODO: TEMP - Remove after converting the message structs to HAL objects
#include <beerocks_message_apmanager.h>

#include <easylogging/easylogging++.h>

// BRCM utils
#define _NET_ETHERNET_H_ // Fix symbol redifinition errors
extern "C" {
#include "utils/bwl_utils.h"
#include <wlioctl.h>
}

using namespace beerocks;

namespace bwl {
namespace brcm {

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_brcm* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_brcm::ap_wlan_hal_brcm(std::string iface_name, bool acs_enabled,
                                   hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Broadcom, acs_enabled,
                    callback),
      base_wlan_hal_brcm(bwl::HALType::AccessPoint, iface_name, acs_enabled, IfaceType::Broadcom,
                         callback)
{
}

ap_wlan_hal_brcm::~ap_wlan_hal_brcm() {}

HALState ap_wlan_hal_brcm::attach(bool block)
{
    // Clear the MAC list
    sta_allow(std::string());

    // Set MAC mode to Blacklist
    // 0 - Disable MAC address matching.
    // 1 - Deny association to stations on the MAC list.
    // 2 - Allow association to stations on the MAC list.

    int mac_mode = 1;
    int ret      = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_SET_MACMODE, &mac_mode,
                           sizeof(mac_mode));
    if (ret < 0) {
        LOG(ERROR) << "WLC_SET_MACMODE failed: " << ret;
        return (m_hal_state = HALState::Failed);
    }

    // Event processing is synchronous
    m_fd_ext_events = 0;

    // update radio info
    refresh_radio_info();

    // On Operational send the AP_Attached event to the AP Manager
    event_queue_push(Event::AP_Attached);

    return (m_hal_state = HALState::Operational);
}

bool ap_wlan_hal_brcm::detach()
{
    // Disable MAC filtering
    // 0 - Disable MAC address matching.
    // 1 - Deny association to stations on the MAC list.
    // 2 - Allow association to stations on the MAC list.

    int mac_mode = 0;
    int ret      = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_SET_MACMODE, &mac_mode,
                           sizeof(mac_mode));
    if (ret < 0) {
        LOG(ERROR) << "WLC_SET_MACMODE failed: " << ret;
        return false;
    }
    return true;
}

bool ap_wlan_hal_brcm::sta_allow(const std::string &mac)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    struct maclist *maclist = (struct maclist *)wl_ioctl_buf;
    uint max                = (WLC_IOCTL_MAXLEN - sizeof(int)) / ETHER_ADDR_LEN;

    // Clear the list
    if (mac.empty()) {
        LOG(DEBUG) << "Clearing MAC list...";

        maclist->count = 0;
        int ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_SET_MACLIST, maclist,
                               sizeof(int));
        if (ret < 0) {
            LOG(ERROR) << "WLC_SET_MACLIST failed: " << ret;
            return false;
        }

        return true;
    }

    // Read the existing mac list
    maclist->count = max;
    int ret        = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_MACLIST, maclist,
                           sizeof(wl_ioctl_buf));
    if (ret < 0) {
        LOG(ERROR) << "WLC_GET_MACLIST failed: " << ret;
        return false;
    }

    if (maclist->count > max) {
        LOG(ERROR) << "Invalid MAC list: " << int(maclist->count);
        return false;
    }

    // Find the requested MAC in the list
    struct ether_addr sta_mac_raw;
    network_utils::mac_from_string((char *)&sta_mac_raw, mac);

    for (uint i = 0; i < maclist->count; i++) {
        if (!memcmp(&sta_mac_raw, &maclist->ea[i], ETHER_ADDR_LEN)) {
            LOG(DEBUG) << "Removing MAC " << mac << " from blacklist";

            // Last (or only) item
            if (i == maclist->count - 1) {
                memset(&maclist->ea[i], 0, ETHER_ADDR_LEN);
            } else {
                memmove(&maclist->ea[i], &maclist->ea[i + 1],
                        (ETHER_ADDR_LEN * (maclist->count - i)));
            }

            // Decrement the number of MACs in the list
            maclist->count--;

            // Update the MAC list
            ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_SET_MACLIST, maclist,
                               sizeof(maclist->count) + maclist->count * sizeof(maclist->ea));

            if (ret < 0) {
                LOG(ERROR) << "WLC_SET_MACLIST failed: " << ret;
                return false;
            }

            break;
        }
    }
    return true;
}

bool ap_wlan_hal_brcm::sta_deny(const std::string &mac, int reject_sta)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    struct maclist *maclist = (struct maclist *)wl_ioctl_buf;
    uint max                = (WLC_IOCTL_MAXLEN - sizeof(int)) / ETHER_ADDR_LEN;

    if (mac.empty()) {
        LOG(WARNING) << "Empty MAC addresses not supported";
        return false;
    }

    // Read the existing mac list
    maclist->count = max;
    int ret        = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_MACLIST, maclist,
                           sizeof(wl_ioctl_buf));
    if (ret < 0) {
        LOG(ERROR) << "WLC_GET_MACLIST failed: " << ret;
        return false;
    }

    if (maclist->count > max) {
        LOG(ERROR) << "Invalid MAC list: " << int(maclist->count);
        return false;
    }

    // Check if the given MAC is not already in the list
    struct ether_addr sta_mac_raw;
    network_utils::mac_from_string((char *)&sta_mac_raw, mac);

    for (uint i = 0; i < maclist->count; i++) {
        if (!memcmp(&sta_mac_raw, &maclist->ea[i], ETHER_ADDR_LEN)) {
            LOG(DEBUG) << "MAC " << mac << " already in the list";
            return true;
        }
    }

    LOG(DEBUG) << "Adding MAC " << mac << " to blacklist";

    // MAC not found --> Add it to the end of the list
    memcpy(&maclist->ea[maclist->count], &sta_mac_raw, ETHER_ADDR_LEN);
    maclist->count++;

    ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_SET_MACLIST, maclist,
                       sizeof(maclist->count) + maclist->count * sizeof(maclist->ea));

    if (ret < 0) {
        LOG(ERROR) << "WLC_SET_MACLIST failed: " << ret;
        return false;
    }

    return true;
}

bool ap_wlan_hal_brcm::sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    scb_val_t scb_val;
    network_utils::mac_from_string((char *)&scb_val.ea, mac);

    // Disassociated because AP is unable to handle all currently associated stations
    scb_val.val = 5;

    int ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_SCB_DEAUTHENTICATE,
                           &scb_val.ea, ETHER_ADDR_LEN);
    if (ret < 0) {
        LOG(ERROR) << "WLC_SCB_DEAUTHENTICATE failed: " << ret;
        return false;
    }

    return true;
}

bool ap_wlan_hal_brcm::sta_bss_steer(const std::string &mac, const std::string &bssid, int chan,
                                     int disassoc_timer, int valid_int)
{
    LOG(TRACE) << __func__ << " mac: " << mac;

    // Since there's currently no support for 802.11v, simply disassociate the device
    return sta_disassoc(beerocks::IFACE_VAP_ID_MIN, mac);
}

bool ap_wlan_hal_brcm::sta_unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                    int vht_center_frequency, int delay,
                                                    int window_size)
{
    return true;
}

bool ap_wlan_hal_brcm::switch_channel(int chan, int bw, int vht_center_frequency) { return true; }

bool ap_wlan_hal_brcm::set_antenna_mode(AntMode mode) { return true; }

bool ap_wlan_hal_brcm::wds_set_mode(WDSMode mode) { return true; }

bool ap_wlan_hal_brcm::wds_add_sta(const std::string &mac) { return true; }

bool ap_wlan_hal_brcm::wds_del_sta(const std::string &mac) { return true; }

bool ap_wlan_hal_brcm::wds_clear_list() { return true; }

bool ap_wlan_hal_brcm::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    return true;
}

bool ap_wlan_hal_brcm::failsafe_channel_get(int &chan, int &bw) { return true; }

bool ap_wlan_hal_brcm::restricted_channels_set(char *channel_list) { return true; }

bool ap_wlan_hal_brcm::restricted_channels_get(char *channel_list) { return true; }

bool ap_wlan_hal_brcm::read_acs_report()
{
    LOG(TRACE) << __func__ << " for interface: " << get_radio_info().iface_name;

    uint32 chan_buf[32 + 1];
    wl_uint32_list_t *list = (wl_uint32_list_t *)chan_buf;

    int ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_VALID_CHANNELS,
                           chan_buf, sizeof(chan_buf));

    if (ret < 0) {
        LOG(ERROR) << "wl_ioctl error: " << ret;
        return false;
    }

    if (!list->count) {
        LOG(DEBUG) << "NO CHANNELS AVAILABLE!";
    } else {
        for (int channel_idx = 0; channel_idx < int(list->count); channel_idx++) {
            auto channel = list->element[channel_idx];
            LOG(DEBUG) << "Supported Channel: " << int(channel);

            if (channel_idx < beerocks::message::SUPPORTED_CHANNELS_LENGTH) {
                if (channel > 14)
                    m_radio_info.is_5ghz = 1;
                m_radio_info.supported_channels[channel_idx].channel     = channel;
                m_radio_info.supported_channels[channel_idx].noise       = 0;
                m_radio_info.supported_channels[channel_idx].bss_overlap = 0;
                m_radio_info.supported_channels[channel_idx].is_dfs      = 0;
                m_radio_info.supported_channels[channel_idx].bandwidth   = m_radio_info.bandwidth;
                m_radio_info.supported_channels[channel_idx].tx_pow      = 0;
            }
        }
    }

    return true;
}

std::string ap_wlan_hal_brcm::get_radio_driver_version() { return "brcm"; }

bool ap_wlan_hal_brcm::set_vap_enable(const std::string &iface_name, const bool enable)
{
    return true;
}

bool ap_wlan_hal_brcm::get_vap_enable(const std::string &iface_name, bool &enable) { return true; }

bool ap_wlan_hal_brcm::process_ext_events()
{
    if (m_radio_info.iface_name.empty()) {
        return true;
    }

    // Read the list of associated clients
    struct maclist *maclist = (struct maclist *)wl_ioctl_buf;
    int max                 = (WLC_IOCTL_MAXLEN - sizeof(int)) / ETHER_ADDR_LEN;

    maclist->count = max;
    int ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_ASSOCLIST, maclist,
                           sizeof(wl_ioctl_buf));
    // int ret = wl_get((char*)m_radio_info.iface_name.c_str(), WLC_GET_ASSOCLIST, maclist, sizeof(wl_ioctl_buf));
    if (ret < 0) {
        LOG(ERROR) << "WLC_GET_ASSOCLIST failed: " << ret;
        return false;
    }

    auto now = std::chrono::steady_clock::now();

    struct ether_addr *ea = (ether_addr *)maclist->ea;
    for (int i = 0; i < int(maclist->count) && i < max; i++, ea++) {

        std::string mac = network_utils::mac_to_string((const uint8_t *)ea);

        // Check if a new station
        if (m_assoc_list.find(mac) == m_assoc_list.end()) {

            LOG(DEBUG) << "STA Connected: " << mac;

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(
                sizeof(beerocks::message::sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
            auto msg = reinterpret_cast<
                beerocks::message::sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(
                msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";

            // Initialize the message
            memset(msg_buff.get(), 0,
                   sizeof(beerocks::message::sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));

            msg->params.vap_id = beerocks::IFACE_VAP_ID_MIN;
            memcpy(msg->params.mac.oct, ea, ETHER_ADDR_LEN);

            // Add the message to the queue
            event_queue_push(Event::STA_Connected, msg_buff);
        }

        // Update the last-seen timestamp
        m_assoc_list[mac] = now;
    }

    // Check for disconnected clients
    for (auto sta = m_assoc_list.begin(); sta != m_assoc_list.end();) {

        if (sta->second < now) {

            LOG(DEBUG) << "STA Disconnected: " << sta->first;

            auto msg_buff = ALLOC_SMART_BUFFER(
                sizeof(beerocks::message::sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));
            auto msg = reinterpret_cast<
                beerocks::message::sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION *>(
                msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";

            // Initialize the message
            memset(msg_buff.get(), 0,
                   sizeof(beerocks::message::sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));

            // Store the MAC address of the disconnected STA
            msg->params.mac = network_utils::mac_from_string(sta->first);

            // Add the message to the queue
            event_queue_push(Event::STA_Disconnected, msg_buff);

            // Delete the STA from the map
            sta = m_assoc_list.erase(sta);

        } else {
            ++sta;
        }
    }

    return true;
}

} // namespace brcm
} // namespace bwl

// AP BRCM HAL Factory Functions
extern "C" {

bwl::ap_wlan_hal *ap_wlan_hal_create(std::string iface_name, bool acs_enabled,
                                     bwl::base_wlan_hal::hal_event_cb_t callback)
{
    return new bwl::brcm::ap_wlan_hal_brcm(iface_name, acs_enabled, callback);
}

void ap_wlan_hal_destroy(bwl::ap_wlan_hal *obj) { delete obj; }
}
