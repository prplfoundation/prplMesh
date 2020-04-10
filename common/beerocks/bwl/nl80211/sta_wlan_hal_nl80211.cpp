/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_nl80211.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

namespace bwl {
namespace nl80211 {

sta_wlan_hal_nl80211::sta_wlan_hal_nl80211(std::string iface_name, hal_event_cb_t callback)
    : base_wlan_hal(), base_wlan_hal_nl80211(bwl::HALType::Station, iface_name, callback, {})
{
}

sta_wlan_hal_nl80211::~sta_wlan_hal_nl80211() { sta_wlan_hal_nl80211::detach(); }

bool sta_wlan_hal_nl80211::detach() { return true; }

bool sta_wlan_hal_nl80211::initiate_scan() { return true; }

int sta_wlan_hal_nl80211::get_scan_results(const std::string &ssid, std::vector<SScanResult> &list,
                                           bool parse_vsie)
{
    return 0;
}

bool sta_wlan_hal_nl80211::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                                   bool mem_only_psk, const std::string &bssid, uint8_t channel,
                                   bool hidden_ssid)
{
    return true;
}

bool sta_wlan_hal_nl80211::disconnect() { return true; }

bool sta_wlan_hal_nl80211::roam(const std::string &bssid, uint8_t channel) { return true; }

bool sta_wlan_hal_nl80211::get_4addr_mode() { return true; }

bool sta_wlan_hal_nl80211::set_4addr_mode(bool enable) { return true; }

bool sta_wlan_hal_nl80211::unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                    int vht_center_frequency, int delay,
                                                    int window_size)
{
    return true;
}

bool sta_wlan_hal_nl80211::is_connected() { return true; }

int sta_wlan_hal_nl80211::get_channel() { return m_active_channel; }

std::string sta_wlan_hal_nl80211::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_nl80211::get_bssid() { return m_active_bssid; }

bool sta_wlan_hal_nl80211::process_nl80211_event(parsed_obj_map_t &parsed_obj) { return true; }

bool sta_wlan_hal_nl80211::update_status() { return false; }

} // namespace nl80211

std::shared_ptr<sta_wlan_hal> sta_wlan_hal_create(std::string iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<nl80211::sta_wlan_hal_nl80211>(iface_name, callback);
}

} // namespace bwl
