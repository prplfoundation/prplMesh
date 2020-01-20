/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "son_actions.h"

#include "db/db_algo.h"
#include "tasks/association_handling_task.h"
#include "tasks/bml_task.h"
#include "tasks/client_steering_task.h"
#include "tasks/ire_network_optimization_task.h"
#include "tasks/optimal_path_task.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_cli.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>

#include "son_master_thread.h"

using namespace beerocks;
using namespace net;
using namespace son;

void son_actions::handle_completed_connection(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                              task_pool &tasks, std::string client_mac)
{
    LOG(INFO) << "handle_completed_connection client_mac=" << client_mac;
    if (!database.set_node_state(client_mac, beerocks::STATE_CONNECTED)) {
        LOG(ERROR) << "set node state failed";
    }

    // update bml listeners
    auto n_type = database.get_node_type(client_mac);
    if (n_type == TYPE_CLIENT) {
        LOG(DEBUG) << "BML, sending connect CONNECTION_CHANGE for mac " << client_mac << " of type "
                   << int(n_type);
        bml_task::connection_change_event new_event;
        new_event.mac = client_mac;
        tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
    }

    auto new_hostap_mac      = database.get_node_parent(client_mac);
    auto previous_hostap_mac = database.get_node_previous_parent(client_mac);
    auto hostaps             = database.get_active_hostaps();

    hostaps.erase(new_hostap_mac); //next operations will be done only on the other APs

    if (database.is_node_wireless(client_mac)) {
        LOG(DEBUG) << "node " << client_mac << " is wireless";
        /*
         * stop monitoring on all other APs
         */
        for (auto &hostap : hostaps) {
            auto agent_mac = database.get_node_parent_ire(hostap);

            LOG(DEBUG) << "STOP_MONITORING_REQUEST hostap_mac=" << hostap << " sta_mac "
                       << client_mac;

            auto stop_request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST>(cmdu_tx);
            if (stop_request == nullptr) {
                LOG(ERROR)
                    << "Failed building ACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST message!";
                return;
            }

            stop_request->mac() = network_utils::mac_from_string(client_mac);
            son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, hostap);
        }

        /*
         * send disassociate request to previous hostap to clear STA mac from its list
         */
        if (!previous_hostap_mac.empty() && previous_hostap_mac != new_hostap_mac) {
            auto previous_agent_mac = database.get_node_parent_ire(previous_hostap_mac);
            auto disassoc_request   = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST>(cmdu_tx);

            if (disassoc_request == nullptr) {
                LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST message!";
                return;
            }
            disassoc_request->mac()    = network_utils::mac_from_string(client_mac);
            disassoc_request->vap_id() = database.get_hostap_vap_id(previous_hostap_mac);
            disassoc_request->type()   = beerocks_message::eDisconnect_Type_Disassoc;

            const auto parent_radio = database.get_node_parent_radio(previous_hostap_mac);
            son_actions::send_cmdu_to_agent(previous_agent_mac, cmdu_tx, database, parent_radio);
            LOG(DEBUG) << "sending DISASSOCIATE request, client " << client_mac << " hostap "
                       << previous_hostap_mac;
        }

        /*
         * launch association handling task for async actions
         * and further handling of the new connection
         */
        auto new_task =
            std::make_shared<association_handling_task>(database, cmdu_tx, tasks, client_mac);
        tasks.add_task(new_task);
    }
}

bool son_actions::add_node_to_default_location(db &database, std::string client_mac)
{
    std::string gw_lan_switch;

    auto gw_container = database.get_nodes_from_hierarchy(0, beerocks::TYPE_GW);
    if (gw_container.empty()) {
        LOG(WARNING)
            << "add_node_to_default_location - can't get GW node, adding to default location...";
    } else {
        auto gw_mac          = *gw_container.begin();
        auto gw_lan_switches = database.get_node_children(gw_mac, beerocks::TYPE_ETH_SWITCH);
        if (gw_lan_switches.empty()) {
            LOG(ERROR) << "add_node_to_default_location - GW has no LAN SWITCH node!";
            return false;
        }
        gw_lan_switch = *gw_lan_switches.begin();
    }

    //LOG(DEBUG) << "adding node " << client_mac << " to db, after getting ARP_MONITOR_NOTIFICATION from source " << int(notification->params.source);
    if (!database.add_node(network_utils::mac_from_string(client_mac),
                           network_utils::mac_from_string(gw_lan_switch))) {
        LOG(ERROR) << "add_node_to_default_location - add_node failed";
        return false;
    }

    if (!database.set_node_state(client_mac, beerocks::STATE_CONNECTING)) {
        LOG(ERROR) << "add_node_to_default_location - set_node_state failed.";
        return false;
    }

    return true;
}

void son_actions::unblock_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::string sta_mac)
{
    LOG(DEBUG) << "unblocking " << sta_mac << " from network";

    auto hostaps        = database.get_active_hostaps();
    auto current_ap_mac = database.get_node_parent(sta_mac);
    for (auto &hostap : hostaps) {
        /*
         * unblock client from all hostaps to prevent it from getting locked out
         */
        if (database.get_hostap_exclude_from_steering_flag(hostap)) {
            LOG(DEBUG) << "hostap " << hostap << " is excluded from steering, skipping";
            continue;
        }
        auto agent_mac = database.get_node_parent_ire(hostap);
        if (!cmdu_tx.create(0,
                            ieee1905_1::eMessageType::CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE)) {
            LOG(ERROR)
                << "cmdu creation of type CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE, has failed";
            break;
        }
        auto association_control_request_tlv =
            cmdu_tx.addClass<wfa_map::tlvClientAssociationControlRequest>();
        if (!association_control_request_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvClientAssociationControlRequest failed";
            break;
        }
        association_control_request_tlv->bssid_to_block_client() =
            network_utils::mac_from_string(current_ap_mac);
        association_control_request_tlv->association_control() =
            wfa_map::tlvClientAssociationControlRequest::UNBLOCK;
        /*
        According to section 11.6 in the Multi-AP Specification
        The Validity Period field in a Client Association Control Request message 
        with Association Control field set to 0x01 (Client Unblocking) is ignored
        */
        association_control_request_tlv->validity_period_sec() = 0;
        association_control_request_tlv->alloc_sta_list();
        auto sta_list         = association_control_request_tlv->sta_list(0);
        std::get<1>(sta_list) = network_utils::mac_from_string(sta_mac);

        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, hostap);
        LOG(DEBUG) << "sending allow request for " << sta_mac << " to " << hostap;
    }
}

int son_actions::steer_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks,
                           std::string sta_mac, std::string chosen_hostap, bool disassoc_imminent,
                           int disassoc_timer_ms, bool steer_restricted)
{
    auto new_task = std::make_shared<client_steering_task>(database, cmdu_tx, tasks, sta_mac,
                                                           chosen_hostap, disassoc_imminent,
                                                           disassoc_timer_ms, steer_restricted);
    tasks.add_task(new_task);
    return new_task->id;
}

bool son_actions::set_hostap_active(db &database, task_pool &tasks, std::string hostap_mac,
                                    bool active)
{
    bool result = database.set_hostap_active(hostap_mac, active);

    if (result) {
        bml_task::connection_change_event new_event;
        new_event.mac   = database.get_node_parent(hostap_mac);
        int bml_task_id = database.get_bml_task_id();
        tasks.push_event(bml_task_id, bml_task::CONNECTION_CHANGE, &new_event);
        LOG(TRACE) << "BML, sending hostap (" << hostap_mac
                   << ") active CONNECTION_CHANGE for IRE mac " << new_event.mac;
    }

    return result;
}

void son_actions::disconnect_client(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                    std::string &client_mac, std::string &bssid, uint32_t type,
                                    uint32_t reason)
{

    auto agent_mac = database.get_node_parent_ire(bssid);

    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST message!";
        return;
    }
    request->mac()    = network_utils::mac_from_string(client_mac);
    request->vap_id() = database.get_hostap_vap_id(bssid);
    request->type()   = type;
    request->reason() = reason;

    const auto parent_radio = database.get_node_parent_radio(bssid);
    son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
    LOG(DEBUG) << "sending DISASSOCIATE request, client " << client_mac << " bssid " << bssid;
}

void son_actions::send_cli_debug_message(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                         std::stringstream &ss)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_RESPONSE_STR>(cmdu_tx);

    if (response == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_RESPONSE_STR message!";
        return;
    }

    //In case we don't have enough space for node length, reserve 1 byte for '\0'
    size_t reserved_size =
        (message_com::get_vs_cmdu_size_on_buffer<beerocks_message::cACTION_CLI_RESPONSE_STR>() - 1);
    size_t max_size = cmdu_tx.getMessageBuffLength() - reserved_size;
    size_t size     = (ss.tellp() > int(max_size)) ? max_size : size_t(ss.tellp());

    if (!response->alloc_buffer(size + 1)) {
        LOG(ERROR) << "Failed buffer allocation";
        return;
    }

    auto buf = response->buffer(0);
    if (!buf) {
        LOG(ERROR) << "Failed buffer allocation";
        return;
    }
    std::copy_n(ss.str().c_str(), size, buf);
    (buf)[size] = 0;

    for (int idx = 0;; idx++) {
        auto sd_ptr = database.get_cli_socket_at(idx);
        if (sd_ptr) {
            message_com::send_cmdu(sd_ptr, cmdu_tx);
        } else {
            break;
        }
    }
}

void son_actions::handle_dead_node(std::string mac, std::string hostap_mac, db &database,
                                   ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks)
{
    int prev_task_id;
    beerocks::eType mac_type      = database.get_node_type(mac);
    std::string parent_hostap_mac = database.get_node_parent(mac);
    auto node_state               = database.get_node_state(mac);

    LOG(DEBUG) << "NOTICE: handling dead node " << mac << " type enum " << int(mac_type)
               << " hostap_mac " << hostap_mac;

    /*
     * stop monitoring if dead node is an IRE or client
     */
    if ((mac_type == beerocks::TYPE_IRE_BACKHAUL || mac_type == beerocks::TYPE_CLIENT) &&
        database.is_node_wireless(mac)) {
        auto agent_mac = database.get_node_parent_ire(hostap_mac);
        LOG(DEBUG) << "STOP_MONITORING mac " << mac << " hostapd " << parent_hostap_mac;
        auto stop_request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST>(cmdu_tx);
        if (stop_request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST message!";
            return;
        }
        stop_request->mac() = network_utils::mac_from_string(mac);

        const auto parent_radio = database.get_node_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
    }
    if (parent_hostap_mac == hostap_mac) {
        if (mac_type == beerocks::TYPE_IRE_BACKHAUL || mac_type == beerocks::TYPE_CLIENT) {
            database.set_node_state(mac, beerocks::STATE_DISCONNECTED);

            // Clear node ipv4
            database.set_node_ipv4(mac);

            // Notify steering task, if any, of disconnect.
            int steering_task = database.get_steering_task_id(mac);
            if (tasks.is_task_running(steering_task))
                tasks.push_event(steering_task, client_steering_task::STA_DISCONNECTED);

            if (database.get_node_handoff_flag(mac)) {
                LOG(DEBUG) << "handoff_flag == true, mac " << mac;
                // We're in the middle of steering, don't mark as disconnected (yet).
                return;
            } else {
                LOG(DEBUG) << "handoff_flag == false, mac " << mac;

                // If we're not in the middle of steering, kill roaming task
                prev_task_id = database.get_roaming_task_id(mac);
                if (tasks.is_task_running(prev_task_id)) {
                    tasks.kill_task(prev_task_id);
                }
            }
        }

        // close slave socket
        if (mac_type == beerocks::TYPE_SLAVE) {
            auto slave_parent = database.get_node_parent(mac);
            LOG(DEBUG) << "slave_parent: " << slave_parent
                       << " slave_parent_type=" << int(database.get_node_type(slave_parent));
            if (database.get_node_type(slave_parent) == beerocks::TYPE_GW) {
                // set platform bridges as non operational
                LOG(DEBUG) << "setting platform with bridge mac " << slave_parent
                           << " as non operational";
                database.set_node_operational_state(slave_parent, false);
            }
            database.set_node_state(mac, beerocks::STATE_DISCONNECTED);
            set_hostap_active(database, tasks, mac, false);
        }

        /*
         * set all nodes in the subtree as disconnected 
         */
        if (mac_type != beerocks::TYPE_CLIENT) {

            auto nodes = database.get_node_subtree(mac);
            for (auto &node_mac : nodes) {
                if (database.get_node_type(node_mac) == beerocks::TYPE_IRE) {
                    // get in here when handling dead node on IRE backhaul
                    // set all platform bridges as non operational
                    LOG(DEBUG) << "setting platform with bridge mac " << node_mac
                               << " as non operational";
                    database.set_node_operational_state(node_mac, false);

                } else if (database.get_node_type(node_mac) == beerocks::TYPE_IRE_BACKHAUL ||
                           database.get_node_type(node_mac) == beerocks::TYPE_CLIENT) {
                    // kill old roaming task
                    prev_task_id = database.get_roaming_task_id(node_mac);
                    if (tasks.is_task_running(prev_task_id)) {
                        tasks.kill_task(prev_task_id);
                    }
                }

                database.set_node_state(node_mac, beerocks::STATE_DISCONNECTED);
                set_hostap_active(database, tasks, node_mac,
                                  false); //implementation checks for hostap node type

                if (database.get_node_type(node_mac) == beerocks::TYPE_IRE ||
                    database.get_node_type(node_mac) == beerocks::TYPE_CLIENT) {
                    bml_task::connection_change_event new_event;
                    new_event.mac = node_mac;
                    tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE,
                                     &new_event);
                    LOG(DEBUG) << "BML, sending client disconnect CONNECTION_CHANGE for mac "
                               << new_event.mac;
                }
            }
        }
    }

    // update bml listeners
    if (node_state == beerocks::STATE_CONNECTED) {
        if (mac_type == beerocks::TYPE_CLIENT) {
            bml_task::connection_change_event new_event;
            new_event.mac = mac;
            tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
            LOG(DEBUG) << "BML, sending client disconnect CONNECTION_CHANGE for mac "
                       << new_event.mac;
        } else if (mac_type == beerocks::TYPE_IRE_BACKHAUL) {
            auto backhauls_bridge = database.get_node_children(mac, beerocks::TYPE_IRE);
            if (backhauls_bridge.empty()) {
                LOG(ERROR) << "backhaul has no bridge node under it!";
            } else {
                for (auto it = backhauls_bridge.begin(); it != backhauls_bridge.end(); it++) {
                    bml_task::connection_change_event new_event;
                    new_event.mac = *it;
                    LOG(DEBUG) << "BML, sending IRE disconnect CONNECTION_CHANGE for mac "
                               << new_event.mac;
                    tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE,
                                     &new_event);
                }
            }
        }
    }

    LOG(DEBUG) << "handling dead node, done for mac " << mac;
}

bool son_actions::validate_beacon_measurement_report(beerocks_message::sBeaconResponse11k report,
                                                     std::string sta_mac, std::string bssid)
{
    return (report.rep_mode == 0) &&
           //      (report.rsni                                  >  0          ) &&
           (report.rcpi < 0) &&
           //      (report.start_time                            >  0          ) &&
           //      (report.duration                              >  0          ) &&
           (report.channel > 0) && (network_utils::mac_to_string(report.sta_mac) == sta_mac) &&
           (network_utils::mac_to_string(report.bssid) == bssid);
}

/**
 * @brief Check if the operating classes of @a radio_basic_caps matches any of the operating classes
 *        in @a bss_info_conf
 *
 * @param radio_basic_caps The AP Radio Basic Capabilities TLV of the radio
 * @param bss_info_conf The BSS Info we try to configure
 * @return true if one of the operating classes overlaps, false if they are disjoint
 */
bool son_actions::has_matching_operating_class(
    wfa_map::tlvApRadioBasicCapabilities &radio_basic_caps,
    const wireless_utils::sBssInfoConf &bss_info_conf)
{
    for (uint8_t i = 0; i < radio_basic_caps.operating_classes_info_list_length(); i++) {
        auto operating_class_info = std::get<1>(radio_basic_caps.operating_classes_info_list(i));
        for (auto operating_class : bss_info_conf.operating_class) {
            if (operating_class == operating_class_info.operating_class()) {
                return true;
            }
        }
    }
    return false;
}

bool son_actions::send_cmdu_to_agent(const std::string &dest_mac,
                                     ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
                                     const std::string &radio_mac)
{
    if (cmdu_tx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
        auto beerocks_header = message_com::get_beerocks_header(cmdu_tx);
        if (!beerocks_header) {
            LOG(ERROR) << "Failed getting beerocks_header!";
            return false;
        }

        beerocks_header->actionhdr()->radio_mac() = network_utils::mac_from_string(radio_mac);
        beerocks_header->actionhdr()->direction() = beerocks::BEEROCKS_DIRECTION_AGENT;
    }

    auto master_thread_ctx = database.get_master_thread_ctx();
    if (master_thread_ctx == nullptr) {
        LOG(ERROR) << "master_thread_context == nullptr";
        return false;
    }

    return master_thread_ctx->send_cmdu_to_bus(cmdu_tx, dest_mac, database.get_local_bridge_mac());
}
