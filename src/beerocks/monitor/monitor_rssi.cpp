/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "monitor_rssi.h"

#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_monitor.h>

using namespace beerocks;
using namespace net;
using namespace son;

monitor_rssi::monitor_rssi(ieee1905_1::CmduMessageTx &cmdu_tx_) : cmdu_tx(cmdu_tx_)
{
    mon_db                   = nullptr;
    slave_socket             = nullptr;
    arp_socket               = -1;
    arp_socket_class         = nullptr;
    m_idle_unit_tx_threshold = DEFAULT_IDLE_UNIT_TX_THRESHOLD;
    m_idle_unit_rx_threshold = DEFAULT_IDLE_UNIT_RX_THRESHOLD;
    m_idle_unit_time_ms      = DEFAULT_IDLE_UNIT_TIME_MS;
}

void monitor_rssi::stop()
{
    if (arp_socket_class) {
        arp_socket_class = nullptr;
    }
    if (arp_socket >= 0) {
        close(arp_socket);
        arp_socket = -1;
    }
    mon_db       = nullptr;
    slave_socket = nullptr;
}

bool monitor_rssi::start(monitor_db *mon_db_, Socket *slave_socket_)
{
    if (!mon_db_ || !slave_socket_) {
        LOG(ERROR) << "invalid input == NULL";
        return false;
    }

    stop();

    mon_db       = mon_db_;
    slave_socket = slave_socket_;

    if ((arp_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
        LOG(ERROR) << "Opening ARP socket";
        return false;
    }
    arp_socket_class = new Socket(arp_socket);
    std::string err  = arp_socket_class->getError();
    if (!err.empty()) {
        LOG(ERROR) << "Opening Socket err:" << err;
        return false;
    }
    return true;
}

Socket *monitor_rssi::get_arp_socket() { return arp_socket_class; }

void monitor_rssi::arp_recv()
{
    if (!mon_db) {
        LOG(WARNING) << "monitor is not attached yet, mon_db=nullptr";
        return;
    }

    network_utils::arp_hdr *arphdr;

    auto rx_buffer      = cmdu_tx.getMessageBuff();
    auto rx_buffer_size = cmdu_tx.getMessageBuffLength();

    int iLen;
    if ((iLen = recv(arp_socket, rx_buffer, rx_buffer_size, 0)) == 0) {
        LOG(ERROR) << "packet size is 0";
        return;
    } else if (iLen < 0) {
        LOG(ERROR) << "recv() < 0, errno:" << strerror(errno);
        return;
    }

    uint16_t proto_type = (uint16_t)(((rx_buffer[12]) << 8) + rx_buffer[13]);
    arphdr              = (network_utils::arp_hdr *)(rx_buffer + 6 + 6 + 2);
    if ((proto_type != ETH_P_ARP) || (ntohs(arphdr->opcode) != ARPOP_REPLY)) {
        // LOG(DEBUG) << "proto_type or arphdr->opcode are wrong type! proto_type=" << proto_type << " ETH_P_ARP= " << ETH_P_ARP << " arphdr->opcode=" << ntohs(arphdr->opcode);
        return;
    }

    std::string sta_mac = network_utils::mac_to_string(arphdr->sender_mac);
    std::string sta_ip  = network_utils::ipv4_to_string(arphdr->sender_ip);
    auto sta_node       = mon_db->sta_find(sta_mac);
    if (sta_node == nullptr) {
        sta_node = mon_db->sta_find_by_ipv4(sta_ip);
    }

    if (sta_node == nullptr) {
        //LOG(DEBUG) << "can't find node by mac=" << sta_mac << " or by ipv4=" << sta_ip << " in db! dropping arp reply";
        return;
    }

    auto arp_state = sta_node->get_arp_state();
    if (arp_state == monitor_sta_node::WAIT_FIRST_REPLY) {
        //LOG(DEBUG) << "arp_recv, state WAIT_FIRST_REPLY";
        //sending notification to slave->master.
        auto id_list = sta_node->get_rx_rssi_request_id_list();

        for (auto request_id : id_list) {
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION>(
                cmdu_tx, request_id);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building "
                              "cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION "
                              "message!";
                break;
            }

            notification->mac() = network_utils::mac_from_string(sta_node->get_mac());

            message_com::send_cmdu(slave_socket, cmdu_tx);
            LOG(DEBUG)
                << "send ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION, sta_mac = "
                << sta_node->get_mac() << " id=" << request_id;
        }

        if (conf_disable_arp) {
            LOG(DEBUG) << "ARP is disabled, returning";
            return;
        }
        // send arp burst
        auto sta_vap_id = sta_node->get_vap_id();
        auto vap_node   = mon_db->vap_get_by_id(sta_vap_id);
        if (vap_node == nullptr) {
            LOG(ERROR) << "can't find sta vap_id=" << sta_vap_id;
            return;
        }

        std::string arp_iface            = vap_node->get_bridge_iface();
        std::string arp_iface_ipv4       = vap_node->get_bridge_ipv4();
        std::string arp_iface_mac        = vap_node->get_bridge_mac();
        std::string sta_bridge_4addr_mac = sta_node->get_bridge_4addr_mac();
        bool is_4addr_client             = (sta_bridge_4addr_mac != network_utils::ZERO_MAC_STRING);
        std::string arp_dst_mac          = is_4addr_client ? sta_bridge_4addr_mac : sta_mac;

        int arp_burst_delay   = mon_db->get_arp_burst_delay();
        int arp_burst_pkt_num = mon_db->get_arp_burst_pkt_num();
        LOG(DEBUG) << "mon_db->arp_burst_delay = " << arp_burst_delay;
        LOG(DEBUG) << "arp_burst_pkt_num = " << arp_burst_pkt_num;
        if (arp_burst_delay < 10) {
            UTILS_SLEEP_MSEC(arp_burst_delay);
        } else {
            for (int i = arp_burst_delay; i > 0; i -= 5) {
                UTILS_SLEEP_MSEC(5);
                network_utils::arp_send(arp_iface, sta_node->get_ipv4(), arp_iface_ipv4,
                                        network_utils::mac_from_string(arp_dst_mac),
                                        network_utils::mac_from_string(arp_iface_mac), 1,
                                        arp_socket);
            }
        }

        for (int i = 0; i < arp_burst_pkt_num; i++) {
            network_utils::arp_send(arp_iface, sta_node->get_ipv4(), arp_iface_ipv4,
                                    network_utils::mac_from_string(arp_dst_mac),
                                    network_utils::mac_from_string(arp_iface_mac), 1, arp_socket);
            UTILS_SLEEP_MSEC(1);
        }
        sta_node->set_arp_state(monitor_sta_node::WAIT_REPLY);
        //LOG(DEBUG) << "state WAIT_FIRST_REPLY --> WAIT_REPLY , sta_mac = " << sta_mac;
    } else if (arp_state == monitor_sta_node::WAIT_REPLY) {
        sta_node->set_last_change_time();
        sta_node->arp_recv_count_inc();
        //LOG(DEBUG) << " on_arp_recv, sta_mac = " <<  sta_mac << " sta_node->arp_recv_count = " << (int)sta_node->arp_recv_count;
        if (sta_node->arp_recv_count_get() >= mon_db->MONITOR_ARP_COUNT_OK) {
            sta_node->set_arp_state(monitor_sta_node::SEND_RESPONSE);
            sta_node->set_rx_rssi_ready(false);
            //LOG(DEBUG) << " state WAIT_REPLY -> SEND_RESPONSE , sta_mac = " << sta_mac << " ip=" << sta_node->ip;
        }
    }
}

// enter every poll event done (~1sec or on req)
void monitor_rssi::process()
{
    bool poll_last = mon_db->is_last_poll();

    for (auto it = mon_db->sta_begin(); it != mon_db->sta_end(); ++it) {
        auto sta_mac  = it->first;
        auto sta_node = it->second;

        auto sta_vap_id = sta_node->get_vap_id();
        auto arp_state  = sta_node->get_arp_state();
        auto &sta_stats = sta_node->get_stats();

        if (arp_state == monitor_sta_node::IDLE) {
            if (!poll_last)
                continue;

            //LOG(DEBUG) << ">> monitor_sta_node::IDLE MAC: " << sta_mac;
            if (sta_stats.rx_rssi_curr != sta_stats.rx_rssi_prev) {
                if (sta_stats.rx_rssi_prev == beerocks::RSSI_INVALID) {
                    sta_stats.rx_rssi_prev = sta_stats.rx_rssi_curr;
                }
                int8_t delta_val = abs(sta_stats.rx_rssi_curr - sta_stats.rx_rssi_prev);
                //LOG(DEBUG) << ">> monitor_sta_node::IDLE, MAC=" << sta_mac << " sta_stats.rx_rssi_curr=" << int(sta_stats.rx_rssi_curr) << " sta_stats.rx_rssi_prev="<<int(sta_stats.rx_rssi_prev) << " delta_val=" << int(delta_val);
                // If radio is 2.4 Ghz, send notification even though threshold is not crossed
                if (delta_val >= conf_rx_rssi_notification_delta_db &&
                    (!is_5ghz ||
                     sta_stats.rx_rssi_curr <= conf_rx_rssi_notification_threshold_dbm)) {
                    sta_stats.rx_rssi_prev = sta_stats.rx_rssi_curr;

                    auto notification = message_com::create_vs_message<
                        beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION>(
                        cmdu_tx);
                    if (notification == nullptr) {
                        LOG(ERROR) << "Failed building "
                                      "ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUES message!";
                        break;
                    }

                    notification->params().mac        = network_utils::mac_from_string(sta_mac);
                    notification->params().rx_rssi    = sta_stats.rx_rssi_curr;
                    notification->params().rx_snr     = sta_stats.rx_snr_curr;
                    notification->params().rx_packets = 100; //dummy value
                    notification->params().rx_phy_rate_100kb = sta_stats.rx_phy_rate_100kb_min;
                    notification->params().tx_phy_rate_100kb = sta_stats.tx_phy_rate_100kb_min;
                    notification->params().vap_id            = sta_vap_id;
                    message_com::send_cmdu(slave_socket, cmdu_tx);
                    LOG(DEBUG) << "state IDLE, DELTA notification MAC: " << sta_mac
                               << " RX RSSI: " << int(sta_stats.rx_rssi_curr)
                               << " delta_val=" << int(delta_val);
                }
            }
            if (!conf_disable_initiative_arp) {
                if (std::chrono::steady_clock::now() >=
                    (sta_node->get_last_change_time() +
                     std::chrono::milliseconds(mon_db->MONITOR_LAST_CHANGE_TIMEOUT_MSEC))) {
                    sta_node->set_arp_state(monitor_sta_node::SEND_ARP);
                    LOG(INFO) << "state IDLE --> SEND_ARP, LAST_CHANGE_TIMEOUT"
                              << " ip=" << sta_node->get_ipv4() << " mac=" << sta_mac
                              << " sending arp";
                }
            }
        } else if (arp_state == monitor_sta_node::SEND_RESPONSE) {
            //LOG(INFO) << "state RESPONSE sta_node->pend_rx_rssi_ready=" << int(sta_node->pend_rx_rssi_ready);
            if (sta_node->get_rx_rssi_ready()) {
                sta_node->set_rx_rssi_ready(false);
                sta_node->arp_recv_count_clear();
                sta_node->arp_retry_count_clear();
                sta_node->set_arp_burst(false);
                sta_node->set_arp_state(monitor_sta_node::IDLE);
                send_rssi_measurement_response(sta_mac, sta_node);
            }
        } else if ((arp_state == monitor_sta_node::WAIT_REPLY ||
                    arp_state == monitor_sta_node::WAIT_FIRST_REPLY) &&
                   (std::chrono::steady_clock::now() >=
                    (sta_node->arp_get_start_time() +
                     std::chrono::milliseconds(mon_db->MONITOR_ARP_TIMEOUT_MSEC)))) {
            sta_node->arp_retry_count_inc();
            if (sta_node->arp_retry_count_get() > mon_db->MONITOR_ARP_RETRY_COUNT) {
                LOG(INFO) << "state WAIT_REPLY|WAIT_FIRST_REPLY --> IDLE, arp_retry_count > "
                             "MONITOR_ARP_RETRY_COUNT";
                if (sta_node->arp_recv_count_get() == 0) {
                    auto notification = message_com::create_vs_message<
                        beerocks_message::cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION>(cmdu_tx);
                    if (notification == nullptr) {
                        LOG(ERROR) << "Failed building "
                                      "cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION message!";
                        break;
                    }

                    notification->mac() = network_utils::mac_from_string(sta_mac);
                    message_com::send_cmdu(slave_socket, cmdu_tx);
                    LOG(INFO) << "arp_recv_count == 0, max arp retry no recv, send > "
                                 "NO_ACTIVITY_NOTIFICATION";
                    sta_node->clear_rx_rssi_request_id_list();
                } else {
                    send_rssi_measurement_response(sta_mac, sta_node);
                }
                sta_node->arp_recv_count_clear();
                sta_node->arp_retry_count_clear();
                sta_node->set_arp_burst(false);
                sta_node->set_arp_state(monitor_sta_node::IDLE);
                //LOG(DEBUG) << "state WAIT_REPLY|WAIT_FIRST_REPLY -> IDLE , sta_mac = " << sta_mac << "ip=" << sta_node->ip;
            } else {
                sta_node->set_arp_state(monitor_sta_node::SEND_ARP);
                //LOG(DEBUG) << "state WAIT_REPLY|WAIT_FIRST_REPLY --> SEND_ARP, timeout, ip=" << sta_node->ip << " recv_count=" <<  (int)sta_node->arp_recv_count << " retry_count=" << (int)sta_node->arp_retry_count << " sta_mac = "<< sta_mac;
            }
        }

        // send arp to target STA //
        if (arp_state == monitor_sta_node::SEND_ARP) {
            if (sta_node->get_arp_burst()) {
                sta_node->set_arp_state(monitor_sta_node::WAIT_FIRST_REPLY);
            } else {
                sta_node->set_arp_state(monitor_sta_node::WAIT_REPLY);
            }
            sta_node->arp_set_start_time();

            auto sta_vap_id = sta_node->get_vap_id();
            auto vap_node   = mon_db->vap_get_by_id(sta_vap_id);
            if (vap_node == nullptr) {
                LOG(ERROR) << "can't find sta vap_id=" << sta_vap_id;
                return;
            }

            if (!conf_disable_arp) {
                std::string arp_iface            = vap_node->get_bridge_iface();
                std::string arp_iface_ipv4       = vap_node->get_bridge_ipv4();
                std::string arp_iface_mac        = vap_node->get_bridge_mac();
                std::string sta_bridge_4addr_mac = sta_node->get_bridge_4addr_mac();
                bool is_4addr_client    = (sta_bridge_4addr_mac != network_utils::ZERO_MAC_STRING);
                std::string arp_dst_mac = is_4addr_client ? sta_bridge_4addr_mac : sta_mac;

                LOG(DEBUG) << "state: SEND_ARP -> "
                           << (sta_node->get_arp_burst() ? "WAIT_FIRST_REPLY" : "WAIT_REPLY")
                           << ", arp_iface = " << arp_iface
                           << ", arp_iface_ipv4 = " << arp_iface_ipv4
                           << ", arp_iface_mac = " << arp_iface_mac
                           << ", is_4addr_client = " << int(is_4addr_client)
                           << ", sta_mac = " << sta_mac << ", dest_ip = " << sta_node->get_ipv4()
                           << ", dst_mac = " << arp_dst_mac;

                network_utils::arp_send(arp_iface, sta_node->get_ipv4(), arp_iface_ipv4,
                                        network_utils::mac_from_string(arp_dst_mac),
                                        network_utils::mac_from_string(arp_iface_mac), 6,
                                        arp_socket);
            } else {
                LOG(DEBUG) << "ARP is disabled, state: SEND_ARP -> SEND_RESPONSE";
                sta_node->set_arp_state(monitor_sta_node::SEND_RESPONSE);
                sta_node->set_rx_rssi_ready(false);
            }
        }
        // Monitor for idle station
        if (sta_node->enable_idle_monitor) {
            monitor_idle_station(sta_mac, sta_node);
        }
    }
}

void monitor_rssi::send_rssi_measurement_response(std::string &sta_mac, monitor_sta_node *sta_node)
{
    auto id_list    = sta_node->get_rx_rssi_request_id_list();
    auto &sta_stats = sta_node->get_stats();

    int rx_packets = mon_db->MONITOR_ARP_PKT_NUM;

    for (auto request_id : id_list) {

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(cmdu_tx,
                                                                                   request_id);
        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUES message!";
            break;
        }

        response->params().mac               = network_utils::mac_from_string(sta_mac);
        response->params().rx_rssi           = sta_stats.rx_rssi_curr;
        response->params().rx_snr            = sta_stats.rx_snr_curr;
        response->params().rx_packets        = rx_packets;
        response->params().rx_phy_rate_100kb = sta_stats.rx_phy_rate_100kb_min;
        response->params().tx_phy_rate_100kb = sta_stats.tx_phy_rate_100kb_min;

        message_com::send_cmdu(slave_socket, cmdu_tx);
        LOG(DEBUG) << "RSSI_MEASUREMENT_RESPONSE sta_mac=" << sta_mac
                   << " rx_rssi: " << int(response->params().rx_rssi) << " id=" << request_id;
    }
    sta_node->clear_rx_rssi_request_id_list();
}

void monitor_rssi::monitor_idle_station(std::string &sta_mac, monitor_sta_node *sta_node)
{
    auto current_time = std::chrono::steady_clock::now();
    auto &sta_stats   = sta_node->get_stats();
    if ((sta_stats.hal_stats.rx_bytes < m_idle_unit_rx_threshold) &&
        (sta_stats.hal_stats.tx_bytes < m_idle_unit_tx_threshold)) {
        if (!sta_node->idle_detected) {
            sta_node->idle_detected_start_time = current_time;
            sta_node->idle_detected            = true;
            LOG(DEBUG) << "IDLE detected MAC: " << sta_mac;
        }
    } else {
        sta_node->idle_detected = false;
    }

    if (sta_node->idle_detected) {
        auto idle_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           current_time - sta_node->idle_detected_start_time)
                           .count();
        if (idle_ms > m_idle_unit_time_ms) {
            LOG(DEBUG) << "IDLE notification MAC: " << sta_mac;
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR)
                    << "Failed building ACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION message!";
                return;
            }

            notification->mac() = network_utils::mac_from_string(sta_mac);
            message_com::send_cmdu(slave_socket, cmdu_tx);

            sta_node->idle_detected       = false;
            sta_node->enable_idle_monitor = false;
        }
    }
}
