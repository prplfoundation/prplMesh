/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "monitor_db.h"

#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

using namespace son;
using namespace beerocks;
using namespace net;

/////////////////////////////////////////////
// class monitor_radio_node
/////////////////////////////////////////////
namespace son {
std::ostream &operator<<(std::ostream &ost, monitor_radio_node &radio_node)
{
    auto &radio_stats = radio_node.get_stats();
    auto tx_bit_rate  = radio_node.get_tx_bit_rate() / 1e+6;
    auto rx_bit_rate  = radio_node.get_rx_bit_rate() / 1e+6;
    ost << std::endl
        << " Channel:" << int(radio_node.get_channel()) << std::endl
        << " Stats delta: " << int(radio_stats.delta_ms) << "[msec]" << std::endl
        << " Bytes (TX|RX): " << radio_stats.hal_stats.tx_bytes << " | "
        << radio_stats.hal_stats.rx_bytes << " , " << std::endl
        << " Packets (TX|RX): " << radio_stats.hal_stats.tx_packets << " | "
        << radio_stats.hal_stats.rx_packets << std::endl
        << " BitRate (TX|RX): " << tx_bit_rate << " | " << rx_bit_rate << " [Mbps] " << std::endl
        << " Ch load: " << int(radio_stats.channel_load_tot_curr) << " [%]"
        << " Ch load others: " << int(radio_stats.channel_load_others) << " [%]"
        << " Ch load idle: " << int(radio_stats.channel_load_idle) << " [%]" << std::endl;

    return (ost);
}

std::ostream &operator<<(std::ostream &ost, monitor_radio_node *radio_node)
{
    return (ost << (monitor_radio_node &)(*radio_node));
}
} // namespace son

double monitor_radio_node::get_rx_bit_rate()
{
    auto &radio_stats = get_stats();
    return (8000.0f * double(radio_stats.hal_stats.rx_bytes) / double(radio_stats.delta_ms));
}

double monitor_radio_node::get_tx_bit_rate()
{
    auto &radio_stats = get_stats();
    return (8000.0f * double(radio_stats.hal_stats.tx_bytes) / double(radio_stats.delta_ms));
}

void monitor_radio_node::clear_stats()
{
    m_radio_stats.delta_ms                        = 0;
    m_radio_stats.last_update_time                = std::chrono::steady_clock::now();
    m_radio_stats.hal_stats                       = {};
    m_radio_stats.channel_load_tot_prev           = 0;
    m_radio_stats.channel_load_tot_curr           = 0;
    m_radio_stats.channel_load_others             = 0;
    m_radio_stats.channel_load_idle               = 0;
    m_radio_stats.channel_load_tot_is_above_hi_th = false;
}

/////////////////////////////////////////////
// class monitor_vap_node
/////////////////////////////////////////////
namespace son {
std::ostream &operator<<(std::ostream &ost, monitor_vap_node &vap_node)
{
    auto &vap_stats  = vap_node.get_stats();
    auto vap_mac     = vap_node.get_mac();
    auto tx_bit_rate = vap_node.get_tx_bit_rate() / 1e+6;
    auto rx_bit_rate = vap_node.get_rx_bit_rate() / 1e+6;
    auto vap_iface   = vap_node.get_iface();
    auto sta_count   = vap_node.sta_get_count();
    ost << std::endl
        << " Iface: " << vap_iface << std::endl
        << " VAP MAC: " << vap_mac << std::endl
        << " Stats delta: " << int(vap_stats.delta_ms) << "[msec]" << std::endl
        << " Bytes (TX|RX): " << vap_stats.hal_stats.tx_bytes << " | "
        << vap_stats.hal_stats.rx_bytes << " , "
        << " Packets (TX|RX): " << vap_stats.hal_stats.tx_packets << " | "
        << vap_stats.hal_stats.rx_packets << std::endl
        << " BitRate (TX|RX): " << tx_bit_rate << " | " << rx_bit_rate << " [Mbps] " << std::endl
        << " STA count: " << int(sta_count) << std::endl
        << " STA total load (TX|RX): " << int(vap_stats.client_tx_load_tot_curr) << " | "
        << int(vap_stats.client_rx_load_tot_curr) << " [%]" << std::endl;
    return (ost);
}

std::ostream &operator<<(std::ostream &ost, monitor_vap_node *vap_node)
{
    return (ost << (monitor_vap_node &)(*vap_node));
}
} // namespace son

std::string monitor_vap_node::get_ipv4()
{
    std::string ip;
    network_utils::linux_iface_get_ip(iface, ip);
    return ip;
}

double monitor_vap_node::get_rx_bit_rate()
{
    auto &vap_stats = get_stats();
    return (8000.0f * double(vap_stats.hal_stats.rx_bytes) / double(vap_stats.delta_ms));
}

double monitor_vap_node::get_tx_bit_rate()
{
    auto &vap_stats = get_stats();
    return (8000.0f * double(vap_stats.hal_stats.tx_bytes) / double(vap_stats.delta_ms));
}

void monitor_vap_node::clear_stats()
{
    m_vap_stats.delta_ms                        = 0;
    m_vap_stats.last_update_time                = std::chrono::steady_clock::now();
    m_vap_stats.hal_stats                       = {};
    m_vap_stats.client_tx_load_tot_prev         = 0;
    m_vap_stats.client_rx_load_tot_prev         = 0;
    m_vap_stats.client_tx_load_tot_curr         = 0;
    m_vap_stats.client_rx_load_tot_curr         = 0;
    m_vap_stats.active_client_count_is_above_th = false;
    m_vap_stats.active_client_count_prev        = 0;
    m_vap_stats.active_client_count_curr        = 0;
}

/////////////////////////////////////////////
// class monitor_sta_node
/////////////////////////////////////////////
namespace son {
std::ostream &operator<<(std::ostream &ost, monitor_sta_node &sta_node)
{
    auto &sta_stats = sta_node.get_stats();
    ost << std::endl
        << " Stats delta: " << int(sta_stats.delta_ms) << "[msec]" << std::endl
        << " RSSI (Load): " << int(sta_stats.rx_rssi_curr) << "[dBm]" << std::endl
        << " SNR (Load): " << int(sta_stats.rx_snr_curr) << std::endl
        << " Bytes (TX|RX): " << sta_stats.hal_stats.tx_bytes << " | "
        << sta_stats.hal_stats.rx_bytes << " , "
        << " Packets (TX|RX): " << sta_stats.hal_stats.tx_packets << " | "
        << sta_stats.hal_stats.rx_packets << std::endl
        << " BitRate (TX|RX): " << sta_node.get_load_tx_bit_rate() / 1e+6 << " | "
        << sta_node.get_load_rx_bit_rate() / 1e+6 << " [Mbps] " << std::endl
        << " PhyRate (TX|RX): " << sta_node.get_load_tx_phy_rate() / 1e+6 << " | "
        << sta_node.get_load_rx_phy_rate() / 1e+6 << " [Mbps]" << std::endl
        << " Load (TX|RX): " << sta_node.get_load_tx_percentage() << " | "
        << sta_node.get_load_rx_percentage() << " [%]" << std::endl;
    return (ost);
}

std::ostream &operator<<(std::ostream &ost, monitor_sta_node *sta_node)
{
    return (ost << (monitor_sta_node &)(*sta_node));
}
} // namespace son

double monitor_sta_node::get_load_rx_phy_rate()
{
    auto &sta_stats = get_stats();
    return (1e+5 * double(sta_stats.rx_phy_rate_100kb_avg));
}

double monitor_sta_node::get_load_tx_phy_rate()
{
    auto &sta_stats = get_stats();
    return (1e+5 * double(sta_stats.tx_phy_rate_100kb_avg));
}

double monitor_sta_node::get_load_rx_bit_rate()
{
    auto &sta_stats = get_stats();
    double bit_rate =
        ((8000.0f * double(sta_stats.hal_stats.rx_bytes)) / double(sta_stats.delta_ms));
    return bit_rate;
}

double monitor_sta_node::get_load_tx_bit_rate()
{
    auto &sta_stats = get_stats();
    double bit_rate =
        ((8000.0 * double(sta_stats.hal_stats.tx_bytes)) / double(sta_stats.delta_ms));
    return bit_rate;
}

double monitor_sta_node::get_load_rx_percentage()
{
    double max_bit_rate_mbps =
        son::wireless_utils::get_load_max_bit_rate_mbps(get_load_rx_phy_rate() / 1e+5);
    if (max_bit_rate_mbps == 0)
        return 100;
    return (100.0 * get_load_rx_bit_rate() / (max_bit_rate_mbps * 1e6));
}

double monitor_sta_node::get_load_tx_percentage()
{
    double max_bit_rate_mbps =
        son::wireless_utils::get_load_max_bit_rate_mbps(get_load_tx_phy_rate() / 1e+5);
    if (max_bit_rate_mbps == 0)
        return 100;
    return (100.0 * get_load_tx_bit_rate() / (max_bit_rate_mbps * 1e6));
}

void monitor_sta_node::reset_poll_data()
{
    m_sta_stats.poll_cnt                           = 0;
    m_sta_stats.hal_stats.rx_rssi_watt             = 0;
    m_sta_stats.hal_stats.rx_rssi_watt_samples_cnt = 0;
    m_sta_stats.hal_stats.rx_snr_watt              = 0;
    m_sta_stats.hal_stats.rx_snr_watt_samples_cnt  = 0;
    m_sta_stats.tx_phy_rate_100kb_min              = 0;
    m_sta_stats.rx_phy_rate_100kb_min              = 0;
    m_sta_stats.tx_phy_rate_100kb_acc              = 0;
    m_sta_stats.rx_phy_rate_100kb_acc              = 0;
}

/////////////////////////////////////////////
// class monitor_db
/////////////////////////////////////////////
monitor_db::monitor_db() {}

monitor_db::~monitor_db()
{
    LOG(DEBUG) << "~monitor_db()";
    clear();
}

void monitor_db::clear()
{
    LOG(DEBUG) << "db clear()";
    poll_id                = 0;
    poll_cnt               = 0;
    poll_next_time         = std::chrono::steady_clock::now();
    last_stats_update_time = std::chrono::steady_clock::now();

    sta_erase_all();
    vap_erase_all();
}

std::shared_ptr<monitor_vap_node> monitor_db::vap_add(const std::string iface, int8_t vap_id)
{
    auto node         = std::make_shared<monitor_vap_node>(iface, vap_id);
    vap_nodes[vap_id] = node;
    LOG(DEBUG) << "Added vapid=" << int(vap_id) << " iface=" << iface
               << "vector_size=" << vap_nodes.size();
    return node;
}

bool monitor_db::vap_remove(int vap_id)
{
    int ret = vap_nodes.erase(vap_id);
    if (ret == 0) {
        LOG(WARNING) << "Tried to remove non existing vap id=" << vap_id;
    }
    return (ret > 0);
}

void monitor_db::vap_erase_all() { vap_nodes.clear(); }

std::shared_ptr<monitor_vap_node> monitor_db::vap_get_by_id(int vap_id)
{
    if (vap_nodes.find(vap_id) != vap_nodes.end()) {
        return vap_nodes.at(vap_id);
    }
    return std::shared_ptr<monitor_vap_node>();
}

std::shared_ptr<monitor_vap_node> monitor_db::get_vap_node(const std::string &bssid)
{
    for (const auto &vap : vap_nodes) {
        const auto &vap_node = vap.second;
        if (vap_node->get_mac() == bssid) {
            return vap_node;
        }
    }
    return std::shared_ptr<monitor_vap_node>();
}

int monitor_db::get_vap_id(const std::string &bssid)
{
    for (const auto &vap : vap_nodes) {
        const auto &vap_node = vap.second;
        if (vap_node->get_mac() == bssid) {
            return vap.first;
        }
    }
    return beerocks::IFACE_ID_INVALID;
}

monitor_sta_node *monitor_db::sta_find(const std::string sta_mac)
{
    auto it = sta_nodes.find(sta_mac);
    if (it != sta_nodes.end()) {
        return it->second;
    }
    return nullptr;
}

monitor_sta_node *monitor_db::sta_find_by_ipv4(const std::string ipv4)
{
    for (auto kv : sta_nodes) {
        if (kv.second->get_ipv4() == ipv4) {
            return kv.second;
        }
    }
    return nullptr;
}

monitor_sta_node *monitor_db::sta_add(const std::string sta_mac, const int8_t vap_id)
{
    auto node = new monitor_sta_node(vap_id, sta_mac);
    sta_nodes.insert(std::make_pair(sta_mac, node));
    auto vap_node = vap_get_by_id(vap_id);
    if (vap_node) {
        vap_node->sta_count_inc();
    }
    return node;
}

void monitor_db::sta_erase(const std::string sta_mac)
{
    auto it = sta_nodes.find(sta_mac);
    if (it != sta_nodes.end()) {
        auto vap_id = it->second->get_vap_id();
        delete it->second;
        sta_nodes.erase(it);
        auto vap_node = vap_get_by_id(vap_id);
        if (vap_node) {
            vap_node->sta_count_dec();
        }
    }
}

void monitor_db::sta_erase_all()
{
    for (auto it = sta_nodes.begin(); it != sta_nodes.end(); it++) {
        delete it->second;
    }
    sta_nodes.clear();
}

std::chrono::steady_clock::time_point monitor_db::get_poll_next_time() { return poll_next_time; }

void monitor_db::set_poll_next_time(std::chrono::steady_clock::time_point pt, bool reset_poll)
{
    poll_next_time = pt;
    if (reset_poll) {
        poll_cnt = 0;
    }
}

std::chrono::steady_clock::time_point monitor_db::get_ap_poll_next_time()
{
    return ap_poll_next_time;
}

void monitor_db::set_ap_poll_next_time(std::chrono::steady_clock::time_point pt, bool reset_poll)
{
    ap_poll_next_time = pt;
}

bool monitor_db::poll_done()
{
    if (((poll_cnt + 1) * MONITOR_DB_POLLING_RATE_MSEC) >= MONITOR_DB_MEASUREMENT_WINDOW_MSEC) {
        poll_cnt = 0;
        poll_id++;
        return true;
    } else {
        poll_cnt++;
        return false;
    }
}

uint32_t monitor_db::get_poll_id() { return poll_id; }

uint8_t monitor_db::get_poll_cnt() { return poll_cnt; }

bool monitor_db::is_last_poll()
{
    bool poll_last =
        (((poll_cnt + 1) * MONITOR_DB_POLLING_RATE_MSEC) >= MONITOR_DB_MEASUREMENT_WINDOW_MSEC);
    return poll_last;
}

uint32_t monitor_sta_node::get_tx_packets()
{
    auto &sta_stats = get_stats();
    return sta_stats.hal_stats.tx_packets;
}

uint32_t monitor_sta_node::get_rx_packets()
{
    auto &sta_stats = get_stats();
    return sta_stats.hal_stats.rx_packets;
}