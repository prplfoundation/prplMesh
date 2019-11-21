/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "monitor_stats.h"

#include <bcl/network/network_utils.h>
#include <bcl/network/socket.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_monitor.h>

using namespace beerocks;
using namespace net;
using namespace son;

monitor_stats::monitor_stats(ieee1905_1::CmduMessageTx &cmdu_tx_) : cmdu_tx(cmdu_tx_)
{
    mon_db       = nullptr;
    next_poll_id = 1;
}

void monitor_stats::stop()
{
    mon_db       = nullptr;
    slave_socket = nullptr;
}

bool monitor_stats::start(monitor_db *mon_db_, Socket *slave_socket_)
{
    if (!mon_db_ || !slave_socket_) {
        LOG(ERROR) << "invalid input == NULL";
        return false;
    }
    mon_db       = mon_db_;
    slave_socket = slave_socket_;
    return true;
}

void monitor_stats::add_request(uint16_t id, uint8_t sync)
{
    if (sync) {
        if (!mon_db) {
            LOG(WARNING) << "monitor is not attached yet, mon_db=nullptr";
            return;
        }
        next_poll_id = mon_db->get_poll_id() + 1;
    }
    requests_list.push_back(id);
}

void monitor_stats::process()
{

    //LOG(DEBUG) << "monitor_stats::process() 2 : db_poll_id=" << int(mon_db->get_poll_id()) << ", next_poll_id=" << int(next_poll_id);

    //check if dd has updates
    auto db_poll_id = mon_db->get_poll_id();
    if (db_poll_id != next_poll_id) {
        return;
    }
    next_poll_id++;

    // Radio
    auto radio_node = mon_db->get_radio_node();

    auto &radio_stats = radio_node->get_stats();

    //if both under threshold_bytes(10K) for threshold_time(10 minutes) sending IDLE notification
    //when threshold go above 20K sending ACTIVE notification.
    bool send_activity_mode_notification = false;

    if (eApActiveMode == beerocks::eApActiveMode::AP_ACTIVE_MODE) {
        // LOG(DEBUG) << "radio_stats.hal_stats.rx_bytes = " << uint32_t(radio_stats.hal_stats.rx_bytes)
        //             << " radio_stats.hal_stats.tx_bytes = " << uint32_t(radio_stats.hal_stats.tx_bytes)
        //             << " conf_ap_idle_threshold_B = " << uint32_t(conf_ap_idle_threshold_B)
        //             << " conf_ap_active_threshold_B = " << uint32_t(conf_ap_active_threshold_B);
        if ((radio_stats.hal_stats.rx_bytes + radio_stats.hal_stats.tx_bytes) <
            (2 * conf_ap_idle_threshold_B) /*2*(AP_IDLE_BYTES_THRESHOLD)*/) {
            if (idle_timer > conf_ap_idle_stable_time_sec /*AP_IDLE_TIME_SEC_THRESHOLD*/) {
                eApActiveMode                   = beerocks::eApActiveMode::AP_IDLE_MODE;
                send_activity_mode_notification = true;
                //LOG(DEBUG) << "->AP_IDLE_MODE - send notification to slave , idle_timer" << int(idle_timer);
            } else {
                //update idle timer
                idle_timer += (!idle_timer)
                                  ? 1
                                  : std::chrono::duration_cast<std::chrono::seconds>(
                                        std::chrono::steady_clock::now() - idle_prev_timestamp)
                                        .count();
                //LOG(DEBUG) << "AP_ACTIVE_MODE - idle_timer = " << int(idle_timer) << " conf_ap_idle_stable_time_sec = " << int(conf_ap_idle_stable_time_sec);
            }
            idle_prev_timestamp = std::chrono::steady_clock::now();
        }
    } else if ((radio_stats.hal_stats.rx_bytes + radio_stats.hal_stats.tx_bytes) >
               2 * (conf_ap_active_threshold_B /*AP_ACTIVE_BYTES_THRESHOLD*/)) { // IDLE
        eApActiveMode                   = beerocks::eApActiveMode::AP_ACTIVE_MODE;
        send_activity_mode_notification = true;
        LOG(DEBUG) << "->AP_ACTIVE_MODE - send notification to slave , idle_timer"
                   << int(idle_timer);
        idle_timer = 0;
    }

    if (send_activity_mode_notification) {
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION message!";
            return;
        }

        notification->params().ap_activity_mode = eApActiveMode;
        message_com::send_cmdu(slave_socket, cmdu_tx);
    }

    //send response
    if (!requests_list.empty()) {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE message!";
            return;
        }

        auto beerocks_header = message_com::get_vs_class_header(cmdu_tx);
        if (!beerocks_header) {
            LOG(ERROR) << "Failed getting beerocks_header!";
            return;
        }

        auto &ap_stats_msg                  = response->ap_stats();
        ap_stats_msg.stats_delta_ms         = radio_stats.delta_ms;
        ap_stats_msg.rx_packets             = radio_stats.hal_stats.rx_packets;
        ap_stats_msg.tx_packets             = radio_stats.hal_stats.tx_packets;
        ap_stats_msg.rx_bytes               = radio_stats.hal_stats.rx_bytes;
        ap_stats_msg.tx_bytes               = radio_stats.hal_stats.tx_bytes;
        ap_stats_msg.errors_sent            = radio_stats.hal_stats.errors_sent;
        ap_stats_msg.errors_received        = radio_stats.hal_stats.errors_received;
        ap_stats_msg.retrans_count          = radio_stats.total_retrans_count;
        ap_stats_msg.noise                  = radio_stats.hal_stats.noise;
        ap_stats_msg.channel_load_percent   = radio_stats.channel_load_tot_curr;
        ap_stats_msg.client_count           = radio_stats.sta_count;
        ap_stats_msg.active_client_count    = radio_stats.active_client_count_curr;
        ap_stats_msg.client_tx_load_percent = radio_stats.client_tx_load_tot_curr;
        ap_stats_msg.client_rx_load_percent = radio_stats.client_rx_load_tot_curr;

        for (auto it = mon_db->sta_begin(); it != mon_db->sta_end(); ++it) {
            auto sta_mac  = it->first;
            auto sta_node = it->second;
            if (sta_node == nullptr) {
                continue;
            }

            auto &sta_stats = sta_node->get_stats();

            if (!response->alloc_sta_stats()) {
                LOG(ERROR) << "tx_buffer overflow!";
                return;
            }

            uint8_t num_of_stas = response->sta_stats_size();
            auto &sta_stats_msg = std::get<1>(response->sta_stats(num_of_stas - 1));

            sta_stats_msg.mac               = network_utils::mac_from_string(sta_mac);
            sta_stats_msg.rx_packets        = sta_stats.hal_stats.rx_packets;
            sta_stats_msg.tx_packets        = sta_stats.hal_stats.tx_packets;
            sta_stats_msg.tx_bytes          = sta_stats.hal_stats.tx_bytes;
            sta_stats_msg.rx_bytes          = sta_stats.hal_stats.rx_bytes;
            sta_stats_msg.retrans_count     = sta_stats.hal_stats.retrans_count;
            sta_stats_msg.tx_phy_rate_100kb = sta_stats.tx_phy_rate_100kb_avg;
            sta_stats_msg.rx_phy_rate_100kb = sta_stats.rx_phy_rate_100kb_avg;
            sta_stats_msg.tx_load_percent   = sta_stats.tx_load_percent_curr;
            sta_stats_msg.rx_load_percent   = sta_stats.rx_load_percent_curr;
            sta_stats_msg.stats_delta_ms    = sta_stats.delta_ms;
            sta_stats_msg.rx_rssi           = sta_stats.rx_rssi_curr;
        }

        beerocks_header->id() = requests_list.front();
        requests_list.pop_front();
        message_com::send_cmdu(slave_socket, cmdu_tx);
    }
}

void monitor_stats::calculate_client_load(monitor_sta_node *sta_node,
                                          monitor_radio_node *radio_node, int active_sta_th)
{
    //update sta values
    auto &sta_stats                = sta_node->get_stats();
    sta_stats.tx_load_percent_prev = sta_stats.tx_load_percent_curr;
    sta_stats.rx_load_percent_prev = sta_stats.rx_load_percent_curr;
    sta_stats.tx_load_percent_curr = sta_node->get_load_tx_percentage();
    if (sta_stats.tx_load_percent_curr > 100) {
        LOG(WARNING) << "LOAD_MEASUREMENT: tx_load_percent_curr = "
                     << int(sta_stats.tx_load_percent_curr);

        sta_stats.tx_load_percent_curr = 100;
    }
    sta_stats.rx_load_percent_curr = sta_node->get_load_rx_percentage();
    if (sta_stats.rx_load_percent_curr > 100) {
        LOG(WARNING) << "LOAD_MEASUREMENT: tx_load_percent_curr = "
                     << int(sta_stats.rx_load_percent_curr);

        sta_stats.rx_load_percent_curr = 100;
    }

    // Update VAP values
    auto &vap_stats = radio_node->get_stats();
    vap_stats.client_tx_load_tot_curr += sta_stats.tx_load_percent_curr;
    vap_stats.client_rx_load_tot_curr += sta_stats.rx_load_percent_curr;

    int sta_load = (sta_stats.tx_load_percent_curr + sta_stats.rx_load_percent_curr);
    if (sta_load >= active_sta_th) {
        vap_stats.active_client_count_curr++;
    }
}
