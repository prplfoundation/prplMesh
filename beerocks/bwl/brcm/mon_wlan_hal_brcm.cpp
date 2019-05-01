/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_brcm.h"

#include <network/network_utils.h>
#include <easylogging/easylogging++.h>

#include <cmath>

// BRCM utils
#define _NET_ETHERNET_H_ // Fix symbol redifinition errors
extern "C" {
#include <wlioctl.h>
#include "utils/bwl_utils.h"
}

namespace bwl {
namespace brcm {

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


static void calc_curr_traffic(uint32_t& val, uint64_t& total, uint32_t& curr)
{
    if (val >= total) {
        curr = val - total;
    } else {
        curr = val;
    }
    total = val;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_brcm::mon_wlan_hal_brcm(
    std::string iface_name, hal_event_cb_t callback) :
    base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Broadcom, false, callback),
    base_wlan_hal_brcm(bwl::HALType::Monitor, iface_name, false, IfaceType::Broadcom, callback)
{
}

mon_wlan_hal_brcm::~mon_wlan_hal_brcm()
{
}

HALState mon_wlan_hal_brcm::attach(bool block)
{
    // Event processing is synchronous
    m_fd_ext_events = 0;

    // update radio info
    refresh_radio_info();
    
    return (m_hal_state = HALState::Operational);;
}

bool mon_wlan_hal_brcm::detach()
{
    return true;
}

bool mon_wlan_hal_brcm::process_ext_events()
{
    return true;
}

bool mon_wlan_hal_brcm::update_radio_stats(SRadioStats& radio_stats)
{
    // LOG(DEBUG) << "update_radio_stats()";
    return true;
}

bool mon_wlan_hal_brcm::update_vap_stats(const std::string vap_iface_name, SVapStats& vap_stats)
{
    // LOG(DEBUG) << "update_vap_stats()";
    return true;
}

bool mon_wlan_hal_brcm::update_stations_stats(const std::string vap_iface_name, const std::string sta_mac, SStaStats& sta_stats)
{
    int ret;
    
    // Read STA RSSI
    scb_val_t scb_val;
    network_utils::mac_from_string((char*)&scb_val.ea, sta_mac);
    ret = bwl_wl_ioctl((char*)vap_iface_name.c_str(), WLC_GET_RSSI, &scb_val, sizeof(scb_val));
    if (ret < 0) {
        LOG(ERROR) << "WLC_GET_RSSI failed: " << ret;
        return false;
    }

    sta_stats.rx_rssi_watt = pow(10, (int8_t(scb_val.val) / 10.0));

    // Read STA stats
    strcpy(wl_ioctl_buf, "sta_info");
    int buflen = strlen(wl_ioctl_buf) + 1;
    network_utils::mac_from_string(wl_ioctl_buf + buflen, sta_mac);
    ret = bwl_wl_ioctl((char*)vap_iface_name.c_str(), WLC_GET_VAR, wl_ioctl_buf, sizeof(wl_ioctl_buf));
    if (ret < 0) {
        LOG(ERROR) << "WLC_GET_VAR(sta_info) failed: " << ret;
        return false;
    }

    sta_info_t* sta_info = (sta_info_t*)wl_ioctl_buf;

    // TX/RX Rates
    sta_stats.tx_phy_rate_100kb = (uint16_t)(sta_info->tx_rate / 100.0);
    sta_stats.rx_phy_rate_100kb = (uint16_t)(sta_info->tx_rate / 100.0);

    // TX/RX Packets
    uint32_t rx_packets = sta_info->rx_ucast_pkts + sta_info->rx_mcast_pkts;
    calc_curr_traffic(rx_packets, sta_stats.rx_packets_cnt, sta_stats.rx_packets);

    calc_curr_traffic(sta_info->tx_pkts, sta_stats.tx_packets_cnt, sta_stats.tx_packets);

    return true;
}

bool mon_wlan_hal_brcm::sta_channel_load_11k_request(const SStaChannelLoadRequest11k& req)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_brcm::sta_beacon_11k_request(const SBeaconRequest11k& req, int& dialog_token)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_brcm::sta_statistics_11k_request(const SStatisticsRequest11k& req)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_brcm::sta_link_measurements_11k_request(const std::string& sta_mac)
{
    LOG(TRACE) << __func__;
    return true;
}

} // namespace brcm
} // namespace bwl

// AP BRCM HAL Factory Functions
extern "C" {

bwl::mon_wlan_hal* mon_wlan_hal_create(
    std::string iface_name, bwl::base_wlan_hal::hal_event_cb_t callback)
{ return new bwl::brcm::mon_wlan_hal_brcm(iface_name, callback); }

void mon_wlan_hal_destroy(bwl::mon_wlan_hal* obj)
{ delete obj; }

}
