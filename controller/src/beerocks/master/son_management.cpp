/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "son_management.h"
#include "son_actions.h"
#include "tasks/bml_task.h"
#include "wfa_ca.h"
#ifdef BEEROCKS_RDKB
#include "tasks/rdkb/rdkb_wlan_task.h"
#endif
#include "db/network_map.h"
#include "tasks/bml_wifi_credentials_update_task.h"
#include "tasks/channel_selection_task.h"
#include "tasks/ire_network_optimization_task.h"
#include "tasks/load_balancer_task.h"

#include <beerocks/tlvf/beerocks_message_bml.h>
#include <beerocks/tlvf/beerocks_message_cli.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

void son_management::handle_cli_message(Socket *sd,
                                        std::shared_ptr<beerocks_message::cACTION_HEADER> header,
                                        ieee1905_1::CmduMessageRx &cmdu_rx,
                                        ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
                                        task_pool &tasks)
{
    bool isOK           = true;
    int8_t currentValue = -1; //ignore currentValue field in the response

    //LOG(DEBUG) << "NEW CLI action=" << int(header->action()) << " action_op=" << int(header->action_op());

    switch (header->action_op()) {

    case beerocks_message::ACTION_CLI_PING_SLAVE_REQUEST: {
        LOG(DEBUG) << "PING_SLAVE_REQUEST from CLI";
        auto cli_request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_PING_SLAVE_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_PING_SLAVE_REQUEST failed";
            isOK = false;
            break;
        }
        std::string slave = network_utils::mac_to_string(cli_request->mac());
        if (database.is_hostap_active(slave)) {
            Socket *hostap_mac_sd = database.get_node_socket(slave);

            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CONTROLLER_PING_REQUEST>(cmdu_tx);
            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                isOK = false;
                break;
            }

            request->total() = cli_request->num_of_req();
            request->seq()   = 0;

            auto size_left =
                MTU_SIZE - message_com::get_vs_cmdu_size_on_buffer<
                               beerocks_message::cACTION_CONTROL_CONTROLLER_PING_REQUEST>();
            if (size_left > cli_request->size()) {
                request->size() = cli_request->size();
            } else {
                LOG(DEBUG) << "PING_MSG_REQUEST size > tx_buffer size left setting size to "
                           << (int)size_left << " bytes";
                request->size() = size_left;
            }

            if (!request->alloc_data(size_left)) {
                LOG(ERROR) << "Failed buffer allocation to size=" << int(size_left);
                isOK = false;
                break;
            }
            auto data_tuple = request->data(0);
            memset(&std::get<1>(data_tuple), 0, size_left);
            if (!database.update_node_last_ping_sent(slave)) {
                LOG(DEBUG) << "PING_MSG_REQUEST received for slave " << slave
                           << " , can't update last ping sent time for ";
            }

            const auto parent_radio = database.get_node_parent_radio(slave);
            son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        }
        break;
    }
    case beerocks_message::ACTION_CLI_PING_ALL_SLAVES_REQUEST: {
        LOG(DEBUG) << "PING_ALL_SLAVES_REQUEST from CLI";
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_PING_ALL_SLAVES_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_PING_SLAVE_REQUEST failed";
            isOK = false;
            break;
        }

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CONTROLLER_PING_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }
        request->total() = cli_request->num_of_req();
        request->seq()   = 0;
        auto size_left =
            MTU_SIZE - message_com::get_vs_cmdu_size_on_buffer<
                           beerocks_message::cACTION_CONTROL_CONTROLLER_PING_REQUEST>();
        if (size_left > cli_request->size()) {
            request->size() = cli_request->size();
        } else {
            LOG(DEBUG) << "PING_MSG_REQUEST size > tx_buffer size left setting size to "
                       << (int)size_left << " bytes";
            request->size() = size_left;
        }

        if (!request->alloc_data(size_left)) {
            LOG(ERROR) << "Failed buffer allocation to size=" << int(size_left);
            isOK = false;
            break;
        }

        auto data_tuple = request->data(0);
        memset(&std::get<1>(data_tuple), 0, size_left);

        auto slaves = database.get_active_hostaps();
        for (const auto &slave : slaves) {
            LOG(DEBUG) << "tending to send PING_MSG_REQUEST to " << slave;
            if (database.is_hostap_active(slave)) {
                Socket *hostap_mac_sd = database.get_node_socket(slave);
                if (!database.update_node_last_ping_sent(slave)) {
                    LOG(DEBUG) << "PING_MSG_REQUEST received for slave " << slave
                               << " , can't update last ping sent time for ";
                }
                LOG(DEBUG) << "send PING_MSG_REQUEST to " << slave;
                son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, slave);
            }
        }
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_TX_ON_REQUEST: {
        auto cli_request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_HOSTAP_TX_ON_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_TX_ON_REQUEST failed";
            isOK = false;
            break;
        }

        auto request =
            message_com::create_vs_message<beerocks_message::cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST>(
                cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }

        std::string hostap_mac  = network_utils::mac_to_string(cli_request->ap_mac());
        Socket *hostap_mac_sd   = database.get_node_socket(hostap_mac);
        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_TX_OFF_REQUEST: {
        auto cli_request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_HOSTAP_TX_OFF_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_TX_OFF_REQUEST failed";
            isOK = false;
            break;
        }

        auto request =
            message_com::create_vs_message<beerocks_message::cACTION_CONTROL_HOSTAP_TX_ON_REQUEST>(
                cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }

        std::string hostap_mac = network_utils::mac_to_string(cli_request->ap_mac());
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_STATS_MEASUREMENT: {
        auto request_in =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_HOSTAP_STATS_MEASUREMENT>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_STATS_MEASUREMENT failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = network_utils::mac_to_string(request_in->ap_mac());
        auto request_out       = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST>(cmdu_tx);
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }

        if (hostap_mac == network_utils::ZERO_MAC_STRING) {
            /*
                 * request from all hostaps
                 */
            LOG(DEBUG) << "CLI requesting stats measurements from all hostaps";
            auto hostaps = database.get_active_hostaps();
            for (auto hostap : hostaps) {
                LOG(DEBUG) << "CLI requesting stats measurement from " << hostap;
                son_actions::send_cmdu_to_agent(database.get_node_socket(hostap), cmdu_tx, hostap);
            }
        } else {
            LOG(DEBUG) << "CLI requesting stats measurements from " << hostap_mac;
            const auto parent_radio = database.get_node_parent_radio(hostap_mac);
            son_actions::send_cmdu_to_agent(database.get_node_socket(hostap_mac), cmdu_tx,
                                            parent_radio);
        }
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST failed";
            isOK = false;
            break;
        }
        auto hostap_mac       = network_utils::mac_to_string(cli_request->ap_mac());
        Socket *hostap_mac_sd = database.get_node_socket(hostap_mac);

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }
        memset(&request->params(), 0, sizeof(beerocks_message::sNeighborSetParams11k));
        request->params().channel = cli_request->channel();
        request->params().bssid   = cli_request->bssid();
        request->params().vap_id  = cli_request->vap_id();

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = network_utils::mac_to_string(cli_request->ap_mac());
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }
        memset(&request->params(), 0, sizeof(beerocks_message::sNeighborSetParams11k));
        request->params().bssid  = cli_request->bssid();
        request->params().vap_id = cli_request->vap_id();

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);

        break;
    }
    case beerocks_message::ACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS failed";
            isOK = false;
            break;
        }

        if (cli_request->attempts() != (int32_t)database.get_slave_stop_on_failure_attempts() &&
            cli_request->attempts() != -1) {
            database.set_slave_stop_on_failure_attempts(cli_request->attempts());
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST>(
                cmdu_tx);
            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                isOK = false;
                break;
            }
            request->attempts() = cli_request->attempts();

            auto ires = database.get_all_connected_ires();
            for (auto ire : ires) {
                auto slaves = database.get_node_children(ire, beerocks::TYPE_SLAVE);
                for (const auto &slave : slaves) {
                    auto slave_sd = database.get_node_socket(slave);
                    if (database.get_node_state(slave) != beerocks::STATE_DISCONNECTED &&
                        slave_sd != nullptr) {
                        const auto parent_radio = database.get_node_parent_radio(slave);
                        son_actions::send_cmdu_to_agent(slave_sd, cmdu_tx, parent_radio);
                    }
                }
            }
        }
        currentValue = (int8_t)database.get_slave_stop_on_failure_attempts();
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_ALLOW_REQUEST: {
        auto cli_request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_ALLOW_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_ALLOW_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(cli_request->hostap_mac());
        LOG(DEBUG) << "CLI client allow request for " << client_mac << " to " << hostap_mac;

        Socket *hostap_mac_sd = database.get_node_socket(hostap_mac);
        auto request =
            message_com::create_vs_message<beerocks_message::cACTION_CONTROL_CLIENT_ALLOW_REQUEST>(
                cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_ALLOW_REQUEST message!";
            isOK = false;
            break;
        }

        request->mac()  = network_utils::mac_from_string(client_mac);
        request->ipv4() = network_utils::ipv4_from_string(database.get_node_ipv4(client_mac));

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_CROSS_RX_RSSI_MEASUREMENT: {

        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CROSS_RX_RSSI_MEASUREMENT failed";
            isOK = false;
            break;
        }

        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(cli_request->hostap_mac());
        std::string sta_parent = database.get_node_parent(client_mac);
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST message!";
            isOK = false;
            break;
        }

        request->params().mac     = network_utils::mac_from_string(client_mac);
        request->params().ipv4    = network_utils::ipv4_from_string(network_utils::ZERO_IP_STRING);
        request->params().channel = database.get_node_channel(sta_parent);
        request->params().bandwidth = database.get_node_bw(sta_parent);
        request->params().vht_center_frequency =
            cli_request->center_frequency() ? cli_request->center_frequency()
                                            : database.get_hostap_vht_center_frequency(sta_parent);
        request->params().cross                  = 0;
        request->params().mon_ping_burst_pkt_num = 0;

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        LOG(DEBUG) << "CLI cross rx_rssi measurement request for sta=" << client_mac
                   << " hostap=" << hostap_mac;
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_DISALLOW_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_DISALLOW_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_DISALLOW_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(cli_request->hostap_mac());
        LOG(DEBUG) << "CLI client disallow request for " << client_mac << " to " << hostap_mac;

        Socket *hostap_mac_sd = database.get_node_socket(hostap_mac);
        auto request          = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_DISALLOW_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_DISALLOW_REQUEST message!";
            isOK = false;
            break;
        }
        request->mac() = network_utils::mac_from_string(client_mac);

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_DISCONNECT_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_DISCONNECT_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_DISCONNECT_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = database.get_node_parent(client_mac);
        LOG(DEBUG) << "CLI client disassociate request, client " << client_mac << " hostap "
                   << hostap_mac;

        Socket *hostap_mac_sd = database.get_node_socket(hostap_mac);
        auto request          = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST message!";
            isOK = false;
            break;
        }
        request->mac()    = network_utils::mac_from_string(client_mac);
        request->vap_id() = database.get_hostap_vap_id(hostap_mac);
        request->reason() = cli_request->reason();
        request->type()   = cli_request->type();

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_BEACON_11K_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_BEACON_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_BEACON_11K_REQUEST failed";
            isOK = false;
            break;
        }

        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());

        /////////////// FOR DEBUG ONLY ////////////////
        if (client_mac == network_utils::ZERO_MAC_STRING) {
            LOG(DEBUG) << "Updating beacon request params on database";
            optimal_path_task::cli_beacon_request_duration  = cli_request->duration();
            optimal_path_task::cli_beacon_request_rand_ival = cli_request->rand_ival();
            optimal_path_task::cli_beacon_request_mode =
                (beerocks::eMeasurementMode11K)cli_request->measurement_mode();
            break;
        } else if (client_mac == network_utils::WILD_MAC_STRING) {
            LOG(DEBUG) << "Updating beacon request params on database to default";
            optimal_path_task::cli_beacon_request_duration  = -1;
            optimal_path_task::cli_beacon_request_rand_ival = -1;
            optimal_path_task::cli_beacon_request_mode      = beerocks::MEASURE_MODE_UNDEFINED;
            break;
        }
        ///////////////////////////////////////////////

        std::string hostap_mac = database.get_node_parent(client_mac);
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);
        LOG(DEBUG) << "CLI beacon request for " << client_mac << " to " << hostap_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST message!";
            isOK = false;
            break;
        }
        //TODO: set params values
        request->params().measurement_mode =
            cli_request
                ->measurement_mode(); // son::eWiFiMeasurementMode11K. Should be replaced with string "passive"/"active"/"table"
        request->params().channel  = cli_request->channel();
        request->params().op_class = cli_request->op_class();
        request->params().repeats =
            cli_request
                ->repeats(); // '0' = once, '65535' = repeats until cancel request, other (1-65534) = specific num of repeats
        request->params().rand_ival =
            cli_request
                ->rand_ival(); // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
        request->params().duration =
            cli_request->duration(); // measurement duration, expressed in units of TUs [=1024usec]
        request->params().sta_mac = cli_request->client_mac();
        request->params().bssid =
            cli_request
                ->bssid(); // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"

        request->params().expected_reports_count = 1;

        // Optional:
        if (cli_request->use_optional_ssid()) {
            request->params().use_optional_ssid = 1; // bool
            string_utils::copy_string((char *)request->params().ssid,
                                      (char *)&std::get<1>(cli_request->ssid(0)),
                                      beerocks::message::WIFI_SSID_MAX_LENGTH);
        } else {
            request->params().use_optional_ssid = 0;
        }

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);

        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST failed";
            isOK = false;
            break;
        }

        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(cli_request->hostap_mac());
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);
        LOG(DEBUG) << "CLI channel load request for " << client_mac << " to " << hostap_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST message!";
            isOK = false;
            break;
        }

        request->params().channel   = cli_request->channel();
        request->params().op_class  = 0;
        request->params().repeats   = 0;
        request->params().rand_ival = 1000;
        request->params().duration  = 50;
        request->params().sta_mac   = cli_request->client_mac();

        request->params().parallel           = 0;
        request->params().enable             = 0;
        request->params().request            = 0;
        request->params().report             = 0;
        request->params().mandatory_duration = 0;

        // Optional:
        request->params().use_optional_ch_load_rep = 0;
        request->params().ch_load_rep_first        = 0;
        request->params().ch_load_rep_second       = 0;

        request->params().use_optional_wide_band_ch_switch = 0;
        request->params().new_ch_width                     = 0;
        request->params().new_ch_center_freq_seg_0         = 0;
        request->params().new_ch_center_freq_seg_1         = 0;

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);

        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_STATISTICS_11K_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_STATISTICS_11K_REQUEST failed";
            isOK = false;
            break;
        }

        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(cli_request->hostap_mac());
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);
        LOG(DEBUG) << "CLI statistics request for " << client_mac << " to " << hostap_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST message!";
            isOK = false;
            break;
        }

        //TODO: set params values
        request->params().group_identity = cli_request->group_identity();
        request->params().repeats =
            0; // '0' = once, '65535' = repeats until cancel request, other (1-65534) = specific num of repeats
        request->params().rand_ival =
            1000; // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
        request->params().duration =
            10; // measurement duration, expressed in units of TUs [=1024usec]
        request->params().sta_mac = cli_request->client_mac();
        request->params().peer_mac_addr =
            cli_request
                ->peer_mac(); // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"

        // Measurement request mode booleans:
        request->params().parallel =
            0; // (for multiple requests)'0' - measurements are to be performed in sequence,
        //  '1' - request that the measurement is to start at the same time as the measurement described
        //  by the next Measurement Request element in the same Measurement Request frame
        request->params().enable  = 0;
        request->params().request = 0;
        request->params().report  = 0;
        request->params().mandatory_duration =
            0; // '0' - the duration can be lower than in the duration field, '1' - duration is mandantory

        // Optional:
        request->params().use_optional_trig_rep_sta_counters = 0; // bool
        // request.params.measurement_count_1;
        // request.params.trigger_timeout_1;
        // request.params.sta_counter_trigger_condition;
        // request.params.dot11FailedCountThreshold;
        // request.params.dot11FCSErrorCountThreshold;
        // request.params.dot11MultipleRetryCountThreshold;
        // request.params.dot11FrameDuplicateCountThreshold;
        // request.params.dot11RTSFailureCountThreshold;
        // request.params.dot11AckFailureCountThreshold;
        // request.params.dot11RetryCountThreshold;

        request->params().use_optional_trig_rep_qos_sta_counters = 0; // bool
        // request.params.measurement_count_2;
        // request.params.trigger_timeout_2;
        // request.params.qos_sta_counter_trigger_condition;
        // request.params.dot11QoSFailedCountThreshold;
        // request.params.dot11QoSRetryCountThreshold;
        // request.params.dot11QoSMultipleRetryCountThreshold;
        // request.params.dot11QoSFrameDuplicateCountThreshold;
        // request.params.dot11QoSRTSCountFailureThreshold;
        // request.params.dot11QoSAckFailureCountThreshold;
        // request.params.dot11QoSDiscardedCountThreshold;

        request->params().use_optional_trig_rep_rsna_counters = 0; // bool
        // request.params.measurement_count_3;
        // request.params.trigger_timeout_3;
        // request.params.rsna_counter_trigger_condition;
        // request.params.dot11RSNAStatsCMACICVErrorsThreshold;
        // request.params.dot11RSNAStatsCMACReplaysThreshold;
        // request.params.dot11RSNAStatsRobustMgmtCCMPReplaysThreshold;
        // request.params.dot11RSNAStatsTKIPICVErrorsThreshold;
        // request.params.dot11RSNAStatsTKIPReplaysThreshold;
        // request.params.dot11RSNAStatsCCMPDecryptErrorsThreshold;
        // request.params.dot11RSNAStatsCCMPReplaysThreshold;

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);

        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST failed";
            isOK = false;
            break;
        }

        std::string client_mac = network_utils::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(cli_request->hostap_mac());
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);
        LOG(DEBUG) << "CLI link measurement request for " << client_mac << " to " << hostap_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST message!";
            isOK = false;
            break;
        }

        request->mac() = cli_request->client_mac();

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST: {
        auto cli_request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = network_utils::mac_to_string(cli_request->mac());
        Socket *hostap_mac_sd  = database.get_node_socket(hostap_mac);
        LOG(DEBUG) << "CLI ap channel switch request for " << hostap_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START message!";
            isOK = false;
            break;
        }

        request->cs_params() = cli_request->cs_params();

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(hostap_mac_sd, cmdu_tx, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS: {
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS failed";
            isOK = false;
            break;
        }
        if (request->isEnable() >= 0) {
            database.settings_diagnostics_measurements(request->isEnable());
            LOG(INFO) << "CLI load_diagnostics_measurements changed to "
                      << int(database.settings_diagnostics_measurements());
        }
        currentValue = database.settings_diagnostics_measurements();
        break;
    }
    case beerocks_message::ACTION_CLI_ENABLE_DEBUG: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_ENABLE_DEBUG>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_ENABLE_DEBUG failed";
            isOK = false;
            break;
        }

        if (request->isEnable() >= 0) {
            if (request->isEnable()) {
                database.add_cli_socket(sd);
            } else {
                database.remove_cli_socket(sd);
            }
        }
        currentValue = database.get_cli_debug_enable(sd);
        break;
    }
    case beerocks_message::ACTION_CLI_DUMP_NODE_INFO: {
        auto cli_request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_DUMP_NODE_INFO>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_DUMP_NODE_INFO failed";
            isOK = false;
            break;
        }
        std::string mac       = network_utils::mac_to_string(cli_request->mac());
        std::string node_info = database.node_to_string(mac);
        std::size_t length    = node_info.size();
        LOG(TRACE) << "CLI dump node info for " << mac
                   << " node info size = " << std::to_string(length);

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_CLI_RESPONSE_STR>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_CLI_RESPONSE_STR message!";
            isOK = false;
            break;
        }

        //In case we don't have enough space for node length, reserve 1 byte for '\0'
        size_t reserved_size =
            (message_com::get_vs_cmdu_size_on_buffer<beerocks_message::cACTION_CLI_RESPONSE_STR>() -
             1);
        size_t max_size = cmdu_tx.getMessageBuffLength() - reserved_size;

        if (length == 0) {
            node_info = "Error: mac does not exist";
            length    = node_info.size();
        }

        size_t size = (length > max_size) ? max_size : length;

        if (!response->alloc_buffer(size)) {
            LOG(ERROR) << "Failed buffer allocation to size=" << int(size);
            isOK = false;
            break;
        }

        std::copy_n(node_info.c_str(), size, response->buffer(0));
        (response->buffer(0))[size] = 0;

        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CLI_OPTIMAL_PATH_TASK: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_OPTIMAL_PATH_TASK>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_OPTIMAL_PATH_TASK failed";
            isOK = false;
            break;
        }
        std::string client_mac = network_utils::mac_to_string(request->client_mac());
        if (database.get_node_state(client_mac) == beerocks::STATE_CONNECTED) {

            int prev_task_id = database.get_roaming_task_id(client_mac);
            if (tasks.is_task_running(prev_task_id)) {
                LOG(TRACE) << "CLI roaming task already running for " << client_mac;
            } else {

                if (database.get_node_type(client_mac) == beerocks::TYPE_CLIENT) {
                    LOG(TRACE) << "CLI start roaming task for " << client_mac;
                    auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, tasks,
                                                                        client_mac, 0, "");
                    tasks.add_task(new_task);
                } else if (database.get_node_type(client_mac) == beerocks::TYPE_IRE_BACKHAUL) {
                    LOG(TRACE) << "CLI start roaming task for " << client_mac;
                    //auto new_task = std::make_shared<optimize_ire_task>(database,  cmdu_tx, tasks, client_mac, "");
                    auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, tasks,
                                                                        client_mac, 0, "");
                    tasks.add_task(new_task);
                }
            }
        } else {
            isOK = false;
        }
        break;
    }
    case beerocks_message::ACTION_CLI_LOAD_BALANCER_TASK: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_LOAD_BALANCER_TASK>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_LOAD_BALANCER_TASK failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = network_utils::mac_to_string(request->ap_mac());
        std::string ire_mac    = database.get_node_parent_ire(hostap_mac);
        LOG(TRACE) << "CLI load notification from hostap " << hostap_mac << " ire mac=" << ire_mac;

        /*
             * start load balancing
             */
        if (database.is_hostap_active(hostap_mac) &&
            database.get_node_state(ire_mac) == beerocks::STATE_CONNECTED &&
            database.get_node_type(ire_mac) != beerocks::TYPE_CLIENT) {
            /*
                 * when a notification arrives, it means a large change in rx_rssi occurred (above the defined thershold)
                 * therefore, we need to create a load balancing task to optimize the network
                 */
            int prev_task_id = database.get_load_balancer_task_id(ire_mac);
            if (tasks.is_task_running(prev_task_id)) {
                LOG(TRACE) << "CLI load balancer task already running for " << ire_mac;
            } else {
                auto new_task = std::make_shared<load_balancer_task>(database, cmdu_tx, tasks,
                                                                     ire_mac, "load_balancer");
                tasks.add_task(new_task);
            }
        } else {
            isOK = false;
        }
        break;
    }
    case beerocks_message::ACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK: {
        LOG(TRACE) << "CLI IRE network optimization scan triggered";
        auto new_task = std::make_shared<ire_network_optimization_task>(
            database, cmdu_tx, tasks, "cli - ire_network_optimization");
        tasks.add_task(new_task);
        break;
    }
    case beerocks_message::ACTION_CLI_BACKHAUL_ROAM_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_BACKHAUL_ROAM_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_BACKHAUL_ROAM_REQUEST failed";
            isOK = false;
            break;
        }
        std::string slave_mac  = network_utils::mac_to_string(request->slave_mac());
        std::string hostap_mac = network_utils::mac_to_string(request->bssid());
        //TODO: we are passing true for imminent by default
        //extend ACTION_CLI_BACKHAUL_ROAM_REQUEST to have imminent variable
        uint8_t disassoc_imminent = uint8_t(1);
        LOG(DEBUG) << "CLI steer IRE request for " << slave_mac << " to hostap: " << hostap_mac;
        son_actions::steer_sta(database, cmdu_tx, tasks, slave_mac, hostap_mac, disassoc_imminent);
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_BSS_STEER_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_CLI_CLIENT_BSS_STEER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_CLIENT_BSS_STEER_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = network_utils::mac_to_string(request->client_mac());
        std::string hostap_mac = network_utils::mac_to_string(request->bssid());

        uint8_t disassoc_imminent = request->disassoc_timer() ? uint8_t(1) : uint8_t(0);
        LOG(DEBUG) << "CLI steer sta request for " << client_mac << " to hostap: " << hostap_mac
                   << " disassoc_imminent=" << int(disassoc_imminent)
                   << " disassoc_timer=" << int(request->disassoc_timer());
        son_actions::steer_sta(database, cmdu_tx, tasks, client_mac, hostap_mac,
                               int(disassoc_imminent), int(request->disassoc_timer()));

        break;
    }
    default: {
        LOG(ERROR) << "Unsupported CLI action_op:" << int(header->action_op());
        isOK = false;
        break;
    }
    }

    //Send response message
    auto response_action_op = beerocks_message::ACTION_CLI_RESPONSE_INT;
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_RESPONSE_INT>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return;
    }
    response->isOK()         = isOK;
    response->currentValue() = currentValue;
    if (response_action_op == beerocks_message::ACTION_CLI_RESPONSE_INT) {
        message_com::send_cmdu(sd, cmdu_tx);
    }
}

void son_management::handle_bml_message(
    Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
    task_pool &tasks)
{
    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_BML_PING_REQUEST: {
        LOG(TRACE) << "ACTION_BML_PING_REQUEST";
        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_PING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_PING_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);

    } break;

    case beerocks_message::ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST";

        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_NW_MAP_UPDATES,
                         &new_event);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "create ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST";

        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_NW_MAP_UPDATES,
                         &new_event);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "create ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_NW_MAP_REQUEST: {
        LOG(TRACE) << "ACTION_BML_NW_MAP_REQUEST";
        network_map::send_bml_network_map_message(database, sd, cmdu_tx, beerocks_header->id());
    } break;

    case beerocks_message::ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_STATS_UPDATES,
                         &new_event);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_STATS_UPDATES,
                         &new_event);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_EVENTS_UPDATES,
                         &new_event);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_EVENTS_UPDATES,
                         &new_event);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE failed";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_CLIENT_ROAMING_REQUEST failed";
            break;
        }

        database.settings_client_optimal_path_roaming(request->isEnable());
        database.settings_client_11k_roaming(request->isEnable());
        LOG(INFO) << "BML client_optimal_path_roaming to "
                  << int(database.settings_client_optimal_path_roaming());
        LOG(INFO) << "BML client_11k_roaming to " << int(database.settings_client_11k_roaming());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_CLIENT_ROAMING_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_GET_CLIENT_ROAMING_RESPONSE failed";
            break;
        }
        response->isEnable() = database.settings_client_optimal_path_roaming();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST: {
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST failed";
            break;
        }

        database.settings_legacy_client_roaming(request->isEnable());
        LOG(INFO) << "BML legacy_client_roaming to "
                  << int(database.settings_legacy_client_roaming());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE failed";
            break;
        }
        response->isEnable() = (database.settings_legacy_client_roaming());

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST: {
        auto request = cmdu_rx.addClass<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST failed";
            break;
        }

        database.settings_client_optimal_path_roaming_prefer_signal_strength(request->isEnable());
        LOG(INFO) << "BML settings_client_roaming_prefer_signal_strength to "
                  << int(database.settings_client_optimal_path_roaming_prefer_signal_strength());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE>(
            cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building "
                          "ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE>(
            cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building "
                          "ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE message!";
            break;
        }

        response->isEnable() =
            database.settings_client_optimal_path_roaming_prefer_signal_strength();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST: {
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST failed";
            break;
        }

        database.settings_client_band_steering(request->isEnable());
        LOG(INFO) << "BML settings_client_band_steering changed to "
                  << int(database.settings_client_band_steering());
        database.settings_client_11k_roaming(request->isEnable());
        LOG(INFO) << "BML settings_client_11k_roaming changed to "
                  << int(database.settings_client_band_steering());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_client_band_steering();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_IRE_ROAMING_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_IRE_ROAMING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_IRE_ROAMING_REQUEST failed";
            break;
        }

        database.settings_ire_roaming(request->isEnable());
        LOG(INFO) << "BML settings_client_band_steering changed to "
                  << int(database.settings_ire_roaming());

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_SET_IRE_ROAMING_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_IRE_ROAMING_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_IRE_ROAMING_REQUEST: {
        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_GET_IRE_ROAMING_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_IRE_ROAMING_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_ire_roaming();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_LOAD_BALANCER_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_LOAD_BALANCER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_LOAD_BALANCER_REQUEST failed";
            break;
        }

        database.settings_load_balancing(request->isEnable());
        LOG(INFO) << "BML settings_load_balancing changed to "
                  << int(database.settings_load_balancing());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_LOAD_BALANCER_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_LOAD_BALANCER_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_LOAD_BALANCER_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_LOAD_BALANCER_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_LOAD_BALANCER_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_load_balancing();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST: {
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST failed";
            break;
        }

        database.settings_service_fairness(request->isEnable());
        LOG(INFO) << "BML settings_service_fairness changed to "
                  << int(database.settings_service_fairness());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_SERVICE_FAIRNESS_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_service_fairness();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_DFS_REENTRY_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_DFS_REENTRY_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_DFS_REENTRY_REQUEST failed";
            break;
        }

        database.settings_dfs_reentry(request->isEnable());
        LOG(INFO) << "BML settings_dfs_reentry changed to " << int(database.settings_dfs_reentry());

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_SET_DFS_REENTRY_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_BML_SET_DFS_REENTRY_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_DFS_REENTRY_REQUEST: {
        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_GET_DFS_REENTRY_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_BML_GET_DFS_REENTRY_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_dfs_reentry();

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST";

        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST failed";
            break;
        }

        //save new configured restricted channel to DB.
        if (request->params().is_global) {
            database.set_global_restricted_channels(request->params().restricted_channels);
        } else {
            database.set_hostap_conf_restricted_channels(
                network_utils::mac_to_string(request->params().hostap_mac),
                request->params().restricted_channels);
        }

        //send restricted channel event to channel selection task
        auto new_event = CHANNEL_SELECTION_ALLOCATE_EVENT(
            channel_selection_task::sConfiguredRestrictedChannels_event);
        new_event->hostap_mac = request->params().hostap_mac;
        std::copy_n(request->params().restricted_channels, sizeof(new_event->restricted_channels),
                    new_event->restricted_channels);
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::CONFIGURED_RESTRICTED_CHANNELS_EVENT,
                         (void *)new_event);

        //send response to bml
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE message!";
            break;
        }

        message_com::send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE message!";
            break;
        }

        auto vec_restricted_channels =
            request->params().is_global
                ? database.get_global_restricted_channels()
                : database.get_hostap_conf_restricted_channels(
                      network_utils::mac_to_string(request->params().hostap_mac));
        std::copy(vec_restricted_channels.begin(), vec_restricted_channels.end(),
                  response->params().restricted_channels);

        //send response to bml
        message_com::send_cmdu(sd, cmdu_tx);
    } break;
    case beerocks_message::ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST: {
        auto bml_request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST>();
        if (bml_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST failed";
            break;
        }
        if (bml_request->params().module_name == beerocks::BEEROCKS_PROCESS_MASTER) {
            database.set_log_level_state((beerocks::eLogLevel)bml_request->params().log_level,
                                         bml_request->params().enable);
        } else {
            if (bml_request->params().module_name == beerocks::BEEROCKS_PROCESS_ALL) {
                database.set_log_level_state((eLogLevel)bml_request->params().log_level,
                                             bml_request->params().enable);
            }
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL>(cmdu_tx);

            if (request == nullptr) {
                LOG(ERROR) << "Failed building ACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL message!";
                break;
            }
            request->params().module_name = bml_request->params().module_name;
            request->params().log_level   = bml_request->params().log_level;
            request->params().enable      = bml_request->params().enable;

            std::string dst_mac = network_utils::mac_to_string(bml_request->params().mac);
            if (dst_mac == network_utils::WILD_MAC_STRING) {
                auto slaves = database.get_active_hostaps();
                for (const auto &slave : slaves) {
                    if (database.is_hostap_active(slave)) {
                        Socket *slave_sd = database.get_node_socket(slave);
                        son_actions::send_cmdu_to_agent(slave_sd, cmdu_tx, slave);
                    }
                }
            } else {
                Socket *slave_sd        = database.get_node_socket(dst_mac);
                const auto parent_radio = database.get_node_parent_radio(dst_mac);
                son_actions::send_cmdu_to_agent(slave_sd, cmdu_tx, parent_radio);
            }
        }
        // send response
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST: {
        LOG(TRACE) << "ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST failed";
            return;
        }

        int prev_task_id = database.get_bml_config_update_task_id();
        if (tasks.is_task_running(prev_task_id)) {
            LOG(DEBUG)
                << "BML Config update task is already running, adding request tot task queue";
            bml_wifi_credentials_update_task::new_credential_event_t event;
            event.cred = request->params();
            event.sd   = sd;
            tasks.push_event(prev_task_id,
                             bml_wifi_credentials_update_task::NEW_CREDENTIALS_REQUEST, &event);
        } else {
            auto new_task = std::make_shared<bml_wifi_credentials_update_task>(
                database, cmdu_tx, tasks, sd, request->params());
            tasks.add_task(new_task);
        }
        break;
    }
    case beerocks_message::ACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST: {

        uint32_t result = 1; //1-fail 0-success

        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST failed";
            return;
        }

        auto vap_list_size = request->vap_list_size();
        if (vap_list_size == 0) {
            LOG(WARNING) << "Received an empty list, clearing the list in the DB";
            database.clear_vap_list();
            result = 0;
        } else {

            LOG(INFO) << "Received " << int(vap_list_size) << " VAPs from BML";

            auto vap_list_tuple = request->vap_list(0);
            if (std::get<0>(vap_list_tuple) == false) {
                LOG(ERROR) << "vap list access fail!";
            } else {

                auto vap_list = std::make_shared<son::db::vaps_list_t>();

                auto bml_vaps = &std::get<1>(vap_list_tuple);
                for (uint8_t i = 0; i < vap_list_size; i++) {
                    vap_list->push_back(
                        std::make_shared<beerocks_message::sConfigVapInfo>(bml_vaps[i]));
                }

                // Store the list in the DB
                database.set_vap_list(vap_list);
                result = 0;
            }
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE>(cmdu_tx,
                                                                             beerocks_header->id());

        response->result() = result;

        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building message cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE ! ";
        } else {
            if (message_com::send_cmdu(sd, cmdu_tx) == false) {
                LOG(ERROR) << "Error sending cmdu message";
            }
        }
        break;
    }
    case beerocks_message::ACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST: {

        uint32_t result = 1; //1-fail 0-success

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE>(cmdu_tx,
                                                                             beerocks_header->id());

        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building message cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE ! ";
            return;
        }

        auto vap_list = database.get_vap_list();
        if (!vap_list || !vap_list->size()) {
            LOG(DEBUG) << "vap list is empty!";

            // Send a valid response with an empty list
            result = 0;

        } else {

            auto vap_list_size = vap_list->size();
            LOG(INFO) << "Returning " << vap_list_size << " VAPs to BML caller";

            if (response->alloc_vap_list(vap_list_size) == false) {
                LOG(ERROR) << "Failed buffer allocation to size = " << int(vap_list_size);
            } else {
                auto vap_list_tuple = response->vap_list(0);
                if (std::get<0>(vap_list_tuple) == false) {
                    LOG(ERROR) << "vap list access fail!";
                } else {
                    auto vaps = &std::get<1>(vap_list_tuple);
                    uint8_t i = 0;
                    for (auto vap : *vap_list) {
                        vaps[i] = *vap;
                        i++;
                    }

                    // Change the result value to success
                    result = 0;
                }
            }
        }

        response->result() = result;
        if (!message_com::send_cmdu(sd, cmdu_tx)) {
            LOG(ERROR) << "Error sending get vaps list responce  message";
        }
        break;
    }
#ifdef BEEROCKS_RDKB
    case beerocks_message::ACTION_BML_STEERING_SET_GROUP_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_SET_GROUP_REQUEST";
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_STEERING_SET_GROUP_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_STEERING_SET_GROUP_REQUEST failed";
            break;
        }

        rdkb_wlan_task::steering_set_group_request_event new_event;
        new_event.sd                 = sd;
        new_event.remove             = request->remove();
        new_event.steeringGroupIndex = request->steeringGroupIndex();
        new_event.cfg_2              = request->cfg_2();
        new_event.cfg_5              = request->cfg_5();

        tasks.push_event(database.get_rdkb_wlan_task_id(),
                         rdkb_wlan_task::events::STEERING_SET_GROUP_REQUEST, &new_event);
        break;
    }
    case beerocks_message::ACTION_BML_STEERING_CLIENT_SET_REQUEST: {
        LOG(TRACE) << "cACTION_BML_STEERING_CLIENT_SET_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_SET_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_SET_REQUEST failed";
            break;
        }

        rdkb_wlan_task::steering_client_set_request_event new_event;
        //checking for remove option
        new_event.sd                 = sd;
        new_event.remove             = request->remove();
        new_event.steeringGroupIndex = request->steeringGroupIndex();
        new_event.bssid              = network_utils::mac_to_string(request->bssid());
        new_event.client_mac         = request->client_mac();
        new_event.config             = request->config();

        tasks.push_event(database.get_rdkb_wlan_task_id(),
                         rdkb_wlan_task::events::STEERING_CLIENT_SET_REQUEST, &new_event);
        break;
    }
    case beerocks_message::ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST";
        auto request = cmdu_rx.addClass<
            beerocks_message::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST failed";
            break;
        }

        //TODO: set database to update sd as listener in rdkb_wlan_task.
        rdkb_wlan_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        if (request->unregister()) {
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_UNREGISTER, &new_event);
        } else {
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_REGISTER, &new_event);
        }

        break;
    }
    case beerocks_message::ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST failed";
            break;
        }

        rdkb_wlan_task::steering_client_disconnect_request_event new_event;
        new_event.sd                 = sd;
        new_event.client_mac         = request->client_mac();
        new_event.steeringGroupIndex = request->steeringGroupIndex();
        new_event.bssid              = network_utils::mac_to_string(request->bssid());
        new_event.type               = request->type();
        new_event.reason             = request->reason();

        tasks.push_event(database.get_rdkb_wlan_task_id(),
                         rdkb_wlan_task::events::STEERING_CLIENT_DISCONNECT_REQUEST, &new_event);
        break;
    }
    case beerocks_message::ACTION_BML_STEERING_CLIENT_MEASURE_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_CLIENT_MEASURE_REQUEST";
        auto request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST failed";
            break;
        }

        rdkb_wlan_task::steering_rssi_measurement_request_event new_event;
        new_event.sd           = sd;
        std::string client_mac = network_utils::mac_to_string(request->client_mac());
        std::string sta_parent = database.get_node_parent(client_mac);

        new_event.bssid            = network_utils::mac_to_string(request->bssid());
        new_event.params.mac       = network_utils::mac_from_string(client_mac);
        new_event.params.ipv4      = network_utils::ipv4_from_string(network_utils::ZERO_IP_STRING);
        new_event.params.channel   = database.get_node_channel(sta_parent);
        new_event.params.bandwidth = database.get_node_bw(sta_parent);
        new_event.params.vht_center_frequency =
            database.get_hostap_vht_center_frequency(sta_parent);
        new_event.params.cross                  = 0;
        new_event.params.mon_ping_burst_pkt_num = 0;

        tasks.push_event(database.get_rdkb_wlan_task_id(),
                         rdkb_wlan_task::events::STEERING_RSSI_MEASUREMENT_REQUEST, &new_event);
        break;
    }
#endif //BEEROCKS_RDKB
    case beerocks_message::ACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST: {

        auto bml_request =
            cmdu_rx.addClass<beerocks_message::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST>();

        if (bml_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST failed";
            return;
        }

        std::string al_mac   = network_utils::mac_to_string(bml_request->al_mac());
        std::string ruid_mac = network_utils::mac_to_string(bml_request->ruid());
        std::string key      = database.get_node_key(al_mac, ruid_mac);
        Socket *son_slave_sd = database.get_node_socket(key);

        if (son_slave_sd == nullptr) {
            LOG(ERROR) << "Could not find socket for al_mac=" << al_mac << ", ruid=" << ruid_mac;
            return;
        }

        LOG(INFO) << "ACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST "
                  << ", al_mac=" << al_mac << ", ruid=" << ruid_mac;

        auto cmdu_header =
            cmdu_tx.create(0, ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE);

        if (cmdu_header == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }

        son_actions::send_cmdu_to_agent(son_slave_sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_WFA_CA_CONTROLLER_REQUEST: {

        wfa_ca::handle_wfa_ca_message(sd, beerocks_header, cmdu_rx, cmdu_tx, database, tasks);
        break;
    }
    default: {
        LOG(ERROR) << "Unsupported BML action_op:" << int(beerocks_header->action_op());
        break;
    }
    }
}
