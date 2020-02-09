/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "association_handling_task.h"
#include "../db/db_algo.h"
#include "../son_actions.h"
#include "client_steering_task.h"
#include "ire_network_optimization_task.h"
#include "optimal_path_task.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/network/socket.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_control.h>

using namespace beerocks;
using namespace net;
using namespace son;

#define BEACON_MEASURE_REQ_TIME_SPAN 3000
#define BEACON_MEASURE_MAX_ATTEMPTS 3
#define REQUEST_RSSI_MEASUREMENT_MAX_ATTEMPTS 5
#define REQUEST_RSSI_MEASUREMENT_DELAY 5000

association_handling_task::association_handling_task(db &database_,
                                                     ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                     task_pool &tasks_, std::string sta_mac_,
                                                     std::string task_name_)
    : task(task_name_), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_), sta_mac(sta_mac_)
{
}

void association_handling_task::work()
{
    switch (state) {
    case START: {
        // If this task already has been created by another event, let it finish and finish the new
        // instance of it.
        int prev_task_id = database.get_association_handling_task_id(sta_mac);
        if (tasks.is_task_running(prev_task_id)) {
            finish();
            return;
        }
        database.assign_association_handling_task_id(sta_mac, id);

        original_parent_mac = database.get_node_parent(sta_mac);

        task_started_timestamp = std::chrono::steady_clock::now();

        if (database.settings_monitor_on_vaps() == false) {
            TASK_LOG(DEBUG) << "started association_handling_task, non main vap connected sta "
                            << sta_mac;
            state = FINISH;
            break;
        }

        TASK_LOG(DEBUG) << "started association_handling_task, rssi measurement on " << sta_mac;
        state = START_RSSI_MONITORING;
        break;
    }

    case START_RSSI_MONITORING: {
        TASK_LOG(DEBUG) << "START_RSSI_MONITORING";
        /*
         * request constant RSSI monitoring for the new client
         */

        std::string new_hostap_mac;
        std::string ipv4;

        if (database.is_node_wireless(sta_mac)) {
            new_hostap_mac = database.get_node_parent(sta_mac);
            if (new_hostap_mac != original_parent_mac ||
                database.get_node_state(sta_mac) != beerocks::STATE_CONNECTED) {
                TASK_LOG(DEBUG) << "sta " << sta_mac << " is no longer connected to "
                                << original_parent_mac << " finishing task";
                finish();
                return;
            }

            LOG(DEBUG) << "START_MONITORING_REQUEST hostap_mac=" << new_hostap_mac << " sta_mac "
                       << sta_mac;

            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST>(cmdu_tx, id);

            if (request == nullptr) {
                LOG(ERROR)
                    << "Failed building ACTION_CONTROL_CLIENT_START_MONITORING_REQUEST message!";
                return;
            }

            request->params().mac     = network_utils::mac_from_string(sta_mac);
            request->params().ipv4    = network_utils::ipv4_from_string(ipv4);
            request->params().channel = database.get_node_channel(sta_mac);
            request->params().vap_id  = database.get_node_vap_id(sta_mac);
            request->params().is_ire  = false;

            //add bridge mac for ires
            if (database.get_node_type(sta_mac) == beerocks::TYPE_IRE_BACKHAUL) {
                request->params().is_ire = true;
                auto bridge_container    = database.get_node_children(sta_mac, beerocks::TYPE_IRE);
                if (!bridge_container.empty()) {
                    std::string bridge_4addr_mac = *bridge_container.begin();
                    request->params().bridge_4addr_mac =
                        network_utils::mac_from_string(bridge_4addr_mac);
                    LOG(DEBUG) << "IRE " << sta_mac << " is on a bridge with mac "
                               << bridge_4addr_mac;
                }
            }

            auto radio_mac = database.get_node_parent_radio(new_hostap_mac);
            auto agent_mac = database.get_node_parent_ire(radio_mac);
            son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac);
        }

        if (database.settings_client_11k_roaming() &&
            (database.get_node_beacon_measurement_support_level(sta_mac) ==
             beerocks::BEACON_MEAS_UNSUPPORTED) &&
            (database.get_node_type(sta_mac) == beerocks::TYPE_CLIENT)) {
            TASK_LOG(DEBUG) << "started association_handling_task, looking for beacon measurement "
                               "capabilities on "
                            << sta_mac;
            state        = CHECK_11K_BEACON_MEASURE_CAP;
            max_attempts = BEACON_MEASURE_MAX_ATTEMPTS;
        } else {
            TASK_LOG(DEBUG) << "started association_handling_task, rssi measurement on " << sta_mac;
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
        }
        break;
    }

    case CHECK_11K_BEACON_MEASURE_CAP: {
        std::string parent_mac = database.get_node_parent(sta_mac);
        std::string radio_mac  = database.get_node_parent_radio(parent_mac);
        auto agent_mac         = database.get_node_parent_ire(parent_mac);

        auto measurement_request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST>(cmdu_tx, id);

        if (measurement_request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST message!";
            return;
        }
        measurement_request->params().measurement_mode =
            beerocks::MEASURE_MODE_ACTIVE; // son::eMeasurementMode11K "passive"/"active"/"table"
        measurement_request->params().channel   = database.get_node_channel(parent_mac);
        measurement_request->params().rand_ival = beerocks::
            BEACON_MEASURE_DEFAULT_RANDOMIZATION_INTERVAL; // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
        measurement_request->params().duration = beerocks::
            BEACON_MEASURE_DEFAULT_ACTIVE_DURATION; // measurement duration, expressed in units of TUs [=1024usec]
        measurement_request->params().sta_mac = network_utils::mac_from_string(sta_mac);
        measurement_request->params().bssid   = network_utils::mac_from_string(
            parent_mac); // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"
        //measurement_request.params.use_optional_ssid = true;
        measurement_request->params().expected_reports_count = 1;
        //string_utils::copy_string((char*)measurement_request.params.ssid, database.get_hostap_vap_ssid(parent_mac).c_str(), sizeof(measurement_request.params.ssid));
        add_pending_mac(radio_mac, beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE);
        TASK_LOG(DEBUG) << "requested beacon measurement request from sta: " << sta_mac
                        << " on hostap: " << parent_mac;

        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac);
        set_responses_timeout(BEACON_MEASURE_REQ_TIME_SPAN);
        break;
    }

    case REQUEST_RSSI_MEASUREMENT_WAIT: {
        if (!database.settings_client_band_steering() &&
            !database.settings_client_optimal_path_roaming()) {
            TASK_LOG(DEBUG) << "band_steering and client_roaming are disabled! skipping on rssi "
                               "measurement, and finish the task";
            state = FINISH;
            break;
        }

        TASK_LOG(DEBUG) << "REQUEST_RSSI_MEASUREMENT_WAIT";
        state               = REQUEST_RSSI_MEASUREMENT;
        int time_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::steady_clock::now() - task_started_timestamp)
                                  .count();
        int new_delay = REQUEST_RSSI_MEASUREMENT_DELAY - time_elapsed_ms;
        TASK_LOG(DEBUG) << "new_delay=" << new_delay << "ms";
        max_attempts = REQUEST_RSSI_MEASUREMENT_MAX_ATTEMPTS;
        attempts     = 0;
        wait_for(new_delay);
        break;
    }

    case REQUEST_RSSI_MEASUREMENT: {

        /*
             * send measurement request to get a valid RSSI reading
             */
        std::string hostap_mac = database.get_node_parent(sta_mac);
        auto agent_mac         = database.get_node_parent_ire(hostap_mac);

        if (hostap_mac != original_parent_mac ||
            database.get_node_state(sta_mac) != beerocks::STATE_CONNECTED) {
            TASK_LOG(DEBUG) << "sta " << sta_mac << " is no longer connected to "
                            << original_parent_mac << " finishing task";
            finish();
            return;
        }

        std::string ipv4 = database.get_node_ipv4(sta_mac);

        auto measurement_request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(cmdu_tx, id);
        if (measurement_request == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST message!";
            return;
        }
        measurement_request->params().mac       = network_utils::mac_from_string(sta_mac);
        measurement_request->params().ipv4      = network_utils::ipv4_from_string(ipv4);
        measurement_request->params().channel   = database.get_node_channel(hostap_mac);
        measurement_request->params().bandwidth = database.get_node_bw(hostap_mac);
        measurement_request->params().cross     = 0;

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);

        TASK_LOG(DEBUG) << "requested rx rssi measurement from " << hostap_mac << " for sta "
                        << sta_mac;

        add_pending_mac(hostap_mac,
                        beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        set_responses_timeout(3000);
        state = FINISH;
        break;
    }

    case FINISH: {

        TASK_LOG(DEBUG) << "response for beacon/rx_rssi measurement from " << original_parent_mac
                        << " for sta " << sta_mac << " was received";
        finalize_new_connection();
        finish();
        break;
    }

    default:
        break;
    }
}

void association_handling_task::handle_response(std::string mac,
                                                std::shared_ptr<beerocks_header> beerocks_header)
{
    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE: {
        const std::string parent_mac = database.get_node_parent(sta_mac);
        auto response =
            beerocks_header
                ->getClass<beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE>();
        if (!response) {
            TASK_LOG(ERROR) << "getClass failed for cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE";
            return;
        }

        LOG(DEBUG) << "beacon response , ID: " << id << std::endl
                   << "sta_mac: " << response->params().sta_mac << std::endl
                   << "measurement_rep_mode: " << (int)response->params().rep_mode << std::endl
                   << "op_class: " << (int)response->params().op_class << std::endl
                   << "channel: "
                   << (int)response->params().channel
                   //<< std::endl << "start_time: "           << (int)response->params.start_time
                   << std::endl
                   << "duration: "
                   << (int)response->params().duration
                   //<< std::endl << "phy_type: "             << (int)response->params.phy_type
                   //<< std::endl << "frame_type: "           << (int)response->params.frame_type
                   << std::endl
                   << "rcpi: " << (int)response->params().rcpi << std::endl
                   << "rsni: " << (int)response->params().rsni << std::endl
                   << "bssid: " << response->params().bssid
            //<< std::endl << "ant_id: "               << (int)response->params.ant_id
            //<< std::endl << "tsf: "                  << (int)response->params.parent_tsf

            //<< std::endl << "new_ch_width: "                         << (int)response->params.new_ch_width
            //<< std::endl << "new_ch_center_freq_seg_0: "             << (int)response->params.new_ch_center_freq_seg_0
            //<< std::endl << "new_ch_center_freq_seg_1: "             << (int)response->params.new_ch_center_freq_seg_1
            ;

        TASK_LOG(DEBUG) << "response for beacon measurement request was received from hostapd "
                        << mac;

        if (son_actions::validate_beacon_measurement_report(response->params(), sta_mac,
                                                            parent_mac)) {
            TASK_LOG(DEBUG) << "sta " << sta_mac << " supports beacon measurement!";
            uint8_t support_level = beerocks::BEACON_MEAS_BSSID_SUPPORTED;

            if (response->params().rsni) {
                //on nexus 5x devices rsni always 0, and they are not supports measurement by ssid (special handling)
                support_level |= beerocks::BEACON_MEAS_SSID_SUPPORTED;
            }

            database.set_node_beacon_measurement_support_level(
                sta_mac, beerocks::eBeaconMeasurementSupportLevel(support_level));
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
            break;
        }

        if (attempts++ > max_attempts) {
            TASK_LOG(DEBUG) << "state CHECK_11K_BEACON_MEASURE_CAP reached maximum attempts="
                            << attempts << " setting sta " << sta_mac
                            << " as beacon measurement unsupported ";
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
        }
        break;
    }
    default: {
        LOG(ERROR) << "Unsupported action_op:" << int(beerocks_header->action_op());
        break;
    }
    }
}

void association_handling_task::finalize_new_connection()
{

    /*
     * see if special handling is required if client just came back from a handover
     */
    if (!database.get_node_handoff_flag(sta_mac)) {
        if (database.get_node_type(sta_mac) == beerocks::TYPE_CLIENT) {
            auto new_task = std::make_shared<optimal_path_task>(
                database, cmdu_tx, tasks, sta_mac, 6000, "handle_completed_connection");
            tasks.add_task(new_task);
        }
    } else {
        LOG(INFO) << "handoff complete for " << sta_mac;

        /* 
         * kill existing roaming task 
         */
        int prev_roaming_task = database.get_roaming_task_id(sta_mac);
        LOG(DEBUG) << "kill prev_roaming_task " << prev_roaming_task;
        tasks.kill_task(prev_roaming_task);

        /*
         * kill load balancer
         */
        int prev_load_balancer_task = database.get_load_balancer_task_id(sta_mac);
        tasks.kill_task(prev_load_balancer_task);

        database.set_node_handoff_flag(sta_mac, false);
    }
}
void association_handling_task::handle_responses_timeout(
    std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
{
    ++attempts;

    switch (state) {
    case REQUEST_RSSI_MEASUREMENT: {
        TASK_LOG(DEBUG) << "response for rx rssi measurement from " << original_parent_mac
                        << " for sta " << sta_mac << " timed out! attempts=" << attempts;
        if (attempts >= max_attempts) {
            /*
            * TODO
            * this shouldn't really happen unless the new IRE/hostap is malfunctioning
            * might require further handling
            */
            TASK_LOG(ERROR) << "state REQUEST_RSSI_MEASUREMENT reached maximum attempts="
                            << attempts << " aborting task!";
            finalize_new_connection();
        } else {
            state = REQUEST_RSSI_MEASUREMENT;
        }
        break;
    }
    case CHECK_11K_BEACON_MEASURE_CAP: {
        TASK_LOG(DEBUG) << "response for beacon measurement request from " << sta_mac
                        << " timed out! attempts=" << attempts;
        if (attempts >= max_attempts) {
            TASK_LOG(DEBUG) << "state CHECK_11K_BEACON_MEASURE_CAP reached maximum attempts="
                            << attempts << " setting sta " << sta_mac
                            << " as beacon measurement unsupported ";
            database.set_node_beacon_measurement_support_level(sta_mac,
                                                               beerocks::BEACON_MEAS_UNSUPPORTED);
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
        }
        break;
    }
    default: {
        TASK_LOG(ERROR) << "Unknown state: " << int(state);
        break;
    }
    }
}
