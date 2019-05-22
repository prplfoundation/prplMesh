/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "monitor_thread.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>

#include <cmath>

using namespace beerocks;
using namespace net;
using namespace son;

#define HAL_MAX_COMMAND_FAILURES 10
#define OPERATION_SUCCESS 0
#define OPERATION_FAIL -1

monitor_thread::monitor_thread(const std::string &slave_uds_, const std::string &monitor_iface_,
                               beerocks::config_file::sConfigSlave &beerocks_slave_conf_,
                               beerocks::logging &logger_)
    : socket_thread(), monitor_iface(monitor_iface_), beerocks_slave_conf(beerocks_slave_conf_),
      bridge_iface(beerocks_slave_conf.bridge_iface), slave_uds(slave_uds_), logger(logger_),
      mon_rssi(cmdu_tx),
#ifdef BEEROCKS_RDKB
      mon_rdkb_hal(cmdu_tx),
#endif
      mon_stats(cmdu_tx)
{
    thread_name = "monitor";

    using namespace std::placeholders; // for `_1`

    // Create new Monitor HAL instance
    mon_wlan_hal = std::shared_ptr<bwl::mon_wlan_hal>(
        mon_wlan_hal_create(monitor_iface_,
                            std::bind(&monitor_thread::hal_event_handler, this, _1)),
        [](bwl::mon_wlan_hal *obj) {
            if (obj)
                mon_wlan_hal_destroy(obj);
        });

    LOG_IF(!mon_wlan_hal, FATAL) << "Failed creating HAL instance!";
}

monitor_thread::~monitor_thread() { stop_monitor_thread(); }

void monitor_thread::on_thread_stop()
{
    LOG(DEBUG) << "on_thread_stop() - call stop_monitor_thread()";
    stop_monitor_thread();
}

void monitor_thread::stop_monitor_thread()
{
    if (received_error_notification_ack_retry == -1) {
        received_error_notification_ack_retry = 5;
    }

    if (received_error_notification_ack_retry > 0) {

        auto radio_node   = mon_db.get_radio_node();
        std::string iface = radio_node->get_iface();

        auto notification =
            message_com::create_vs_message<beerocks_message::cACTION_MONITOR_ERROR_NOTIFICATION>(
                cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building cACTION_MONITOR_ERROR_NOTIFICATION message!";
            return;
        }
        notification->error_code() = thread_last_error_code;
        message_com::send_cmdu(slave_socket, cmdu_tx);
        received_error_notification_ack_retry--;

    } else {

        LOG(ERROR) << "disconnecting monitor_thread sockets";
        if (mon_hal_ext_events || slave_socket)
            LOG(DEBUG) << "stop_monitor_thread()";

        if (mon_hal_ext_events) {
            mon_rssi.stop();
            mon_stats.stop();
#ifdef BEEROCKS_RDKB
            mon_rdkb_hal.stop();
#endif
            mon_wlan_hal->detach();
            remove_socket(mon_hal_ext_events);
            delete mon_hal_ext_events;
            mon_hal_ext_events = nullptr;
            remove_socket(mon_rssi.get_arp_socket());
        }

        if (mon_hal_int_events) {
            remove_socket(mon_hal_int_events);
            delete mon_hal_int_events;
            mon_hal_int_events = nullptr;
        }

        if (slave_socket) {
            remove_socket(slave_socket);
            slave_socket->closeSocket();
            delete slave_socket;
            slave_socket = nullptr;
        }
    }

    should_stop = true;
}

bool monitor_thread::socket_disconnected(Socket *sd)
{
    LOG(TRACE) << "socket disconnected!";

    if (!sd) {
        LOG(ERROR) << "Invalid socket!";
        return false;
    }

    if (slave_socket && (sd == slave_socket)) {
        LOG(ERROR) << "slave socket disconnected!, stop_monitor_thread()";
        stop_monitor_thread();
        return false;
    } else if (mon_hal_ext_events && (sd == mon_hal_ext_events)) {
        LOG(ERROR) << "mon_hal_ext_events socket disconnected!";
        thread_last_error_code = MONITOR_THREAD_ERROR_HAL_DISCONNECTED;
        stop_monitor_thread();
        return false;
    } else if (mon_hal_int_events && (sd == mon_hal_int_events)) {
        LOG(ERROR) << "mon_hal_int_events socket disconnected!";
        thread_last_error_code = MONITOR_THREAD_ERROR_HAL_DISCONNECTED;
        stop_monitor_thread();
        return false;
    }

    return true;
}

std::string monitor_thread::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

bool monitor_thread::init()
{
    LOG(DEBUG) << "init() start";

    auto radio_node = mon_db.get_radio_node();
    radio_node->set_iface(monitor_iface);

    set_select_timeout(mon_db.MONITOR_DB_POLLING_RATE_MSEC);

    received_error_notification_ack_retry = -1;

    mon_rssi.conf_disable_arp = (beerocks_slave_conf.debug_disable_arp == "1");

    // Initialize the monitor hal
    // if(!mon_hal.init(&mon_db)){
    //     LOG(ERROR) << "mon_hal initialization failed";
    //     return false;
    // }

    //connect to slave //
    if (slave_socket)
        delete slave_socket;
    slave_socket    = new SocketClient(slave_uds);
    std::string err = slave_socket->getError();
    if (!err.empty()) {
        LOG(ERROR) << "slave_socket: " << err;
        delete slave_socket;
        slave_socket = nullptr;
        return false;
    }

    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_MONITOR_JOINED_NOTIFICATION>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    message_com::send_cmdu(slave_socket, cmdu_tx);

    add_socket(slave_socket);
    LOG(DEBUG) << "init() end";
    return true;
}

void monitor_thread::after_select(bool timeout)
{
    // Continue only if slave is connected
    if (slave_socket == nullptr) {
        LOG(DEBUG) << "slave_socket == nullptr";
        return;
    }

    // ???
    if (received_error_notification_ack_retry == 0) {
        LOG(TRACE) << "received_error_notification_ack_retry";
        stop_monitor_thread();
        return;
    }

    // Unexpected HAL detach or too many failed commands
    if ((mon_wlan_hal->get_state() != bwl::HALState::Operational && mon_hal_attached == true) ||
        (hal_command_failures_count > HAL_MAX_COMMAND_FAILURES)) {
        LOG(ERROR) << "Unexpected HAL detach detected - Failed commands: "
                   << hal_command_failures_count;
        thread_last_error_code = MONITOR_THREAD_ERROR_SUDDEN_DETACH;
        stop_monitor_thread();
        return;
    }

    // If the HAL is not yet attached
    if (mon_hal_int_events == nullptr) { // monitor not attached
        auto attach_state = mon_wlan_hal->attach();
        if (last_attach_state != attach_state) {
            LOG(DEBUG) << "attach_state = " << int(attach_state);
            last_attach_state = attach_state;
        }
        if (attach_state == bwl::HALState::Operational) {

            LOG(DEBUG) << "attach_state == bwl::HALState::Operational";

            // Initialize VAPs in the DB
            update_vaps_in_db();

            // External events
            int ext_events_fd = mon_wlan_hal->get_ext_events_fd();
            if (ext_events_fd > 0) {
                mon_hal_ext_events = new Socket(ext_events_fd);
                add_socket(mon_hal_ext_events);
                // No external event FD is available, we will trigger the process method periodically
            } else if (ext_events_fd == 0) {
                mon_hal_ext_events = nullptr;
            } else {
                LOG(ERROR) << "Invalid external event file descriptor: " << ext_events_fd;
                thread_last_error_code = MONITOR_THREAD_ERROR_ATTACH_FAIL;
                stop_monitor_thread();
                return;
            }

            // Internal events
            int int_events_fd = mon_wlan_hal->get_int_events_fd();
            if (int_events_fd > 0) {

                // NOTE: Eventhough the internal events are not socket based, at this
                //       point we have to use the Socket class wrapper to add the
                //       file descriptor into the select()
                mon_hal_int_events = new Socket(int_events_fd);
                add_socket(mon_hal_int_events);
            } else {
                LOG(ERROR) << "Invalid internal event file descriptor: " << int_events_fd;
                thread_last_error_code = MONITOR_THREAD_ERROR_ATTACH_FAIL;
                stop_monitor_thread();
                return;
            }

            // start local monitors //
            LOG(TRACE) << "mon_stats.start()";
            if (!mon_stats.start(&mon_db, slave_socket)) {
                LOG(ERROR) << "mon_stats.start() failed";
                thread_last_error_code = MONITOR_THREAD_ERROR_ATTACH_FAIL;
                stop_monitor_thread();
                return;
            }

            LOG(TRACE) << "mon_rssi.start()";
            if (!mon_rssi.start(&mon_db, slave_socket)) {
                LOG(ERROR) << "mon_rssi.start() failed";
                thread_last_error_code = MONITOR_THREAD_ERROR_ATTACH_FAIL;
                stop_monitor_thread();
                return;
            }

#ifdef BEEROCKS_RDKB
            LOG(TRACE) << "mon_rdkb_hal.start()";
            if (!mon_rdkb_hal.start(&mon_db, slave_socket)) {
                LOG(ERROR) << "mon_rdkb_hal.start() failed";
                thread_last_error_code = MONITOR_THREAD_ERROR_ATTACH_FAIL;
                stop_monitor_thread();
                return;
            }
#endif

            mon_rssi.is_5ghz = mon_wlan_hal->get_radio_info().is_5ghz;

            add_socket(mon_rssi.get_arp_socket());

            LOG(DEBUG) << "Monitor attach process finished successfully!";
            mon_hal_attached = true;

        } else if (attach_state == bwl::HALState::Failed) {
            LOG(ERROR) << "Failed attaching to WLAN HAL, call stop_monitor_thread()";
            thread_last_error_code = MONITOR_THREAD_ERROR_ATTACH_FAIL;
            stop_monitor_thread();
            return;
        }

        // HAL is attached and operational
    } else {

        // Process ARP events
        if (read_ready(mon_rssi.get_arp_socket())) {
            clear_ready(mon_rssi.get_arp_socket());
            mon_rssi.arp_recv();
        }

        // Process external events
        if (mon_hal_ext_events) {
            if (read_ready(mon_hal_ext_events)) {
                if (!mon_wlan_hal->process_ext_events()) {
                    LOG(ERROR) << "process_ext_events() failed!";
                    thread_last_error_code = MONITOR_THREAD_ERROR_REPORT_PROCESS_FAIL;
                    stop_monitor_thread();
                    return;
                }
                clear_ready(mon_hal_ext_events);
            }
        } else {
            // There is no socket for external events, so we simply try
            // to process any available periodically
            if (!mon_wlan_hal->process_ext_events()) {
                LOG(ERROR) << "process_ext_events() failed!";
                thread_last_error_code = MONITOR_THREAD_ERROR_REPORT_PROCESS_FAIL;
                stop_monitor_thread();
                return;
            }
        }

        // Process internal events
        if (mon_hal_int_events && read_ready(mon_hal_int_events)) {
            clear_ready(mon_hal_int_events);
            // NOTE: A callback (hal_event_handler()) will be invoked for pending events
            if (!mon_wlan_hal->process_int_events()) {
                LOG(ERROR) << "process_int_events() failed!";
                thread_last_error_code = MONITOR_THREAD_ERROR_REPORT_PROCESS_FAIL;
                stop_monitor_thread();
                return;
            }
        }

        if (pending_11k_events.size() > 0) {
            auto now_time = std::chrono::steady_clock::now();
            for (auto it = pending_11k_events.begin(); it != pending_11k_events.end();) {
                if (std::chrono::milliseconds(5000) <= (now_time - it->second.timestamp)) {
                    it = pending_11k_events.erase(it);
                } else {
                    ++it;
                }
            }
        }

        auto now = std::chrono::steady_clock::now();

        // Update DB - Polling
        if (now >= mon_db.get_poll_next_time()) {

            mon_db.set_poll_next_time(
                std::chrono::steady_clock::now() +
                std::chrono::milliseconds(mon_db.MONITOR_DB_POLLING_RATE_MSEC));

            // Update the statistics
            update_sta_stats();
            // NOTE: Radio & VAP statistics are updated only on last poll cycle
            if (mon_db.is_last_poll())
                update_ap_stats();

            send_heartbeat();

            mon_db.poll_done();
        }

        if (now >= mon_db.get_ap_poll_next_time()) {
            mon_db.set_ap_poll_next_time(
                std::chrono::steady_clock::now() +
                std::chrono::seconds(mon_db.MONITOR_DB_AP_POLLING_RATE_SEC));

            // Updated tx state in mon_man_hal
            if (!mon_wlan_hal->refresh_radio_info()) {
                LOG(WARNING) << "Failed refreshing the radio info";
                // Try to ping hostap
                if (!mon_wlan_hal->ping()) {
                    LOG(ERROR) << "Failed ping hostap, notify agent...";
                    auto notification = message_com::create_vs_message<
                        beerocks_message::cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION>(cmdu_tx);
                    if (notification == nullptr) {
                        LOG(ERROR) << "Failed building "
                                      "cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION message!";
                        stop_monitor_thread();
                        return;
                    }

                    notification->vap_id() = beerocks::IFACE_RADIO_ID;
                    message_com::send_cmdu(slave_socket, cmdu_tx);
                    return;
                }
            }

            int8_t new_tx_state               = mon_wlan_hal->get_radio_info().tx_enabled;
            int8_t new_hostap_enabled_state   = mon_wlan_hal->get_radio_info().wifi_ctrl_enabled;
            bool tx_state_changed             = false;
            bool hostap_enabled_state_changed = false;

            if (mon_db.get_ap_tx_enabled() != new_tx_state) { // tx was changed
                mon_db.set_ap_tx_enabled(new_tx_state);
                tx_state_changed = true;
            }

            if (mon_db.get_hostapd_enabled() !=
                new_hostap_enabled_state) { // ap_enabled was changed
                if (new_hostap_enabled_state == 2) {
                    LOG(DEBUG) << "wifi_ctrl_enabled=2 on already attached to Hostapd";
                }
                mon_db.set_hostapd_enabled(new_hostap_enabled_state);
                hostap_enabled_state_changed = true;
            }

            if (tx_state_changed || hostap_enabled_state_changed) {
                // LOG(DEBUG) << "wifi_ctrl_enabled=" << int(new_hostap_enabled_state) << ", tx_enabled=" << int(new_tx_state);

                auto notification = message_com::create_vs_message<
                    beerocks_message::cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION>(cmdu_tx);
                if (notification == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return;
                }
                notification->new_tx_state()             = new_tx_state;
                notification->new_hostap_enabled_state() = new_hostap_enabled_state;
                message_com::send_cmdu(slave_socket, cmdu_tx);
            }
        }

        mon_rssi.process();
        mon_stats.process();
#ifdef BEEROCKS_RDKB
        mon_rdkb_hal.process();
#endif
    }
}

bool monitor_thread::update_sta_stats()
{
    auto poll_cnt  = mon_db.get_poll_cnt();
    auto poll_last = mon_db.is_last_poll();

    for (auto it = mon_db.sta_begin(); it != mon_db.sta_end(); ++it) {

        auto sta_mac  = it->first;
        auto sta_node = it->second;

        if (sta_node == nullptr) {
            LOG(WARNING) << "Invalid node pointer for STA = " << sta_mac;
            continue;
        }

        auto vap_node   = mon_db.vap_get_by_id(sta_node->get_vap_id());
        auto &sta_stats = sta_node->get_stats();

        // Update the stats
        if (!mon_wlan_hal->update_stations_stats(vap_node->get_iface(), sta_mac,
                                                 sta_stats.hal_stats)) {
            LOG(ERROR) << "Failed updating STA (" << sta_mac << ") statistics!";
            return false;
        }

        // Reset STA poll data
        if (poll_cnt == 0) {
            sta_node->reset_poll_data();
        }
        sta_stats.poll_cnt++;

        // Update TX Phy Rate
        auto val = sta_stats.hal_stats.tx_phy_rate_100kb;
        if (poll_cnt == 0 || val < sta_stats.tx_phy_rate_100kb_min) {
            sta_stats.tx_phy_rate_100kb_min = val;
        }
        sta_stats.tx_phy_rate_100kb_acc += val;
        if (poll_last) {
            sta_stats.tx_phy_rate_100kb_avg =
                float(sta_stats.tx_phy_rate_100kb_acc) / float(sta_stats.poll_cnt);
        }

        // Update RX Phy Rate
        val = sta_stats.hal_stats.rx_phy_rate_100kb;
        if (poll_cnt == 0 || val < sta_stats.rx_phy_rate_100kb_min) {
            sta_stats.rx_phy_rate_100kb_min = val;
        }
        sta_stats.rx_phy_rate_100kb_acc += val;
        if (poll_last) {
            sta_stats.rx_phy_rate_100kb_avg =
                float(sta_stats.rx_phy_rate_100kb_acc) / float(sta_stats.poll_cnt);
        }

        // Update RSSI
        if (poll_last) {
            if (sta_stats.hal_stats.rx_rssi_watt_samples_cnt > 0) {
                float rssi_watt = sta_stats.hal_stats.rx_rssi_watt /
                                  float(sta_stats.hal_stats.rx_rssi_watt_samples_cnt);
                float rssi_db = 10 * log10(rssi_watt);
                if (sta_stats.rx_rssi_curr != int8_t(rssi_db)) {
                    sta_node->set_last_change_time();
                }
                sta_stats.rx_rssi_curr = int8_t(rssi_db);
                //LOG(INFO)  << sta_mac << ", rx_rssi=" << int(rssi_db);
            }

            sta_node->set_rx_rssi_ready(true);
        }

        // Update SNR
        if (poll_last) {
            if (sta_stats.hal_stats.rx_snr_watt_samples_cnt > 0) {
                float snr_watt = sta_stats.hal_stats.rx_snr_watt /
                                 float(sta_stats.hal_stats.rx_snr_watt_samples_cnt);
                float snr_db = 10 * log10(snr_watt);
                if (sta_stats.rx_snr_curr != int8_t(snr_db)) {
                    sta_node->set_last_change_time();
                }
                sta_stats.rx_snr_curr = int8_t(snr_db);
                //LOG(INFO)  << sta_mac << ", rx_snr=" << int(snr_db);
            }

            sta_node->set_rx_snr_ready(true);
        }

        // Update the measurement timestamp
        auto now = std::chrono::steady_clock::now();
        auto time_span =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - sta_stats.last_update_time);
        sta_stats.delta_ms         = float(time_span.count());
        sta_stats.last_update_time = now;
    }

    return true;
}

bool monitor_thread::update_ap_stats()
{
    // Radio Statistics
    auto &radio_stats = mon_db.get_radio_node()->get_stats();

    // Update radio statistics
    if (!mon_wlan_hal->update_radio_stats(radio_stats.hal_stats)) {
        LOG(ERROR) << "Failed updating Radio statistics!";
        return false;
    }

    // Update the measurement timestamp
    auto now = std::chrono::steady_clock::now();
    auto time_span =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - radio_stats.last_update_time);
    radio_stats.delta_ms         = float(time_span.count());
    radio_stats.last_update_time = now;

    // VAP Statistics
    radio_stats.total_retrans_count = 0;
    radio_stats.sta_count           = 0;
    // For every available VAP
    for (int vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id <= beerocks::IFACE_VAP_ID_MAX; vap_id++) {

        auto vap_node = mon_db.vap_get_by_id(vap_id);

        // Break if there are no more available VAPs
        if (!vap_node) {
            continue;
        }

        auto &vap_stats = vap_node->get_stats();

        // Update the stats
        if (!mon_wlan_hal->update_vap_stats(vap_node->get_iface(), vap_stats.hal_stats)) {
            LOG(ERROR) << "Failed updating VAP statistics!";
            return false;
        }

        // Update radio countnters
        radio_stats.total_retrans_count += vap_stats.hal_stats.retrans_count;
        radio_stats.sta_count += vap_node->sta_get_count();

        // Update the measurement timestamp
        auto now = std::chrono::steady_clock::now();
        auto time_span =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - vap_stats.last_update_time);
        vap_stats.delta_ms         = float(time_span.count());
        vap_stats.last_update_time = now;
    }

    return true;
}

bool monitor_thread::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header == nullptr) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    if (beerocks_header->action() != beerocks_message::ACTION_MONITOR) {
        LOG(ERROR) << "Unsupported action: " << int(beerocks_header->action());
        return false;
    } else if (slave_socket != sd) {
        LOG(ERROR) << "slave_socket != sd";
        return false;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_MONITOR_SON_CONFIG_UPDATE: {
        LOG(TRACE) << "received ACTION_MONITOR_SON_CONFIG_UPDATE";
        auto update = cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_SON_CONFIG_UPDATE>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_SON_CONFIG_UPDATE failed";
            return false;
        }
        mon_stats.conf_total_ch_load_notification_hi_th_percent =
            update->config().monitor_total_ch_load_notification_hi_th_percent;
        mon_stats.conf_total_ch_load_notification_lo_th_percent =
            update->config().monitor_total_ch_load_notification_lo_th_percent;
        mon_stats.conf_total_ch_load_notification_delta_th_percent =
            update->config().monitor_total_ch_load_notification_delta_th_percent;
        mon_stats.conf_min_active_client_count = update->config().monitor_min_active_clients;
        mon_stats.conf_active_client_th        = update->config().monitor_active_client_th;
        mon_stats.conf_client_load_notification_delta_th_percent =
            update->config().monitor_client_load_notification_delta_th_percent;
        mon_stats.conf_ap_idle_threshold_B     = update->config().monitor_ap_idle_threshold_B;
        mon_stats.conf_ap_active_threshold_B   = update->config().monitor_ap_active_threshold_B;
        mon_stats.conf_ap_idle_stable_time_sec = update->config().monitor_ap_idle_stable_time_sec;
        mon_rssi.conf_rx_rssi_notification_delta_db =
            update->config().monitor_rx_rssi_notification_delta_db;
        mon_rssi.conf_rx_rssi_notification_threshold_dbm =
            update->config().monitor_rx_rssi_notification_threshold_dbm;
        mon_rssi.conf_disable_initiative_arp = update->config().monitor_disable_initiative_arp;
        break;
    }
    case beerocks_message::ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST: {
        // LOG(TRACE) << "received ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST"; // floods the log
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST failed";
            return false;
        }
        mon_stats.add_request(beerocks_header->id(), request->sync());
        if (request->sync()) {
            mon_db.set_poll_next_time(std::chrono::steady_clock::now(), true);
        }
        break;
    }
    case beerocks_message::ACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL: {
        LOG(TRACE) << "received ACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL failed";
            return false;
        }
        logger.set_log_level_state((eLogLevel)request->params().log_level,
                                   request->params().enable);
        break;
    }
    case beerocks_message::ACTION_MONITOR_ERROR_NOTIFICATION_ACK: {
        LOG(TRACE) << "received ACTION_MONITOR_ERROR_NOTIFICATION_ACK";
        received_error_notification_ack_retry = 0;
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_START_MONITORING_REQUEST: {
        LOG(TRACE) << "received ACTION_MONITOR_CLIENT_START_MONITORING_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST failed";
            return false;
        }
        std::string sta_mac  = network_utils::mac_to_string(request->params().mac);
        std::string sta_ipv4 = network_utils::ipv4_to_string(request->params().ipv4);
        std::string set_bridge_4addr_mac =
            network_utils::mac_to_string(request->params().bridge_4addr_mac);
        int vap_id = int(request->params().vap_id);
        LOG(INFO) << "ACTION_MONITOR_CLIENT_START_MONITORING_REQUEST=" << sta_mac
                  << " ip=" << sta_ipv4 << " vap_id=" << vap_id;

        auto sta_node = mon_db.sta_find(sta_mac);
        if (sta_node) {
            mon_db.sta_erase(sta_mac);
        }

        auto vap_node = mon_db.vap_get_by_id(vap_id);
        if (vap_node == nullptr) {
            LOG(ERROR) << "vap_id " << vap_id << " doesn't not exists";
            return false;
        }

        sta_node = mon_db.sta_add(sta_mac, vap_id);
        sta_node->set_ipv4(sta_ipv4);
        sta_node->set_bridge_4addr_mac(set_bridge_4addr_mac);
#ifdef BEEROCKS_RDKB
        //clean rdkb monitor data if already in database.
        auto client = mon_rdkb_hal.conf_get_client(sta_mac);
        if (client != nullptr) {
            client->setStartTime(std::chrono::steady_clock::now());
            client->setLastSampleTime(std::chrono::steady_clock::now());
            client->setAccumulatedPackets(0);
            client->clearData();
        }
#endif
        break;
    }
#ifdef BEEROCKS_RDKB
    case beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST: {

        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST failed";
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE,
                OPERATION_FAIL);
            return false;
        }

        const auto bssid = network_utils::mac_to_string(request->params().cfg.bssid);
        int vap_id       = mon_db.get_vap_id(bssid);

        LOG(TRACE) << "ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST" << std::endl
                   << "remove " << int(request->params().remove) << std::endl
                   << "bssid " << bssid << std::endl
                   << "inactCheckIntervalSec " << request->params().cfg.inactCheckIntervalSec
                   << std::endl
                   << "inactCheckThresholdSec " << request->params().cfg.inactCheckThresholdSec
                   << std::endl
                   << "vap_id " << int(vap_id);

        if (vap_id == IFACE_ID_INVALID) {
            LOG(ERROR) << "wrong vap_id: " << int(vap_id);
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE,
                OPERATION_FAIL);
            return false;
        }

        if (request->params().remove) {
            if (mon_rdkb_hal.conf_erase_ap(vap_id) == false) {
                LOG(ERROR) << "failed removing vap_id:" << int(vap_id) << " configuration";
                send_steering_return_status(
                    beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE,
                    OPERATION_FAIL);
                return false;
            }
            LOG(INFO) << "vap_id: " << int(vap_id) << " configuration was removed";
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE,
                OPERATION_SUCCESS);
            break;
        }
        auto ap = mon_rdkb_hal.conf_add_ap(vap_id);
        if (ap == nullptr) {
            LOG(ERROR) << "add rdkb_hall ap configuration fail";
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE,
                OPERATION_FAIL);
            return false;
        }

        ap->setInactCheckIntervalSec(request->params().cfg.inactCheckIntervalSec);
        ap->setInactCheckThresholdSec(request->params().cfg.inactCheckThresholdSec);
        ap->setInactCheckThresholdPackets(0); //TODO: set when cmdu has this param.

        //send success status
        send_steering_return_status(
            beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE, OPERATION_SUCCESS);

        break;
    }
    case beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_REQUEST: {

        LOG(TRACE) << "received ACTION_MONITOR_STEERING_CLIENT_SET_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST failed";
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE, OPERATION_FAIL);
            return false;
        }

        std::string sta_mac = network_utils::mac_to_string(request->params().client_mac);
        if (request->params().remove) {
            if (mon_rdkb_hal.conf_erase_client(sta_mac) == false) {
                LOG(ERROR) << "failed removing client:" << sta_mac << " configuration";
                send_steering_return_status(
                    beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE, OPERATION_FAIL);
                return false;
            }
            LOG(DEBUG) << "client: " << sta_mac << " configuration was removed";
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE, OPERATION_SUCCESS);
            break;
        }

        const auto bssid = network_utils::mac_to_string(request->params().bssid);
        int vap_id       = mon_db.get_vap_id(bssid);

        LOG(DEBUG) << "snrInactXing " << request->params().config.snrInactXing << std::endl
                   << "snrHighXing " << request->params().config.snrHighXing << std::endl
                   << "snrLowXing " << request->params().config.snrLowXing << std::endl
                   << "vapId " << vap_id;

        if (vap_id == IFACE_ID_INVALID) {
            LOG(ERROR) << "wrong vap_id:" << int(vap_id);
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE, OPERATION_FAIL);
            return false;
        }

        auto client = mon_rdkb_hal.conf_add_client(sta_mac);
        if (client == nullptr) {
            LOG(ERROR) << "add rdkb_hall client configuration fail";
            send_steering_return_status(
                beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE, OPERATION_FAIL);
            return false;
        }

        client->setSnrHighXing(request->params().config.snrHighXing);
        client->setSnrLowXing(request->params().config.snrLowXing);
        client->setSnrInactXing(request->params().config.snrInactXing);
        client->setVapIndex(vap_id);

        send_steering_return_status(beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE,
                                    OPERATION_SUCCESS);
        break;
    }
#endif //BEEROCKS_RDKB
    case beerocks_message::ACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST: {
        LOG(TRACE) << "received ACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST failed";
            return false;
        }
        std::string sta_mac = network_utils::mac_to_string(request->mac());
        LOG(INFO) << "ACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST=" << sta_mac;
        mon_db.sta_erase(sta_mac);

        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST: {
        LOG(TRACE) << "received ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST";

        auto request =
            cmdu_rx
                .addClass<beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST failed";
            return false;
        }
        std::string sta_mac = network_utils::mac_to_string(request->params().mac);
        auto sta_node       = mon_db.sta_find(sta_mac);
        if (sta_node == nullptr) {
            LOG(ERROR) << "RX_RSSI_MEASUREMENT REQUEST sta_mac=" << sta_mac
                       << " sta not assoc, id=" << beerocks_header->id();
            break;
        }

        sta_node->push_rx_rssi_request_id(beerocks_header->id());

        if (request->params().cross) {
            sta_node->set_arp_burst(true);
            if (sta_node->get_arp_state() != monitor_sta_node::IDLE) {
                sta_node->arp_recv_count_clear();
            }
            mon_db.set_arp_burst_pkt_num(request->params().mon_ping_burst_pkt_num);
            mon_db.set_arp_burst_delay(request->params().measurement_delay);
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building message!";
                break;
            }
            response->mac() = network_utils::mac_from_string(sta_mac);
            message_com::send_cmdu(slave_socket, cmdu_tx);
            LOG(DEBUG) << "send ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE, sta_mac = "
                       << sta_mac << " id=" << beerocks_header->id();
            sta_node->set_arp_state(monitor_sta_node::SEND_ARP);
            LOG(INFO) << "RX_RSSI_MEASUREMENT REQUEST cross, resetting state to SEND_ARP,"
                      << " sta_mac=" << sta_mac << " id=" << beerocks_header->id();

        } else if (sta_node->get_arp_state() == monitor_sta_node::IDLE) {
            sta_node->set_arp_burst(false);
            sta_node->set_arp_state(monitor_sta_node::SEND_ARP);
            LOG(INFO) << "RX_RSSI_MEASUREMENT REQUEST: state IDLE -> SEND_ARP,"
                      << " cross=0 sta_mac=" << sta_mac << " id=" << beerocks_header->id();

        } else {
            LOG(INFO) << "RX_RSSI_MEASUREMENT REQUEST state=" << int(sta_node->get_arp_state())
                      << " sta_mac=" << sta_mac << " id=" << beerocks_header->id();
        }

        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_BEACON_11K_REQUEST: {
        LOG(TRACE) << "received ACTION_MONITOR_CLIENT_BEACON_11K_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST failed";
            return false;
        }

        int dialog_token;

        // TODO: TEMPORARY CONVERSION!
        bwl::SBeaconRequest11k bwl_request;

        bwl_request.measurement_mode = request->params().measurement_mode;
        bwl_request.channel          = request->params().channel;
        bwl_request.op_class         = request->params().op_class;
        bwl_request.repeats          = request->params().repeats;
        bwl_request.rand_ival        = request->params().rand_ival;
        bwl_request.duration         = request->params().duration;
        std::copy_n(request->params().sta_mac.oct, sizeof(bwl_request.sta_mac.oct),
                    bwl_request.sta_mac.oct);
        std::copy_n(request->params().bssid.oct, sizeof(bwl_request.bssid.oct),
                    bwl_request.bssid.oct);
        bwl_request.parallel               = request->params().parallel;
        bwl_request.enable                 = request->params().enable;
        bwl_request.request                = request->params().request;
        bwl_request.report                 = request->params().report;
        bwl_request.mandatory_duration     = request->params().mandatory_duration;
        bwl_request.expected_reports_count = request->params().expected_reports_count;
        bwl_request.use_optional_ssid      = request->params().use_optional_ssid;
        std::copy_n(request->params().ssid, sizeof(bwl_request.ssid), bwl_request.ssid);
        bwl_request.use_optional_ap_ch_report = request->params().use_optional_ap_ch_report;
        std::copy_n(request->params().ap_ch_report, sizeof(bwl_request.ap_ch_report),
                    bwl_request.ap_ch_report);
        bwl_request.use_optional_req_elements = request->params().use_optional_req_elements;
        std::copy_n(request->params().req_elements, sizeof(bwl_request.req_elements),
                    bwl_request.req_elements);
        bwl_request.use_optional_wide_band_ch_switch =
            request->params().use_optional_wide_band_ch_switch;
        bwl_request.new_ch_width             = request->params().new_ch_width;
        bwl_request.new_ch_center_freq_seg_0 = request->params().new_ch_center_freq_seg_0;
        bwl_request.new_ch_center_freq_seg_1 = request->params().new_ch_center_freq_seg_1;

        mon_wlan_hal->sta_beacon_11k_request(bwl_request, dialog_token);

        sEvent11k event_11k = {network_utils::mac_to_string(request->params().sta_mac),
                               dialog_token, std::chrono::steady_clock::now(),
                               beerocks_header->id()};

        for (int i = 0; i < request->params().expected_reports_count; i++) {
            pending_11k_events.insert(std::make_pair("RRM_EVENT_BEACON_REP_RXED", event_11k));
        }
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST: {
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST failed";
            return false;
        }

        // debug_channel_load_11k_request(request);

        // TODO: TEMPORARY CONVERSION!
        bwl::SStaChannelLoadRequest11k bwl_request;

        bwl_request.channel                  = request->params().channel;
        bwl_request.op_class                 = request->params().op_class;
        bwl_request.repeats                  = request->params().repeats;
        bwl_request.rand_ival                = request->params().rand_ival;
        bwl_request.duration                 = request->params().duration;
        bwl_request.parallel                 = request->params().parallel;
        bwl_request.enable                   = request->params().enable;
        bwl_request.request                  = request->params().request;
        bwl_request.report                   = request->params().report;
        bwl_request.mandatory_duration       = request->params().mandatory_duration;
        bwl_request.use_optional_ch_load_rep = request->params().use_optional_ch_load_rep;
        bwl_request.ch_load_rep_first        = request->params().ch_load_rep_first;
        bwl_request.ch_load_rep_second       = request->params().ch_load_rep_second;
        bwl_request.use_optional_wide_band_ch_switch =
            request->params().use_optional_wide_band_ch_switch;
        bwl_request.new_ch_width             = request->params().new_ch_width;
        bwl_request.new_ch_center_freq_seg_0 = request->params().new_ch_center_freq_seg_0;
        bwl_request.new_ch_center_freq_seg_1 = request->params().new_ch_center_freq_seg_1;
        std::copy_n(request->params().sta_mac.oct, sizeof(bwl_request.sta_mac.oct),
                    bwl_request.sta_mac.oct);

        mon_wlan_hal->sta_channel_load_11k_request(bwl_request);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST: {
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST failed";
            return false;
        }

        // debug_statistics_11k_request(request);

        // TODO: TEMPORARY CONVERSION!
        bwl::SStatisticsRequest11k bwl_request;

        bwl_request.group_identity     = request->params().group_identity;
        bwl_request.parallel           = request->params().parallel;
        bwl_request.enable             = request->params().enable;
        bwl_request.request            = request->params().request;
        bwl_request.report             = request->params().report;
        bwl_request.mandatory_duration = request->params().mandatory_duration;
        bwl_request.repeats            = request->params().repeats;
        bwl_request.rand_ival          = request->params().rand_ival;
        bwl_request.duration           = request->params().duration;
        bwl_request.use_optional_trig_rep_sta_counters =
            request->params().use_optional_trig_rep_sta_counters;
        bwl_request.measurement_count_1           = request->params().measurement_count_1;
        bwl_request.trigger_timeout_1             = request->params().trigger_timeout_1;
        bwl_request.sta_counter_trigger_condition = request->params().sta_counter_trigger_condition;
        bwl_request.dot11FailedCountThreshold     = request->params().dot11FailedCountThreshold;
        bwl_request.dot11FCSErrorCountThreshold   = request->params().dot11FCSErrorCountThreshold;
        bwl_request.dot11MultipleRetryCountThreshold =
            request->params().dot11MultipleRetryCountThreshold;
        bwl_request.dot11FrameDuplicateCountThreshold =
            request->params().dot11FrameDuplicateCountThreshold;
        bwl_request.dot11RTSFailureCountThreshold = request->params().dot11RTSFailureCountThreshold;
        bwl_request.dot11AckFailureCountThreshold = request->params().dot11AckFailureCountThreshold;
        bwl_request.dot11RetryCountThreshold      = request->params().dot11RetryCountThreshold;
        bwl_request.use_optional_trig_rep_qos_sta_counters =
            request->params().use_optional_trig_rep_qos_sta_counters;
        bwl_request.measurement_count_2 = request->params().measurement_count_2;
        bwl_request.trigger_timeout_2   = request->params().trigger_timeout_2;
        bwl_request.qos_sta_counter_trigger_condition =
            request->params().qos_sta_counter_trigger_condition;
        bwl_request.dot11QoSFailedCountThreshold = request->params().dot11QoSFailedCountThreshold;
        bwl_request.dot11QoSRetryCountThreshold  = request->params().dot11QoSRetryCountThreshold;
        bwl_request.dot11QoSMultipleRetryCountThreshold =
            request->params().dot11QoSMultipleRetryCountThreshold;
        bwl_request.dot11QoSFrameDuplicateCountThreshold =
            request->params().dot11QoSFrameDuplicateCountThreshold;
        bwl_request.dot11QoSRTSCountFailureThreshold =
            request->params().dot11QoSRTSCountFailureThreshold;
        bwl_request.dot11QoSAckFailureCountThreshold =
            request->params().dot11QoSAckFailureCountThreshold;
        bwl_request.dot11QoSDiscardedCountThreshold =
            request->params().dot11QoSDiscardedCountThreshold;
        bwl_request.use_optional_trig_rep_rsna_counters =
            request->params().use_optional_trig_rep_rsna_counters;
        bwl_request.measurement_count_3 = request->params().measurement_count_3;
        bwl_request.trigger_timeout_3   = request->params().trigger_timeout_3;
        bwl_request.rsna_counter_trigger_condition =
            request->params().rsna_counter_trigger_condition;
        bwl_request.dot11RSNAStatsCMACICVErrorsThreshold =
            request->params().dot11RSNAStatsCMACICVErrorsThreshold;
        bwl_request.dot11RSNAStatsCMACReplaysThreshold =
            request->params().dot11RSNAStatsCMACReplaysThreshold;
        bwl_request.dot11RSNAStatsRobustMgmtCCMPReplaysThreshold =
            request->params().dot11RSNAStatsRobustMgmtCCMPReplaysThreshold;
        bwl_request.dot11RSNAStatsTKIPICVErrorsThreshold =
            request->params().dot11RSNAStatsTKIPICVErrorsThreshold;
        bwl_request.dot11RSNAStatsTKIPReplaysThreshold =
            request->params().dot11RSNAStatsTKIPReplaysThreshold;
        bwl_request.dot11RSNAStatsCCMPDecryptErrorsThreshold =
            request->params().dot11RSNAStatsCCMPDecryptErrorsThreshold;
        bwl_request.dot11RSNAStatsCCMPReplaysThreshold =
            request->params().dot11RSNAStatsCCMPReplaysThreshold;
        std::copy_n(request->params().sta_mac.oct, sizeof(bwl_request.sta_mac.oct),
                    bwl_request.sta_mac.oct);
        std::copy_n(request->params().peer_mac_addr.oct, sizeof(bwl_request.peer_mac_addr.oct),
                    bwl_request.peer_mac_addr.oct);

        mon_wlan_hal->sta_statistics_11k_request(bwl_request);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST: {
        auto request =
            cmdu_rx
                .addClass<beerocks_message::cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST failed";
            return false;
        }
        std::string mac_str = network_utils::mac_to_string(request->mac());

        LOG(DEBUG) << "ACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST:" << std::endl
                   << "mac=" << mac_str;

        mon_wlan_hal->sta_link_measurements_11k_request(mac_str);
        break;
    }
    default: {
        LOG(ERROR) << "Unsupported MONITOR action_op: " << int(beerocks_header->action_op());
        break;
    }
    }

    return true;
}

bool monitor_thread::hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr)
{
    if (!event_ptr) {
        LOG(ERROR) << "Invalid event!";
        return false;
    }

    if (!slave_socket) {
        LOG(ERROR) << "slave_socket == nullptr";
        return false;
    }

    // Monitor Event & Data
    typedef bwl::mon_wlan_hal::Event Event;
    auto event = (Event)(event_ptr->first);
    auto data  = event_ptr->second.get();

    switch (event) {

    case Event::RRM_Channel_Load_Response: {

        auto hal_data = static_cast<bwl::SStaChannelLoadResponse11k *>(data);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE message!";
            return false;
        }

        // TODO: TEMPORARY CONVERSION!
        response->params().channel           = hal_data->channel;
        response->params().channel_load      = hal_data->channel_load;
        response->params().op_class          = hal_data->op_class;
        response->params().rep_mode          = hal_data->rep_mode;
        response->params().dialog_token      = hal_data->dialog_token;
        response->params().measurement_token = hal_data->measurement_token;
        response->params().duration          = hal_data->duration;
        response->params().start_time        = hal_data->start_time;
        response->params().use_optional_wide_band_ch_switch =
            hal_data->use_optional_wide_band_ch_switch;
        response->params().new_ch_width             = hal_data->new_ch_width;
        response->params().new_ch_center_freq_seg_0 = hal_data->new_ch_center_freq_seg_0;
        response->params().new_ch_center_freq_seg_1 = hal_data->new_ch_center_freq_seg_1;
        std::copy_n(hal_data->sta_mac.oct, sizeof(response->params().sta_mac.oct),
                    response->params().sta_mac.oct);

        // debug_channel_load_11k_response(msg);

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    case Event::RRM_Beacon_Response: {

        auto hal_data = static_cast<bwl::SBeaconResponse11k *>(data);
        int id        = 0;
        LOG(INFO) << "Received beacon measurement response on BSSID: "
                  << network_utils::mac_to_string((sMacAddr &)hal_data->bssid)
                  << ", dialog_token: " << int(hal_data->dialog_token);

        // TODO: Can be changed to iterator loop?
        auto event_map = pending_11k_events.equal_range("RRM_EVENT_BEACON_REP_RXED");
        for (auto it = event_map.first; it != event_map.second;) {
            if ((it->second.dialog_token == hal_data->dialog_token) ||
                (hal_data->dialog_token == 0)) {

                id = it->second.id;

                auto response = message_com::create_vs_message<
                    beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE>(cmdu_tx, id);
                if (response == nullptr) {
                    LOG(ERROR)
                        << "Failed building cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE message!";
                    break;
                }

                // TODO: TEMPORARY CONVERSION!
                response->params().channel                  = hal_data->channel;
                response->params().op_class                 = hal_data->op_class;
                response->params().dialog_token             = hal_data->dialog_token;
                response->params().measurement_token        = hal_data->measurement_token;
                response->params().rep_mode                 = hal_data->rep_mode;
                response->params().phy_type                 = hal_data->phy_type;
                response->params().frame_type               = hal_data->frame_type;
                response->params().rcpi                     = hal_data->rcpi;
                response->params().rsni                     = hal_data->rsni;
                response->params().ant_id                   = hal_data->ant_id;
                response->params().duration                 = hal_data->duration;
                response->params().parent_tsf               = hal_data->parent_tsf;
                response->params().start_time               = hal_data->start_time;
                response->params().new_ch_width             = hal_data->new_ch_width;
                response->params().new_ch_center_freq_seg_0 = hal_data->new_ch_center_freq_seg_0;
                response->params().new_ch_center_freq_seg_1 = hal_data->new_ch_center_freq_seg_1;
                response->params().use_optional_wide_band_ch_switch =
                    hal_data->use_optional_wide_band_ch_switch;
                std::copy_n(hal_data->sta_mac.oct, sizeof(response->params().sta_mac.oct),
                            response->params().sta_mac.oct);
                std::copy_n(hal_data->bssid.oct, sizeof(response->params().bssid.oct),
                            response->params().bssid.oct);

                pending_11k_events.erase(it);
                LOG(INFO) << "Sending beacon measurement reponse on BSSID: "
                          << network_utils::mac_to_string(response->params().bssid)
                          << " to task_id: " << id;

                message_com::send_cmdu(slave_socket, cmdu_tx);
                break;
            } else {
                ++it;
            }
        }

    } break;

    case Event::RRM_STA_Statistics_Response: {

        auto hal_data = static_cast<bwl::SStatisticsResponse11k *>(data);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE message!";
            break;
        }

        response->params().dialog_token               = hal_data->dialog_token;
        response->params().measurement_token          = hal_data->measurement_token;
        response->params().rep_mode                   = hal_data->rep_mode;
        response->params().group_identity             = hal_data->group_identity;
        response->params().statistics_group_data_size = hal_data->statistics_group_data_size;
        response->params().duration                   = hal_data->duration;
        response->params().use_optional_rep_reason    = hal_data->use_optional_rep_reason;
        response->params().average_trigger            = hal_data->average_trigger;
        response->params().consecutive_trigger        = hal_data->consecutive_trigger;
        response->params().delay_trigger              = hal_data->delay_trigger;
        std::copy_n(hal_data->statistics_group_data,
                    sizeof(response->params().statistics_group_data),
                    response->params().statistics_group_data);
        std::copy_n(hal_data->sta_mac.oct, sizeof(response->params().sta_mac.oct),
                    response->params().sta_mac.oct);

        // debug_statistics_11k_response(msg);

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    case Event::RRM_Link_Measurement_Response: {

        auto hal_data = static_cast<bwl::SLinkMeasurementsResponse11k *>(data);
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE message!";
            break;
        }

        response->params().dialog_token                 = hal_data->dialog_token;
        response->params().rep_mode                     = hal_data->rep_mode;
        response->params().rx_ant_id                    = hal_data->rx_ant_id;
        response->params().tx_ant_id                    = hal_data->tx_ant_id;
        response->params().rcpi                         = hal_data->rcpi;
        response->params().rsni                         = hal_data->rsni;
        response->params().transmit_power               = hal_data->transmit_power;
        response->params().link_margin                  = hal_data->link_margin;
        response->params().use_optional_dmg_link_margin = hal_data->use_optional_dmg_link_margin;
        response->params().dmg_link_margin_activity     = hal_data->dmg_link_margin_activity;
        response->params().dmg_link_margin_mcs          = hal_data->dmg_link_margin_mcs;
        response->params().dmg_link_margin_link_margin  = hal_data->dmg_link_margin_link_margin;
        response->params().dmg_link_margin_snr          = hal_data->dmg_link_margin_snr;
        response->params().use_optional_dmg_link_adapt_ack =
            hal_data->use_optional_dmg_link_adapt_ack;
        response->params().dmg_link_adapt_ack_activity = hal_data->dmg_link_adapt_ack_activity;
        response->params().dmg_link_margin_reference_timestamp =
            hal_data->dmg_link_margin_reference_timestamp;
        response->params().dmg_link_adapt_ack_reference_timestamp =
            hal_data->dmg_link_adapt_ack_reference_timestamp;
        std::copy_n(hal_data->sta_mac.oct, sizeof(response->params().sta_mac.oct),
                    response->params().sta_mac.oct);

        // debug_link_measurements_11k_response(msg);

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    case Event::AP_Enabled: {
        if (!data) {
            LOG(ERROR) << "AP_Enabled without data";
            return false;
        }
        auto msg = static_cast<bwl::sHOSTAP_ENABLED_NOTIFICATION *>(data);
        LOG(INFO) << "AP_Enabled on vap_id = " << int(msg->vap_id);

        update_vaps_in_db();
    } break;

    case Event::AP_Disabled: {
        if (!data) {
            LOG(ERROR) << "AP_Disabled without data";
            return false;
        }
        auto msg = static_cast<bwl::sHOSTAP_DISABLED_NOTIFICATION *>(data);
        LOG(INFO) << "AP_Disabled on vap_id = " << int(msg->vap_id);

        if (msg->vap_id == beerocks::IFACE_RADIO_ID) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION>(cmdu_tx);
            if (response == nullptr) {
                LOG(ERROR)
                    << "Failed building cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION message!";
                break;
            }

            response->vap_id() = msg->vap_id;
            message_com::send_cmdu(slave_socket, cmdu_tx);
        }

        update_vaps_in_db();

    } break;

    // Unhandled events
    default: {
        LOG(ERROR) << "Unhandled event: " << int(event);
        return false;
    }
    }

    return true;
}

// void monitor_thread::debug_channel_load_11k_request(message::sACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST* request)
// {
//     LOG(DEBUG) << "ACTION_MONITOR_CLIENT_CLIENT_CHANNEL_LOAD_11K_REQUEST:"
//     << std::endl << "channel: "              << (int)request->params.channel
//     << std::endl << "op_class: "             << (int)request->params.op_class
//     << std::endl << "repeats: "              << (int)request->params.repeats
//     << std::endl << "rand_ival: "            << (int)request->params.rand_ival
//     << std::endl << "duration: "             << (int)request->params.duration
//     << std::endl << "sta_mac: "              << network_utils::mac_to_string(request->params.sta_mac)
//     << std::endl << "parallel: "             << (int)request->params.parallel
//     << std::endl << "enable: "               << (int)request->params.enable
//     << std::endl << "request: "              << (int)request->params.request
//     << std::endl << "report: "               << (int)request->params.report
//     << std::endl << "mandatory_duration: "   << (int)request->params.mandatory_duration;
//     //Optional:
//     // << std::endl << "use_optional_ch_load_rep: "             << (int)request->params.use_optional_ch_load_rep
//     // << std::endl << "ch_load_rep_first: "                    << (int)request->params.ch_load_rep_first
//     // << std::endl << "ch_load_rep_second: "                   << (int)request->params.ch_load_rep_second
//     // << std::endl << "use_optional_wide_band_ch_switch: "     << (int)request->params.use_optional_wide_band_ch_switch
//     // << std::endl << "new_ch_width: "                         << (int)request->params.new_ch_width
//     // << std::endl << "new_ch_center_freq_seg_0: "             << (int)request->params.new_ch_center_freq_seg_0
//     // << std::endl << "new_ch_center_freq_seg_1: "             << (int)request->params.new_ch_center_freq_seg_1;
// }

// void monitor_thread::debug_beacon_11k_request(message::sACTION_MONITOR_CLIENT_BEACON_11K_REQUEST *request)
// {
//     LOG(DEBUG) << "ACTION_MONITOR_CLIENT_BEACON_REQUEST:" << std::endl
//     << std::endl << "measurement_mode: "                 << (int)request->params.measurement_mode
//     << std::endl << "channel: "                          << (int)request->params.channel
//     << std::endl << "op_class: "                         << (int)request->params.op_class
//     << std::endl << "repeats: "                          << (int)request->params.repeats
//     << std::endl << "rand_ival: "                        << (int)request->params.rand_ival
//     << std::endl << "duration: "                         << (int)request->params.duration
//     << std::endl << "sta_mac: "                          << network_utils::mac_to_string(request->params.sta_mac)
//     << std::endl << "bssid: "                            << network_utils::mac_to_string(request->params.bssid)
//     << std::endl << "parallel: "                         << (int)request->params.parallel
//     << std::endl << "enable: "                           << (int)request->params.enable
//     << std::endl << "request: "                          << (int)request->params.request
//     << std::endl << "report: "                           << (int)request->params.report
//     << std::endl << "mandatory_duration: "               << (int)request->params.mandatory_duration
//     //Optional:
//     << std::endl << "use_optional_ssid: "                << (int)request->params.use_optional_ssid
//     << std::endl << "ssid: "                             << (const char*)request->params.ssid
//     // << std::endl << "use_optional_ap_ch_report: "        << (int)request->params.use_optional_ap_ch_report
//     // << std::endl << "ap_ch_report: "                     << (int)request->params.ap_ch_report[0]
//     // << std::endl << "use_optional_req_elements: "        << (int)request->params.use_optional_req_elements
//     // << std::endl << "req_elements: "                     << (int)request->params.req_elements[0]
//     // << std::endl << "use_optional_wide_band_ch_switch: " << (int)request->params.use_optional_wide_band_ch_switch
//     // << std::endl << "new_ch_width: "                     << (int)request->params.new_ch_width
//     // << std::endl << "new_ch_center_freq_seg_0: "         << (int)request->params.new_ch_center_freq_seg_0
//     // << std::endl << "new_ch_center_freq_seg_1: "         << (int)request->params.new_ch_center_freq_seg_1
//     ;
// }

// void monitor_thread::debug_statistics_11k_request(message::sACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST* request)
// {
//     LOG(DEBUG) << "ACTION_MONITOR_CLIENT_STATISTICS_REQUEST:" << std::endl
//     << std::endl << "group_identity: "                               << (int)request->params.group_identity
//     << std::endl << "repeats: "                                      << (int)request->params.repeats
//     << std::endl << "rand_ival: "                                    << (int)request->params.rand_ival
//     << std::endl << "duration: "                                     << (int)request->params.duration
//     << std::endl << "sta_mac: "                                      << network_utils::mac_to_string(request->params.sta_mac)
//     << std::endl << "peer_mac_addr: "                                << network_utils::mac_to_string(request->params.peer_mac_addr)
//     << std::endl << "parallel: "                                     << (int)request->params.parallel
//     << std::endl << "enable: "                                       << (int)request->params.enable
//     << std::endl << "request: "                                      << (int)request->params.request
//     << std::endl << "report: "                                       << (int)request->params.report
//     << std::endl << "mandatory_duration: "                           << (int)request->params.mandatory_duration;
//     //Optional:
//     // << std::endl << "use_optional_trig_rep_sta_counters: "           << (int)request->params.use_optional_trig_rep_sta_counters
//     // << std::endl << "measurement_count_1: "                          << (int)request->params.measurement_count_1
//     // << std::endl << "trigger_timeout_1: "                            << (int)request->params.trigger_timeout_1
//     // << std::endl << "sta_counter_trigger_condition: "                << (int)request->params.sta_counter_trigger_condition
//     // << std::endl << "dot11FailedCountThreshold: "                    << (int)request->params.dot11FailedCountThreshold
//     // << std::endl << "dot11FCSErrorCountThreshold: "                  << (int)request->params.dot11FCSErrorCountThreshold
//     // << std::endl << "dot11MultipleRetryCountThreshold: "             << (int)request->params.dot11MultipleRetryCountThreshold
//     // << std::endl << "dot11FrameDuplicateCountThreshold: "            << (int)request->params.dot11FrameDuplicateCountThreshold
//     // << std::endl << "dot11RTSFailureCountThreshold: "                << (int)request->params.dot11RTSFailureCountThreshold
//     // << std::endl << "dot11AckFailureCountThreshold: "                << (int)request->params.dot11AckFailureCountThreshold
//     // << std::endl << "dot11RetryCountThreshold: "                     << (int)request->params.dot11RetryCountThreshold
//     // << std::endl << "use_optional_trig_rep_qos_sta_counters: "       << (int)request->params.use_optional_trig_rep_qos_sta_counters
//     // << std::endl << "measurement_count_2: "                          << (int)request->params.measurement_count_2
//     // << std::endl << "trigger_timeout_2: "                            << (int)request->params.trigger_timeout_2
//     // << std::endl << "qos_sta_counter_trigger_condition: "            << (int)request->params.qos_sta_counter_trigger_condition
//     // << std::endl << "dot11QoSFailedCountThreshold: "                 << (int)request->params.dot11QoSFailedCountThreshold
//     // << std::endl << "dot11QoSRetryCountThreshold: "                  << (int)request->params.dot11QoSRetryCountThreshold
//     // << std::endl << "dot11QoSMultipleRetryCountThreshold: "          << (int)request->params.dot11QoSMultipleRetryCountThreshold
//     // << std::endl << "dot11QoSFrameDuplicateCountThreshold: "         << (int)request->params.dot11QoSFrameDuplicateCountThreshold
//     // << std::endl << "dot11QoSRTSCountFailureThreshold: "             << (int)request->params.dot11QoSRTSCountFailureThreshold
//     // << std::endl << "dot11QoSAckFailureCountThreshold: "             << (int)request->params.dot11QoSAckFailureCountThreshold
//     // << std::endl << "dot11QoSDiscardedCountThreshold: "              << (int)request->params.dot11QoSDiscardedCountThreshold
//     // << std::endl << "use_optional_trig_rep_rsna_counters: "          << (int)request->params.use_optional_trig_rep_rsna_counters
//     // << std::endl << "measurement_count_3: "                          << (int)request->params.measurement_count_3
//     // << std::endl << "trigger_timeout_3: "                            << (int)request->params.trigger_timeout_3
//     // << std::endl << "rsna_counter_trigger_condition: "               << (int)request->params.rsna_counter_trigger_condition
//     // << std::endl << "dot11RSNAStatsCMACICVErrorsThreshold: "         << (int)request->params.dot11RSNAStatsCMACICVErrorsThreshold
//     // << std::endl << "dot11RSNAStatsCMACReplaysThreshold: "           << (int)request->params.dot11RSNAStatsCMACReplaysThreshold
//     // << std::endl << "dot11RSNAStatsRobustMgmtCCMPReplaysThreshold: " << (int)request->params.dot11RSNAStatsRobustMgmtCCMPReplaysThreshold
//     // << std::endl << "dot11RSNAStatsTKIPICVErrorsThreshold: "         << (int)request->params.dot11RSNAStatsTKIPICVErrorsThreshold
//     // << std::endl << "dot11RSNAStatsTKIPReplaysThreshold: "           << (int)request->params.dot11RSNAStatsTKIPReplaysThreshold
//     // << std::endl << "dot11RSNAStatsCCMPDecryptErrorsThreshold: "     << (int)request->params.dot11RSNAStatsCCMPDecryptErrorsThreshold
//     // << std::endl << "dot11RSNAStatsCCMPReplaysThreshold: "           << (int)request->params.dot11RSNAStatsCCMPReplaysThreshold
//     // ;

// }

// void monitor_thread::debug_channel_load_11k_response(message::sACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE* event)
// {
//     LOG(DEBUG) << "DATA TEST:"
//     << std::endl << "sta_mac: "              << network_utils::mac_to_string(event->params.sta_mac)
//     << std::endl << "measurement_rep_mode: " << (int)event->params.rep_mode
//     << std::endl << "op_class: "             << (int)event->params.op_class
//     << std::endl << "channel: "              << (int)event->params.channel
//     << std::endl << "start_time: "           << (int)event->params.start_time
//     << std::endl << "duration: "             << (int)event->params.duration
//     << std::endl << "channel_load: "         << (int)event->params.channel_load

//     << std::endl << "new_ch_width: "                         << (int)event->params.new_ch_width
//     << std::endl << "new_ch_center_freq_seg_0: "             << (int)event->params.new_ch_center_freq_seg_0
//     << std::endl << "new_ch_center_freq_seg_1: "             << (int)event->params.new_ch_center_freq_seg_1
//     ;
// }

// void monitor_thread::debug_beacon_11k_response(message::sACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE* event)
// {
//     LOG(DEBUG) << "DATA TEST:"
//     << std::endl << "sta_mac: "              << network_utils::mac_to_string(event->params.sta_mac)
//     << std::endl << "measurement_rep_mode: " << (int)event->params.rep_mode
//     << std::endl << "op_class: "             << (int)event->params.op_class
//     << std::endl << "channel: "              << (int)event->params.channel
//     << std::endl << "start_time: "           << (int)event->params.start_time
//     << std::endl << "duration: "             << (int)event->params.duration
//     << std::endl << "phy_type: "             << (int)event->params.phy_type
//     << std::endl << "frame_type: "           << (int)event->params.frame_type
//     << std::endl << "rcpi: "                 << (int)event->params.rcpi
//     << std::endl << "rsni: "                 << (int)event->params.rsni
//     << std::endl << "bssid: "                << network_utils::mac_to_string(event->params.bssid)
//     << std::endl << "ant_id: "               << (int)event->params.ant_id
//     << std::endl << "tsf: "                  << (int)event->params.parent_tsf

//     << std::endl << "new_ch_width: "                         << (int)event->params.new_ch_width
//     << std::endl << "new_ch_center_freq_seg_0: "             << (int)event->params.new_ch_center_freq_seg_0
//     << std::endl << "new_ch_center_freq_seg_1: "             << (int)event->params.new_ch_center_freq_seg_1
//     ;
// }

// void monitor_thread::debug_statistics_11k_response(message::sACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE* event)
// {
//     std::string statistics_group_data;
//     for(uint8_t i=0;i<event->params.statistics_group_data_size;i++){
//         statistics_group_data +=  string_utils::to_string(event->params.statistics_group_data[i]) + ",";
//     }
//     statistics_group_data.pop_back(); // deletes last comma

//     LOG(DEBUG) << "DATA TEST:"
//     << std::endl << "sta_mac: "              << network_utils::mac_to_string(event->params.sta_mac)
//     << std::endl << "measurement_rep_mode: " << (int)event->params.rep_mode
//     << std::endl << "duration: "             << (int)event->params.duration
//     << std::endl << "group_identity: "       << (int)event->params.group_identity
//     << std::endl << "statistics_group_data: "<< statistics_group_data

//     << std::endl << "average_trigger: "      << (int)event->params.average_trigger
//     << std::endl << "consecutive_trigger: "  << (int)event->params.consecutive_trigger
//     << std::endl << "delay_trigger: "        << (int)event->params.delay_trigger
//     ;
// }

// void monitor_thread::debug_link_measurements_11k_response(message::sACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE* event)
// {
//     LOG(DEBUG) << "DATA TEST:"
//     << std::endl << "sta_mac: "          << network_utils::mac_to_string(event->params.sta_mac)
//     << std::endl << "transmit_power: "   << (int)event->params.transmit_power
//     << std::endl << "link_margin: "      << (int)event->params.link_margin
//     << std::endl << "rx_ant_id: "        << (int)event->params.rx_ant_id
//     << std::endl << "tx_ant_id: "        << (int)event->params.tx_ant_id
//     << std::endl << "rcpi: "             << (int)event->params.rcpi
//     << std::endl << "rsni: "             << (int)event->params.rsni

//     << std::endl << "dmg_link_margin_activity: "                 << (int)event->params.dmg_link_margin_activity
//     << std::endl << "dmg_link_margin_mcs: "                      << (int)event->params.dmg_link_margin_mcs
//     << std::endl << "dmg_link_margin_link_margin: "              << (int)event->params.dmg_link_margin_link_margin
//     << std::endl << "dmg_link_margin_snr: "                      << (int)event->params.dmg_link_margin_snr
//     << std::endl << "dmg_link_margin_reference_timestamp: "      << (int)event->params.dmg_link_margin_reference_timestamp
//     << std::endl << "dmg_link_adapt_ack_activity: "              << (int)event->params.dmg_link_adapt_ack_activity
//     << std::endl << "dmg_link_adapt_ack_reference_timestamp: "   << (int)event->params.dmg_link_adapt_ack_reference_timestamp
//     ;
// }

void monitor_thread::send_heartbeat()
{
    if (slave_socket == nullptr) {
        LOG(ERROR) << "process_keep_alive(): slave_socket is nullptr!";
        return;
    }

    //LOG(DEBUG) << "sending HEARTBEAT notification";
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_MONITOR_HEARTBEAT_NOTIFICATION>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return;
    }

    message_com::send_cmdu(slave_socket, cmdu_tx);
}

void monitor_thread::update_vaps_in_db()
{
    if (!mon_wlan_hal->refresh_vaps_info()) {
        LOG(ERROR) << "Failed to refresh vaps info!";
        return;
    }
    const auto &radio_vaps = mon_wlan_hal->get_radio_info().available_vaps;

    std::string bridge_iface_mac;
    std::string bridge_iface_ip;
    network_utils::linux_iface_get_mac(bridge_iface, bridge_iface_mac);
    network_utils::linux_iface_get_ip(bridge_iface, bridge_iface_ip);

    for (int vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id < beerocks::IFACE_VAP_ID_MAX; vap_id++) {

        // if vap exist in HAL, update it in the local db.
        if (radio_vaps.find(vap_id) != radio_vaps.end()) {
            auto iface_name = beerocks::utils::get_iface_string_from_iface_vap_ids(
                mon_wlan_hal->get_radio_info().iface_name, vap_id);

            auto curr_vap = radio_vaps.at(vap_id);

            auto vap_node = mon_db.vap_add(iface_name, vap_id);
            vap_node->set_mac(curr_vap.mac);

            vap_node->set_bridge_iface(bridge_iface);
            vap_node->set_bridge_mac(bridge_iface_mac);
            vap_node->set_bridge_ipv4(bridge_iface_ip);

        } else if (mon_db.vap_get_by_id(vap_id)) { // vap does not exist in HAL but is in local DB
            mon_db.vap_remove(vap_id);
        }
    }
}
#ifdef BEEROCKS_RDKB
void monitor_thread::send_steering_return_status(beerocks_message::eActionOp_MONITOR ActionOp,
                                                 int32_t status)
{
    switch (ActionOp) {
    case beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }
        response->params().error_code = status;
        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }
        response->params().error_code = status;
        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    default: {
        LOG(ERROR) << "UNKNOWN ActionOp was received, ActionOp = " << int(ActionOp);
        break;
    }
    }
    return;
}
#endif //BEEROCKS_RDKB