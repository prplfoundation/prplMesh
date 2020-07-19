/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_dummy.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

namespace bwl {
namespace dummy {

sta_wlan_hal_dummy::sta_wlan_hal_dummy(const std::string &iface_name, hal_event_cb_t callback,
                                       const bwl::hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::Station, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_dummy(bwl::HALType::Station, iface_name, callback, hal_conf)
{
}

sta_wlan_hal_dummy::~sta_wlan_hal_dummy() { sta_wlan_hal_dummy::detach(); }

bool sta_wlan_hal_dummy::start_wps_pbc() { return true; }

bool sta_wlan_hal_dummy::detach() { return true; }

bool sta_wlan_hal_dummy::initiate_scan() { return true; }

int sta_wlan_hal_dummy::get_scan_results(const std::string &ssid, std::vector<SScanResult> &list,
                                         bool parse_vsie)
{
    return 0;
}

bool sta_wlan_hal_dummy::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                                 bool mem_only_psk, const std::string &bssid, uint8_t channel,
                                 bool hidden_ssid)
{
    return true;
}

bool sta_wlan_hal_dummy::disconnect() { return true; }

bool sta_wlan_hal_dummy::roam(const sMacAddr &bssid, uint8_t channel) { return true; }

bool sta_wlan_hal_dummy::get_4addr_mode() { return true; }

bool sta_wlan_hal_dummy::set_4addr_mode(bool enable) { return true; }

bool sta_wlan_hal_dummy::unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                  int vht_center_frequency, int delay,
                                                  int window_size)
{
    return true;
}

bool sta_wlan_hal_dummy::is_connected() { return true; }

int sta_wlan_hal_dummy::get_channel() { return m_active_channel; }

std::string sta_wlan_hal_dummy::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_dummy::get_bssid() { return m_active_bssid; }

bool sta_wlan_hal_dummy::process_dummy_data(parsed_obj_map_t &parsed_obj) { return true; }

bool sta_wlan_hal_dummy::process_dummy_event(parsed_obj_map_t &parsed_obj) { return true; }

bool sta_wlan_hal_dummy::update_status()
{
    m_active_bssid   = "00:00:00:00:00:00";
    m_active_channel = 6;
    m_active_ssid    = "test";
    return true;
}

} // namespace dummy

std::shared_ptr<sta_wlan_hal> sta_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback,
                                                  const bwl::hal_conf_t &hal_conf)
{
    return std::make_shared<dummy::sta_wlan_hal_dummy>(iface_name, callback, hal_conf);
}

} // namespace bwl
