/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "db.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <algorithm>

using namespace beerocks;
using namespace son;
using namespace net;

void db::set_log_level_state(const beerocks::eLogLevel &log_level, const bool &new_state)
{
    logger.set_log_level_state(log_level, new_state);
}

// General set/get

bool db::has_node(sMacAddr mac)
{
    auto n = get_node(mac);
    return (n != nullptr);
}

bool db::add_virtual_node(sMacAddr mac, sMacAddr real_node_mac)
{
    //TODO prototype code, untested
    if (mac == network_utils::ZERO_MAC) {
        LOG(ERROR) << "can't insert node with empty mac";
        return false;
    }

    auto real_node = get_node(real_node_mac);

    if (!real_node) {
        LOG(ERROR) << "node " << network_utils::mac_to_string(real_node_mac) << " does not exist";
        return false;
    }

    /*
     * TODO 
     * the regular add_node() function should take care of a situation where the real node
     * already exists and is moved to a different hierarchy
     * it should be able to find its virtual nodes and move them to the appropriate hierarchy as well
     */

    nodes[real_node->hierarchy].insert(
        std::make_pair(network_utils::mac_to_string(mac), real_node));
    return true;
}

bool db::add_node(const sMacAddr &mac, const sMacAddr &parent_mac, beerocks::eType type,
                  const sMacAddr &radio_identifier)
{
    if (mac == network_utils::ZERO_MAC) {
        LOG(ERROR) << "can't insert node with empty mac";
        return false;
    }

    auto parent_node = get_node(parent_mac);
    // if parent node does not exist, new_hierarchy will be equal to 0
    int new_hierarchy = get_node_hierarchy(parent_node) + 1;
    if (new_hierarchy >= HIERARCHY_MAX) {
        LOG(ERROR) << "hierarchy too high for node " << network_utils::mac_to_string(mac);
        return false;
    }

    auto n = get_node(mac);
    if (n) { // n is not nullptr
        LOG(DEBUG) << "node with mac " << network_utils::mac_to_string(mac)
                   << " already exists, updating";
        n->set_type(type);
        if (n->parent_mac != network_utils::mac_to_string(parent_mac)) {
            n->previous_parent_mac = n->parent_mac;
            n->parent_mac          = network_utils::mac_to_string(parent_mac);
        }
        int old_hierarchy = get_node_hierarchy(n);
        if (old_hierarchy >= 0 && old_hierarchy < HIERARCHY_MAX) {
            nodes[old_hierarchy].erase(network_utils::mac_to_string(mac));
        } else {
            LOG(ERROR) << "old hierarchy " << old_hierarchy << " for node "
                       << network_utils::mac_to_string(mac) << " is invalid!!!";
        }
        auto subtree = get_node_subtree(n);
        int offset   = new_hierarchy - old_hierarchy;
        adjust_subtree_hierarchy(subtree, offset);
    } else {
        LOG(DEBUG) << "node with mac " << network_utils::mac_to_string(mac)
                   << " being created, the type is " << type;
        n             = std::make_shared<node>(type, network_utils::mac_to_string(mac));
        n->parent_mac = network_utils::mac_to_string(parent_mac);
    }
    n->radio_identifier = network_utils::mac_to_string(radio_identifier);
    n->hierarchy        = new_hierarchy;
    nodes[new_hierarchy].insert(std::make_pair(network_utils::mac_to_string(mac), n));

    if (radio_identifier != network_utils::ZERO_MAC) {
        std::string ruid_key =
            get_node_key(network_utils::mac_to_string(parent_mac), n->radio_identifier);
        if (ruid_key.empty()) {
            LOG(ERROR) << "can't insert node with empty RUID";
            return false;
        }
        // if already exists set instead of insert
        if (get_node(ruid_key)) {
            nodes[new_hierarchy].erase(ruid_key);
        }
        nodes[new_hierarchy].insert(std::make_pair(ruid_key, n));
    }

    return true;
}

bool db::remove_node(const sMacAddr &mac)
{
    int i;
    for (i = 0; i < HIERARCHY_MAX; i++) {
        auto it = nodes[i].find(network_utils::mac_to_string(mac));
        if (it != nodes[i].end()) {
            std::string ruid_key =
                get_node_key(it->second->parent_mac, it->second->radio_identifier);
            std::string node_mac = it->second->mac;

            if (last_accessed_node_mac == network_utils::mac_to_string(mac)) {
                last_accessed_node_mac = std::string();
                last_accessed_node     = nullptr;
            }

            // map may include 2 keys to same node - if so remove other key-node pair from map
            // if removed by mac
            if (network_utils::mac_to_string(mac) == node_mac) {
                nodes[i].erase(it);
                // if ruid_key exists for this node
                if (!ruid_key.empty()) {
                    nodes[i].erase(ruid_key);
                }
                // if removed by ruid_key
            } else if (network_utils::mac_to_string(mac) == ruid_key) {
                nodes[i].erase(node_mac);
            }

            return true;
        }
    }

    return false;
}

bool db::set_node_type(std::string mac, beerocks::eType type)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->set_type(type);
    return true;
}

beerocks::eType db::get_node_type(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return beerocks::TYPE_UNDEFINED;
    }
    return n->get_type();
}

bool db::set_local_slave_mac(std::string mac)
{
    if (!local_slave_mac.empty()) {
        LOG(WARNING) << "local_slave_mac != empty";
    }
    local_slave_mac = mac;
    return true;
}

std::string db::get_local_slave_mac() { return local_slave_mac; }

bool db::set_node_ipv4(std::string mac, std::string ipv4)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->ipv4 = ipv4;
    return true;
}

std::string db::get_node_ipv4(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::string();
    }
    return n->ipv4;
}

bool db::set_node_manufacturer(std::string mac, std::string manufacturer)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->manufacturer = manufacturer;
    return true;
}

std::string db::get_node_manufacturer(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::string();
    }
    return n->manufacturer;
}

int db::get_node_channel(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return 0;
    }
    return n->channel;
}

bool db::set_node_vap_id(std::string mac, int8_t vap_id)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->vap_id = vap_id;
    return true;
}

int8_t db::get_node_vap_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return beerocks::IFACE_ID_INVALID;
    }
    return n->vap_id;
}

bool db::get_cs_op_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->cs_op;
}

bool db::set_cs_op_flag(std::string mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cs_op = flag;
    return true;
}

bool db::get_cs_lb_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->cs_lb;
}

bool db::set_cs_lb_flag(std::string mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cs_lb = flag;
    return true;
}

bool db::set_global_restricted_channels(uint8_t *restricted_channels)
{
    if (!restricted_channels) {
        return false;
    }
    global_restricted_channels.clear();
    std::copy(restricted_channels, restricted_channels + message::RESTRICTED_CHANNEL_LENGTH,
              std::back_inserter(global_restricted_channels));
    return true;
}

std::vector<uint8_t> db::get_global_restricted_channels() { return global_restricted_channels; }

bool db::set_hostap_conf_restricted_channels(std::string hostap_mac, uint8_t *restricted_channels)
{
    auto n = get_node(hostap_mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << hostap_mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << hostap_mac << " is not a valid hostap!";
        return false;
    } else if (!restricted_channels) {
        LOG(WARNING) << __FUNCTION__ << "node " << hostap_mac << " restricted_channels not valid";
        return false;
    }
    n->hostap->conf_restricted_channels.clear();
    std::copy(restricted_channels, restricted_channels + message::RESTRICTED_CHANNEL_LENGTH,
              std::back_inserter(n->hostap->conf_restricted_channels));
    for (auto elm : n->hostap->conf_restricted_channels) {
        LOG(WARNING) << __FUNCTION__ << " elm = " << int(elm);
    }
    return true;
}

std::vector<uint8_t> db::get_hostap_conf_restricted_channels(std::string hostap_mac)
{
    auto n = get_node(hostap_mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << hostap_mac << " does not exist!";
        return std::vector<uint8_t>();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << hostap_mac << " is not a valid hostap!";
        return std::vector<uint8_t>();
    }
    return n->hostap->conf_restricted_channels;
}

bool db::set_node_beacon_measurement_support_level(
    std::string mac, beerocks::eBeaconMeasurementSupportLevel support_beacon_measurement)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    if (!n->supports_beacon_measurement) { // sticky
        n->supports_beacon_measurement = support_beacon_measurement;
    }
    return true;
}

beerocks::eBeaconMeasurementSupportLevel
db::get_node_beacon_measurement_support_level(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return beerocks::BEACON_MEAS_UNSUPPORTED;
    }
    return n->supports_beacon_measurement;
}

bool db::set_node_name(std::string mac, std::string name)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->name = name;
    return true;
}

std::string db::get_node_name(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::string();
    }
    return n->name;
}

bool db::set_node_state(std::string mac, beerocks::eNodeState state)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->state             = state;
    n->last_state_change = std::chrono::steady_clock::now();
    return true;
}

beerocks::eNodeState db::get_node_state(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::STATE_MAX;
    }
    return n->state;
}

bool db::set_node_operational_state(std::string bridge_mac, bool operational)
{
    auto n = get_node(bridge_mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << bridge_mac << " does not exist!";
        return false;
    }

    if (n->get_type() != beerocks::TYPE_GW && n->get_type() != beerocks::TYPE_IRE) {
        LOG(WARNING) << __FUNCTION__ << " - node " << bridge_mac << " is not bridge type ";
        return false;
    }

    n->operational_state = operational;
    return true;
}

int8_t db::get_node_operational_state(std::string bridge_mac)
{
    auto n = get_node(bridge_mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << bridge_mac << " does not exist!";
        return -1;
    }

    if (n->get_type() != beerocks::TYPE_GW && n->get_type() != beerocks::TYPE_IRE) {
        LOG(WARNING) << __FUNCTION__ << " - node " << bridge_mac << " is not bridge type";
        return -1;
    }

    return n->operational_state;
}

std::chrono::steady_clock::time_point db::get_last_state_change(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::chrono::steady_clock::time_point();
    }
    return n->last_state_change;
}

bool db::set_node_handoff_flag(std::string mac, bool handoff)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->handoff = handoff;
    if (n->get_type() == beerocks::TYPE_IRE_BACKHAUL) {
        n->ire_handoff = handoff;
    }
    return true;
}

bool db::get_node_handoff_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }

    if (n->get_type() == beerocks::TYPE_IRE_BACKHAUL) {
        return n->ire_handoff;
    } else {
        return n->handoff;
    }
}

bool db::set_node_confined_flag(std::string mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->confined = flag;
    return true;
}

bool db::get_node_confined_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    return n->confined;
}

bool db::update_node_last_seen(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_seen = std::chrono::steady_clock::now();
    return true;
}

bool db::update_node_last_ping_sent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_ping_sent = std::chrono::steady_clock::now();
    return true;
}

std::chrono::steady_clock::time_point db::get_node_last_seen(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::chrono::steady_clock::now();
    }

    return n->last_seen;
}

std::chrono::steady_clock::time_point db::get_node_last_ping_sent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::chrono::steady_clock::now();
    }
    return n->last_ping_sent;
}

bool db::update_node_last_ping_received(std::string mac, int seq)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_ping_received = std::chrono::steady_clock::now();
    n->last_seen          = n->last_ping_received;

    n->last_ping_delta_ms =
        (double)((std::chrono::duration_cast<std::chrono::duration<double>>(
                      get_node_last_ping_received(mac) - get_node_last_ping_sent(mac)))
                     .count()) *
        1000;
    if (n->last_ping_delta_ms < n->last_ping_min_ms || (seq == 0)) {
        n->last_ping_min_ms = n->last_ping_delta_ms;
    }
    if (n->last_ping_delta_ms > n->last_ping_max_ms || (seq == 0)) {
        n->last_ping_max_ms = n->last_ping_delta_ms;
    }
    if (seq == 0) {
        n->last_ping_avg_ms_acc = n->last_ping_delta_ms;
    } else {
        n->last_ping_avg_ms_acc += n->last_ping_delta_ms;
    }

    return true;
}

bool db::update_node_last_ping_received_avg(std::string mac, int total_seq)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_ping_avg_ms = n->last_ping_avg_ms_acc / total_seq;
    return true;
}

std::chrono::steady_clock::time_point db::get_node_last_ping_received(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::chrono::steady_clock::now();
    }
    return n->last_ping_received;
}

int db::get_node_last_ping_delta_ms(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_delta_ms;
}

int db::get_node_last_ping_min_ms(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_min_ms;
}

int db::get_node_last_ping_max_ms(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_max_ms;
}

int db::get_node_last_ping_avg_ms(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_avg_ms;
}

bool db::set_hostap_active(std::string mac, bool active)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return false;
    }
    n->hostap->active = active;
    return true;
}

bool db::is_hostap_active(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    return n->hostap->active;
}

bool db::set_hostap_backhaul_manager(std::string mac, bool is_backhaul_manager)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return false;
    }
    n->hostap->is_backhaul_manager = is_backhaul_manager;
    return true;
}

bool db::is_hostap_backhaul_manager(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    return n->hostap->is_backhaul_manager;
}

std::string db::get_hostap_backhaul_manager(std::string ire)
{
    auto n = get_node(ire);
    if (!n) {
        LOG(ERROR) << "node " << ire << " does not exist!";
        return std::string();
    } else if (n->get_type() != beerocks::TYPE_IRE && n->get_type() != beerocks::TYPE_GW) {
        LOG(ERROR) << "ire " << ire << " not an IRE or GW";
        return std::string();
    }
    auto ire_hostaps = get_node_children(ire, beerocks::TYPE_SLAVE);
    for (auto &hostap : ire_hostaps) {
        if ((is_hostap_backhaul_manager(hostap)) &&
            get_node_state(hostap) == beerocks::STATE_CONNECTED) {
            return hostap;
        }
    }
    LOG(ERROR) << "ire " << ire << " return empty backhaul";
    return std::string();
}

bool db::is_ap_out_of_band(std::string mac, std::string sta_mac)
{
    bool client_on_5ghz =
        (wireless_utils::which_freq(get_node_channel(sta_mac)) == eFreqType::FREQ_5G);

    if (((wireless_utils::which_freq(get_node_channel(mac)) == eFreqType::FREQ_24G) &&
         client_on_5ghz) ||
        ((wireless_utils::which_freq(get_node_channel(mac)) == eFreqType::FREQ_5G) &&
         (!client_on_5ghz))) {
        return true;
    }
    return false;
}

bool db::is_node_wireless(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    return utils::is_node_wireless(n->iface_type);
}

Socket *db::get_node_socket(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return nullptr;
    }
    if (n->get_type() == beerocks::TYPE_SLAVE || n->get_type() == TYPE_ETH_SWITCH) {
        const auto parent_mac = get_node_parent(mac);
        n                     = get_node(parent_mac);
        if (!n) {
            LOG(WARNING) << __FUNCTION__ << " - node " << parent_mac << " does not exist!";
            return nullptr;
        }
    }
    return n->socket;
}

bool db::set_node_socket(std::string mac, Socket *socket)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_GW && n->get_type() != beerocks::TYPE_IRE) {
        LOG(ERROR) << __FUNCTION__ << " - node " << mac << " is not a bridge type=!"
                   << int(n->get_type());
        return false;
    }
    n->socket = socket;
    return true;
}

bool db::disconnected_slave_mac_queue_empty() { return disconnected_slave_mac_queue.empty(); }

std::string db::disconnected_slave_mac_queue_pop()
{
    std::string node_mac;
    if (disconnected_slave_mac_queue.size() != 0) {
        node_mac = disconnected_slave_mac_queue.front();
        disconnected_slave_mac_queue.pop();
    }
    return node_mac;
}

void db::disconnected_slave_mac_queue_push(std::string node_mac)
{
    disconnected_slave_mac_queue.push(node_mac);
}

std::string db::node_to_string(std::string mac)
{
    auto n = get_node(mac);
    std::ostringstream os;
    if (n != nullptr) {
        os << n;
    } else {
        os << "";
    }
    return os.str();
}
//
// DB node functions (get only)
//
int db::get_node_hierarchy(std::string mac)
{
    auto n = get_node(mac);
    return get_node_hierarchy(n);
}

std::set<std::string> db::get_nodes(int type)
{
    std::set<std::string> ret;
    for (auto node_map : nodes) {
        for (auto kv : node_map) {
            if ((type < 0 || kv.second->get_type() == type) && (kv.second->mac == kv.first)) {
                ret.insert(kv.first);
            }
        }
    }
    return ret;
}

std::set<std::string> db::get_device_nodes()
{
    std::set<std::string> ret;
    for (auto node_map : nodes) {
        for (auto kv : node_map) {
            if ((kv.second->get_type() != beerocks::TYPE_SLAVE) && (kv.second->mac == kv.first)) {
                ret.insert(kv.first);
            }
        }
    }
    return ret;
}

std::set<std::string> db::get_active_hostaps()
{
    std::set<std::string> ret;
    for (auto node_map : nodes) {
        for (auto kv : node_map) {
            if (kv.second->get_type() == beerocks::TYPE_SLAVE && kv.second->hostap != nullptr &&
                kv.second->state == beerocks::STATE_CONNECTED && kv.first == kv.second->mac &&
                kv.second->hostap->active) {
                ret.insert(kv.first);
            }
        }
    }
    return ret;
}

std::set<std::string> db::get_all_connected_ires()
{
    std::set<std::string> ret;
    for (auto node_map : nodes) {
        for (auto kv : node_map) {
            if (((kv.second->get_type() == beerocks::TYPE_IRE) &&
                 (kv.second->state == beerocks::STATE_CONNECTED)) ||
                (kv.second->get_type() == beerocks::TYPE_GW)) {
                ret.insert(kv.first);
            }
        }
    }
    return ret;
}

std::set<std::string> db::get_all_backhaul_manager_slaves()
{
    std::set<std::string> ret;
    for (auto node_map : nodes) {
        for (auto kv : node_map) {
            if (kv.second->get_type() == beerocks::TYPE_SLAVE && kv.second->hostap != nullptr &&
                kv.first == kv.second->mac && kv.second->hostap->is_backhaul_manager) {
                ret.insert(kv.first);
            }
        }
    }
    return ret;
}

std::set<std::string> db::get_nodes_from_hierarchy(int hierarchy, int type)
{
    std::set<std::string> result;

    if (hierarchy < 0 || hierarchy >= HIERARCHY_MAX) {
        LOG(ERROR) << "invalid hierarchy";
        return result;
    }

    for (auto kv : nodes[hierarchy]) {
        if ((type < 0 || kv.second->get_type() == type) && (kv.second->mac == kv.first)) {
            result.insert(kv.first);
        }
    }

    return result;
}
std::string db::get_gw_mac()
{
    auto gw_container = get_nodes_from_hierarchy(0, beerocks::TYPE_GW);
    if (gw_container.empty()) {
        LOG(ERROR) << "can't get GW node!";
        return std::string();
    }

    auto gw_mac = *gw_container.begin();
    LOG(DEBUG) << "gw_mac = " << gw_mac;
    return gw_mac;
}

std::set<std::string> db::get_node_subtree(std::string mac)
{
    std::set<std::string> subtree;

    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << "node " << mac << " does not exist!";
    }
    auto subtree_ = get_node_subtree(n);
    for (auto s : subtree_) {
        subtree.insert(s->mac);
    }
    return subtree;
}

std::string db::get_node_parent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << "node " << mac << " does not exist!";
        return std::string();
    }
    return n->parent_mac;
}

std::string db::get_node_parent_hostap(std::string mac)
{
    std::string parent_backhaul = get_node_parent_backhaul(mac);
    if (is_node_wireless(parent_backhaul)) {
        return get_node_parent(parent_backhaul);
    } else {
        LOG(DEBUG) << "node " << parent_backhaul << " is not connected wirelessly";
        return std::string();
    }
}

std::string db::get_node_parent_backhaul(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::string();
    }

    std::string ire;
    if (n->get_type() == beerocks::TYPE_IRE) {
        ire = mac;
    } else {
        ire = get_node_parent_ire(mac);
    }

    return get_node_parent(ire);
}

std::string db::get_node_parent_ire(std::string mac)
{
    auto n = get_node(mac);
    if (!n || n->get_type() == beerocks::TYPE_GW) {
        return std::string();
    }

    std::shared_ptr<node> p;
    do {
        p = get_node(n->parent_mac);
        if (!p) {
            LOG(DEBUG) << "node " << mac << " has no valid parent IRE";
            return std::string();
        }
        n = p;
    } while (p->get_type() != beerocks::TYPE_IRE && p->get_type() != beerocks::TYPE_GW);

    return p->mac;
}

std::string db::get_node_previous_parent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << "node " << mac << " does not exist!";
        return std::string();
    }
    return n->previous_parent_mac;
}

std::set<std::string> db::get_node_siblings(std::string mac, int type)
{
    std::set<std::string> siblings;
    auto n = get_node(mac);

    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist";
        return siblings;
    }

    auto parent = get_node(n->parent_mac);
    if (!parent) {
        LOG(ERROR) << "parent for node " << mac << " does not exist";
        return siblings;
    }

    int hierarchy = get_node_hierarchy(parent) + 1;
    if (hierarchy >= 0 && hierarchy < HIERARCHY_MAX) {
        for (auto &it : nodes[hierarchy]) {
            if (it.first == it.second->mac) {
                auto sib = it.second;
                if ((sib->parent_mac == parent->mac) && (mac != sib->mac) &&
                    (type == beerocks::TYPE_ANY || sib->get_type() == type)) {
                    siblings.insert(sib->mac);
                }
            }
        }
    }
    return siblings;
}

std::set<std::string> db::get_node_children(std::string mac, int type, int state)
{
    std::set<std::string> children_macs;
    auto n = get_node(mac);

    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist";
        return children_macs;
    }

    std::set<std::shared_ptr<node>> children_nodes;
    if (n->mac == mac) {
        children_nodes = get_node_children(n, type, state);
    } else {
        children_nodes = get_node_children(n, type, state, mac);
    }
    for (auto c : children_nodes) {
        children_macs.insert(c->mac);
    }
    return children_macs;
}

//
// Capabilities
//

const beerocks::message::sRadioCapabilities *db::get_station_current_capabilities(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return nullptr;
    }
    return (&n->capabilities);
}

bool db::set_station_capabilities(const std::string &client_mac,
                                  beerocks::message::sRadioCapabilities &sta_cap)
{
    auto n = get_node(client_mac);

    if (!n) {
        LOG(ERROR) << "station node not found.... ";
        return false;
    }

    auto parent_radio = get_node_parent_radio(client_mac);

    if (parent_radio.empty()) {
        return false;
    }

    if (is_node_5ghz(parent_radio)) {
        if (n->m_sta_5ghz_capabilities.cap_flag == true) {
            //skip setting 5Ghz station parameters if it is already learned once
            LOG(INFO) << "m_sta_5ghz_capabilities already set once";
        } else {
            n->m_sta_5ghz_capabilities          = sta_cap;
            n->m_sta_5ghz_capabilities.cap_flag = true;
        }

        n->capabilities = n->m_sta_5ghz_capabilities;

    } else {
        if (n->m_sta_24ghz_capabilities.cap_flag == true) {
            //skip setting 2.4Ghz station parameters if it is already learned once
            LOG(INFO) << "m_sta_24ghz_capabilities already set once";
        } else {
            n->m_sta_24ghz_capabilities          = sta_cap;
            n->m_sta_24ghz_capabilities.cap_flag = true;
        }

        n->capabilities = n->m_sta_24ghz_capabilities;
    }
    return true;
}

const beerocks::message::sRadioCapabilities *
db::get_station_capabilities(const std::string &client_mac, bool is_bandtype_5ghz)
{
    std::shared_ptr<node> n = get_node(client_mac);

    if (!n) {
        LOG(ERROR) << "Gateway node not found.... ";
        return nullptr;
    }

    if (is_bandtype_5ghz) {
        if (n->m_sta_5ghz_capabilities.cap_flag == true) {
            return &n->m_sta_5ghz_capabilities;
        } else {
            return nullptr;
        }
    } else {
        if (n->m_sta_24ghz_capabilities.cap_flag == true) {
            return &n->m_sta_24ghz_capabilities;
        } else {
            return nullptr;
        }
    }
}

bool db::set_hostap_ant_num(std::string mac, beerocks::eWiFiAntNum ant_num)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    n->capabilities.ant_num = ant_num;
    return true;
}

beerocks::eWiFiAntNum db::get_hostap_ant_num(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::ANT_NONE;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return beerocks::ANT_NONE;
    }
    return beerocks::eWiFiAntNum(n->capabilities.ant_num);
}

bool db::set_hostap_ant_gain(std::string mac, int ant_gain)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->ant_gain = ant_gain;
    return true;
}

int db::get_hostap_ant_gain(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->ant_gain;
}

bool db::set_hostap_conducted_power(std::string mac, int conducted_power)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->conducted_power = conducted_power;
    return true;
}

int db::get_hostap_conducted_power(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->conducted_power;
}

bool db::set_hostap_supported_channels(std::string mac, beerocks_message::sWifiChannel *channels,
                                       int length)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    std::vector<beerocks_message::sWifiChannel> supported_channels_(channels, channels + length);
    n->hostap->supported_channels = supported_channels_;

    if (n->hostap->supported_channels.size() == 0) {
        LOG(ERROR) << "No supported channels";
        return false;
    }

    if (wireless_utils::which_freq(n->hostap->supported_channels[0].channel) ==
        eFreqType::FREQ_5G) {
        n->supports_5ghz = true;
    } else if (wireless_utils::which_freq(n->hostap->supported_channels[0].channel) ==
               eFreqType::FREQ_24G) {
        n->supports_24ghz = true;
    } else {
        LOG(ERROR) << "unknown frequency! channel:" << n->hostap->supported_channels[0].channel;
        return false;
    }

    return true;
}

const std::vector<beerocks_message::sWifiChannel> db::get_hostap_supported_channels(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::vector<beerocks_message::sWifiChannel>();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return std::vector<beerocks_message::sWifiChannel>();
    }
    return n->hostap->supported_channels;
}

std::string db::get_hostap_supported_channels_string(const std::string &radio_mac)
{
    std::ostringstream os;
    auto supported_channels = get_hostap_supported_channels(radio_mac);
    for (const auto &val : supported_channels) {
        if (val.channel > 0) {
            os << " ch = " << int(val.channel) << " | dfs = " << int(val.is_dfs_channel)
               << " | tx_pow = " << int(val.tx_pow) << " | noise = " << int(val.noise)
               << " [dbm] | bss_overlap = " << int(val.bss_overlap) << std::endl;
        }
    }

    return os.str();
}

/**
 * @brief Add supported operating class to the database.
 * Currently this function is a wrapper which converts the operating
 * class to a set of supported channels and updates the list of currently
 * supported channels.
 * 
 * @param mac radio mac
 * @param operating class operating class to add
 * @tx_power transmit power
 * @non_operable_channels list of statically non-operable channels
 * @return true on success
 * @return false on failure
 */
bool db::add_hostap_supported_operating_class(const std::string &radio_mac, uint8_t operating_class,
                                              uint8_t tx_power,
                                              const std::vector<uint8_t> &non_operable_channels)
{
    auto supported_channels = get_hostap_supported_channels(radio_mac);
    auto channel_set        = wireless_utils::operating_class_to_channel_set(operating_class);

    // Update current channels
    for (auto c : channel_set) {
        auto channel = std::find_if(
            supported_channels.begin(), supported_channels.end(),
            [&c](const beerocks_message::sWifiChannel &ch) { return ch.channel == c; });
        if (channel != supported_channels.end()) {
            channel->tx_pow = tx_power;
            //TODO fill other channel parameters
        } else {
            beerocks_message::sWifiChannel ch = {0};
            ch.channel                        = c;
            ch.tx_pow                         = tx_power;
            supported_channels.push_back(ch);
        }
    }

    // Delete non-operable channels
    for (auto c : non_operable_channels) {
        auto channel = std::find_if(
            supported_channels.begin(), supported_channels.end(),
            [&c](const beerocks_message::sWifiChannel &ch) { return ch.channel == c; });
        if (channel != supported_channels.end())
            supported_channels.erase(channel);
    }

    set_hostap_supported_channels(radio_mac, &supported_channels[0], supported_channels.size());
    // dump new supported channels state
    LOG(DEBUG) << "New supported channels for hostap" << radio_mac << " operating class "
               << int(operating_class) << std::endl
               << get_hostap_supported_channels_string(radio_mac);

    return true;
}

bool db::set_hostap_band_capability(std::string mac, beerocks::eRadioBandCapability capability)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->capability = capability;
    return true;
}

beerocks::eRadioBandCapability db::get_hostap_band_capability(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::SUBBAND_CAPABILITY_UNKNOWN;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return beerocks::SUBBAND_CAPABILITY_UNKNOWN;
    }
    return n->hostap->capability;
}

bool db::capability_check(std::string mac, int channel)
{
    auto band       = wireless_utils::which_subband(channel);
    auto capability = get_hostap_band_capability(mac);
    if (band == beerocks::SUBBAND_UNKNOWN || capability == beerocks::SUBBAND_CAPABILITY_UNKNOWN) {
        LOG(ERROR) << "band or capability unknown!!";
        return false;
    } else if (int(band) == int(capability) || capability == beerocks::BOTH_SUBBAND) {
        return true;
    }
    return false;
}

bool db::get_node_5ghz_support(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->supports_5ghz;
}

bool db::get_node_24ghz_support(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->supports_24ghz;
}

bool db::is_node_24ghz(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(ERROR) << "node " << mac << " does not exist! return false as default";
        return false;
    }
    if (wireless_utils::which_freq(n->channel) == eFreqType::FREQ_24G) {
        return true;
    }
    return false;
}

bool db::is_node_5ghz(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(ERROR) << "node " << mac << " does not exist! return false as default";
        return false;
    }
    if (wireless_utils::which_freq(n->channel) == eFreqType::FREQ_5G) {
        return true;
    }
    return false;
}

bool db::update_node_failed_5ghz_steer_attempt(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }

    if (++n->failed_5ghz_steer_attemps >= config.roaming_5ghz_failed_attemps_threshold) {
        n->supports_5ghz = false;
    }
    return true;
}

bool db::update_node_failed_24ghz_steer_attempt(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }

    if (++n->failed_24ghz_steer_attemps >= config.roaming_24ghz_failed_attemps_threshold) {
        n->supports_24ghz = false;
    }
    return true;
}

bool db::update_node_11v_responsiveness(std::string mac, bool success)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }

    if (success) {
        LOG(DEBUG) << "updating node " << mac << " as supporting 11v";
        n->failed_11v_request_count = 0;
        n->supports_11v             = true;
    } else {
        if (++n->failed_11v_request_count >= config.roaming_11v_failed_attemps_threshold) {
            LOG(DEBUG) << "node " << mac
                       << " exceeded maximum 11v failed attempts, updating as not supporting 11v";
            n->supports_11v = false;
        }
    }

    return true;
}

bool db::get_node_11v_capability(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->supports_11v;
}

bool db::set_hostap_iface_id(std::string mac, int8_t iface_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->iface_id = iface_id;
    return true;
}

int8_t db::get_hostap_iface_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::IFACE_TYPE_UNSUPPORTED;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return beerocks::IFACE_TYPE_UNSUPPORTED;
    }
    return n->hostap->iface_id;
}

bool db::set_hostap_vap_list(const std::string &mac,
                             const std::unordered_map<int8_t, sVapElement> &vap_list)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->vaps_info = vap_list;
    return true;
}

std::unordered_map<int8_t, sVapElement> &db::get_hostap_vap_list(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return *(std::make_shared<std::unordered_map<int8_t, sVapElement>>());
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " is not a valid hostap!";
        return *(std::make_shared<std::unordered_map<int8_t, sVapElement>>());
    }

    return n->hostap->vaps_info;
}

bool db::remove_vap(const std::string &radio_mac, int vap_id)
{
    return (get_hostap_vap_list(radio_mac).erase(vap_id) == 1);
}

bool db::add_vap(const std::string &radio_mac, int vap_id, std::string bssid, std::string ssid,
                 bool backhual)
{
    if (!has_node(network_utils::mac_from_string(bssid)) &&
        !add_virtual_node(network_utils::mac_from_string(bssid),
                          network_utils::mac_from_string(radio_mac))) {
        return false;
    }

    auto &vaps_info                = get_hostap_vap_list(radio_mac);
    vaps_info[vap_id].mac          = bssid;
    vaps_info[vap_id].ssid         = ssid;
    vaps_info[vap_id].backhaul_vap = backhual;

    return true;
}

std::set<std::string> db::get_hostap_vaps_bssids(const std::string &mac)
{
    std::set<std::string> bssid_set;
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return bssid_set;
    }

    if (n->get_type() != beerocks::TYPE_SLAVE) {
        // Only slaves have vap's
        return bssid_set;
    }
    auto vap_list = get_hostap_vap_list(mac);
    for (auto &vap : vap_list) {
        bssid_set.insert(vap.second.mac);
    }
    return bssid_set;
}

std::string db::get_hostap_ssid(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::string();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return std::string();
    }
    for (auto const &it : n->hostap->vaps_info) {
        if (it.second.mac == mac) {
            return it.second.ssid;
        }
    }
    return std::string();
}

std::string db::get_hostap_vap_with_ssid(const std::string &mac, const std::string &ssid)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::string();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return std::string();
    }
    for (auto const &it : n->hostap->vaps_info) {
        if (it.second.ssid == ssid) {
            return it.second.mac;
        }
    }
    return std::string();
}

std::string db::get_hostap_vap_mac(const std::string &mac, const int vap_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::string();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return std::string();
    }

    auto it = n->hostap->vaps_info.find(vap_id);
    return (it != n->hostap->vaps_info.end()) ? it->second.mac : std::string();
}

std::string db::get_node_parent_radio(const std::string &mac)
{
    // if mac is a client mac, get_node_parent will return vap bssid.
    // If the mac is Vap bssid, get_node will return a radio node.
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::string();
    }
    if (n->get_type() == beerocks::TYPE_CLIENT) {
        const auto parent_bssid = get_node_parent(mac);
        n                       = get_node(parent_bssid);
        if (!n) {
            LOG(WARNING) << __FUNCTION__ << " - node " << parent_bssid << " does not exist!";
            return std::string();
        }
    }
    return n->mac;
}

int8_t db::get_hostap_vap_id(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return IFACE_ID_INVALID;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return IFACE_ID_INVALID;
    }

    for (auto const &it : n->hostap->vaps_info) {
        if (it.second.mac == mac) {
            return it.first;
        }
    }
    return IFACE_ID_INVALID;
}

bool db::get_hostap_advertise_ssid_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    return n->hostap->advertise_ssid;
}

bool db::set_hostap_advertise_ssid_flag(std::string mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->advertise_ssid = flag;
    return true;
}

bool db::get_hostap_repeater_mode_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    return n->hostap->enable_repeater_mode;
}

bool db::set_hostap_repeater_mode_flag(std::string mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->enable_repeater_mode = flag;
    return true;
}

bool db::set_hostap_iface_name(std::string mac, std::string iface_name)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->iface_name = iface_name;
    return true;
}

std::string db::get_hostap_iface_name(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return "INVALID";
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return "INVALID";
    }

    return n->hostap->iface_name;
}

bool db::set_hostap_iface_type(std::string mac, beerocks::eIfaceType iface_type)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->iface_type = iface_type;
    return true;
}

beerocks::eIfaceType db::get_hostap_iface_type(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::IFACE_TYPE_UNSUPPORTED;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return beerocks::IFACE_TYPE_UNSUPPORTED;
    }
    return n->hostap->iface_type;
}

bool db::set_node_backhaul_iface_type(std::string mac, beerocks::eIfaceType iface_type)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    if (is_node_wireless(mac) && (iface_type > beerocks::IFACE_TYPE_WIFI_END ||
                                  iface_type == beerocks::IFACE_TYPE_UNSUPPORTED)) {
        LOG(ERROR) << "this should not happend!";
        return false;
    }
    n->iface_type = iface_type;
    return true;
}

bool db::set_hostap_driver_version(std::string mac, std::string version)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->driver_version = version;
    return true;
}

std::string db::get_hostap_driver_version(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return "INVALID";
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return "INVALID";
    }

    return n->hostap->driver_version;
}

beerocks::eIfaceType db::get_node_backhaul_iface_type(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::IFACE_TYPE_UNSUPPORTED;
    }
    return n->iface_type;
}

bool db::set_node_platform(std::string mac, beerocks::ePlatform platform)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->platform = platform;
    return true;
}

beerocks::ePlatform db::get_node_platform(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::PLATFORM_UNSUPPORTED;
    }
    return n->platform;
}

std::string db::get_5ghz_sibling_hostap(std::string mac)
{
    auto siblings = get_node_siblings(mac, beerocks::TYPE_SLAVE);
    for (auto &hostap : siblings) {
        if (get_node_5ghz_support(hostap)) {
            auto n = get_node(hostap);
            if (!n) {
                LOG(ERROR) << "node " << hostap << " does not exist";
                return std::string();
            }
            return hostap;
        }
    }
    return std::string();
}

bool db::set_hostap_activity_mode(std::string mac, eApActiveMode ap_activity_mode)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(ERROR) << "node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return false;
    }
    n->hostap->ap_activity_mode = ap_activity_mode;
    return true;
}

beerocks::eApActiveMode db::get_hostap_activity_mode(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(ERROR) << "node " << mac << " does not exist!";
        return AP_INVALID_MODE;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return AP_INVALID_MODE;
    }
    return n->hostap->ap_activity_mode;
}

bool db::set_radar_hit_stats(std::string mac, uint8_t channel, uint8_t bw, bool is_csa_entry)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    sWifiChannelRadarStats radar_statistics = {
        .channel = channel, .bandwidth = bw, .channel_ext_above_secondary = 0};

    //CSA enter channel
    if (is_csa_entry) {
        if (n->hostap->Radar_stats.size() == RADAR_STATS_LIST_MAX) {
            n->hostap->Radar_stats.pop_back();
        }
        auto now                             = std::chrono::steady_clock::now();
        radar_statistics.csa_enter_timestamp = now;
        radar_statistics.csa_exit_timestamp  = now;
        n->hostap->Radar_stats.push_front(radar_statistics);
        // for_each(begin(n.hostap->Radar_stats) , end(n.hostap->Radar_stats), [&](sWifiChannelRadarStats radar_stat){
        for (auto &radar_stat : n->hostap->Radar_stats) {
            auto delta_radar = std::chrono::duration_cast<std::chrono::seconds>(
                                   radar_stat.csa_exit_timestamp - radar_stat.csa_enter_timestamp)
                                   .count();
            // if(delta_radar)
            LOG(DEBUG) << "channel = " << int(radar_stat.channel)
                       << " bw = " << int(radar_stat.bandwidth)
                       << " time_in_channel_sec = " << int(delta_radar);
            ;
        }
        return true;
    }
    //CSA exit channel
    n->hostap->Radar_stats.front().csa_exit_timestamp = std::chrono::steady_clock::now();

    return true;
}

bool db::set_supported_channel_radar_affected(std::string mac, std::vector<uint8_t> channels,
                                              bool affected)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    auto channels_count = channels.size();
    LOG(DEBUG) << " channels_count = " << int(channels_count);
    auto it =
        find_if(std::begin(n->hostap->supported_channels), std::end(n->hostap->supported_channels),
                [&](beerocks_message::sWifiChannel supported_channel) {
                    return supported_channel.channel == *channels.begin();
                });

    if (it == std::end(n->hostap->supported_channels)) {
        LOG(ERROR) << "channels not found ,not suppose to happen!!";
        return false;
    }
    std::for_each(it, std::next(it, channels_count),
                  [&](beerocks_message::sWifiChannel &supported_channel) {
                      LOG(DEBUG) << " supported_channel = " << int(supported_channel.channel)
                                 << " affected = " << int(affected);
                      supported_channel.radar_affected = affected;
                  });

    // for(auto supported_channel : n->hostap->supported_channels) {
    //     if(supported_channel.channel > 0) {
    //         LOG(DEBUG) <<" supported_channel = " << int(supported_channel.channel) << " affected = " << int(supported_channel.radar_affected);
    //     }
    // }

    return true;
}

bool db::set_hostap_is_dfs(std::string mac, bool enable)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->is_dfs = enable;
    return true;
}

bool db::get_hostap_is_dfs(std::string mac)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    return n->hostap->is_dfs;
}

bool db::set_hostap_cac_completed(std::string mac, bool enable)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->cac_completed = enable;
    return true;
}

bool db::get_hostap_cac_completed(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    return n->hostap->cac_completed;
}
bool db::set_hostap_is_on_fail_safe(std::string mac, bool enable)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->on_fail_safe_channel = enable;
    return true;
}

bool db::get_hostap_is_on_fail_safe(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    return n->hostap->on_fail_safe_channel;
}

bool db::set_hostap_is_on_sub_band(std::string mac, bool enable)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->on_sub_band_channel = enable;
    return true;
}

bool db::get_hostap_is_on_sub_band(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    return n->hostap->on_sub_band_channel;
}

bool db::set_hostap_on_dfs_reentry(std::string mac, bool enable)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->on_dfs_reentry = enable;
    return true;
}

bool db::get_hostap_on_dfs_reentry(std::string mac)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    return n->hostap->on_dfs_reentry;
}

bool db::set_hostap_dfs_reentry_clients(std::string mac, std::set<std::string> dfs_reentry_clients)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->dfs_reentry_clients = dfs_reentry_clients;
    for_each(begin(n->hostap->dfs_reentry_clients), end(n->hostap->dfs_reentry_clients),
             [&](std::string dfs_reentry_client) {
                 LOG(DEBUG) << "dfs_reentry_client = " << dfs_reentry_client;
             });
    return true;
}

std::set<std::string> db::get_hostap_dfs_reentry_clients(std::string mac)
{
    auto n = get_node(mac);

    std::set<std::string> ret;
    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return ret;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return ret;
    }
    for_each(begin(n->hostap->dfs_reentry_clients), end(n->hostap->dfs_reentry_clients),
             [&](std::string dfs_reentry_client) {
                 LOG(DEBUG) << "dfs_reentry_client = " << dfs_reentry_client;
             });
    return n->hostap->dfs_reentry_clients;
}

bool db::clear_hostap_dfs_reentry_clients(std::string mac)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }

    n->hostap->dfs_reentry_clients.clear();
    return true;
}

bool db::set_hostap_is_acs_enabled(std::string mac, bool enable)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    LOG(DEBUG) << __FUNCTION__ << ", enable = " << int(enable);
    n->hostap->is_acs_enabled = enable;
    return true;
}

bool db::get_hostap_is_acs_enabled(std::string mac)
{
    auto n = get_node(mac);

    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    LOG(DEBUG) << __FUNCTION__ << "n->hostap->is_acs_enabled = " << int(n->hostap->is_acs_enabled);
    return n->hostap->is_acs_enabled;
}

//
// CLI
//
void db::add_cli_socket(Socket *sd)
{
    if (sd) {
        remove_cli_socket(sd);
        cli_debug_sockets.push_back(sd);
    }
}

void db::remove_cli_socket(Socket *sd)
{
    if (sd) {
        for (auto it = cli_debug_sockets.begin(); it < cli_debug_sockets.end(); it++) {
            if (sd == (*it)) {
                cli_debug_sockets.erase(it);
                return;
            }
        }
    }
}
bool db::get_cli_debug_enable(Socket *sd)
{
    if (sd) {
        for (auto it = cli_debug_sockets.begin(); it < cli_debug_sockets.end(); it++) {
            if (sd == (*it)) {
                return true;
            }
        }
    }
    return false;
}

void db::set_slave_stop_on_failure_attempts(int attempts)
{
    slaves_stop_on_failure_attempts = attempts;
}

int db::get_slave_stop_on_failure_attempts() { return slaves_stop_on_failure_attempts; }

Socket *db::get_cli_socket_at(int idx)
{
    if (idx < int(cli_debug_sockets.size())) {
        return cli_debug_sockets.at(idx);
    }
    return nullptr;
}

//
// BML
//
void db::add_bml_socket(Socket *sd)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return;
            }
        }
        sBmlListener bml_listener = {0};
        bml_listener.sd           = sd;
        bml_listeners_sockets.push_back(bml_listener);
    }
}

void db::remove_bml_socket(Socket *sd)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                bml_listeners_sockets.erase(it);
                return;
            }
        }
    }
}

bool db::get_bml_nw_map_update_enable(Socket *sd)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return (*it).map_updates;
            }
        }
    }
    return false;
}

bool db::set_bml_nw_map_update_enable(Socket *sd, bool update_enable)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                (*it).map_updates = update_enable;
                return true;
            }
        }
    }
    return false;
}

bool db::get_bml_stats_update_enable(Socket *sd)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return (*it).stats_updates;
            }
        }
    }
    return false;
}

bool db::set_bml_stats_update_enable(Socket *sd, bool update_enable)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                (*it).stats_updates = update_enable;
                return true;
            }
        }
    }
    return false;
}

bool db::get_bml_events_update_enable(Socket *sd)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return (*it).events_updates;
            }
        }
    }
    return false;
}

bool db::set_bml_events_update_enable(Socket *sd, bool update_enable)
{
    if (sd) {
        for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                (*it).events_updates = update_enable;
                return true;
            }
        }
    }
    return false;
}

Socket *db::get_bml_socket_at(int idx)
{
    if (idx < int(bml_listeners_sockets.size())) {
        return bml_listeners_sockets.at(idx).sd;
    }
    return nullptr;
}

bool db::is_bml_listener_exist()
{
    bool listener_exist;
    for (auto it = bml_listeners_sockets.begin(); it < bml_listeners_sockets.end(); it++) {
        listener_exist = (*it).map_updates || (*it).stats_updates || (*it).events_updates;
        if (listener_exist) {
            return true;
        }
    }
    return false;
}

//
// Measurements
//
bool db::set_node_beacon_measurement(std::string sta_mac, std::string ap_mac, int8_t rcpi,
                                     uint8_t rsni)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        LOG(WARNING) << __FUNCTION__ << " - node " << sta_mac << " does not exist!";
        return false;
    }
    sta->set_beacon_measurement(ap_mac, rcpi, rsni);
    return true;
}

bool db::get_node_beacon_measurement(std::string sta_mac, std::string ap_mac, int8_t &rcpi,
                                     uint8_t &rsni)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        LOG(WARNING) << __FUNCTION__ << " - node " << sta_mac << " does not exist!";
        rcpi = beerocks::RSSI_INVALID;
        rsni = 0;
        return false;
    }
    return sta->get_beacon_measurement(ap_mac, rcpi, rsni);
}

bool db::set_node_cross_rx_rssi(std::string sta_mac, std::string ap_mac, int8_t rssi,
                                int8_t rx_packets)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        return false;
    }
    sta->set_cross_rx_rssi(ap_mac, rssi, rx_packets);
    return true;
}

bool db::get_node_cross_rx_rssi(std::string sta_mac, std::string ap_mac, int8_t &rssi,
                                int8_t &rx_packets)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        rssi       = beerocks::RSSI_INVALID;
        rx_packets = 0;
        return false;
    }
    return sta->get_cross_rx_rssi(ap_mac, rssi, rx_packets);
}

bool db::set_node_cross_rx_phy_rate_100kb(std::string mac, uint16_t rx_phy_rate_100kb)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cross_rx_phy_rate_100kb = rx_phy_rate_100kb;
    return true;
}

bool db::set_node_cross_tx_phy_rate_100kb(std::string mac, uint16_t tx_phy_rate_100kb)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cross_tx_phy_rate_100kb = tx_phy_rate_100kb;
    return true;
}

uint16_t db::get_node_cross_rx_phy_rate_100kb(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->cross_rx_phy_rate_100kb;
}

uint16_t db::get_node_cross_tx_phy_rate_100kb(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->cross_tx_phy_rate_100kb;
}

bool db::clear_node_cross_rssi(std::string sta_mac)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        return false;
    }
    sta->clear_cross_rssi();
    return true;
}

bool db::set_node_cross_estimated_tx_phy_rate(std::string mac, double phy_rate)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cross_estimated_tx_phy_rate = phy_rate;
    return true;
}

double db::get_node_cross_estimated_tx_phy_rate(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->cross_estimated_tx_phy_rate;
}

bool db::set_hostap_stats_info(std::string mac, beerocks_message::sApStatsParams *params)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    if (params == nullptr) { // clear stats
        n->clear_hostap_stats_info();
    } else {
        auto p                          = n->hostap->stats_info;
        p->active_sta_count             = params->active_client_count;
        p->rx_packets                   = params->rx_packets;
        p->tx_packets                   = params->tx_packets;
        p->rx_bytes                     = params->rx_bytes;
        p->tx_bytes                     = params->tx_bytes;
        p->errors_sent                  = params->errors_sent;
        p->errors_received              = params->errors_received;
        p->retrans_count                = params->retrans_count;
        p->noise                        = params->noise;
        p->channel_load_percent         = params->channel_load_percent;
        p->total_client_tx_load_percent = params->client_tx_load_percent;
        p->total_client_rx_load_percent = params->client_rx_load_percent;
        p->stats_delta_ms               = params->stats_delta_ms;
        p->timestamp                    = std::chrono::steady_clock::now();
    }
    return true;
}

void db::clear_hostap_stats_info(std::string mac) { set_hostap_stats_info(mac, nullptr); }

bool db::set_node_stats_info(std::string mac, beerocks_message::sStaStatsParams *params)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    if (params == nullptr) { // clear stats
        n->clear_node_stats_info();
    } else {
        auto p               = n->stats_info;
        p->rx_packets        = params->rx_packets;
        p->tx_packets        = params->tx_packets;
        p->tx_bytes          = params->tx_bytes;
        p->rx_bytes          = params->rx_bytes;
        p->retrans_count     = params->retrans_count;
        p->tx_phy_rate_100kb = params->tx_phy_rate_100kb;
        p->rx_phy_rate_100kb = params->rx_phy_rate_100kb;
        p->tx_load_percent   = params->tx_load_percent;
        p->rx_load_percent   = params->rx_load_percent;
        p->stats_delta_ms    = params->stats_delta_ms;
        p->rx_rssi           = params->rx_rssi;
        p->timestamp         = std::chrono::steady_clock::now();
    }
    return true;
}

void db::clear_node_stats_info(std::string mac) { set_node_stats_info(mac, nullptr); }

int db::get_hostap_stats_measurement_duration(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->stats_info->stats_delta_ms;
}

std::chrono::steady_clock::time_point db::get_node_stats_info_timestamp(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::chrono::steady_clock::time_point();
    }
    return n->stats_info->timestamp;
}

std::chrono::steady_clock::time_point db::get_hostap_stats_info_timestamp(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        std::chrono::steady_clock::time_point();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        std::chrono::steady_clock::time_point();
    }
    return n->hostap->stats_info->timestamp;
}

uint32_t db::get_node_rx_bytes(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_bytes;
}

uint32_t db::get_node_tx_bytes(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_bytes;
}

uint32_t db::get_hostap_total_sta_rx_bytes(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->stats_info->rx_bytes;
}

uint32_t db::get_hostap_total_sta_tx_bytes(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->stats_info->tx_bytes;
}

double db::get_node_rx_bitrate(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return (1000 * 8 * double(n->stats_info->rx_bytes) / n->stats_info->stats_delta_ms) / 1e+6;
}

double db::get_node_tx_bitrate(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return (1000 * 8 * double(n->stats_info->tx_bytes) / n->stats_info->stats_delta_ms) / 1e+6;
}

uint16_t db::get_node_rx_phy_rate_100kb(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_phy_rate_100kb;
}

uint16_t db::get_node_tx_phy_rate_100kb(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_phy_rate_100kb;
}

int db::get_hostap_channel_load_percent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->stats_info->channel_load_percent;
}

int db::get_hostap_total_client_tx_load_percent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->stats_info->total_client_tx_load_percent;
}

int db::get_hostap_total_client_rx_load_percent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->stats_info->total_client_rx_load_percent;
}

int db::get_node_rx_load_percent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_load_percent;
}

int db::get_node_tx_load_percent(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_load_percent;
}

int8_t db::get_load_rx_rssi(std::string sta_mac)
{
    auto n = get_node(sta_mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_rssi;
}

uint16_t db::get_load_rx_phy_rate_100kb(std::string sta_mac)
{
    auto n = get_node(sta_mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_phy_rate_100kb;
}

uint16_t db::get_load_tx_phy_rate_100kb(std::string sta_mac)
{
    auto n = get_node(sta_mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_phy_rate_100kb;
}

bool db::set_measurement_delay(std::string mac, int measurement_delay)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return false;
    }
    n->measurement_delay = measurement_delay;
    LOG(DEBUG) << "set_measurement_delay: mac " << mac
               << " n->measurement_delay = " << int(n->measurement_delay);
    return true;
}

int db::get_measurement_delay(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return -1;
    }
    //LOG(DEBUG) << "get_measurement_delay: mac " << mac << " n->measurement_delay = " << int(n->measurement_delay);
    return n->measurement_delay;
}

std::chrono::steady_clock::time_point db::get_measurement_sent_timestamp(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return std::chrono::steady_clock::time_point();
    }
    //LOG(DEBUG) << "get_measurement_dry_run: mac " << mac << " n->measurement_dry_run" ;
    return n->measurement_sent_timestamp;
}

bool db::set_measurement_sent_timestamp(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return false;
    }
    n->measurement_sent_timestamp = std::chrono::steady_clock::now();
    LOG(DEBUG) << "set_measurement_sent_timestamp: mac " << mac;
    return true;
}

int db::get_measurement_recv_delta(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return -1;
    }
    LOG(DEBUG) << "get_measurement_recv_delta: mac " << mac
               << " n->measurement_recv_delta = " << int(n->measurement_recv_delta)
               << " actual delay = " << int((n->measurement_recv_delta / 2));
    return n->measurement_recv_delta;
}

bool db::set_measurement_recv_delta(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return false;
    }
    auto measurement_recv_timestamp = std::chrono::steady_clock::now();
    n->measurement_recv_delta       = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    measurement_recv_timestamp - n->measurement_sent_timestamp)
                                    .count();
    //LOG(DEBUG) << "set_measurement_recv_delta: mac " << mac << " n->measurement_recv_delta = " << int(n->measurement_recv_delta);
    return true;
}

int db::get_measurement_window_size(std::string mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->measurement_window_size;
}

bool db::set_measurement_window_size(std::string mac, int window_size)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return false;
    }
    n->measurement_window_size = window_size;
    return true;
}

bool db::get_hostap_exclude_from_steering_flag(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return false;
    }
    return n->hostap->exclude_from_steering;
}

bool db::set_hostap_exclude_from_steering_flag(std::string mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return false;
    }

    n->hostap->exclude_from_steering = flag;
    return true;
}

bool db::set_node_channel_bw(const std::string &mac, int channel, beerocks::eWiFiBandwidth bw,
                             bool channel_ext_above_secondary, int8_t channel_ext_above_primary,
                             uint16_t vht_center_frequency)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        LOG(ERROR) << "node " << mac << "does not exist ";
        return false;
    }
    if (n->get_type() == beerocks::TYPE_SLAVE) {
        if (n->hostap != nullptr) {
            n->hostap->channel_ext_above_primary = channel_ext_above_primary;
            n->hostap->vht_center_frequency      = vht_center_frequency;
            auto is_dfs                          = wireless_utils::is_dfs_channel(channel);
            set_hostap_is_dfs(mac, is_dfs);
        } else {
            LOG(ERROR) << __FUNCTION__ << " - node " << mac << " is null!";
            return false;
        }
    }

    n->channel                     = channel;
    n->bandwidth                   = bw;
    n->channel_ext_above_secondary = channel_ext_above_secondary;
    if (wireless_utils::which_freq(channel) == eFreqType::FREQ_24G) { //2.4G
        n->supports_24ghz             = true;
        n->failed_24ghz_steer_attemps = 0;
    } else if (wireless_utils::which_freq(channel) == eFreqType::FREQ_5G) {
        n->supports_5ghz             = true;
        n->failed_5ghz_steer_attemps = 0;
    } else {
        LOG(ERROR) << "frequency type unknown, channel=" << int(channel);
    }

    auto children = get_node_children(n);
    for (auto child : children) {
        child->channel                     = channel;
        child->bandwidth                   = bw;
        child->channel_ext_above_secondary = channel_ext_above_secondary;
    }
    return true;
}

beerocks::eWiFiBandwidth db::get_node_bw(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::BANDWIDTH_MAX;
    }
    return n->bandwidth;
}

bool db::get_node_channel_ext_above_secondary(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    return n->channel_ext_above_secondary;
}

bool db::get_hostap_channel_ext_above_primary(std::string hostap_mac)
{
    auto n = get_node(hostap_mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << hostap_mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << hostap_mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->channel_ext_above_primary;
}

int db::get_node_bw_int(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return 0;
    }
    return get_node_bw_int(n);
}

std::string db::get_node_key(const std::string &al_mac, const std::string &ruid)
{

    if (al_mac.empty() || ruid.empty()) {
        return std::string();
    }

    return al_mac + "_" + ruid;
}

uint16_t db::get_hostap_vht_center_frequency(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return 0;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        return 0;
    }
    return n->hostap->vht_center_frequency;
}

//
// tasks
//

bool db::assign_association_handling_task_id(std::string mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->association_handling_task_id = new_task_id;
    return true;
}

int db::get_association_handling_task_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->association_handling_task_id;
}

bool db::assign_steering_task_id(std::string mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->steering_task_id = new_task_id;
    return true;
}

int db::get_steering_task_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->steering_task_id;
}

bool db::assign_roaming_task_id(std::string mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->roaming_task_id = new_task_id;
    return true;
}

int db::get_roaming_task_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->roaming_task_id;
}

bool db::assign_load_balancer_task_id(std::string mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->load_balancer_task_id = new_task_id;
    return true;
}

int db::get_load_balancer_task_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->load_balancer_task_id;
}

bool db::assign_client_locating_task_id(std::string mac, int new_task_id, bool new_connection)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    if (new_connection) {
        n->client_locating_task_id_new_connection = new_task_id;
    } else {
        n->client_locating_task_id_exist_connection = new_task_id;
    }
    return true;
}

int db::get_client_locating_task_id(std::string mac, bool new_connection)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    if (new_connection) {
        return n->client_locating_task_id_new_connection;
    }
    return n->client_locating_task_id_exist_connection;
}

bool db::assign_ire_4addr_mode_transition_task_id(std::string mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->ire_4addr_mode_transition_task_id = new_task_id;
    return true;
}

int db::get_ire_4addr_mode_transition_task_id(std::string mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->ire_4addr_mode_transition_task_id;
}

bool db::assign_channel_selection_task_id(int new_task_id)
{
    channel_selection_task_id = new_task_id;
    return true;
}

int db::get_channel_selection_task_id() { return channel_selection_task_id; }

bool db::assign_network_optimization_task_id(int new_task_id)
{
    network_optimization_task_id = new_task_id;
    return true;
}

int db::get_network_optimization_task_id() { return network_optimization_task_id; }

bool db::assign_bml_task_id(int new_task_id)
{
    bml_task_id = new_task_id;
    return true;
}

int db::get_bml_task_id() { return bml_task_id; }

bool db::assign_rdkb_wlan_task_id(int new_task_id)
{
    rdkb_wlan_task_id = new_task_id;
    return true;
}

int db::get_rdkb_wlan_task_id() { return rdkb_wlan_task_id; }

bool db::assign_bml_config_update_task_id(int new_task_id)
{
    config_update_task_id = new_task_id;
    return true;
}

int db::get_bml_config_update_task_id() { return config_update_task_id; }

void db::lock() { db_mutex.lock(); }

void db::unlock() { db_mutex.unlock(); }

//
// certification
//

void db::setting_certification_mode(bool en)
{
    if (!en) {
        remove_certification_tx_buffer();
        config.certification_mode = false;
        return;
    }
    if (!allocate_certification_tx_buffer()) {
        config.certification_mode = false;
        LOG(DEBUG) << "failed to allocate certification_tx_buffer";
    }

    config.certification_mode = true;
}

void db::add_bss_info_configuration(const std::string &al_mac, const bss_info_conf_t &bss_info)
{
    bss_infos[al_mac].push_back(bss_info);
}

std::list<db::bss_info_conf_t> &db::get_bss_info_configuration(const std::string &al_mac)
{
    // If al_mac not exist, it will be added, and return empty list
    return bss_infos[al_mac];
}

void db::clear_bss_info_configuration() { bss_infos.clear(); }

//
// PRIVATE FUNCTIONS
//   must be used from a thread safe context
//
int db::get_node_hierarchy(std::shared_ptr<node> n)
{
    if (!n) {
        return -1;
    }
    //redundant but more efficient this way
    return n->hierarchy;
}

std::shared_ptr<node> db::get_node(std::string key)
{
    if (key == last_accessed_node_mac) {
        return last_accessed_node;
    }

    for (int i = 0; i < HIERARCHY_MAX; i++) {
        auto it = nodes[i].find(key);
        if (it != nodes[i].end()) {
            last_accessed_node_mac = key;
            last_accessed_node     = it->second;
            return it->second;
        }
    }
    return nullptr;
}

std::shared_ptr<node> db::get_node(sMacAddr mac)
{
    std::string key =
        mac == network_utils::ZERO_MAC ? std::string() : network_utils::mac_to_string(mac);
    return get_node(key);
}

std::shared_ptr<node> db::get_node(sMacAddr al_mac, sMacAddr ruid)
{
    std::string key = std::string();
    if (al_mac != network_utils::ZERO_MAC && ruid != network_utils::ZERO_MAC)
        key = network_utils::mac_to_string(al_mac) + network_utils::mac_to_string(ruid);

    return get_node(key);
}

std::set<std::shared_ptr<node>> db::get_node_subtree(std::shared_ptr<node> n)
{
    std::set<std::shared_ptr<node>> subtree;

    if (!n) {
        LOG(ERROR) << "node is nullptr!";
        return subtree;
    }

    int i = get_node_hierarchy(n) + 1;

    if (i >= HIERARCHY_MAX) {
        return subtree;
    }

    for (auto &node_element : nodes[i]) {
        if (node_element.first == node_element.second->mac) {
            auto subtree_node = node_element.second;
            if (subtree_node->parent_mac == n->mac) {
                subtree.insert(subtree_node);
                std::set<std::shared_ptr<node>> sub_subtree = get_node_subtree(subtree_node);
                subtree.insert(sub_subtree.begin(), sub_subtree.end());
            }
        }
    }
    return subtree;
}

std::set<std::shared_ptr<node>> db::get_node_children(std::shared_ptr<node> n, int type, int state,
                                                      std::string parent_mac)
{
    std::set<std::shared_ptr<node>> children;

    if (!n) {
        LOG(ERROR) << "node is nullptr!";
        return children;
    }

    auto bssids = get_hostap_vaps_bssids(n->mac);
    bssids.insert(n->mac);

    int hierarchy = get_node_hierarchy(n) + 1;

    if (hierarchy >= 0 && hierarchy < HIERARCHY_MAX) {
        for (auto &node_element : nodes[hierarchy]) {
            auto child = node_element.second;
            if ((child->mac == node_element.first) &&
                (bssids.find(child->parent_mac) != bssids.end() &&
                 (type == beerocks::TYPE_ANY || child->get_type() == type) &&
                 (state == beerocks::STATE_ANY || child->state == state) &&
                 (parent_mac.empty() || child->parent_mac == parent_mac))) {
                children.insert(child);
            }
        }
    }
    return children;
}

void db::adjust_subtree_hierarchy(std::shared_ptr<node> n)
{
    if (!n) {
        LOG(ERROR) << "node is nullptr!";
        return;
    }

    int hierarchy = get_node_hierarchy(n);

    for (int i = 0; i < HIERARCHY_MAX; ++i) {
        for (auto it = nodes[i].begin(); it != nodes[i].end();) {
            auto subtree_node = it->second;
            if (subtree_node->parent_mac == n->mac) {
                int new_hierarchy = hierarchy + 1;
                if (new_hierarchy >= HIERARCHY_MAX) {
                    LOG(ERROR) << "new hierarchy is too high!";
                    return;
                }
                it = nodes[i].erase(it);
                nodes[new_hierarchy].insert(std::make_pair(subtree_node->mac, subtree_node));
                subtree_node->hierarchy = new_hierarchy;
                adjust_subtree_hierarchy(subtree_node);
            } else {
                ++it;
            }
        }
    }
}

void db::adjust_subtree_hierarchy(std::set<std::shared_ptr<node>> subtree, int offset)
{
    for (auto s : subtree) {
        int new_hierarchy = s->hierarchy + offset;
        if (new_hierarchy >= HIERARCHY_MAX || new_hierarchy < 0) {
            LOG(ERROR) << "invalid new_hierarchy=" << new_hierarchy << " for node " << s->mac;
            continue;
        }
        nodes[s->hierarchy].erase(s->mac);
        nodes[new_hierarchy].insert({s->mac, s});
        s->hierarchy = new_hierarchy;
    }
}

void db::rewind()
{
    current_hierarchy = 0;
    db_it             = nodes[current_hierarchy].begin();
}

bool db::get_next_node(std::shared_ptr<node> &n, int &hierarchy)
{
    bool last = false;

    if (db_it != nodes[current_hierarchy].end()) {
        n         = db_it->second;
        hierarchy = current_hierarchy;
        ++db_it;
    }

    if (db_it == nodes[current_hierarchy].end()) {
        current_hierarchy++;
        if (current_hierarchy >= HIERARCHY_MAX) {
            current_hierarchy = 0;
            last              = true;
        }
        db_it = nodes[current_hierarchy].begin();
    }
    return last;
}

bool db::get_next_node(std::shared_ptr<node> &n)
{
    bool last = false;

    if (db_it != nodes[current_hierarchy].end()) {
        n = db_it->second;
        ++db_it;
    }

    if (db_it == nodes[current_hierarchy].end()) {
        current_hierarchy++;
        if (current_hierarchy >= HIERARCHY_MAX) {
            current_hierarchy = 0;
            last              = true;
        }
        db_it = nodes[current_hierarchy].begin();
    }
    return last;
}

int db::get_node_bw_int(std::shared_ptr<node> &n)
{
    int bw;
    switch (n->bandwidth) {
    case beerocks::BANDWIDTH_20:
        bw = 20;
        break;
    case beerocks::BANDWIDTH_40:
        bw = 40;
        break;
    default:
        bw = 80;
        break;
    }
    return bw;
}

void db::set_vap_list(std::shared_ptr<db::vaps_list_t> vaps_list) { m_vap_list = vaps_list; }

void db::clear_vap_list()
{
    if (m_vap_list) {
        m_vap_list.reset();
    }
}

const std::shared_ptr<db::vaps_list_t> db::get_vap_list() { return m_vap_list; }
