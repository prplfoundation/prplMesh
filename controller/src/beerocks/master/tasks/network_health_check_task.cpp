/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "network_health_check_task.h"
#include "../db/db_algo.h"
#include "../son_actions.h"
#include "bml_task.h"

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

network_health_check_task::network_health_check_task(db &database_,
                                                     ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                     task_pool &tasks_, int starting_delay_ms_,
                                                     std::string task_name_)
    : task(task_name_), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_),
      starting_delay_ms(starting_delay_ms_)
{
}
void network_health_check_task::work()
{
    switch (state) {
    case START: {
        if (starting_delay_ms) {
            wait_for(starting_delay_ms);
            starting_delay_ms = 0;
            break;
        }
        if (suspected_dis_clients.empty()) {
            TASK_LOG(DEBUG) << "suspected_dis_clients.empty() - wait for 60 sec";
            wait_for(task_timeout_ms);
        }
        state = IRE_HEALTH_CHECK;
        break;
    }
    case IRE_HEALTH_CHECK: {
        auto ires = database.get_all_connected_ires();
        for (auto &ire : ires) {
            if (database.get_node_type(ire) == beerocks::TYPE_GW) {
                continue;
            }
            auto backhaul_manager_hostap = database.get_hostap_backhaul_manager(ire);
            if (backhaul_manager_hostap.empty()) {
                TASK_LOG(DEBUG) << "backhaul_manager_hostap.empty()!";
                continue;
            }

            auto last_seen = database.get_node_last_seen(backhaul_manager_hostap);
            auto now       = std::chrono::steady_clock::now();
            auto last_seen_delta =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - last_seen).count();
            if (last_seen_delta > ire_last_seen_timeout_ms) {
                auto backhaul        = database.get_node_parent(ire);
                auto backhaul_parent = database.get_node_parent(backhaul);
                TASK_LOG(DEBUG) << "handle_dead_node ire(backhaul) = " << ire
                                << " hostap = " << backhaul_manager_hostap
                                << " backhaul = " << backhaul
                                << " last_seen_delta=" << int(last_seen_delta);
                son_actions::handle_dead_node(backhaul, backhaul_parent, database, cmdu_tx, tasks);
            }
        }
        state = CLIENT_HEALTH_CHECK;
        break;
    }

    case CLIENT_HEALTH_CHECK: {
        auto clients = database.get_nodes(beerocks::TYPE_CLIENT);
        for (auto &client : clients) {
            auto last_seen = database.get_node_last_seen(client);
            if (!database.is_node_wireless(client) &&
                (database.get_node_state(client) == beerocks::STATE_CONNECTED)) {
                auto now = std::chrono::steady_clock::now();
                auto last_seen_delta =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - last_seen).count();
                if (last_seen_delta > (client_last_seen_timeout_ms) &&
                    (suspected_dis_clients.find(client) == suspected_dis_clients.end())) {
                    suspected_dis_clients.insert(client);
                    //TASK_LOG(DEBUG) << "insert client = " << client << " last_seen_delta = " << int(last_seen_delta);
                }
            }
        }
        state = SEND_QUERY;
        break;
    }
    case SEND_QUERY: {
        if (!(suspected_dis_clients.empty()) && pending_node.empty()) {
            for (auto it = suspected_dis_clients.begin(); it != suspected_dis_clients.end();) {
                //send query messages to client.
                //TASK_LOG(DEBUG) << "send query to client - mac = " << *it;
                if (!send_arp_query(*it)) {
                    TASK_LOG(DEBUG) << "failed to send query on client " << *it << " erased client";
                    it = suspected_dis_clients.erase(it);
                    pending_node.clear();
                } else {
                    ++it;
                }
            }
        }
        state = START;
        break;
    }
    }
}

void network_health_check_task::handle_response(std::string mac,
                                                std::shared_ptr<beerocks_header> beerocks_header)
{
    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CONTROL_ARP_QUERY_RESPONSE: {
        auto response =
            beerocks_header->getClass<beerocks_message::cACTION_CONTROL_ARP_QUERY_RESPONSE>();
        if (!response) {
            TASK_LOG(ERROR) << "getClass failed for cACTION_CONTROL_ARP_QUERY_RESPONSE";
            return;
        }

        std::string ipv4    = network_utils::ipv4_to_string(response->params().ipv4);
        std::string arp_mac = network_utils::mac_to_string(response->params().mac);

        TASK_LOG(DEBUG) << "received response from slave " << mac << ":" << std::endl
                        << "   arp_mac=" << arp_mac << std::endl
                        << "   arp_ipv4=" << ipv4 << std::endl
                        << "   arp_state=" << int(response->params().state)
                        << " arp_source=" << int(response->params().source);

        database.update_node_last_seen(arp_mac);

        if (suspected_dis_clients.find(arp_mac) != suspected_dis_clients.end()) {
            TASK_LOG(DEBUG) << "arp_mac = " << arp_mac
                            << " is alive!! erasing from ires/clients set";
            suspected_dis_clients.erase(arp_mac);
            TASK_LOG(DEBUG) << "suspected_dis_clients.erase " << arp_mac;
            pending_node.clear();
        } else {
            TASK_LOG(DEBUG) << "arp_mac = " << arp_mac
                            << " not found!! on IRE's/CLIENT's set , not supposed to get here!! ";
        }
        break;
    }
    default: {
        TASK_LOG(ERROR) << "Unsupported action_op:" << int(beerocks_header->action_op());
        break;
    }
    }
}

bool network_health_check_task::send_arp_query(std::string mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CONTROL_ARP_QUERY_REQUEST>(cmdu_tx,
                                                                                            id);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    request->params().mac  = network_utils::mac_from_string(mac);
    auto ipv4              = database.get_node_ipv4(mac);
    request->params().ipv4 = network_utils::ipv4_from_string(ipv4);

    const auto parent_radio = database.get_node_parent_radio(mac);

    auto agent_mac = database.get_node_parent_ire(parent_radio);

    if (database.get_node_state(parent_radio) != beerocks::STATE_CONNECTED) {
        LOG(WARNING) << "parent_mac not connected , parent_mac = " << parent_radio;
        return false;
    }

    if (!son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio)) {
        LOG(ERROR) << "send_message failed - parent_mac " << parent_radio << " client " << mac;
        return false;
    }

    add_pending_mac(parent_radio, beerocks_message::ACTION_CONTROL_ARP_QUERY_RESPONSE);
    pending_node = mac;
    set_responses_timeout(5 * 1000);
    TASK_LOG(DEBUG) << "(parent_mac= " << parent_radio << ") for client_mac=" << mac
                    << " ipv4=" << ipv4;
    return true;
}

void network_health_check_task::handle_responses_timeout(
    std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
{
    LOG(WARNING) << "handle_responses_timeout";
    for (auto entry : timed_out_macs) {
        std::string mac = entry.first;
        TASK_LOG(DEBUG) << "response from " << mac << " timed out";

        if (!suspected_dis_clients.empty()) {
            if (suspected_dis_clients.find(pending_node) != suspected_dis_clients.end()) {
                suspected_dis_clients.erase(pending_node);
                //TASK_LOG(DEBUG) << "suspected_dis_clients.erase " << pending_node;
                auto eth_switch = database.get_node_parent(pending_node);
                LOG(WARNING) << "CLIENT is not responding!! handle dead client mac = "
                             << pending_node;
                son_actions::handle_dead_node(pending_node, eth_switch, database, cmdu_tx, tasks);
                pending_node.clear();
                return;
            }
        }
    }
}
