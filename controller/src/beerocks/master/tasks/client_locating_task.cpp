/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "client_locating_task.h"
#include "../db/db_algo.h"
#include "../son_actions.h"
#include "bml_task.h"

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

client_locating_task::client_locating_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                           task_pool &tasks_, std::string client_mac_,
                                           bool new_connection_, int starting_delay_ms_,
                                           std::string eth_switch_, std::string task_name_)
    : task(task_name_), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_),
      client_mac(client_mac_), new_connection(new_connection_),
      starting_delay_ms(starting_delay_ms_), eth_switch(eth_switch_)
{
}

void client_locating_task::work()
{
    if (database.get_node_type(client_mac) == beerocks::TYPE_IRE) {
        auto ire_backhaul = database.get_node_parent_backhaul(client_mac);
        if (database.is_node_wireless(ire_backhaul)) {
            TASK_LOG(DEBUG) << "IRE is wireless -> finish task";
            finish();
        }
    } else if (database.is_node_wireless(client_mac)) {
        TASK_LOG(DEBUG) << "client is wireless -> finish task";
        finish();
    }
    switch (state) {
    case START: {
        if ((!new_connection) && (eth_switch == network_utils::ZERO_MAC_STRING)) {
            TASK_LOG(DEBUG) << "bad task input args -> finish task";
            finish();
        } else if ((!new_connection) &&
                   (database.get_node_state(client_mac) == beerocks::STATE_DISCONNECTED)) {
            TASK_LOG(DEBUG)
                << "task in disconnect mode when client is already disconnected -> finish task";
            finish();
        } else if (database.get_node_type(client_mac) == beerocks::TYPE_IRE_BACKHAUL) {
            TASK_LOG(DEBUG) << "not handeling IRE backhaul type -> finish task";
            finish();
        } else {
            int prev_task_id = database.get_client_locating_task_id(client_mac, new_connection);
            tasks.kill_task(prev_task_id);
            database.assign_client_locating_task_id(client_mac, id, new_connection);
            TASK_LOG(DEBUG) << "new task initiate with new_connection "
                            << std::string(new_connection ? "on" : "off");

            client_ipv4 = database.get_node_ipv4(client_mac);

            state = SEND_ARP_QUERIES;
            wait_for(starting_delay_ms);
        }
    } break;

    case SEND_ARP_QUERIES: {
        auto ires        = database.get_all_connected_ires();
        pending_ires_num = 0;

        for (auto &ire : ires) {
            auto request =
                message_com::create_vs_message<beerocks_message::cACTION_CONTROL_ARP_QUERY_REQUEST>(
                    cmdu_tx, id);
            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                continue;
            }
            request->params().mac  = network_utils::mac_from_string(client_mac);
            request->params().ipv4 = network_utils::ipv4_from_string(client_ipv4);

            if ((ire == client_mac) || (client_mac == database.get_node_parent_backhaul(ire))) {
                continue;
            }
            auto backhaul_manager_hostap = database.get_hostap_backhaul_manager(ire);
            if (backhaul_manager_hostap.empty()) {
                TASK_LOG(WARNING) << "backhaul_manager_hostap.empty()!";
                continue;
            }
            Socket *sd = database.get_node_socket(backhaul_manager_hostap);
            if (sd == nullptr) {
                TASK_LOG(WARNING) << "NULL socket for connected backhaul_manager_hostap!";
                continue;
            }

            son_actions::send_cmdu_to_agent(sd, cmdu_tx, backhaul_manager_hostap);

            add_pending_mac(backhaul_manager_hostap,
                            beerocks_message::ACTION_CONTROL_ARP_QUERY_RESPONSE);
            pending_ires_num++;

            TASK_LOG(DEBUG) << "requesting arp query from hostap_backhaul " << ire
                            << " (backhaul_manager_hostap=" << backhaul_manager_hostap
                            << ") for client_mac=" << client_mac << " ipv4=" << client_ipv4;
        }
        TASK_LOG(DEBUG) << "waiting for arp respones";
        set_responses_timeout(task_timeout_seconds * 1000);
        state = FINISH;
    } break;

    case FINISH: {
        /*
             * right now this task only handles ETH clients
             */
        if (new_connection) {
            if (!deepest_slave.empty() &&
                pending_ires_num == 0) { // set hierarchy if received answer from all ires
                TASK_LOG(DEBUG) << "deepest slave is: " << deepest_slave
                                << " hierarchy=" << deepest_slave_hierarchy << " placing client "
                                << client_mac << " under it";
                auto eth_sw_container =
                    database.get_node_siblings(deepest_slave, beerocks::TYPE_ETH_SWITCH);
                if (eth_sw_container.empty()) {
                    TASK_LOG(ERROR) << "eth_sw_container empty for slave " << deepest_slave;
                } else {
                    auto client_parent     = database.get_node_parent(client_mac);
                    auto client_state      = database.get_node_state(client_mac);
                    std::string eth_sw_mac = *eth_sw_container.begin();

                    TASK_LOG(DEBUG)
                        << "client_mac = " << client_mac << " (" << int(client_state) << ")"
                        << ", client_parent = " << client_parent << ", eth_sw_mac = " << eth_sw_mac;

                    // update database and bml listeners if client moved or newly connected
                    if ((client_parent != eth_sw_mac) ||
                        (client_state != beerocks::STATE_CONNECTED)) {
                        // update node
                        if (database.get_node_type(client_mac) == beerocks::TYPE_IRE) {
                            auto ire_backhaul = database.get_node_parent_backhaul(client_mac);
                            database.add_node(network_utils::mac_from_string(ire_backhaul),
                                              network_utils::mac_from_string(eth_sw_mac),
                                              beerocks::TYPE_IRE_BACKHAUL);
                            database.add_node(network_utils::mac_from_string(client_mac),
                                              network_utils::mac_from_string(ire_backhaul),
                                              beerocks::TYPE_IRE);
                        } else {
                            database.add_node(network_utils::mac_from_string(client_mac),
                                              network_utils::mac_from_string(eth_sw_mac));
                            database.set_node_state(client_mac, beerocks::STATE_CONNECTED);
                        }

                        // update bml listeners
                        bml_task::connection_change_event new_event;
                        new_event.mac = client_mac;
                        tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE,
                                         &new_event);
                        TASK_LOG(DEBUG)
                            << "BML, sending client eth connect CONNECTION_CHANGE for mac "
                            << new_event.mac;
                    }
                }
            } else {
                TASK_LOG(DEBUG) << "couldn't find deepest slave";
            }
        } else {
            TASK_LOG(DEBUG) << "ETH client disconnected! " << client_mac
                            << " from eth_switch=" << eth_switch;
            // non of the pending ires replied about that client
            if (database.get_node_type(client_mac) == beerocks::TYPE_IRE) {
                auto backhaul_mac = database.get_node_parent(client_mac);
                son_actions::handle_dead_node(backhaul_mac, eth_switch, database, cmdu_tx, tasks);
            } else if ((database.get_node_type(client_mac) == beerocks::TYPE_CLIENT) ||
                       (database.get_node_type(client_mac) == beerocks::TYPE_IRE_BACKHAUL)) {
                son_actions::handle_dead_node(client_mac, eth_switch, database, cmdu_tx, tasks);
            }
        }
        finish();
    } break;
    }
}

void client_locating_task::handle_response(
    std::string mac, beerocks_message::eActionOp_CONTROL action_op,
    std::shared_ptr<beerocks::beerocks_header> beerocks_header)
{
    switch (action_op) {
    case beerocks_message::ACTION_CONTROL_ARP_QUERY_RESPONSE: {
        auto response =
            beerocks_header->getClass<beerocks_message::cACTION_CONTROL_ARP_QUERY_RESPONSE>();

        if (!response) {
            TASK_LOG(ERROR) << "getClass failed for cACTION_CONTROL_ARP_QUERY_RESPONSE";
            return;
        }

        pending_ires_num--;

        std::string ipv4    = network_utils::ipv4_to_string(response->params().ipv4);
        std::string arp_mac = network_utils::mac_to_string(response->params().mac);

        TASK_LOG(DEBUG) << "received response from slave " << mac << ":" << std::endl
                        << "   arp_mac=" << arp_mac << std::endl
                        << "   arp_ipv4=" << ipv4 << std::endl
                        << "   arp_state=" << int(response->params().state)
                        << " arp_source=" << int(response->params().source);

        if (new_connection) {
            if (client_ipv4 == ipv4 &&
                /*(response->params.state == beerocks::ARP_NUD_REACHABLE || response->params.state == beerocks::ARP_NUD_STALE) &&*/
                (response->params().source == beerocks::ARP_SRC_ETH_FRONT ||
                 response->params().source == beerocks::ARP_SRC_WIRELESS_FRONT)) {
                TASK_LOG(DEBUG) << "slave mac " << mac << " has client_mac " << client_mac
                                << " ipv4=" << client_ipv4 << " on front iface";
                int hierarchy = database.get_node_hierarchy(mac);
                if (hierarchy > deepest_slave_hierarchy) {
                    deepest_slave_hierarchy = hierarchy;
                    deepest_slave           = mac;
                    TASK_LOG(DEBUG) << "deepest slave so far: " << deepest_slave
                                    << " hierarchy=" << deepest_slave_hierarchy;
                }
            }
            if (!pending_ires_num) {
                work(); // go to FINISH STATE
            }
        } else {
            TASK_LOG(DEBUG) << "finish task";
            finish();
        }
        break;
    }
    default: {
        TASK_LOG(ERROR) << "Unsupported action_op:" << int(action_op);
        break;
    }
    }
}
