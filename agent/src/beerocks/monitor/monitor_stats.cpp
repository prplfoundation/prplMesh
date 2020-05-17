/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
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
#include <tlvf/wfa_map/tlvApMetrics.h>

using namespace beerocks;
using namespace net;
using namespace son;

/**
 * Implementation-specific measurement period of channel utilization.
 * Currently we use this constant value but a more elaborate solution should read it from
 * configuration.
 */
static constexpr uint8_t ap_metrics_channel_utilization_measurement_period_s = 10;

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

void monitor_stats::add_request(uint16_t id, uint8_t sync, const sMacAddr &sta_mac)
{
    if (sync) {
        if (!mon_db) {
            LOG(WARNING) << "monitor is not attached yet, mon_db=nullptr";
            return;
        }
        next_poll_id = mon_db->get_poll_id() + 1;
    }
    requests_list.push_back(sMeasurementsRequest(id, sta_mac));
}

void monitor_stats::send_hostap_measurements(const sMeasurementsRequest &request,
                                             const monitor_radio_node::SRadioStats &radio_stats)
{
    auto response = message_com::create_vs_message<
        beerocks_message::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE>(cmdu_tx);
    if (!response) {
        LOG(ERROR) << "Failed building cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE message!";
        return;
    }

    auto beerocks_header = message_com::get_beerocks_header(cmdu_tx);
    if (!beerocks_header) {
        LOG(ERROR) << "Failed getting beerocks_header!";
        return;
    }

    if (!response->alloc_ap_stats()) {
        LOG(ERROR) << "tx_buffer overflow!";
    }

    auto ap_stats_tuple = response->ap_stats(0);
    if (!std::get<0>(ap_stats_tuple)) {
        LOG(ERROR) << "Failed to get element";
        return;
    }
    auto &ap_stats_msg = std::get<1>(ap_stats_tuple);

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

    auto total_sta_count = mon_db->get_sta_count();
    size_t elements_to_allocate;
    uint32_t sta_count;

    auto allocate_sta_stats_elements = [&]() {
        elements_to_allocate =
            cmdu_tx.elements_in_message(sizeof(beerocks_message::sStaStatsParams));

        elements_to_allocate = std::min(total_sta_count, elements_to_allocate);

        if (!response->alloc_sta_stats(elements_to_allocate)) {
            LOG(ERROR) << "tx_buffer overflow! elements_to_allocate=" << int(elements_to_allocate);
            return;
        }

        sta_count = 0;
    };

    allocate_sta_stats_elements();

    for (auto it = mon_db->sta_begin(); it != mon_db->sta_end(); ++it) {
        auto sta_mac  = it->first;
        auto sta_node = it->second;
        if (sta_node == nullptr) {
            continue;
        }

        const auto &sta_stats = sta_node->get_stats();

        if (elements_to_allocate == sta_count) {
            message_com::send_cmdu(slave_socket, cmdu_tx);
            response = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE>(cmdu_tx);
            if (!response) {
                LOG(ERROR) << "Failed building "
                              "cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE message!";
                return;
            }

            total_sta_count -= sta_count;
            allocate_sta_stats_elements();
        }

        auto &sta_stats_msg = std::get<1>(response->sta_stats(sta_count));

        sta_stats_msg.mac               = tlvf::mac_from_string(sta_mac);
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

        sta_count++;
    }

    beerocks_header->actionhdr()->id() = request.message_id;
    message_com::send_cmdu(slave_socket, cmdu_tx);
}

void monitor_stats::send_associated_sta_link_metrics(const sMeasurementsRequest &request)
{
    auto sta = std::find_if(mon_db->sta_begin(), mon_db->sta_end(),
                            [&](std::pair<std::string, monitor_sta_node *> sta) {
                                auto sta_mac  = sta.first;
                                auto sta_node = sta.second;
                                return sta_node && (tlvf::mac_from_string(sta_mac) == request.mac);
                            });

    if (sta == mon_db->sta_end()) {
        LOG(ERROR) << "Could not find STA for ASSOCIATED_STA_LINK_METRIC_RESPONSE";
        return;
    }

    auto sta_node = sta->second;

    auto sta_metrics = message_com::create_vs_message<
        beerocks_message::cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE>(
        cmdu_tx, request.message_id);
    if (!sta_metrics) {
        LOG(ERROR) << "Failed building "
                      "cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE message!";
        return;
    }

    // there is exactly 1 BSSID per STA
    if (!sta_metrics->alloc_bssid_info_list(1)) {
        LOG(ERROR) << "Failed allocate_bssid_info_list";
        return;
    }
    const auto &sta_stats = sta_node->get_stats();

    sta_metrics->sta_mac() = request.mac;

    beerocks_message::sBssidInfo &bss_info = std::get<1>(sta_metrics->bssid_info_list(0));

    bss_info.earliest_measurement_delta = sta_stats.delta_ms;
    // TODO: MAC data rate and Phy rate are not necessarily the same
    // https://github.com/prplfoundation/prplMesh/issues/1195
    bss_info.downlink_estimated_mac_data_rate_mbps = sta_stats.rx_phy_rate_100kb_avg / 10;
    bss_info.uplink_estimated_mac_data_rate_mbps   = sta_stats.tx_phy_rate_100kb_avg / 10;
    bss_info.sta_measured_uplink_rssi_dbm_enc      = sta_stats.rx_rssi_curr;

    LOG(DEBUG) << "Send ACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE "
               << "for mac " << sta_metrics->sta_mac() << ", message_id = " << request.message_id;
    message_com::send_cmdu(slave_socket, cmdu_tx);
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
        auto request = requests_list.front();
        LOG(TRACE) << "Serving reqeust: mid = " << request.message_id << " MAC = " << request.mac;
        if (request.mac == beerocks::net::network_utils::ZERO_MAC) {
            send_hostap_measurements(request, radio_stats);
        } else {
            send_associated_sta_link_metrics(request);
        }

        requests_list.pop_front();
    }

    int delta_val;

    if (radio_stats.channel_load_tot_curr > 100) {
        LOG(DEBUG)
            << "radio channel_load_tot_curr > 100, changing to radio channel_load_tot_curr = 100 "
            << std::endl;
        radio_stats.channel_load_tot_curr = 100;
    }
    radio_stats.channel_load_tot_prev = radio_stats.channel_load_tot_curr;

    radio_stats.client_tx_load_tot_prev  = radio_stats.client_tx_load_tot_curr;
    radio_stats.client_rx_load_tot_prev  = radio_stats.client_rx_load_tot_curr;
    radio_stats.active_client_count_prev = radio_stats.active_client_count_curr;

    // clear stats
    radio_stats.client_tx_load_tot_curr  = 0;
    radio_stats.client_rx_load_tot_curr  = 0;
    radio_stats.active_client_count_curr = 0;

    //calculations for each sta on the radio
    for (auto it = mon_db->sta_begin(); it != mon_db->sta_end(); ++it) {
        auto sta_mac  = it->first;
        auto sta_node = it->second;
        if (sta_node == nullptr) {
            LOG(ERROR) << "sta_node == nullptr !";
            return;
        }

        calculate_client_load(sta_node, radio_node, conf_active_client_th);
        // LOG(DEBUG) << "STATS_MEASUREMENT sta_mac=" << sta_mac << " tx_phy_rate= " << sta_node->get_load_tx_phy_rate() <<
        //                       " , tx_load = " << sta_node->get_load_tx_percentage() << "[%]" <<
        //                       " , tx_packets = " << sta_node->get_stats().tx_packets <<
        //                       " , tx_bytes = " << sta_node->get_stats().tx_bytes << std::endl;
        //" , tx_bit_rate = " << sta_node->get_stats().tx_bit_rate() <<
    }

    if (radio_stats.client_tx_load_tot_curr > 100) {
        LOG(DEBUG)
            << "vap client_tx_load_tot_curr > 100, changing to vap client_tx_load_tot_curr = 100 "
            << std::endl;
        radio_stats.client_tx_load_tot_curr = 100;
    }
    if (radio_stats.client_rx_load_tot_curr > 100) {
        LOG(DEBUG)
            << "vap client_rx_load_tot_curr > 100, changing to radio client_rx_load_tot_curr = 100 "
            << std::endl;
        radio_stats.client_rx_load_tot_curr = 100;
    }

    radio_stats.channel_load_idle = 100 - radio_stats.channel_load_tot_curr;
    radio_stats.channel_load_others =
        radio_stats.channel_load_tot_curr -
        (radio_stats.client_tx_load_tot_curr + radio_stats.client_rx_load_tot_curr);

    //send notifications
    bool send_notification = false;
    /*
    *    channel load
    */
    //check if channel load passed monitor_channel_load_notification_th_percent
    if ((radio_stats.channel_load_tot_curr >= conf_total_ch_load_notification_hi_th_percent) &&
        (!radio_stats.channel_load_tot_is_above_hi_th)) {
        radio_stats.channel_load_tot_is_above_hi_th = true;
        send_notification                           = true;
        LOG(DEBUG) << "LOAD_NOTIFICATION, Th up, channel_load="
                   << int(radio_stats.channel_load_tot_curr);
    } else if ((radio_stats.channel_load_tot_curr <
                conf_total_ch_load_notification_lo_th_percent) &&
               (radio_stats.channel_load_tot_is_above_hi_th)) {
        radio_stats.channel_load_tot_is_above_hi_th = false;
        send_notification                           = true;
        LOG(DEBUG) << "LOAD_NOTIFICATION, Th down, channel_load="
                   << int(radio_stats.channel_load_tot_curr);
    }
    //check if current channel load delta passed monitor_channel_load_notification_delta_th_percent
    if (radio_stats.channel_load_tot_is_above_hi_th) {
        delta_val = abs(radio_stats.channel_load_tot_prev - radio_stats.channel_load_tot_curr);
        if (delta_val >= conf_total_ch_load_notification_delta_th_percent) {
            send_notification = true;
            LOG(DEBUG) << "LOAD_NOTIFICATION, Th delta, channel_load delta_val=" << int(delta_val);
        }
    }

    /*
    *    radio_stats active STA count
    */
    //check if current number of active STAs passed min_active_sta_count
    if ((radio_stats.active_client_count_curr < conf_min_active_client_count) &&
        (radio_stats.active_client_count_is_above_th)) {
        radio_stats.active_client_count_is_above_th = false;
        send_notification                           = true;
        LOG(DEBUG) << "LOAD_NOTIFICATION, Th down, vap active_sta_count="
                   << int(radio_stats.active_client_count_curr);
    } else if ((radio_stats.active_client_count_curr >= conf_min_active_client_count) &&
               (!radio_stats.active_client_count_is_above_th)) {
        radio_stats.active_client_count_is_above_th = true;
        send_notification                           = true;
        LOG(DEBUG) << "LOAD_NOTIFICATION, Th up, vap  active_sta_count="
                   << int(radio_stats.active_client_count_curr);
    } else if ((radio_stats.active_client_count_curr < conf_min_active_client_count) &&
               (!radio_stats.active_client_count_is_above_th)) {
        send_notification = false;
    }
    //check if new active_sta was added
    if (radio_stats.active_client_count_is_above_th) {
        delta_val = radio_stats.active_client_count_curr - radio_stats.active_client_count_prev;
        if (delta_val > 0) {
            send_notification = true;
            LOG(DEBUG) << "LOAD_NOTIFICATION, change, vap  active_sta_count="
                       << int(radio_stats.active_client_count_curr);
        }
    }

    /*
    *    active STA tx/rx load
    */
    //check if STAs load delta passed sta_load_notification_delta_th_percent
    if (radio_stats.active_client_count_is_above_th) {
        delta_val =
            abs((radio_stats.client_tx_load_tot_prev + radio_stats.client_rx_load_tot_prev) -
                (radio_stats.client_tx_load_tot_curr + radio_stats.client_rx_load_tot_curr));
        if (delta_val >= conf_client_load_notification_delta_th_percent) {
            send_notification = true;
            LOG(DEBUG) << "LOAD_NOTIFICATION, change, vap  sta load delta Th, delta_val="
                       << int(delta_val);
        }
    }

    if (send_notification) {
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION>(cmdu_tx);
        if (!notification) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION message!";
            return;
        }
        notification->params().stats_delta_ms         = radio_stats.delta_ms;
        notification->params().active_client_count    = radio_stats.active_client_count_curr;
        notification->params().client_tx_load_percent = radio_stats.client_tx_load_tot_curr;
        notification->params().client_rx_load_percent = radio_stats.client_rx_load_tot_curr;
        notification->params().channel_load_percent   = radio_stats.channel_load_tot_curr;
        message_com::send_cmdu(slave_socket, cmdu_tx);
    }

    /**
     * If a Multi-AP Agent receives a Metric Reporting Policy TLV with AP Metrics Channel
     * Utilization Reporting Threshold field set to a non-zero value for a given radio, it
     * shall measure the channel utilization on that radio in each consecutive implementation-
     * specific measurement period and, if the most recently measured channel utilization has
     * crossed the reporting threshold in either direction (with respect to the previous
     * measurement), it shall send an AP Metrics Response message to the Multi-AP Controller
     * containing one AP Metrics TLV for each of the BSSs on that radio.
     */
    auto &info = radio_node->ap_metrics_reporting_info();
    if (0 != info.ap_channel_utilization_reporting_threshold) {
        auto now = std::chrono::steady_clock::now();
        int elapsed_time_s =
            std::chrono::duration_cast<std::chrono::seconds>(
                now - info.ap_metrics_channel_utilization_last_reporting_time_point)
                .count();

        if (elapsed_time_s >= ap_metrics_channel_utilization_measurement_period_s) {
            info.ap_metrics_channel_utilization_last_reporting_time_point = now;

            // TODO:
            // Measure channel utilization and, if difference with previous value is greater than
            // threshold, then send an AP Metrics Response message to the Multi-AP Controller
            // containing one AP Metrics TLV for each of the BSSs on this radio.
        }
    }
}

bool monitor_stats::add_ap_metrics(ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &bssid)
{
    auto ap_metrics_response_tlv = cmdu_tx.addClass<wfa_map::tlvApMetrics>();

    if (!ap_metrics_response_tlv) {
        LOG(ERROR) << "Couldn't addClass tlvApMetrics";
        return false;
    }

    //TODO: fill ap_metrics_response_tlv with valid data (now valid just bssid_response)
    ap_metrics_response_tlv->bssid()                                      = bssid;
    ap_metrics_response_tlv->channel_utilization()                        = 10;
    ap_metrics_response_tlv->number_of_stas_currently_associated()        = 2;
    ap_metrics_response_tlv->estimated_service_parameters().include_ac_be = 1;

    if (!ap_metrics_response_tlv->alloc_estimated_service_info_field(3)) {
        LOG(ERROR)
            << "Couldn't allocate ap_metrics_response_tlv->alloc_estimated_service_info_field";
        return false;
    }
    std::fill_n(ap_metrics_response_tlv->estimated_service_info_field(), 3, 0);

    return true;
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
