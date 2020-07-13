/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "db.h"

#include <bcl/beerocks_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <bpl/bpl_db.h>
#include <easylogging++.h>

#include <algorithm>

using namespace beerocks;
using namespace beerocks_message;
using namespace son;
using namespace net;

const std::string db::TIMESTAMP_STR            = "timestamp";
const std::string db::TIMELIFE_DELAY_STR       = "timelife";
const std::string db::INITIAL_RADIO_ENABLE_STR = "initial_radio_enable";
const std::string db::INITIAL_RADIO_STR        = "initial_radio";
const std::string db::SELECTED_BAND_ENABLE_STR = "selected_band_enable";
const std::string db::SELECTED_BANDS_STR       = "selected_bands";

// static
std::string db::type_to_string(beerocks::eType type)
{
    switch (type) {
    case beerocks::eType::TYPE_GW:
        return "gateway";
    case beerocks::eType::TYPE_IRE:
        return "ire";
    case beerocks::eType::TYPE_IRE_BACKHAUL:
        return "ire_bh";
    case beerocks::eType::TYPE_SLAVE:
        return "slave";
    case beerocks::eType::TYPE_CLIENT:
        return "client";
    case beerocks::eType::TYPE_ETH_SWITCH:
        return "eth_switch";
    case beerocks::eType::TYPE_ANY:
        return "any";
    default:
        return {};
    }
}

std::string db::client_db_entry_from_mac(const sMacAddr &mac)
{
    std::string db_entry = tlvf::mac_to_string(mac);
    std::replace(db_entry.begin(), db_entry.end(), ':', '_');

    return db_entry;
}

sMacAddr db::client_db_entry_to_mac(const std::string &db_entry)
{
    std::string entry = db_entry;

    std::replace(entry.begin(), entry.end(), '_', ':');

    return tlvf::mac_from_string(entry);
}

std::string db::timestamp_to_string_seconds(const std::chrono::steady_clock::time_point timestamp)
{
    return std::to_string(
        std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count());
}

std::chrono::steady_clock::time_point db::timestamp_from_seconds(int timestamp_sec)
{
    return std::chrono::steady_clock::time_point(std::chrono::seconds(timestamp_sec));
}

// static - end

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
        LOG(ERROR) << "node " << real_node_mac << " does not exist";
        return false;
    }

    /*
     * TODO
     * the regular add_node() function should take care of a situation where the real node
     * already exists and is moved to a different hierarchy
     * it should be able to find its virtual nodes and move them to the appropriate hierarchy as well
     */

    nodes[real_node->hierarchy].insert(std::make_pair(tlvf::mac_to_string(mac), real_node));
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
        LOG(ERROR) << "hierarchy too high for node " << mac;
        return false;
    }

    auto n = get_node(mac);
    if (n) { // n is not nullptr
        LOG(DEBUG) << "node with mac " << mac << " already exists, updating";
        n->set_type(type);
        if (n->parent_mac != tlvf::mac_to_string(parent_mac)) {
            n->previous_parent_mac = n->parent_mac;
            n->parent_mac          = tlvf::mac_to_string(parent_mac);
        }
        int old_hierarchy = get_node_hierarchy(n);
        if (old_hierarchy >= 0 && old_hierarchy < HIERARCHY_MAX) {
            nodes[old_hierarchy].erase(tlvf::mac_to_string(mac));
        } else {
            LOG(ERROR) << "old hierarchy " << old_hierarchy << " for node " << mac
                       << " is invalid!!!";
        }
        auto subtree = get_node_subtree(n);
        int offset   = new_hierarchy - old_hierarchy;
        adjust_subtree_hierarchy(subtree, offset);
    } else {
        LOG(DEBUG) << "node with mac " << mac << " being created, the type is " << type;
        n             = std::make_shared<node>(type, tlvf::mac_to_string(mac));
        n->parent_mac = tlvf::mac_to_string(parent_mac);
    }
    n->radio_identifier = tlvf::mac_to_string(radio_identifier);
    n->hierarchy        = new_hierarchy;
    nodes[new_hierarchy].insert(std::make_pair(tlvf::mac_to_string(mac), n));

    if (radio_identifier != network_utils::ZERO_MAC) {
        std::string ruid_key = get_node_key(tlvf::mac_to_string(parent_mac), n->radio_identifier);
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
        auto it = nodes[i].find(tlvf::mac_to_string(mac));
        if (it != nodes[i].end()) {
            std::string ruid_key =
                get_node_key(it->second->parent_mac, it->second->radio_identifier);
            std::string node_mac = it->second->mac;

            if (last_accessed_node_mac == tlvf::mac_to_string(mac)) {
                last_accessed_node_mac = std::string();
                last_accessed_node     = nullptr;
            }

            // map may include 2 keys to same node - if so remove other key-node pair from map
            // if removed by mac
            if (tlvf::mac_to_string(mac) == node_mac) {
                it = nodes[i].erase(it);
                // if ruid_key exists for this node
                if (!ruid_key.empty()) {
                    nodes[i].erase(ruid_key);
                }
                // if removed by ruid_key
            } else if (tlvf::mac_to_string(mac) == ruid_key) {
                nodes[i].erase(node_mac);
            }

            return true;
        }
    }

    return false;
}

bool db::set_node_type(const std::string &mac, beerocks::eType type)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->set_type(type);
    return true;
}

beerocks::eType db::get_node_type(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return beerocks::TYPE_UNDEFINED;
    }
    return n->get_type();
}

bool db::set_local_slave_mac(const std::string &mac)
{
    if (!local_slave_mac.empty()) {
        LOG(WARNING) << "local_slave_mac != empty";
    }
    local_slave_mac = mac;
    return true;
}

std::string db::get_local_slave_mac() { return local_slave_mac; }

bool db::set_node_ipv4(const std::string &mac, const std::string &ipv4)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->ipv4 = ipv4;
    return true;
}

std::string db::get_node_ipv4(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::string();
    }
    return n->ipv4;
}

bool db::set_node_manufacturer(const std::string &mac, const std::string &manufacturer)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->manufacturer = manufacturer;
    return true;
}

std::string db::get_node_manufacturer(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::string();
    }
    return n->manufacturer;
}

int db::get_node_channel(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return 0;
    }
    return n->channel;
}

int db::get_hostap_operating_class(const sMacAddr &mac)
{
    auto mac_str = tlvf::mac_to_string(mac);
    auto n       = get_node(mac_str);
    if (!n) {
        LOG(WARNING) << "node " << mac_str << " does not exist!";
        return 0;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || !n->hostap) {
        LOG(WARNING) << "node " << mac_str << " is not a valid hostap!";
        return 0;
    }
    return n->hostap->operating_class;
}

bool db::set_node_vap_id(const std::string &mac, int8_t vap_id)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->vap_id = vap_id;
    return true;
}

int8_t db::get_node_vap_id(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return beerocks::IFACE_ID_INVALID;
    }
    return n->vap_id;
}

bool db::get_cs_op_flag(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->cs_op;
}

bool db::set_cs_op_flag(const std::string &mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cs_op = flag;
    return true;
}

bool db::get_cs_lb_flag(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->cs_lb;
}

bool db::set_cs_lb_flag(const std::string &mac, bool flag)
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

bool db::set_hostap_conf_restricted_channels(const std::string &hostap_mac,
                                             uint8_t *restricted_channels)
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

std::vector<uint8_t> db::get_hostap_conf_restricted_channels(const std::string &hostap_mac)
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
    const std::string &mac, beerocks::eBeaconMeasurementSupportLevel support_beacon_measurement)
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
db::get_node_beacon_measurement_support_level(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return beerocks::BEACON_MEAS_UNSUPPORTED;
    }
    return n->supports_beacon_measurement;
}

bool db::set_node_name(const std::string &mac, std::string name)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->name = name;
    return true;
}

std::string db::get_node_name(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::string();
    }
    return n->name;
}

bool db::set_node_state(const std::string &mac, beerocks::eNodeState state)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->state             = state;
    n->last_state_change = std::chrono::steady_clock::now();
    return true;
}

beerocks::eNodeState db::get_node_state(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::STATE_MAX;
    }
    return n->state;
}

bool db::set_node_operational_state(const std::string &bridge_mac, bool operational)
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

int8_t db::get_node_operational_state(const std::string &bridge_mac)
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

std::chrono::steady_clock::time_point db::get_last_state_change(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::chrono::steady_clock::time_point();
    }
    return n->last_state_change;
}

bool db::set_node_handoff_flag(const std::string &mac, bool handoff)
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

bool db::get_node_handoff_flag(const std::string &mac)
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

bool db::set_node_confined_flag(const std::string &mac, bool flag)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->confined = flag;
    return true;
}

bool db::get_node_confined_flag(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    return n->confined;
}

bool db::update_node_last_seen(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_seen = std::chrono::steady_clock::now();
    return true;
}

bool db::update_node_last_ping_sent(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_ping_sent = std::chrono::steady_clock::now();
    return true;
}

std::chrono::steady_clock::time_point db::get_node_last_seen(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::chrono::steady_clock::now();
    }

    return n->last_seen;
}

std::chrono::steady_clock::time_point db::get_node_last_ping_sent(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::chrono::steady_clock::now();
    }
    return n->last_ping_sent;
}

bool db::update_node_last_ping_received(const std::string &mac, int seq)
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

bool db::update_node_last_ping_received_avg(const std::string &mac, int total_seq)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->last_ping_avg_ms = n->last_ping_avg_ms_acc / total_seq;
    return true;
}

std::chrono::steady_clock::time_point db::get_node_last_ping_received(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::chrono::steady_clock::now();
    }
    return n->last_ping_received;
}

int db::get_node_last_ping_delta_ms(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_delta_ms;
}

int db::get_node_last_ping_min_ms(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_min_ms;
}

int db::get_node_last_ping_max_ms(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_max_ms;
}

int db::get_node_last_ping_avg_ms(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->last_ping_avg_ms;
}

std::unordered_map<sMacAddr, std::unordered_map<sMacAddr, son::node::link_metrics_data>> &
db::get_link_metric_data_map()
{
    return m_link_metric_data;
}

std::unordered_map<sMacAddr, son::node::ap_metrics_data> &db::get_ap_metric_data_map()
{
    return m_ap_metric_data;
}

bool db::set_hostap_active(const std::string &mac, bool active)
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

bool db::is_hostap_active(const std::string &mac)
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

bool db::set_hostap_backhaul_manager(const std::string &mac, bool is_backhaul_manager)
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

bool db::is_hostap_backhaul_manager(const std::string &mac)
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

bool db::is_ap_out_of_band(const std::string &mac, const std::string &sta_mac)
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

bool db::is_node_wireless(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    return utils::is_node_wireless(n->iface_type);
}

std::string db::node_to_string(const std::string &mac)
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
int db::get_node_hierarchy(const std::string &mac)
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

std::set<std::string> db::get_node_subtree(const std::string &mac)
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

std::string db::get_node_parent(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << "node " << mac << " does not exist!";
        return std::string();
    }
    return n->parent_mac;
}

std::string db::get_node_parent_hostap(const std::string &mac)
{
    std::string parent_backhaul = get_node_parent_backhaul(mac);
    if (is_node_wireless(parent_backhaul)) {
        return get_node_parent(parent_backhaul);
    } else {
        LOG(DEBUG) << "node " << parent_backhaul << " is not connected wirelessly";
        return std::string();
    }
}

std::string db::get_node_parent_backhaul(const std::string &mac)
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

std::string db::get_node_parent_ire(const std::string &mac)
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

std::string db::get_node_previous_parent(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << "node " << mac << " does not exist!";
        return std::string();
    }
    return n->previous_parent_mac;
}

std::set<std::string> db::get_node_siblings(const std::string &mac, int type)
{
    std::set<std::string> siblings;
    auto n = get_node(mac);

    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist";
        return siblings;
    }

    auto parent = get_node(n->parent_mac);
    if (!parent) {
        LOG(WARNING) << "parent for node " << mac << " does not exist";
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

std::set<std::string> db::get_node_children(const std::string &mac, int type, int state)
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

std::list<sMacAddr> db::get_1905_1_neighbors(const sMacAddr &al_mac)
{
    auto al_mac_str = tlvf::mac_to_string(al_mac);
    std::list<sMacAddr> neighbors_al_macs;
    auto all_al_macs = get_nodes(beerocks::TYPE_IRE);

    // According to IEEE 1905.1 a neighbor is defined as a first circle only, so we need to filter
    // out the childrens from second circle and above.
    for (const auto &al_mac_iter : all_al_macs) {
        if (get_node_parent_ire(al_mac_iter) == al_mac_str) {
            neighbors_al_macs.push_back(tlvf::mac_from_string(al_mac_iter));
        }
    }

    // Add the parent bridge as well to the neighbors list
    auto parent_bridge = get_node_parent_ire(tlvf::mac_to_string(al_mac));
    if (!parent_bridge.empty()) {
        neighbors_al_macs.push_back(tlvf::mac_from_string(parent_bridge));
    }

    // Add siblings Nodes
    auto siblings = get_node_siblings(al_mac_str, beerocks::TYPE_IRE);
    for (const auto &sibling : siblings) {
        neighbors_al_macs.push_back(tlvf::mac_from_string(sibling));
    }

    return neighbors_al_macs;
}

//
// Capabilities
//

const beerocks::message::sRadioCapabilities *
db::get_station_current_capabilities(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return nullptr;
    }
    return (&n->capabilities);
}

bool db::set_station_capabilities(const std::string &client_mac,
                                  const beerocks::message::sRadioCapabilities &sta_cap)
{
    auto n = get_node(client_mac);

    if (!n) {
        LOG(ERROR) << "client node not found " << client_mac;
        return false;
    }

    auto parent_radio = get_node_parent_radio(client_mac);

    if (parent_radio.empty()) {
        LOG(ERROR) << "parent radio node found for client " << client_mac;
        return false;
    }

    if (is_node_5ghz(parent_radio)) {
        n->m_sta_5ghz_capabilities       = sta_cap;
        n->m_sta_5ghz_capabilities.valid = true;
        n->capabilities                  = n->m_sta_5ghz_capabilities;

    } else {
        n->m_sta_24ghz_capabilities       = sta_cap;
        n->m_sta_24ghz_capabilities.valid = true;
        n->capabilities                   = n->m_sta_24ghz_capabilities;
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
        if (n->m_sta_5ghz_capabilities.valid == true) {
            return &n->m_sta_5ghz_capabilities;
        } else {
            return nullptr;
        }
    } else {
        if (n->m_sta_24ghz_capabilities.valid == true) {
            return &n->m_sta_24ghz_capabilities;
        } else {
            return nullptr;
        }
    }
}

bool db::set_hostap_ant_num(const std::string &mac, beerocks::eWiFiAntNum ant_num)
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

beerocks::eWiFiAntNum db::get_hostap_ant_num(const std::string &mac)
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

bool db::set_hostap_ant_gain(const std::string &mac, int ant_gain)
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

int db::get_hostap_ant_gain(const std::string &mac)
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

bool db::set_hostap_tx_power(const std::string &mac, int tx_power)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    n->hostap->tx_power = tx_power;
    return true;
}

int db::get_hostap_tx_power(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return -1;
    }
    return n->hostap->tx_power;
}

bool db::set_hostap_supported_channels(const std::string &mac,
                                       beerocks::message::sWifiChannel *channels, int length)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return false;
    }
    std::vector<beerocks::message::sWifiChannel> supported_channels_(channels, channels + length);
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
        LOG(ERROR) << "unknown frequency! channel:"
                   << int(n->hostap->supported_channels[0].channel);
        return false;
    }

    return true;
}

const std::vector<beerocks::message::sWifiChannel>
db::get_hostap_supported_channels(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return std::vector<beerocks::message::sWifiChannel>();
    } else if (n->get_type() != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(WARNING) << __FUNCTION__ << "node " << mac << " is not a valid hostap!";
        return std::vector<beerocks::message::sWifiChannel>();
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
               << " | bw = " << int(val.channel_bandwidth) << " | tx_pow = " << int(val.tx_pow)
               << " | noise = " << int(val.noise) << " [dbm]"
               << " | bss_overlap = " << int(val.bss_overlap) << std::endl;
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
    auto class_bw           = wireless_utils::operating_class_to_bandwidth(operating_class);
    // Update current channels
    for (auto c : channel_set) {
        auto channel = std::find_if(
            supported_channels.begin(), supported_channels.end(),
            [&c](const beerocks::message::sWifiChannel &ch) { return ch.channel == c; });
        if (channel != supported_channels.end()) {
            channel->tx_pow            = tx_power;
            channel->channel_bandwidth = class_bw;
        } else {
            beerocks::message::sWifiChannel ch;
            ch.channel           = c;
            ch.tx_pow            = tx_power;
            ch.channel_bandwidth = class_bw;
            supported_channels.push_back(ch);
        }
    }

    // Delete non-operable channels
    for (auto c : non_operable_channels) {
        auto channel = std::find_if(
            supported_channels.begin(), supported_channels.end(),
            [&c](const beerocks::message::sWifiChannel &ch) { return ch.channel == c; });
        if (channel != supported_channels.end())
            supported_channels.erase(channel);
    }

    set_hostap_supported_channels(radio_mac, &supported_channels[0], supported_channels.size());
    // dump new supported channels state
    // LOG(DEBUG) << "New supported channels for hostap" << radio_mac << " operating class "
    //            << int(operating_class) << std::endl
    //            << get_hostap_supported_channels_string(radio_mac);

    return true;
}

bool db::set_hostap_band_capability(const std::string &mac,
                                    beerocks::eRadioBandCapability capability)
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

beerocks::eRadioBandCapability db::get_hostap_band_capability(const std::string &mac)
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

bool db::capability_check(const std::string &mac, int channel)
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

bool db::get_node_5ghz_support(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->supports_5ghz;
}

bool db::get_node_24ghz_support(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->supports_24ghz;
}

bool db::is_node_24ghz(const std::string &mac)
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

bool db::is_node_5ghz(const std::string &mac)
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

bool db::update_node_failed_5ghz_steer_attempt(const std::string &mac)
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

bool db::update_node_failed_24ghz_steer_attempt(const std::string &mac)
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

bool db::update_node_11v_responsiveness(const std::string &mac, bool success)
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

bool db::get_node_11v_capability(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    return n->supports_11v;
}

bool db::set_hostap_iface_id(const std::string &mac, int8_t iface_id)
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

int8_t db::get_hostap_iface_id(const std::string &mac)
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
    if (!has_node(tlvf::mac_from_string(bssid)) &&
        !add_virtual_node(tlvf::mac_from_string(bssid), tlvf::mac_from_string(radio_mac))) {
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

bool db::is_vap_on_steer_list(const std::string &bssid)
{
    if (config.load_steer_on_vaps.empty()) {
        return true;
    }

    auto vap_name = get_hostap_iface_name(bssid);
    if (vap_name == "INVALID") {
        LOG(ERROR) << "vap name is invalid for bssid " << bssid;
        return false;
    }

    auto vap_id = get_hostap_vap_id(bssid);
    if (vap_id == IFACE_ID_INVALID) {
        LOG(ERROR) << "vap id is invalid for bssid " << bssid;
        return false;
    }

    vap_name         = utils::get_iface_string_from_iface_vap_ids(vap_name, vap_id);
    auto &steer_vaps = config.load_steer_on_vaps;
    if (steer_vaps.find(vap_name) == std::string::npos) {
        return false;
    }
    return true;
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

    auto it = std::find_if(
        n->hostap->vaps_info.begin(), n->hostap->vaps_info.end(),
        [&](const std::pair<int8_t, sVapElement> &vap) { return vap.second.ssid == ssid; });

    if (it == n->hostap->vaps_info.end()) {
        // no vap with same ssid is found
        return std::string();
    }
    return it->second.mac;
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

bool db::get_hostap_repeater_mode_flag(const std::string &mac)
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

bool db::set_hostap_repeater_mode_flag(const std::string &mac, bool flag)
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

bool db::set_hostap_iface_name(const std::string &mac, std::string iface_name)
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

std::string db::get_hostap_iface_name(const std::string &mac)
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

bool db::set_hostap_iface_type(const std::string &mac, beerocks::eIfaceType iface_type)
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

beerocks::eIfaceType db::get_hostap_iface_type(const std::string &mac)
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

bool db::set_node_backhaul_iface_type(const std::string &mac, beerocks::eIfaceType iface_type)
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

bool db::set_hostap_driver_version(const std::string &mac, std::string version)
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

std::string db::get_hostap_driver_version(const std::string &mac)
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

beerocks::eIfaceType db::get_node_backhaul_iface_type(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::IFACE_TYPE_UNSUPPORTED;
    }
    return n->iface_type;
}

std::string db::get_5ghz_sibling_hostap(const std::string &mac)
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

bool db::set_hostap_activity_mode(const std::string &mac, eApActiveMode ap_activity_mode)
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

beerocks::eApActiveMode db::get_hostap_activity_mode(const std::string &mac)
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

bool db::set_radar_hit_stats(const std::string &mac, uint8_t channel, uint8_t bw, bool is_csa_entry)
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

bool db::set_supported_channel_radar_affected(const std::string &mac, std::vector<uint8_t> channels,
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
    if (channels_count < 1) {
        LOG(ERROR) << "the given channel list must contain at least one value";
        return false;
    }
    auto it =
        find_if(std::begin(n->hostap->supported_channels), std::end(n->hostap->supported_channels),
                [&](beerocks::message::sWifiChannel supported_channel) {
                    return supported_channel.channel == *channels.begin();
                });

    if (it == std::end(n->hostap->supported_channels)) {
        LOG(ERROR) << "channels not found ,not suppose to happen!!";
        return false;
    }
    std::for_each(it, std::next(it, channels_count),
                  [&](beerocks::message::sWifiChannel &supported_channel) {
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

bool db::set_hostap_is_dfs(const std::string &mac, bool enable)
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

bool db::get_hostap_is_dfs(const std::string &mac)
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

bool db::set_hostap_cac_completed(const std::string &mac, bool enable)
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

bool db::get_hostap_cac_completed(const std::string &mac)
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
bool db::set_hostap_is_on_fail_safe(const std::string &mac, bool enable)
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

bool db::get_hostap_is_on_fail_safe(const std::string &mac)
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

bool db::set_hostap_is_on_sub_band(const std::string &mac, bool enable)
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

bool db::get_hostap_is_on_sub_band(const std::string &mac)
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

bool db::set_hostap_on_dfs_reentry(const std::string &mac, bool enable)
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

bool db::get_hostap_on_dfs_reentry(const std::string &mac)
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

bool db::set_hostap_dfs_reentry_clients(const std::string &mac,
                                        const std::set<std::string> &dfs_reentry_clients)
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
             [&](const std::string &dfs_reentry_client) {
                 LOG(DEBUG) << "dfs_reentry_client = " << dfs_reentry_client;
             });
    return true;
}

std::set<std::string> db::get_hostap_dfs_reentry_clients(const std::string &mac)
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
             [&](const std::string &dfs_reentry_client) {
                 LOG(DEBUG) << "dfs_reentry_client = " << dfs_reentry_client;
             });
    return n->hostap->dfs_reentry_clients;
}

bool db::clear_hostap_dfs_reentry_clients(const std::string &mac)
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

bool db::set_hostap_is_acs_enabled(const std::string &mac, bool enable)
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

bool db::get_hostap_is_acs_enabled(const std::string &mac)
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
// Channel Scan
//
bool db::set_channel_scan_is_enabled(const sMacAddr &mac, bool enable)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    hostap->continuous_scan_config.is_enabled = enable;
    return true;
}

bool db::get_channel_scan_is_enabled(const sMacAddr &mac)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    return hostap->continuous_scan_config.is_enabled;
}

bool db::set_channel_scan_interval_sec(const sMacAddr &mac, int interval_sec)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    hostap->continuous_scan_config.interval_sec = interval_sec;
    return true;
}

int db::get_channel_scan_interval_sec(const sMacAddr &mac)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    return hostap->continuous_scan_config.interval_sec;
}

bool db::set_channel_scan_in_progress(const sMacAddr &mac, bool scan_in_progress, bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    LOG(DEBUG) << (single_scan ? "single" : "continuous") << " scan "
               << (scan_in_progress ? "is" : "isn't") << " in progress.";
    (single_scan ? hostap->single_scan_status : hostap->continuous_scan_status).scan_in_progress =
        scan_in_progress;

    return true;
}

bool db::get_channel_scan_in_progress(const sMacAddr &mac, bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    return (single_scan ? hostap->single_scan_status : hostap->continuous_scan_status)
        .scan_in_progress;
}

bool db::set_channel_scan_results_status(const sMacAddr &mac,
                                         beerocks::eChannelScanStatusCode error_code,
                                         bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    LOG(DEBUG) << (single_scan ? "single" : "continuous")
               << " scan, last scan error code = " << int(error_code);

    (single_scan ? hostap->single_scan_status : hostap->continuous_scan_status)
        .last_scan_error_code = error_code;

    return true;
}

beerocks::eChannelScanStatusCode db::get_channel_scan_results_status(const sMacAddr &mac,
                                                                     bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return beerocks::eChannelScanStatusCode::INTERNAL_FAILURE;
    }

    return (single_scan ? hostap->single_scan_status : hostap->continuous_scan_status)
        .last_scan_error_code;
}

bool db::set_channel_scan_dwell_time_msec(const sMacAddr &mac, int dwell_time_msec,
                                          bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    LOG(DEBUG) << (single_scan ? "single" : "continuous")
               << ", dwell time msec = " << dwell_time_msec;

    if (dwell_time_msec < 0) {
        LOG(ERROR) << "Invalid dwell time: " << dwell_time_msec
                   << ". Only positive values are supported!";
        return false;
    }

    (single_scan ? hostap->single_scan_config : hostap->continuous_scan_config).dwell_time_msec =
        dwell_time_msec;

    return true;
}

int db::get_channel_scan_dwell_time_msec(const sMacAddr &mac, bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    return (single_scan ? hostap->single_scan_config : hostap->continuous_scan_config)
        .dwell_time_msec;
}

bool db::is_channel_scan_pool_supported(const sMacAddr &mac,
                                        const std::unordered_set<uint8_t> &channel_pool)
{
    auto supported_channels = get_hostap_supported_channels(tlvf::mac_to_string(mac));
    for (const auto &channel : channel_pool) {
        auto found_channel =
            std::find_if(supported_channels.begin(), supported_channels.end(),
                         [&channel](const beerocks::message::sWifiChannel &supported_channel) {
                             return supported_channel.channel == channel;
                         });
        if (found_channel == supported_channels.end()) {
            LOG(ERROR) << "channel #" << int(channel) << " is not supported";
            return false;
        }
    }
    return true;
}

bool db::set_channel_scan_pool(const sMacAddr &mac, const std::unordered_set<uint8_t> &channel_pool,
                               bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    if (!is_channel_scan_pool_supported(mac, channel_pool)) {
        LOG(ERROR) << "setting channel pool failed, one of the channels is not supported!";
        return false;
    }

    (single_scan ? hostap->single_scan_config : hostap->continuous_scan_config).channel_pool =
        channel_pool;

    LOG(DEBUG) << (single_scan ? "single" : "continuous")
               << " scan, setting channel pool succeeded!";

    return true;
}

const std::unordered_set<uint8_t> &db::get_channel_scan_pool(const sMacAddr &mac, bool single_scan)
{
    static std::unordered_set<uint8_t> empty;

    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return empty;
    }

    return (single_scan ? hostap->single_scan_config : hostap->continuous_scan_config).channel_pool;
}

bool db::is_channel_in_pool(const sMacAddr &mac, uint8_t channel, bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    auto &pool =
        (single_scan ? hostap->single_scan_config : hostap->continuous_scan_config).channel_pool;
    return pool.find(channel) != pool.end();
}

bool db::clear_channel_scan_results(const sMacAddr &mac, bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    (single_scan ? hostap->single_scan_results : hostap->continuous_scan_results).clear();

    return true;
}

bool db::add_channel_scan_results(const sMacAddr &mac, const sChannelScanResults &scan_result,
                                  bool single_scan)
{
    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return false;
    }

    (single_scan ? hostap->single_scan_results : hostap->continuous_scan_results)
        .push_back(scan_result);

    return true;
}

const std::list<sChannelScanResults> &db::get_channel_scan_results(const sMacAddr &mac,
                                                                   bool single_scan)
{
    static std::list<sChannelScanResults> empty;

    auto hostap = get_hostap_by_mac(mac);
    if (!hostap) {
        LOG(ERROR) << "unable to get hostap";
        return empty;
    }

    return (single_scan ? hostap->single_scan_results : hostap->continuous_scan_results);
}

//
// Client Persistent Data
//
bool db::is_client_in_persistent_db(const sMacAddr &mac)
{
    auto client_db_entry = client_db_entry_from_mac(mac);

    return bpl::db_has_entry(type_to_string(beerocks::eType::TYPE_CLIENT), client_db_entry);
}

bool db::add_client_to_persistent_db(const sMacAddr &mac,
                                     const std::unordered_map<std::string, std::string> &params)
{
    // if persistent db is disabled
    if (!config.persistent_db) {
        LOG(ERROR) << "persistent db is disabled";
        return false;
    }

    auto db_entry = client_db_entry_from_mac(mac);

    if (bpl::db_has_entry(type_to_string(beerocks::eType::TYPE_CLIENT), db_entry)) {
        // if entry already exists in DB
        if (!remove_client_entry_and_update_counter(db_entry)) {
            LOG(ERROR) << "failed to remove client entry " << db_entry
                       << "from persistent db (for re-adding)";
            return false;
        }
    } else if (bpl::db_has_entry(std::string(), db_entry)) {
        // if entry exists in db but with different type
        LOG(ERROR) << "client entry cannot be added to persistent db, " << db_entry
                   << " already exists but with different type";
        return false;
    }
    // add entry to the persistent db
    if (!add_client_entry_and_update_counter(db_entry, params)) {
        LOG(ERROR) << "failed to add client entry " << db_entry << " to persistent db";
        return false;
    }

    LOG(DEBUG) << "added client entry " << db_entry
               << " to persistent db, total clients count in persisttent-db: "
               << m_persistent_db_clients_count;

    return true;
}

std::chrono::steady_clock::time_point db::get_client_parameters_last_edit(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return std::chrono::steady_clock::time_point::min();
    }

    return node->client_parameters_last_edit;
}

bool db::set_client_time_life_delay(const sMacAddr &mac,
                                    const std::chrono::seconds &time_life_delay_sec,
                                    bool save_to_persistent_db)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    LOG(DEBUG) << "time_life_delay_sec = " << time_life_delay_sec.count();

    auto timestamp = std::chrono::steady_clock::now();
    if (save_to_persistent_db) {
        // if persistent db is disabled
        if (!config.persistent_db) {
            LOG(DEBUG) << "persistent db is disabled";
        } else {
            LOG(DEBUG) << "configuring persistent-db, timelife = " << time_life_delay_sec.count();

            std::unordered_map<std::string, std::string> values_map;
            values_map[TIMESTAMP_STR]      = timestamp_to_string_seconds(timestamp);
            values_map[TIMELIFE_DELAY_STR] = std::to_string(time_life_delay_sec.count());

            // update the persistent db
            if (!update_client_entry_in_persistent_db(mac, values_map)) {
                LOG(ERROR) << "failed to update client entry in persistent-db to for " << mac;
                return false;
            }
        }
    }

    node->client_time_life_delay_sec  = time_life_delay_sec;
    node->client_parameters_last_edit = timestamp;

    return true;
}

std::chrono::seconds db::get_client_time_life_delay(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return std::chrono::seconds::zero();
    }

    return node->client_time_life_delay_sec;
}

bool db::set_client_stay_on_initial_radio(const sMacAddr &mac, bool stay_on_initial_radio,
                                          bool save_to_persistent_db)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    LOG(DEBUG) << "stay_on_initial_radio = " << stay_on_initial_radio;

    auto timestamp = std::chrono::steady_clock::now();
    if (save_to_persistent_db) {
        // if persistent db is disabled
        if (!config.persistent_db) {
            LOG(DEBUG) << "persistent db is disabled";
        } else {
            LOG(DEBUG) << "configuring persistent-db, initial_radio_enable = "
                       << stay_on_initial_radio;

            std::unordered_map<std::string, std::string> values_map;
            values_map[TIMESTAMP_STR]            = timestamp_to_string_seconds(timestamp);
            values_map[INITIAL_RADIO_ENABLE_STR] = std::to_string(stay_on_initial_radio);

            // update the persistent db
            if (!update_client_entry_in_persistent_db(mac, values_map)) {
                LOG(ERROR) << "failed to update client entry in persistent-db to for " << mac;
                return false;
            }
        }
    }

    node->client_stay_on_initial_radio =
        (stay_on_initial_radio) ? eTriStateBool::ENABLE : eTriStateBool::DISABLE;
    node->client_parameters_last_edit = timestamp;

    return true;
}

eTriStateBool db::get_client_stay_on_initial_radio(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return eTriStateBool::NOT_CONFIGURED;
    }

    return node->client_stay_on_initial_radio;
}

bool db::set_client_initial_radio(const sMacAddr &mac, const sMacAddr &initial_radio_mac,
                                  bool save_to_persistent_db)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    LOG(DEBUG) << "initial_radio = " << initial_radio_mac;

    auto timestamp = std::chrono::steady_clock::now();
    if (save_to_persistent_db) {
        // if persistent db is disabled
        if (!config.persistent_db) {
            LOG(DEBUG) << "persistent db is disabled";
        } else {
            LOG(DEBUG) << "configuring persistent-db, initial_radio = " << initial_radio_mac;

            std::unordered_map<std::string, std::string> values_map;
            values_map[TIMESTAMP_STR]     = timestamp_to_string_seconds(timestamp);
            values_map[INITIAL_RADIO_STR] = tlvf::mac_to_string(initial_radio_mac);

            // update the persistent db
            if (!update_client_entry_in_persistent_db(mac, values_map)) {
                LOG(ERROR) << "failed to update client entry in persistent-db to for " << mac;
                return false;
            }
        }
    }

    node->client_initial_radio        = initial_radio_mac;
    node->client_parameters_last_edit = timestamp;

    return true;
}

sMacAddr db::get_client_initial_radio(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return network_utils::ZERO_MAC;
    }

    return node->client_initial_radio;
}

bool db::set_client_stay_on_selected_band(const sMacAddr &mac, bool stay_on_selected_band,
                                          bool save_to_persistent_db)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    LOG(DEBUG) << "stay_on_selected_band = " << stay_on_selected_band;

    auto timestamp = std::chrono::steady_clock::now();
    if (save_to_persistent_db) {
        // if persistent db is disabled
        if (!config.persistent_db) {
            LOG(DEBUG) << "persistent db is disabled";
        } else {
            LOG(DEBUG) << "configuring persistent-db, selected_band_enable = "
                       << stay_on_selected_band;

            std::unordered_map<std::string, std::string> values_map;
            values_map[TIMESTAMP_STR]            = timestamp_to_string_seconds(timestamp);
            values_map[SELECTED_BAND_ENABLE_STR] = std::to_string(stay_on_selected_band);

            // update the persistent db
            if (!update_client_entry_in_persistent_db(mac, values_map)) {
                LOG(ERROR) << "failed to update client entry in persistent-db to for " << mac;
                return false;
            }
        }
    }

    node->client_stay_on_selected_band =
        (stay_on_selected_band) ? eTriStateBool::ENABLE : eTriStateBool::DISABLE;
    node->client_parameters_last_edit = timestamp;

    return true;
}

eTriStateBool db::get_client_stay_on_selected_band(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return eTriStateBool::NOT_CONFIGURED;
    }

    return node->client_stay_on_selected_band;
}

bool db::set_client_selected_bands(const sMacAddr &mac, beerocks::eFreqType selected_bands,
                                   bool save_to_persistent_db)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    LOG(DEBUG) << "selected_band = " << int(selected_bands);

    auto timestamp = std::chrono::steady_clock::now();
    if (save_to_persistent_db) {
        // if persistent db is disabled
        if (!config.persistent_db) {
            LOG(DEBUG) << "persistent db is disabled";
        } else {
            LOG(DEBUG) << ", configuring persistent-db, selected_bands = " << int(selected_bands);

            std::unordered_map<std::string, std::string> values_map;
            values_map[TIMESTAMP_STR]      = timestamp_to_string_seconds(timestamp);
            values_map[SELECTED_BANDS_STR] = std::to_string(selected_bands);

            // update the persistent db
            if (!update_client_entry_in_persistent_db(mac, values_map)) {
                LOG(ERROR) << "failed to update client entry in persistent-db to for " << mac;
                return false;
            }
        }
    }

    node->client_selected_bands       = selected_bands;
    node->client_parameters_last_edit = timestamp;

    return true;
}

beerocks::eFreqType db::get_client_selected_bands(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return beerocks::eFreqType::FREQ_UNKNOWN;
    }

    return node->client_selected_bands;
}

bool db::clear_client_persistent_db(const sMacAddr &mac)
{
    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    LOG(DEBUG) << "setting client " << mac << " runtime info to default values";

    node->client_parameters_last_edit  = std::chrono::steady_clock::time_point::min();
    node->client_time_life_delay_sec   = std::chrono::seconds::zero();
    node->client_stay_on_initial_radio = eTriStateBool::NOT_CONFIGURED;
    node->client_initial_radio         = network_utils::ZERO_MAC;
    node->client_stay_on_selected_band = eTriStateBool::NOT_CONFIGURED;
    node->client_selected_bands        = beerocks::eFreqType::FREQ_UNKNOWN;

    // if persistent db is enabled
    if (config.persistent_db) {
        auto db_entry = client_db_entry_from_mac(mac);
        if (!bpl::db_has_entry(type_to_string(beerocks::eType::TYPE_CLIENT), db_entry)) {
            LOG(DEBUG) << "client entry does not exist in persistent-db for " << db_entry;
            return true;
        }

        LOG(DEBUG) << "removing client entry " << db_entry << " from persistent db";
        if (!remove_client_entry_and_update_counter(db_entry)) {
            LOG(ERROR) << "failed to remove client entry " << db_entry;
            return false;
        }
    }

    return true;
}

bool db::update_client_persistent_db(const sMacAddr &mac)
{
    // if persistent db is disabled
    if (!config.persistent_db) {
        LOG(ERROR) << "persistent db is disabled";
        return false;
    }

    auto node = get_node_verify_type(mac, beerocks::TYPE_CLIENT);
    if (!node) {
        LOG(ERROR) << "client node not found for mac " << mac;
        return false;
    }

    // any persistent parameter update also sets the last-edit timestamp
    // if it is with default value - no other persistent configuration was performed
    if (node->client_parameters_last_edit == std::chrono::steady_clock::time_point::min()) {
        LOG(DEBUG) << "persistent client parameters are empty for " << mac
                   << ", no need to update persistent-db";
        return true;
    }

    std::unordered_map<std::string, std::string> values_map;

    //fill values map of client persistent params
    values_map[TIMESTAMP_STR] = timestamp_to_string_seconds(node->client_parameters_last_edit);

    if (node->client_time_life_delay_sec != std::chrono::seconds::zero()) {
        LOG(DEBUG) << "setting client time-life-delay in persistent-db to for " << mac << " to "
                   << node->client_time_life_delay_sec.count();
        values_map[TIMELIFE_DELAY_STR] = std::to_string(node->client_time_life_delay_sec.count());
    }

    if (node->client_stay_on_initial_radio != eTriStateBool::NOT_CONFIGURED) {
        auto enable = (node->client_stay_on_initial_radio == eTriStateBool::ENABLE);
        LOG(DEBUG) << "setting client stay-on-initial-radio in persistent-db to for " << mac
                   << " to " << enable;
        values_map[INITIAL_RADIO_ENABLE_STR] = std::to_string(enable);
    }

    if (node->client_initial_radio != network_utils::ZERO_MAC) {
        LOG(DEBUG) << "setting client initial-radio in persistent-db to for " << mac << " to "
                   << node->client_initial_radio;
        values_map[INITIAL_RADIO_STR] = tlvf::mac_to_string(node->client_initial_radio);
    }

    if (node->client_stay_on_selected_band != eTriStateBool::NOT_CONFIGURED) {
        auto enable = (node->client_stay_on_selected_band == eTriStateBool::ENABLE);
        LOG(DEBUG) << "setting client stay-on-selected-band in persistent-db to for " << mac
                   << " to " << enable;
        values_map[SELECTED_BAND_ENABLE_STR] = std::to_string(enable);
    }

    if (node->client_selected_bands != beerocks::eFreqType::FREQ_UNKNOWN) {
        LOG(DEBUG) << "setting client selected-bands in persistent-db to for " << mac << " to "
                   << node->client_selected_bands;
        values_map[SELECTED_BANDS_STR] = std::to_string(node->client_selected_bands);
    }

    // update the persistent db
    if (!update_client_entry_in_persistent_db(mac, values_map)) {
        LOG(ERROR) << "failed to update client entry in persistent-db to for " << mac;
        return false;
    }

    LOG(DEBUG) << "client successfully updated in persistent-db for " << mac;

    return true;
}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
db::load_persistent_db_clients()
{
    // if persistent db is disabled
    if (!config.persistent_db) {
        LOG(ERROR) << "persistent db is disabled";
        return {};
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> clients;
    if (!bpl::db_get_entries_by_type(type_to_string(beerocks::eType::TYPE_CLIENT), clients)) {
        LOG(ERROR) << "failed to get all clients from persistent DB";
        return {};
    }

    if (clients.empty()) {
        LOG(DEBUG) << "persistent DB doesn't exist (or empty) or doesn't contain clients";
        return {};
    }

    return clients;
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
                it = cli_debug_sockets.erase(it);
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
                it = bml_listeners_sockets.erase(it);
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

bool db::set_bml_topology_update_enable(Socket *sd, bool update_enable)
{
    if (!sd) {
        return false;
    }
    auto it = std::find_if(bml_listeners_sockets.begin(), bml_listeners_sockets.end(),
                           [&](const sBmlListener &element) { return element.sd == sd; });
    if (it == bml_listeners_sockets.end()) {
        LOG(ERROR) << "set_bml_topology_update_enable failed!, cannot find bml listener";
        return false;
    }
    it->topology_updates = update_enable;
    return true;
}

bool db::get_bml_topology_update_enable(Socket *sd)
{
    if (!sd) {
        return false;
    }
    auto it = std::find_if(bml_listeners_sockets.begin(), bml_listeners_sockets.end(),
                           [&](const sBmlListener &element) { return element.sd == sd; });
    if (it == bml_listeners_sockets.end()) {
        LOG(ERROR) << "set_bml_topology_update_enable failed!, cannot find bml listener";
        return false;
    }
    return it->topology_updates;
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
    for (const auto &listener : bml_listeners_sockets) {
        listener_exist = listener.map_updates || listener.stats_updates ||
                         listener.events_updates || listener.topology_updates;
        if (listener_exist) {
            return true;
        }
    }
    return false;
}

//
// Measurements
//
bool db::set_node_beacon_measurement(const std::string &sta_mac, std::string ap_mac, int8_t rcpi,
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

bool db::get_node_beacon_measurement(const std::string &sta_mac, std::string ap_mac, int8_t &rcpi,
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

bool db::set_node_cross_rx_rssi(const std::string &sta_mac, std::string ap_mac, int8_t rssi,
                                int8_t rx_packets)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        return false;
    }
    sta->set_cross_rx_rssi(ap_mac, rssi, rx_packets);
    return true;
}

bool db::get_node_cross_rx_rssi(const std::string &sta_mac, std::string ap_mac, int8_t &rssi,
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

bool db::set_node_cross_rx_phy_rate_100kb(const std::string &mac, uint16_t rx_phy_rate_100kb)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cross_rx_phy_rate_100kb = rx_phy_rate_100kb;
    return true;
}

bool db::set_node_cross_tx_phy_rate_100kb(const std::string &mac, uint16_t tx_phy_rate_100kb)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cross_tx_phy_rate_100kb = tx_phy_rate_100kb;
    return true;
}

uint16_t db::get_node_cross_rx_phy_rate_100kb(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->cross_rx_phy_rate_100kb;
}

uint16_t db::get_node_cross_tx_phy_rate_100kb(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->cross_tx_phy_rate_100kb;
}

bool db::clear_node_cross_rssi(const std::string &sta_mac)
{
    auto sta = get_node(sta_mac);
    if (sta == nullptr) {
        return false;
    }
    sta->clear_cross_rssi();
    return true;
}

bool db::set_node_cross_estimated_tx_phy_rate(const std::string &mac, double phy_rate)
{
    auto n = get_node(mac);
    if (!n) {
        return false;
    }
    n->cross_estimated_tx_phy_rate = phy_rate;
    return true;
}

double db::get_node_cross_estimated_tx_phy_rate(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->cross_estimated_tx_phy_rate;
}

bool db::set_hostap_stats_info(const std::string &mac, beerocks_message::sApStatsParams *params)
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

void db::clear_hostap_stats_info(const std::string &mac) { set_hostap_stats_info(mac, nullptr); }

bool db::set_node_stats_info(const std::string &mac, beerocks_message::sStaStatsParams *params)
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

void db::clear_node_stats_info(const std::string &mac) { set_node_stats_info(mac, nullptr); }

int db::get_hostap_stats_measurement_duration(const std::string &mac)
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

std::chrono::steady_clock::time_point db::get_node_stats_info_timestamp(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return std::chrono::steady_clock::time_point();
    }
    return n->stats_info->timestamp;
}

std::chrono::steady_clock::time_point db::get_hostap_stats_info_timestamp(const std::string &mac)
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

uint32_t db::get_node_rx_bytes(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_bytes;
}

uint32_t db::get_node_tx_bytes(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_bytes;
}

uint32_t db::get_hostap_total_sta_rx_bytes(const std::string &mac)
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

uint32_t db::get_hostap_total_sta_tx_bytes(const std::string &mac)
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

double db::get_node_rx_bitrate(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return (1000 * 8 * double(n->stats_info->rx_bytes) / n->stats_info->stats_delta_ms) / 1e+6;
}

double db::get_node_tx_bitrate(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return (1000 * 8 * double(n->stats_info->tx_bytes) / n->stats_info->stats_delta_ms) / 1e+6;
}

uint16_t db::get_node_rx_phy_rate_100kb(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_phy_rate_100kb;
}

uint16_t db::get_node_tx_phy_rate_100kb(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_phy_rate_100kb;
}

int db::get_hostap_channel_load_percent(const std::string &mac)
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

int db::get_hostap_total_client_tx_load_percent(const std::string &mac)
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

int db::get_hostap_total_client_rx_load_percent(const std::string &mac)
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

int db::get_node_rx_load_percent(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_load_percent;
}

int db::get_node_tx_load_percent(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_load_percent;
}

int8_t db::get_load_rx_rssi(const std::string &sta_mac)
{
    auto n = get_node(sta_mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_rssi;
}

uint16_t db::get_load_rx_phy_rate_100kb(const std::string &sta_mac)
{
    auto n = get_node(sta_mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->rx_phy_rate_100kb;
}

uint16_t db::get_load_tx_phy_rate_100kb(const std::string &sta_mac)
{
    auto n = get_node(sta_mac);
    if (!n) {
        return -1;
    }
    return n->stats_info->tx_phy_rate_100kb;
}

bool db::set_measurement_delay(const std::string &mac, int measurement_delay)
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

int db::get_measurement_delay(const std::string &mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return -1;
    }
    //LOG(DEBUG) << "get_measurement_delay: mac " << mac << " n->measurement_delay = " << int(n->measurement_delay);
    return n->measurement_delay;
}

std::chrono::steady_clock::time_point db::get_measurement_sent_timestamp(const std::string &mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return std::chrono::steady_clock::time_point();
    }
    //LOG(DEBUG) << "get_measurement_dry_run: mac " << mac << " n->measurement_dry_run" ;
    return n->measurement_sent_timestamp;
}

bool db::set_measurement_sent_timestamp(const std::string &mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return false;
    }
    n->measurement_sent_timestamp = std::chrono::steady_clock::now();
    LOG(DEBUG) << "set_measurement_sent_timestamp: mac " << mac;
    return true;
}

int db::get_measurement_recv_delta(const std::string &mac)
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

bool db::set_measurement_recv_delta(const std::string &mac)
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

int db::get_measurement_window_size(const std::string &mac)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return -1;
    }
    return n->measurement_window_size;
}

bool db::set_measurement_window_size(const std::string &mac, int window_size)
{
    std::shared_ptr<node> n = get_node(mac);
    if (!n) {
        return false;
    }
    n->measurement_window_size = window_size;
    return true;
}

bool db::get_hostap_exclude_from_steering_flag(const std::string &mac)
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

bool db::set_hostap_exclude_from_steering_flag(const std::string &mac, bool flag)
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
            if (channel <= 13)
                n->hostap->operating_class = 81;
            else if (channel <= 48)
                n->hostap->operating_class = 115;
            else
                n->hostap->operating_class = 121;
        } else {
            LOG(ERROR) << __FUNCTION__ << " - node " << mac << " is null!";
            return false;
        }
    }

    LOG(INFO) << "set node " << mac << " to channel=" << channel << ", bw=" << int(bw)
              << ", channel_ext_above_secondary=" << int(channel_ext_above_secondary)
              << ", channel_ext_above_primary=" << int(channel_ext_above_primary)
              << ", vht_center_frequency=" << int(vht_center_frequency);

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

beerocks::eWiFiBandwidth db::get_node_bw(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return beerocks::BANDWIDTH_MAX;
    }
    return n->bandwidth;
}

bool db::get_node_channel_ext_above_secondary(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    return n->channel_ext_above_secondary;
}

bool db::get_hostap_channel_ext_above_primary(const std::string &hostap_mac)
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

int db::get_node_bw_int(const std::string &mac)
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

uint16_t db::get_hostap_vht_center_frequency(const std::string &mac)
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

bool db::assign_association_handling_task_id(const std::string &mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->association_handling_task_id = new_task_id;
    return true;
}

int db::get_association_handling_task_id(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->association_handling_task_id;
}

bool db::assign_steering_task_id(const std::string &mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->steering_task_id = new_task_id;
    return true;
}

int db::get_steering_task_id(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->steering_task_id;
}

bool db::assign_roaming_task_id(const std::string &mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->roaming_task_id = new_task_id;
    return true;
}

int db::get_roaming_task_id(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->roaming_task_id;
}

bool db::assign_load_balancer_task_id(const std::string &mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->load_balancer_task_id = new_task_id;
    return true;
}

int db::get_load_balancer_task_id(const std::string &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return -1;
    }
    return n->load_balancer_task_id;
}

bool db::assign_client_locating_task_id(const std::string &mac, int new_task_id,
                                        bool new_connection)
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

int db::get_client_locating_task_id(const std::string &mac, bool new_connection)
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

bool db::assign_ire_4addr_mode_transition_task_id(const std::string &mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << mac << " does not exist!";
        return false;
    }
    n->ire_4addr_mode_transition_task_id = new_task_id;
    return true;
}

int db::get_ire_4addr_mode_transition_task_id(const std::string &mac)
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

bool db::assign_dynamic_channel_selection_task_id(const sMacAddr &mac, int new_task_id)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << tlvf::mac_to_string(mac)
                     << " does not exist!";
        return false;
    }
    n->dynamic_channel_selection_task_id = new_task_id;
    return true;
}

int db::get_dynamic_channel_selection_task_id(const sMacAddr &mac)
{
    auto n = get_node(mac);
    if (!n) {
        LOG(WARNING) << __FUNCTION__ << " - node " << tlvf::mac_to_string(mac)
                     << " does not exist!";
        return -1;
    }
    return n->dynamic_channel_selection_task_id;
}

void db::lock() { db_mutex.lock(); }

void db::unlock() { db_mutex.unlock(); }

void db::add_bss_info_configuration(const sMacAddr &al_mac,
                                    const wireless_utils::sBssInfoConf &bss_info)
{
    bss_infos[al_mac].push_back(bss_info);
}

std::list<wireless_utils::sBssInfoConf> &db::get_bss_info_configuration(const sMacAddr &al_mac)
{
    // If al_mac not exist, it will be added, and return empty list
    return bss_infos[al_mac];
}

void db::clear_bss_info_configuration() { bss_infos.clear(); }

void db::clear_bss_info_configuration(const sMacAddr &al_mac) { bss_infos[al_mac].clear(); }

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
    std::string key = mac == network_utils::ZERO_MAC ? std::string() : tlvf::mac_to_string(mac);
    return get_node(key);
}

std::shared_ptr<node> db::get_node(sMacAddr al_mac, sMacAddr ruid)
{
    std::string key = std::string();
    if (al_mac != network_utils::ZERO_MAC && ruid != network_utils::ZERO_MAC)
        key = tlvf::mac_to_string(al_mac) + tlvf::mac_to_string(ruid);

    return get_node(key);
}

std::shared_ptr<node> db::get_node_verify_type(const sMacAddr &mac, beerocks::eType type)
{
    auto node = get_node(mac);
    if (!node) {
        LOG(ERROR) << "node not found for mac " << mac;
        return nullptr;
    } else if (node->get_type() != type) {
        LOG(ERROR) << "node " << mac << " type(" << node->get_type() << ") != requested-type("
                   << type << ")";
        return nullptr;
    }

    return node;
}

std::shared_ptr<node::radio> db::get_hostap_by_mac(const sMacAddr &mac)
{
    auto n = get_node(mac);
    beerocks::eType t;
    if (!n) {
        LOG(ERROR) << "node not found.... ";
        return nullptr;
    } else if ((t = n->get_type()) != beerocks::TYPE_SLAVE || n->hostap == nullptr) {
        LOG(ERROR) << "node " << tlvf::mac_to_string(mac) << " type is #" << (int)t;
        LOG(ERROR) << "node " << tlvf::mac_to_string(mac) << " is not a valid hostap!";
        return nullptr;
    }

    return n->hostap;
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

bool db::is_prplmesh(const sMacAddr &mac)
{
    auto node = get_node(mac);
    if (!node) {
        LOG(ERROR) << "can't find node with mac " << mac << ", consider as not prplmesh";
        return false;
    }
    return node->is_prplmesh;
}

void db::set_prplmesh(const sMacAddr &mac)
{
    auto local_bridge_mac = tlvf::mac_from_string(get_local_bridge_mac());
    auto ire_type         = local_bridge_mac == mac ? beerocks::TYPE_GW : beerocks::TYPE_IRE;
    if (!get_node(mac)) {
        add_node(mac, beerocks::net::network_utils::ZERO_MAC, ire_type);
    }
    get_node(mac)->is_prplmesh = true;
}

bool db::update_client_entry_in_persistent_db(
    const sMacAddr &mac, const std::unordered_map<std::string, std::string> &values_map)
{
    auto db_entry        = client_db_entry_from_mac(mac);
    auto type_client_str = type_to_string(beerocks::eType::TYPE_CLIENT);

    if (!bpl::db_has_entry(type_client_str, db_entry)) {
        if (!add_client_to_persistent_db(mac, values_map)) {
            LOG(ERROR) << "failed to add client entry in persistent-db for " << mac;
            return false;
        }
    } else if (!bpl::db_set_entry(type_client_str, db_entry, values_map)) {
        LOG(ERROR) << "failed to set client in persistent-db for " << mac;
        return false;
    }

    return true;
}

bool db::set_node_params_from_map(const sMacAddr &mac,
                                  const std::unordered_map<std::string, std::string> &values_map)
{
    auto node = get_node(mac);
    if (!node) {
        LOG(WARNING) << " - node " << mac << " does not exist!";
        return false;
    }

    for (const auto &param : values_map) {
        if (param.first == TIMESTAMP_STR) {
            LOG(DEBUG) << "setting node client_parameters_last_edit to " << param.second << " for "
                       << mac;
            node->client_parameters_last_edit =
                timestamp_from_seconds(string_utils::stoi(param.second));
        } else if (param.first == TIMELIFE_DELAY_STR) {
            LOG(DEBUG) << "setting node client_time_life_delay_sec to " << param.second << " for "
                       << mac;
            node->client_time_life_delay_sec =
                std::chrono::seconds(string_utils::stoi(param.second));
        } else if (param.first == INITIAL_RADIO_ENABLE_STR) {
            LOG(DEBUG) << "setting node client_stay_on_initial_radio to " << param.second << " for "
                       << mac;

            node->client_stay_on_initial_radio =
                (param.second == "1") ? eTriStateBool::ENABLE : eTriStateBool::DISABLE;
        } else if (param.first == INITIAL_RADIO_STR) {
            LOG(DEBUG) << "setting node client_initial_radio to " << param.second << " for " << mac;

            node->client_initial_radio = tlvf::mac_from_string(param.second);
        } else if (param.first == SELECTED_BAND_ENABLE_STR) {
            LOG(DEBUG) << "setting node client_stay_on_selected_band to " << param.second << " for "
                       << mac;

            node->client_stay_on_selected_band =
                (param.second == "1") ? eTriStateBool::ENABLE : eTriStateBool::DISABLE;
        } else if (param.first == SELECTED_BANDS_STR) {
            LOG(DEBUG) << "setting node client_selected_bands to " << param.second << " for "
                       << mac;
            node->client_selected_bands = beerocks::eFreqType(string_utils::stoi(param.second));
        } else {
            LOG(WARNING) << "Unknown parameter, skipping: " << param.first << " for " << mac;
        }
    }

    return true;
}

bool db::add_client_entry_and_update_counter(
    const std::string &entry_name, const std::unordered_map<std::string, std::string> &values_map)
{
    if (!bpl::db_add_entry(type_to_string(beerocks::eType::TYPE_CLIENT), entry_name, values_map)) {
        LOG(ERROR) << "failed to add client entry " << entry_name << " to persistent db";
        return false;
    }

    ++m_persistent_db_clients_count;

    return true;
}

bool db::remove_client_entry_and_update_counter(const std::string &entry_name)
{
    if (!bpl::db_remove_entry(type_to_string(beerocks::eType::TYPE_CLIENT), entry_name)) {
        LOG(ERROR) << "failed to remove entry " << entry_name << "from persistent db";
        return false;
    }
    --m_persistent_db_clients_count;

    return false;
}
