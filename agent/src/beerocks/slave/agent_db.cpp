/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "agent_db.h"

#include <easylogging++.h>

namespace beerocks {

AgentDB::sRadio *AgentDB::radio(const std::string &iface_name)
{
    if (iface_name.empty()) {
        LOG(ERROR) << "Given radio iface name is empty";
        return nullptr;
    }

    auto radio_it = std::find_if(m_radios.begin(), m_radios.end(), [&](const sRadio &radio_entry) {
        return iface_name == radio_entry.front.iface_name ||
               iface_name == radio_entry.back.iface_name;
    });

    return radio_it == m_radios.end() ? nullptr : &(*radio_it);
}

bool AgentDB::add_radio(const std::string &front_iface_name, const std::string &back_iface_name)
{
    if (front_iface_name.empty() && back_iface_name.empty()) {
        LOG(ERROR) << "Both front and back interface names are empty!";
        return false;
    }

    if (!front_iface_name.empty() && radio(front_iface_name)) {
        LOG(DEBUG) << "Radio entry of front iface " << front_iface_name
                   << " already exists. Ignore.";
        return true;
    }

    if (!back_iface_name.empty() && radio(back_iface_name)) {
        LOG(DEBUG) << "Radio entry of back iface " << back_iface_name << " already exists. Ignore.";
        return true;
    }

    m_radios.emplace_back(front_iface_name, back_iface_name);
    m_radios_list.push_back(&m_radios.back());

    return true;
}

AgentDB::sRadio *AgentDB::get_radio_by_mac(const sMacAddr &mac, eMacType mac_type_hint)
{
    bool all_mac_types = mac_type_hint == eMacType::ALL;
    auto radio_it = std::find_if(m_radios.begin(), m_radios.end(), [&](const sRadio &radio_entry) {
        if (all_mac_types || mac_type_hint == eMacType::RADIO) {
            if (radio_entry.front.iface_mac == mac || radio_entry.back.iface_mac == mac) {
                return true;
            }
        }
        if (all_mac_types || mac_type_hint == eMacType::BSSID) {
            auto &bssid_list = radio_entry.front.bssids;
            auto bssid_it =
                std::find_if(bssid_list.begin(), bssid_list.end(),
                             [&](const sRadio::sFront::sBssid &bssid) { return bssid.mac == mac; });
            if (bssid_it != bssid_list.end()) {
                return true;
            }
        }
        if (all_mac_types || mac_type_hint == eMacType::CLIENT) {
            auto client_it = radio_entry.associated_clients.find(mac);
            return client_it != radio_entry.associated_clients.end();
        }
        // MAC is not one of the front\back radio MACs nor bssid MAC.
        return false;
    });

    return radio_it == m_radios.end() ? nullptr : &(*radio_it);
}

void AgentDB::erase_client(const sMacAddr &client_mac, sMacAddr bssid)
{
    if (bssid != net::network_utils::ZERO_MAC) {
        auto radio = get_radio_by_mac(bssid, eMacType::BSSID);
        if (!radio) {
            return;
        }
        radio->associated_clients.erase(client_mac);
        return;
    }

    for (auto &radio : m_radios) {
        radio.associated_clients.erase(client_mac);
    }
}

bool AgentDB::get_mac_by_ssid(const sMacAddr &ruid, const std::string &ssid, sMacAddr &value)
{
    value      = net::network_utils::ZERO_MAC;
    auto radio = get_radio_by_mac(ruid, AgentDB::eMacType::RADIO);
    if (!radio) {
        LOG(ERROR) << "No radio with ruid '" << ruid << "' found!";
        return false;
    }

    for (const auto &bssid : radio->front.bssids) {
        if (bssid.ssid == ssid) {
            value = bssid.mac;
            return true;
        }
    }
    return false;
}

bool AgentDB::get_bsta_mac_by_ruid(const sMacAddr &ruid, sMacAddr &value)
{
    value      = net::network_utils::ZERO_MAC;
    auto radio = get_radio_by_mac(ruid, AgentDB::eMacType::RADIO);
    if (!radio) {
        LOG(ERROR) << "No radio with ruid '" << ruid << "' found!";
        return false;
    }

    value = radio->back.iface_mac;
    return true;
}

} // namespace beerocks
