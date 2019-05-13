/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MONITOR_DB_H
#define MONITOR_DB_H

#include <beerocks/bcl/beerocks_defines.h>
#include <beerocks/bwl/mon_wlan_hal_types.h>

#include <chrono>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace son {
////////////////////////////////////////////
////////////////////////////////////////////
class monitor_sta_node {
public:
    monitor_sta_node(const int8_t vap_id_, const std::string mac_) { vap_id = vap_id_, mac = mac_; }

    enum eArpState {
        IDLE = 0,
        SEND_ARP,
        WAIT_FIRST_REPLY,
        WAIT_REPLY,
        SEND_RESPONSE,
    };

    void set_last_change_time() { last_change_time = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point get_last_change_time() { return last_change_time; }

    int8_t get_vap_id() { return vap_id; }

    void set_ipv4(std::string ip) { ipv4 = ip; }
    std::string get_ipv4() { return ipv4; }

    void set_bridge_4addr_mac(const std::string bridge_mac_4addr_)
    {
        bridge_mac_4addr = bridge_mac_4addr_;
    }
    std::string get_bridge_4addr_mac() { return bridge_mac_4addr; }

    std::string get_mac() { return mac; }

    void set_arp_state(eArpState state) { arp_state = state; }
    eArpState get_arp_state() { return arp_state; }

    void set_arp_burst(bool en) { arp_burst = en; }
    bool get_arp_burst() { return arp_burst; }

    void set_rx_rssi_ready(bool en) { rx_rssi_ready = en; }
    bool get_rx_rssi_ready() { return rx_rssi_ready; }

    void set_rx_snr_ready(bool en) { rx_snr_ready = en; }
    bool get_rx_snr_ready() { return rx_snr_ready; }

    void push_rx_rssi_request_id(uint16_t id) { pending_rx_rssi_requests_id.push_back(id); }
    std::list<uint16_t> &get_rx_rssi_request_id_list() { return pending_rx_rssi_requests_id; }
    void clear_rx_rssi_request_id_list() { pending_rx_rssi_requests_id.clear(); }

    void arp_set_start_time() { arp_time = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point arp_get_start_time() { return arp_time; }

    void arp_recv_count_clear() { arp_recv_count = 0; }
    void arp_recv_count_inc() { arp_recv_count++; }
    uint8_t arp_recv_count_get() { return arp_recv_count; }

    void arp_retry_count_clear() { arp_retry_count = 0; }
    void arp_retry_count_inc() { arp_retry_count++; }
    uint8_t arp_retry_count_get() { return arp_retry_count; }

    double get_load_rx_phy_rate();
    double get_load_tx_phy_rate();
    double get_load_rx_bit_rate();
    double get_load_tx_bit_rate();
    double get_load_rx_percentage();
    double get_load_tx_percentage();
    uint32_t get_tx_packets();
    uint32_t get_rx_packets();
    void reset_poll_data();

    friend std::ostream &operator<<(std::ostream &os, const monitor_sta_node &sta_node);
    friend std::ostream &operator<<(std::ostream &os, const monitor_sta_node *sta_node);

    // Statistics //
    struct SStaStats {
        uint8_t poll_cnt                                       = 0;
        uint16_t delta_ms                                      = 0;
        std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();

        int8_t rx_rssi_prev            = beerocks::RSSI_INVALID;
        int8_t rx_rssi_curr            = beerocks::RSSI_INVALID;
        int8_t rx_snr_curr             = beerocks::SNR_INVALID;
        uint16_t tx_phy_rate_100kb_avg = 0;
        uint16_t tx_phy_rate_100kb_min = 0;
        uint16_t tx_phy_rate_100kb_acc = 0;
        uint16_t rx_phy_rate_100kb_avg = 0;
        uint16_t rx_phy_rate_100kb_min = 0;
        uint16_t rx_phy_rate_100kb_acc = 0;

        bwl::SStaStats hal_stats = {};

        uint8_t tx_load_percent_curr = 0;
        uint8_t tx_load_percent_prev = 0;
        uint8_t rx_load_percent_curr = 0;
        uint8_t rx_load_percent_prev = 0;
    };

    SStaStats &get_stats() { return m_sta_stats; }

    // Idle stations //
    bool idle_detected       = false;
    bool enable_idle_monitor = false;
    std::chrono::steady_clock::time_point idle_detected_start_time;

private:
    int8_t vap_id = beerocks::IFACE_ID_INVALID;
    std::string ipv4;
    /*
             * IREs working in 4address mode operate from behind their bridge
             * we have to know it in order to send and receive ARP messages
             */
    std::string mac;
    std::string bridge_mac_4addr;
    eArpState arp_state     = eArpState::IDLE;
    bool rx_rssi_ready      = false;
    bool rx_snr_ready       = false;
    bool arp_burst          = false;
    uint8_t arp_recv_count  = 0;
    uint8_t arp_retry_count = 0;
    std::list<uint16_t> pending_rx_rssi_requests_id;
    std::chrono::steady_clock::time_point last_change_time;
    std::chrono::steady_clock::time_point arp_time = std::chrono::steady_clock::now();
    SStaStats m_sta_stats;
};

////////////////////////////////////////////
////////////////////////////////////////////
class monitor_vap_node {
public:
    monitor_vap_node(const std::string iface_, const int8_t vap_id_)
    {
        iface  = iface_;
        vap_id = vap_id_;
        // LOG(DEBUG) << "new vap_node=" << uint32_t(this);
        // LOG(DEBUG) << "new vap_stats=" << uint32_t(p_stats);
    }
    ~monitor_vap_node() {}

    std::string get_iface() { return iface; }
    int8_t get_vap_id() { return vap_id; }

    void set_mac(const std::string ap_mac_) { mac = ap_mac_; }
    std::string get_mac() { return mac; }

    std::string get_ipv4();

    void set_bridge_iface(const std::string bridge_iface_) { bridge_iface = bridge_iface_; }
    std::string get_bridge_iface() { return bridge_iface; }

    void set_bridge_mac(const std::string bridge_mac_) { bridge_mac = bridge_mac_; }
    std::string get_bridge_mac() { return bridge_mac; }

    void set_bridge_ipv4(const std::string bridge_ipv4_) { bridge_ipv4 = bridge_ipv4_; }
    std::string get_bridge_ipv4() { return bridge_ipv4; }

    void sta_count_inc() { sta_count += 1; }
    void sta_count_dec()
    {
        if (sta_count > 0)
            sta_count -= 1;
    }
    int sta_get_count() { return sta_count; }

    double get_rx_bit_rate();
    double get_tx_bit_rate();

    friend std::ostream &operator<<(std::ostream &os, const monitor_vap_node &vap_node);
    friend std::ostream &operator<<(std::ostream &os, const monitor_vap_node *vap_node);

    // Statistics //
    struct SVapStats {
        uint16_t delta_ms                                      = 0;
        std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();

        bwl::SVapStats hal_stats = {};

        uint8_t client_tx_load_tot_prev      = 0;
        uint8_t client_rx_load_tot_prev      = 0;
        uint8_t client_tx_load_tot_curr      = 0;
        uint8_t client_rx_load_tot_curr      = 0;
        bool active_client_count_is_above_th = false;
        int active_client_count_prev         = 0;
        int active_client_count_curr         = 0;
    };

    SVapStats &get_stats() { return m_vap_stats; }

    void clear_stats();

private:
    int8_t vap_id;
    std::string iface;
    int sta_count = 0;
    std::string mac;
    std::string ipv4;
    std::string bridge_iface;
    std::string bridge_mac;
    std::string bridge_ipv4;

    SVapStats m_vap_stats;
};

////////////////////////////////////////////
////////////////////////////////////////////
class monitor_radio_node {
public:
    monitor_radio_node() {}
    ~monitor_radio_node() {}

    void set_iface(const std::string iface_) { iface = iface_; }
    std::string get_iface() { return iface; }

    void set_channel(const uint8_t channel_) { channel = channel_; }
    uint8_t get_channel() { return channel; }

    double get_rx_bit_rate();
    double get_tx_bit_rate();

    friend std::ostream &operator<<(std::ostream &os, const monitor_radio_node &radio_node);
    friend std::ostream &operator<<(std::ostream &os, const monitor_radio_node *radio_node);

    // Statistics //
    struct SRadioStats {
        uint16_t delta_ms                                      = 0;
        std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();

        bwl::SRadioStats hal_stats = {};

        uint32_t total_retrans_count = 0;

        uint8_t channel_load_tot_prev        = 0;
        uint8_t channel_load_tot_curr        = 0;
        uint8_t channel_load_others          = 0;
        uint8_t channel_load_idle            = 0;
        bool channel_load_tot_is_above_hi_th = false;

        uint8_t client_tx_load_tot_prev      = 0;
        uint8_t client_rx_load_tot_prev      = 0;
        uint8_t client_tx_load_tot_curr      = 0;
        uint8_t client_rx_load_tot_curr      = 0;
        bool active_client_count_is_above_th = false;
        int active_client_count_prev         = 0;
        int active_client_count_curr         = 0;
        uint32_t sta_count                   = 0;
    };

    SRadioStats &get_stats() { return m_radio_stats; }

    void clear_stats();

private:
    std::string iface;
    uint8_t channel = 0;
    SRadioStats m_radio_stats;
};

////////////////////////////////////////////
////////////////////////////////////////////
class monitor_db {
public:
    monitor_db();
    ~monitor_db();

    void clear();

    // Radio //
    monitor_radio_node *get_radio_node() { return &radio_node; }
    int8_t get_hostapd_enabled() { return ap_hostapd_enabled; }
    void set_hostapd_enabled(int8_t enabled) { ap_hostapd_enabled = enabled; }
    int8_t get_ap_tx_enabled() { return ap_tx_enabled; }
    void set_ap_tx_enabled(int8_t enabled) { ap_tx_enabled = enabled; }

    // VAP //
    std::shared_ptr<monitor_vap_node> vap_add(const std::string iface, int8_t vap_id);
    std::shared_ptr<monitor_vap_node> vap_get_by_id(int vap_id);
    int get_vap_id(const std::string &bssid);
    std::shared_ptr<monitor_vap_node> get_vap_node(const std::string &bssid);
    bool vap_remove(int vap_id);
    int get_vap_count() { return vap_nodes.size(); }
    void vap_erase_all();

    // STA's //
    monitor_sta_node *sta_add(const std::string sta_mac, const int8_t vap_id);
    void sta_erase(const std::string sta_mac);
    void sta_erase_all();
    monitor_sta_node *sta_find(const std::string mac);
    monitor_sta_node *sta_find_by_ipv4(const std::string ipv4);
    std::unordered_map<std::string, monitor_sta_node *>::iterator sta_begin()
    {
        return sta_nodes.begin();
    }
    std::unordered_map<std::string, monitor_sta_node *>::iterator sta_end()
    {
        return sta_nodes.end();
    }

    // Monitor parameters //
    void set_arp_burst_delay(int arp_burst_delay_) { arp_burst_delay = arp_burst_delay_; }
    uint8_t get_arp_burst_delay() { return arp_burst_delay; }
    void set_arp_burst_pkt_num(int arp_burst_pkt_num_) { arp_burst_pkt_num = arp_burst_pkt_num_; }
    uint8_t get_arp_burst_pkt_num() { return arp_burst_pkt_num; }

    // Polling functions //
    std::chrono::steady_clock::time_point get_poll_next_time();
    void set_poll_next_time(std::chrono::steady_clock::time_point pt, bool reset_poll = false);
    bool poll_done();
    uint32_t get_poll_id();
    uint8_t get_poll_cnt();
    bool is_last_poll();

    std::chrono::steady_clock::time_point get_ap_poll_next_time();
    void set_ap_poll_next_time(std::chrono::steady_clock::time_point pt, bool reset_poll = false);

    const int MONITOR_LAST_CHANGE_TIMEOUT_MSEC   = 30000;
    const int MONITOR_DB_POLLING_RATE_MSEC       = 250;
    const int MONITOR_DB_MEASUREMENT_WINDOW_MSEC = (4 * MONITOR_DB_POLLING_RATE_MSEC);
    const int MONITOR_DB_AP_POLLING_RATE_SEC     = 5;

    const int MONITOR_ARP_TIMEOUT_MSEC = 550;
    const int MONITOR_ARP_PKT_NUM      = 6;

    const int MONITOR_ARP_COUNT_OK      = 4;
    const int MONITOR_ARP_WAKE_COUNT_OK = 1;
    const int MONITOR_ARP_RETRY_COUNT   = 10;

private:
    uint32_t poll_id = 0;
    int8_t poll_cnt  = 0;
    std::chrono::steady_clock::time_point last_stats_update_time;
    std::chrono::steady_clock::time_point poll_next_time;
    std::chrono::steady_clock::time_point ap_poll_next_time;
    monitor_radio_node radio_node;
    std::unordered_map<int8_t, std::shared_ptr<monitor_vap_node>> vap_nodes;
    std::unordered_map<std::string, monitor_sta_node *> sta_nodes;

    int8_t ap_tx_enabled      = 0;
    int8_t ap_hostapd_enabled = 2;

    int arp_burst_pkt_num = 0;
    int arp_burst_delay   = 0;
};

} // namespace son

#endif
