/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "rdkb_wlan_task.h"
#include "../../db/db.h"
#include "../../son_actions.h"
#include "rdkb/bml_rdkb_defs.h"

#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <algorithm>
#include <climits>

using namespace beerocks;
using namespace son;

rdkb_wlan_task::rdkb_wlan_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_,
                               task_pool &tasks_)
    : task("rdkb wlan task"), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_)
{
}

void rdkb_wlan_task::work()
{
    if (!database.settings_rdkb_extensions()) {
        LOG(DEBUG) << "RDKB extensions are disabled, killing task";
        finish();
        return;
    }

    switch (state) {
    case START: {
        int prev_task_id = database.get_rdkb_wlan_task_id();
        tasks.kill_task(prev_task_id);
        database.assign_rdkb_wlan_task_id(id);

        state = IDLE;
        break;
    }
    case IDLE: {
        pending_event_check_timeout();
        break;
    }
    }
}

void rdkb_wlan_task::handle_event(int event_type, void *obj)
{
    std::vector<Socket *> events_updates_listeners;
    uint32_t idx = 0;
    Socket *sd;

    while ((sd = get_bml_rdkb_wlan_socket_at(idx)) != nullptr) {
        if (get_bml_rdkb_wlan_events_update_enable(sd)) {
            events_updates_listeners.push_back(sd);
        }
        idx++;
    }

    switch (event_type) {

    case STEERING_EVENT_REGISTER: {
        if (obj) {
            auto event_obj = (listener_general_register_unregister_event *)obj;
            TASK_LOG(DEBUG) << "STEERING_EVENT_REGISTER event was received";
            add_bml_rdkb_wlan_socket(event_obj->sd);
            if (!set_bml_rdkb_wlan_events_update_enable(event_obj->sd, true)) {
                TASK_LOG(ERROR) << "fail in changing events_update registration";
                send_bml_response(event_type, event_obj->sd, -BML_RET_REGISTERTION_FAIL);
                break;
            }
            send_bml_response(event_type, event_obj->sd);
        }
        break;
    }
    case STEERING_EVENT_UNREGISTER: {
        if (obj) {
            auto event_obj = (listener_general_register_unregister_event *)obj;
            TASK_LOG(DEBUG) << "UNREGISTER_TO_MONITOR_EVENT_UPDATES event was received";

            if (!set_bml_rdkb_wlan_events_update_enable(event_obj->sd, false)) {
                TASK_LOG(DEBUG) << "fail in changing stats_update unregistration";
                send_bml_response(event_type, event_obj->sd, -BML_RET_REGISTERTION_FAIL);
                break;
            }
            send_bml_response(event_type, event_obj->sd);
            if (!is_bml_rdkb_wlan_listener_exist()) {
                state = IDLE;
            }
        }
        break;
    }
    case STEERING_SET_GROUP_REQUEST: {
        if (obj) {
            auto event_obj = (rdkb_wlan_task::steering_set_group_request_event *)obj;
            TASK_LOG(INFO) << "STEERING_SET_GROUP_REQUEST event was received - remove - "
                           << int(event_obj->remove) << "group_index "
                           << int(event_obj->steeringGroupIndex);

            if (!event_obj->remove) {
                auto bssid        = net::network_utils::mac_to_string(event_obj->cfg_2.bssid);
                auto radio_mac_2g = database.get_node_parent_radio(bssid);
                bssid             = net::network_utils::mac_to_string(event_obj->cfg_5.bssid);
                auto radio_mac_5g = database.get_node_parent_radio(bssid);
                if (radio_mac_2g.empty() || radio_mac_5g.empty() ||
                    !database.is_node_24ghz(radio_mac_2g) || !database.is_node_5ghz(radio_mac_5g)) {
                    TASK_LOG(ERROR) << "Couldn't find 2.4G or 5G parent node or band mismatch. ";
                    send_bml_response(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd,
                                      -BML_RET_INVALID_ARGS);
                    break;
                }
            }

            beerocks_message::sSteeringApConfig cfg_2, cfg_5;
            int32_t ret;
            if ((ret = steering_group_fill_ap_configuration(event_obj, cfg_2, cfg_5)) < 0) {
                LOG(ERROR) << "STEERING_SET_GROUP_REQUEST Failed to fill ap configuration";
                send_bml_response(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd, ret);
                break;
            }

            rdkb_db.print_db();

            //send new VS message to each slave with his specific data.
            auto radios = database.get_active_hostaps();
            for (const auto &radio_mac : radios) {
                auto update = message_com::create_vs_message<
                    beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST>(cmdu_tx);
                if (update == nullptr) {
                    TASK_LOG(ERROR) << "Failed building message!";
                    send_bml_response(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd,
                                      -BML_RET_CMDU_FAIL);
                    break;
                }
                update->params().remove             = event_obj->remove;
                update->params().steeringGroupIndex = event_obj->steeringGroupIndex;
                if (database.is_node_5ghz(radio_mac)) {
                    update->params().cfg = cfg_5;
                } else { //(database.is_node_2.4ghz(hostap))
                    update->params().cfg = cfg_2;
                }
                auto agent_mac = database.get_node_parent_ire(radio_mac);
                if (!son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac)) {
                    send_bml_response(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd,
                                      -BML_RET_CMDU_FAIL);
                    break;
                }
                TASK_LOG(DEBUG)
                    << "Send  cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST to radio_mac = "
                    << radio_mac;
                //expecting response from monitor and ap_manager
                add_pending_events(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd);
            }
        }
        break;
    }
    case STEERING_SET_GROUP_RESPONSE: {
        if (obj) {
            auto event_obj = (rdkb_wlan_task::steering_set_group_response_event *)obj;
            TASK_LOG(INFO) << "STEERING_SET_GROUP_RESPONSE event was received";
            auto res = check_for_pending_events(event_type);
            if (!res.first && !res.second) {
                TASK_LOG(ERROR) << "unexpected event STEERING_SET_GROUP_RESPONSE ";
                break;
            } else if (!res.first && res.second) {
                TASK_LOG(ERROR) << "timeout for STEERING_SET_GROUP_RESPONSE  event";
                send_bml_response(event_type, res.second, -BML_RET_TIMEOUT);
            } else if (res.first && !res.second) {
                TASK_LOG(INFO) << "waiting to another STEERING_SET_GROUP_RESPONSE event";
                break;
            } else if (event_obj->ret_code < 0) {
                send_bml_response(event_type, res.second, -BML_RET_OP_FAILED);
                break;
            }
            send_bml_response(event_type, res.second);
        }
        break;
    }
    case STEERING_CLIENT_SET_REQUEST: {
        if (obj) {
            auto event_obj  = (rdkb_wlan_task::steering_client_set_request_event *)obj;
            auto client_mac = net::network_utils::mac_to_string(event_obj->client_mac);
            TASK_LOG(INFO) << "STEERING_CLIENT_SET_REQUEST event was received for client_mac "
                           << client_mac << " bssid " << event_obj->bssid;

            auto radio_mac = database.get_node_parent_radio(event_obj->bssid);
            if (radio_mac.empty()) {
                TASK_LOG(INFO) << "Couldn't find radio with bssid " << event_obj->bssid;
                send_bml_response(int(STEERING_CLIENT_SET_RESPONSE), event_obj->sd,
                                  -BML_RET_OP_FAILED);
                break;
            }
            auto update = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST>(cmdu_tx);
            if (update == nullptr) {
                TASK_LOG(ERROR) << "Failed building message!";
                send_bml_response(int(STEERING_CLIENT_SET_RESPONSE), event_obj->sd,
                                  -BML_RET_CMDU_FAIL);
                break;
            }
            auto res =
                event_obj->remove
                    ? rdkb_db.clear_client_config(client_mac, event_obj->bssid,
                                                  event_obj->steeringGroupIndex)
                    : rdkb_db.set_client_config(client_mac, event_obj->bssid,
                                                event_obj->steeringGroupIndex, event_obj->config);
            rdkb_db.print_db();
            if (!res) {
                TASK_LOG(ERROR) << "STEERING_CLIENT_SET_REQUEST db configuration failed";
                send_bml_response(int(STEERING_CLIENT_SET_RESPONSE), event_obj->sd,
                                  -BML_RET_CMDU_FAIL);
                break;
            }

            update->params().remove             = event_obj->remove;
            update->params().steeringGroupIndex = event_obj->steeringGroupIndex;
            update->params().bssid      = net::network_utils::mac_from_string(event_obj->bssid);
            update->params().client_mac = event_obj->client_mac;
            update->params().config     = event_obj->config;

            LOG(DEBUG) << "Sending ACTION_CONTROL_STEERING_CLIENT_SET_REQUEST to radio "
                       << radio_mac;
            auto agent_mac = database.get_node_parent_ire(radio_mac);
            if (!son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac)) {
                send_bml_response(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd,
                                  -BML_RET_CMDU_FAIL);
                break;
            }
            add_pending_events(int(STEERING_CLIENT_SET_RESPONSE), event_obj->sd, 2);
        }
        break;
    }
    case STEERING_CLIENT_SET_RESPONSE: {
        if (obj) {
            auto event_obj = (rdkb_wlan_task::steering_client_set_response_event *)obj;
            TASK_LOG(INFO) << "STEERING_CLIENT_SET_RESPONSE event was received";
            auto res = check_for_pending_events(event_type);
            if (!res.first && !res.second) {
                TASK_LOG(ERROR) << "unexpected event STEERING_CLIENT_SET_RESPONSE ";
                break;
            } else if (!res.first && res.second) {
                send_bml_response(event_type, res.second, -BML_RET_TIMEOUT);
            } else if (res.first && !res.second) {
                TASK_LOG(ERROR) << "not suppose to happen for STEERING_CLIENT_SET_RESPONSE event!!";
                break;
            } else if (event_obj->ret_code < 0) {
                send_bml_response(event_type, res.second, -BML_RET_OP_FAILED);
                break;
            }
            send_bml_response(event_type, res.second);
        }
        break;
    }
    case STEERING_CLIENT_DISCONNECT_REQUEST: {
        if (obj) {
            auto event_obj         = (steering_client_disconnect_request_event *)obj;
            std::string client_mac = net::network_utils::mac_to_string(event_obj->client_mac);
            TASK_LOG(INFO) << "STEERING_CLIENT_DISCONNECT_REQUEST received for " << client_mac;
            son_actions::disconnect_client(database, cmdu_tx, client_mac, event_obj->bssid,
                                           event_obj->type, event_obj->reason);

            add_pending_events(int(STEERING_CLIENT_DISCONNECT_RESPONSE), event_obj->sd);
        }
        break;
    }
    case STEERING_CLIENT_DISCONNECT_RESPONSE: {
        if (obj) {
            auto event_obj = (rdkb_wlan_task::steering_client_disconnect_response_event *)obj;
            TASK_LOG(INFO) << "STEERING_CLIENT_DISCONNECT_RESPONSE event was received";
            auto res = check_for_pending_events(event_type);
            if (!res.first && !res.second) {
                TASK_LOG(ERROR) << "unexpected event STEERING_CLIENT_DISCONNECT_RESPONSE ";
                break;
            } else if (!res.first && res.second) {
                send_bml_response(event_type, res.second, -BML_RET_TIMEOUT);
            } else if (res.first && !res.second) {
                TASK_LOG(ERROR)
                    << "not suppose to happen for STEERING_CLIENT_DISCONNECT_RESPONSE event!!";
                break;
            } else if (event_obj->ret_code < 0) {
                send_bml_response(event_type, res.second, -BML_RET_OP_FAILED);
                break;
            }
            send_bml_response(event_type, res.second);
        }
        break;
    }
    case STEERING_RSSI_MEASUREMENT_REQUEST: {
        if (obj) {
            auto event_obj = (steering_rssi_measurement_request_event *)obj;
            TASK_LOG(INFO) << "STEERING_RSSI_MEASUREMENT_REQUEST event was received for client_mac "
                           << event_obj->params.mac;

            auto update = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(cmdu_tx, id);
            if (update == nullptr) {
                TASK_LOG(ERROR) << "Failed building "
                                   "cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST message!";
                send_bml_response(int(STEERING_RSSI_MEASUREMENT_RESPONSE), event_obj->sd,
                                  -BML_RET_CMDU_FAIL);
                break;
            }

            update->params().channel                = event_obj->params.channel;
            update->params().cross                  = event_obj->params.cross;
            update->params().bandwidth              = event_obj->params.bandwidth;
            update->params().mon_ping_burst_pkt_num = event_obj->params.mon_ping_burst_pkt_num;
            update->params().vht_center_frequency   = event_obj->params.vht_center_frequency;
            update->params().measurement_delay      = event_obj->params.measurement_delay;
            std::copy_n(event_obj->params.mac.oct, sizeof(update->params().mac.oct),
                        update->params().mac.oct);
            std::copy_n(event_obj->params.ipv4.oct, sizeof(update->params().mac.oct),
                        update->params().ipv4.oct);

            auto radio_mac = database.get_node_parent_radio(event_obj->bssid);
            if (radio_mac.empty()) {
                TASK_LOG(ERROR) << "Couldn't find radio with bssid " << event_obj->bssid;
                send_bml_response(int(STEERING_RSSI_MEASUREMENT_RESPONSE), event_obj->sd,
                                  -BML_RET_CLIENT_NOT_FOUND);
                break;
            }

            // check that client exists in DB and connected to provided bssid
            std::string client_mac =
                beerocks::net::network_utils::mac_to_string(event_obj->params.mac);
            auto bssid       = std::string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                send_bml_response(int(STEERING_RSSI_MEASUREMENT_RESPONSE), event_obj->sd,
                                  -BML_RET_CLIENT_NOT_CONFIGURED);
                break;
            }

            if (database.get_node_parent(client_mac) !=
                    event_obj->bssid || // client is not connected to provided bssid
                database.get_node_state(client_mac) == beerocks::STATE_DISCONNECTED) {
                TASK_LOG(ERROR) << "Client " << client_mac << " is not connected to "
                                << event_obj->bssid;
                send_bml_response(int(STEERING_RSSI_MEASUREMENT_RESPONSE), event_obj->sd,
                                  -BML_RET_CLIENT_NOT_CONNECTED);
                break;
            }

            auto agent_mac = database.get_node_parent_ire(radio_mac);
            if (!son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac)) {
                send_bml_response(int(STEERING_SET_GROUP_RESPONSE), event_obj->sd,
                                  -BML_RET_CMDU_FAIL);
                break;
            }

            add_pending_events(int(STEERING_RSSI_MEASUREMENT_RESPONSE), event_obj->sd);
            //TODO - add a proper return code logic to measurement
            auto res = check_for_pending_events(int(STEERING_RSSI_MEASUREMENT_RESPONSE));
            send_bml_response(int(STEERING_RSSI_MEASUREMENT_RESPONSE), res.second);
        }
        break;
    }
    case STEERING_RSSI_MEASUREMENT_RESPONSE: {
        if (obj) {
            TASK_LOG(DEBUG) << "STEERING_RSSI_MEASUREMENT_RESPONSE received";
            //TODO consider add response from monitor.
            break;
        }
    }
    case STEERING_REMOVE_SOCKET: {
        if (obj) {
            auto event_obj = (listener_general_register_unregister_event *)obj;
            if (is_bml_rdkb_wlan_listener_socket(event_obj->sd)) {
                TASK_LOG(DEBUG) << "STEERING_REMOVE_SOCKET event was received";

                if (!set_bml_rdkb_wlan_events_update_enable(event_obj->sd, false)) {
                    TASK_LOG(ERROR) << "fail in set_bml_rdkb_wlan_events_update_enable";
                }

                remove_bml_rdkb_wlan_socket(event_obj->sd);

                if (!is_bml_rdkb_wlan_listener_exist()) {
                    state = IDLE;
                }
            }
        }
        break;
    }
    //events
    case STEERING_EVENT_CLIENT_ACTIVITY_AVAILABLE: {
        if (obj) {
            auto event_obj   = (beerocks_message::sSteeringEvActivity *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->client_mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_CLIENT_ACTIVITY_AVAILABLE client_mac = " << client_mac
                           << " bssid = " << bssid << " group index " << int(group_index);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG) << "STEERING_EVENT_CLIENT_ACTIVITY_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto steering_event_client_activity_availble  = (BML_EVENT_STEERING *)event->data;
            steering_event_client_activity_availble->type = BML_STEERING_EVENT_CLIENT_ACTIVITY;

            steering_event_client_activity_availble->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN,
                        steering_event_client_activity_availble->bssid);
            steering_event_client_activity_availble->timestamp_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            std::copy_n(event_obj->client_mac.oct, BML_MAC_ADDR_LEN,
                        steering_event_client_activity_availble->data.activity.client_mac);
            steering_event_client_activity_availble->data.activity.active = event_obj->active;

            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }
    case STEERING_EVENT_SNR_XING_AVAILABLE: {
        if (obj) {
            auto event_obj   = (beerocks_message::sSteeringEvSnrXing *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->client_mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_SNR_XING_AVAILABLE client_mac = " << client_mac
                           << " bssid = " << bssid << " group index " << int(group_index);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG) << "STEERING_EVENT_SNR_XING_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto steering_event_snr_xing_availble  = (BML_EVENT_STEERING *)event->data;
            steering_event_snr_xing_availble->type = BML_STEERING_EVENT_SNR_XING;

            steering_event_snr_xing_availble->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN,
                        steering_event_snr_xing_availble->bssid);
            steering_event_snr_xing_availble->timestamp_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            std::copy_n(event_obj->client_mac.oct, BML_MAC_ADDR_LEN,
                        steering_event_snr_xing_availble->data.snrXing.client_mac);
            steering_event_snr_xing_availble->data.snrXing.snr = event_obj->snr;
            steering_event_snr_xing_availble->data.snrXing.inactveXing =
                BML_STEERING_SNR_CHANGE(event_obj->inactveXing);
            steering_event_snr_xing_availble->data.snrXing.highXing =
                BML_STEERING_SNR_CHANGE(event_obj->highXing);
            steering_event_snr_xing_availble->data.snrXing.lowXing =
                BML_STEERING_SNR_CHANGE(event_obj->lowXing);
            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }
    case STEERING_EVENT_SNR_AVAILABLE: {
        if (obj) {
            auto event_obj   = (beerocks_message::sSteeringEvSnr *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->client_mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_SNR_AVAILABLE client_mac = " << client_mac
                           << " bssid = " << bssid << " group index " << int(group_index);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG) << "STEERING_EVENT_SNR_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto steering_event_snr_availble  = (BML_EVENT_STEERING *)event->data;
            steering_event_snr_availble->type = BML_STEERING_EVENT_SNR;

            steering_event_snr_availble->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN, steering_event_snr_availble->bssid);
            steering_event_snr_availble->timestamp_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            std::copy_n(event_obj->client_mac.oct, BML_MAC_ADDR_LEN,
                        steering_event_snr_availble->data.snr.client_mac);
            steering_event_snr_availble->data.snr.snr = event_obj->snr;
            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }
    case STEERING_EVENT_PROBE_REQ_AVAILABLE: {
        if (obj) {
            auto event_obj   = (beerocks_message::sSteeringEvProbeReq *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->client_mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_PROBE_REQ_AVAILABLE client_mac = " << client_mac
                           << " bssid = " << bssid << " group index " << int(group_index);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG) << "STEERING_EVENT_PROBE_REQ_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto steering_event_probe_req_availble  = (BML_EVENT_STEERING *)event->data;
            steering_event_probe_req_availble->type = BML_STEERING_EVENT_PROBE_REQ;

            steering_event_probe_req_availble->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN,
                        steering_event_probe_req_availble->bssid);
            steering_event_probe_req_availble->timestamp_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            std::copy_n(event_obj->client_mac.oct, BML_MAC_ADDR_LEN,
                        steering_event_probe_req_availble->data.probeReq.client_mac);
            steering_event_probe_req_availble->data.probeReq.snr       = event_obj->rx_snr;
            steering_event_probe_req_availble->data.probeReq.broadcast = event_obj->broadcast;
            steering_event_probe_req_availble->data.probeReq.blocked   = event_obj->blocked;
            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }
    case STEERING_EVENT_AUTH_FAIL_AVAILABLE: {
        if (obj) {
            auto event_obj   = (beerocks_message::sSteeringEvAuthFail *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->client_mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_AUTH_FAIL_AVAILABLE client_mac = " << client_mac
                           << " bssid = " << bssid << " group index " << int(group_index);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG) << "STEERING_EVENT_AUTH_FAIL_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto steering_event_auth_fail_availble  = (BML_EVENT_STEERING *)event->data;
            steering_event_auth_fail_availble->type = BML_STEERING_EVENT_AUTH_FAIL;

            steering_event_auth_fail_availble->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN,
                        steering_event_auth_fail_availble->bssid);
            steering_event_auth_fail_availble->timestamp_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            std::copy_n(event_obj->client_mac.oct, BML_MAC_ADDR_LEN,
                        steering_event_auth_fail_availble->data.authFail.client_mac);
            steering_event_auth_fail_availble->data.authFail.snr        = event_obj->rx_snr;
            steering_event_auth_fail_availble->data.authFail.reason     = event_obj->reason;
            steering_event_auth_fail_availble->data.authFail.bsBlocked  = event_obj->blocked;
            steering_event_auth_fail_availble->data.authFail.bsRejected = event_obj->reject;
            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }
    case STEERING_EVENT_CLIENT_CONNECT_AVAILABLE: {

        if (obj) {
            auto event_obj   = (beerocks_message::sClientAssociationParams *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_CLIENT_CONNECT_AVAILABLE client_mac = " << client_mac
                           << " bssid = " << bssid << " group index " << int(group_index);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG) << "STEERING_EVENT_CLIENT_CONNECT_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto band_5g     = database.is_node_5ghz(database.get_node_parent_radio(client_mac));
            auto client_caps = database.get_station_capabilities(client_mac, band_5g);

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto connect_event                = (BML_EVENT_STEERING *)event->data;
            connect_event->type               = BML_STEERING_EVENT_CLIENT_CONNECT;
            connect_event->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN, connect_event->bssid);
            connect_event->timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                              std::chrono::steady_clock::now().time_since_epoch())
                                              .count();

            std::copy_n(net::network_utils::mac_from_string(client_mac).oct, BML_MAC_ADDR_LEN,
                        connect_event->data.connect.client_mac);
            connect_event->data.connect.isBTMSupported = client_caps->btm_supported;
            connect_event->data.connect.isRRMSupported = client_caps->rrm_supported;
            connect_event->data.connect.bandCap2G      = client_caps->band_2g_capable;
            connect_event->data.connect.bandCap5G      = client_caps->band_5g_capable;

            connect_event->data.connect.datarateInfo.maxChwidth = client_caps->max_ch_width;
            connect_event->data.connect.datarateInfo.maxStreams = client_caps->max_streams;
            connect_event->data.connect.datarateInfo.phyMode    = client_caps->phy_mode;
            connect_event->data.connect.datarateInfo.maxMCS     = client_caps->max_mcs;
            connect_event->data.connect.datarateInfo.maxTxpower = client_caps->max_tx_power;
            connect_event->data.connect.datarateInfo.isStaticSmps =
                (client_caps->ht_sm_power_save == HT_SM_POWER_SAVE_MODE_STATIC);
            connect_event->data.connect.datarateInfo.isMUMimoSupported =
                client_caps->mumimo_supported;

            connect_event->data.connect.rmCaps.linkMeas      = client_caps->link_meas;
            connect_event->data.connect.rmCaps.neighRpt      = client_caps->nr_enabled;
            connect_event->data.connect.rmCaps.bcnRptPassive = client_caps->beacon_report_passive;
            connect_event->data.connect.rmCaps.bcnRptActive  = client_caps->beacon_report_active;
            connect_event->data.connect.rmCaps.bcnRptTable   = client_caps->beacon_report_table;
            connect_event->data.connect.rmCaps.lciMeas       = client_caps->lci_meas;
            connect_event->data.connect.rmCaps.ftmRangeRpt   = client_caps->fmt_range_report;
            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }
    case STEERING_EVENT_CLIENT_DISCONNECT_AVAILABLE: {
        if (obj) {
            auto event_obj   = (beerocks_message::sSteeringEvDisconnect *)obj;
            auto client_mac  = net::network_utils::mac_to_string(event_obj->client_mac);
            auto bssid       = net::network_utils::mac_to_string(event_obj->bssid);
            auto group_index = rdkb_db.get_group_index(client_mac, bssid);
            TASK_LOG(INFO) << "STEERING_EVENT_CLIENT_DISCONNECT_AVAILABLE client_mac = "
                           << client_mac << " bssid = " << bssid << " group index "
                           << int(group_index) << " reason " << int(event_obj->reason) << " source "
                           << int(event_obj->source) << " type " << int(event_obj->type);

            if (events_updates_listeners.empty()) {
                TASK_LOG(DEBUG)
                    << "STEERING_EVENT_CLIENT_DISCONNECT_AVAILABLE no listener ignoring";
                break;
            }

            if (group_index == -1) {
                TASK_LOG(ERROR) << "event for un-configured client mac - " << client_mac
                                << " ignored";
                break;
            }

            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BML_STEERING_EVENTS_UPDATE>(cmdu_tx);
            if (response == nullptr) {
                TASK_LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENTS_UPDATE message!";
                return;
            }

            if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_STEERING))) {
                TASK_LOG(ERROR) << "Failed to alloc buffer";
                return;
            }

            auto event  = (BML_EVENT *)response->buffer(0);
            event->type = BML_EVENT_TYPE_STEERING;
            auto size   = sizeof(BML_EVENT);
            event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, response->buffer(0), size);

            auto steering_event_client_disconnect_availble  = (BML_EVENT_STEERING *)event->data;
            steering_event_client_disconnect_availble->type = BML_STEERING_EVENT_CLIENT_DISCONNECT;
            steering_event_client_disconnect_availble->steeringGroupIndex = group_index;
            std::copy_n(event_obj->bssid.oct, BML_MAC_ADDR_LEN,
                        steering_event_client_disconnect_availble->bssid);
            steering_event_client_disconnect_availble->timestamp_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();

            std::copy_n(event_obj->client_mac.oct, BML_MAC_ADDR_LEN,
                        steering_event_client_disconnect_availble->data.disconnect.client_mac);
            steering_event_client_disconnect_availble->data.disconnect.reason = event_obj->reason;
            steering_event_client_disconnect_availble->data.disconnect.source =
                BML_DISCONNECT_SOURCE(event_obj->source);
            steering_event_client_disconnect_availble->data.disconnect.type =
                BML_DISCONNECT_TYPE(event_obj->type);
            send_bml_event_to_listeners(cmdu_tx, events_updates_listeners);
        }
        break;
    }

    case STEERING_SLAVE_JOIN: {
        if (!obj) {
            LOG(ERROR) << "STEERING_SLAVE_JOIN without data!";
            break;
        }

        TASK_LOG(INFO) << "STEERING_SLAVE_JOIN event was received";
        auto event_obj = (steering_slave_join_event *)obj;
        if (rdkb_db.get_steering_group_list().empty()) {
            TASK_LOG(INFO) << "no configuration to re-send to agent, radio_mac -"
                           << event_obj->radio_mac;
            break;
        }
        send_steering_conf_to_agent(event_obj->radio_mac);
        break;
    }

    default: {
        TASK_LOG(ERROR) << "UNKNOWN event was received";
        break;
    }
    }
}

bool rdkb_wlan_task::is_bml_rdkb_wlan_listener_exist()
{
    bool listener_exist;
    for (auto it = bml_rdkb_wlan_listeners_sockets.begin();
         it < bml_rdkb_wlan_listeners_sockets.end(); it++) {
        listener_exist = (*it).events_updates;
        if (listener_exist) {
            return true;
        }
    }
    return false;
}

bool rdkb_wlan_task::is_bml_rdkb_wlan_listener_socket(Socket *sd)
{
    if (sd) {
        for (auto it = bml_rdkb_wlan_listeners_sockets.begin();
             it < bml_rdkb_wlan_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return true;
            }
        }
    }
    return false;
}

Socket *rdkb_wlan_task::get_bml_rdkb_wlan_socket_at(uint32_t idx)
{
    if (idx < (bml_rdkb_wlan_listeners_sockets.size())) {
        return bml_rdkb_wlan_listeners_sockets.at(idx).sd;
    }
    return nullptr;
}

bool rdkb_wlan_task::get_bml_rdkb_wlan_events_update_enable(Socket *sd)
{
    if (sd) {
        for (auto it = bml_rdkb_wlan_listeners_sockets.begin();
             it < bml_rdkb_wlan_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return (*it).events_updates;
            }
        }
    }
    return false;
}

bool rdkb_wlan_task::set_bml_rdkb_wlan_events_update_enable(Socket *sd, bool update_enable)
{
    if (sd) {
        for (auto it = bml_rdkb_wlan_listeners_sockets.begin();
             it < bml_rdkb_wlan_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                (*it).events_updates = update_enable;
                return true;
            }
        }
    }
    return false;
}

void rdkb_wlan_task::add_bml_rdkb_wlan_socket(Socket *sd)
{
    if (sd) {
        for (auto it = bml_rdkb_wlan_listeners_sockets.begin();
             it < bml_rdkb_wlan_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                return;
            }
        }
        sBmlRdkbWlanListener bml_rdkb_wlan_listener = {0};
        bml_rdkb_wlan_listener.sd                   = sd;
        bml_rdkb_wlan_listeners_sockets.push_back(bml_rdkb_wlan_listener);
    }
}

void rdkb_wlan_task::remove_bml_rdkb_wlan_socket(Socket *sd)
{
    if (sd) {
        for (auto it = bml_rdkb_wlan_listeners_sockets.begin();
             it < bml_rdkb_wlan_listeners_sockets.end(); it++) {
            if (sd == (*it).sd) {
                bml_rdkb_wlan_listeners_sockets.erase(it);
                return;
            }
        }
    }
}

void rdkb_wlan_task::send_bml_event_to_listeners(ieee1905_1::CmduMessageTx &cmdu_tx,
                                                 std::vector<Socket *> &bml_listeners)
{
    for (auto it = bml_listeners.begin(); it != bml_listeners.end(); ++it) {
        message_com::send_cmdu(*it, cmdu_tx);
    }
}

bool rdkb_wlan_task::send_steering_conf_to_agent(const std::string &radio_mac)
{
    auto agent_mac     = database.get_node_parent_ire(radio_mac);
    auto is_radio_5ghz = database.is_node_5ghz(radio_mac);

    for (const auto &steering_group : rdkb_db.get_steering_group_list()) {
        auto update = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST>(cmdu_tx);
        if (update == nullptr) {
            TASK_LOG(ERROR) << "Failed building message!";
            return false;
        }

        update->params().steeringGroupIndex = steering_group.first;
        auto &client_list                   = is_radio_5ghz
                                ? steering_group.second->get_config_5ghz().get_client_config_list()
                                : steering_group.second->get_config_2ghz().get_client_config_list();
        update->params().cfg = is_radio_5ghz
                                   ? steering_group.second->get_config_5ghz().get_ap_config()
                                   : steering_group.second->get_config_2ghz().get_ap_config();
        auto bssid = is_radio_5ghz ? steering_group.second->get_config_5ghz().bssid
                                   : steering_group.second->get_config_2ghz().bssid;
        TASK_LOG(DEBUG) << "send cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST to agent "
                        << agent_mac << " radio_mac " << radio_mac;
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac);
        //sending client configuration for specifc group
        for (auto client_entry : client_list) {
            auto update = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST>(cmdu_tx);
            if (update == nullptr) {
                TASK_LOG(ERROR) << "Failed building message!";
                return false;
            }

            update->params().steeringGroupIndex = steering_group.first;
            update->params().bssid              = net::network_utils::mac_from_string(bssid);
            update->params().client_mac = net::network_utils::mac_from_string(client_entry.first);
            ;
            update->params().config = *(client_entry.second->get_client_config());
            TASK_LOG(DEBUG) << "send cACTION_CONTROL_STEERING_CLIENT_SET to agent " << agent_mac
                            << " radio_mac " << radio_mac;
            son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac);
        }
    }
    return true;
}

int32_t
rdkb_wlan_task::steering_group_fill_ap_configuration(steering_set_group_request_event *event_obj,
                                                     beerocks_message::sSteeringApConfig &cfg_2,
                                                     beerocks_message::sSteeringApConfig &cfg_5)
{
    if (!event_obj->remove) {
        if (event_obj->cfg_2.inactCheckIntervalSec > event_obj->cfg_2.inactCheckThresholdSec ||
            event_obj->cfg_5.inactCheckIntervalSec > event_obj->cfg_5.inactCheckThresholdSec) {
            TASK_LOG(ERROR) << "STEERING_SET_GROUP_REQUEST inactCheckIntervalSec >= "
                               "inactCheckThresholdSec , invalid configuration";
            return -BML_RET_INVALID_CONFIGURATION;
        }
        rdkb_db.set_steering_group_config(event_obj->steeringGroupIndex, event_obj->cfg_2,
                                          event_obj->cfg_5);
    } else {
        auto group_list = rdkb_db.get_steering_group_list();
        if (group_list.find(event_obj->steeringGroupIndex) == group_list.end()) {
            TASK_LOG(ERROR) << "STEERING_SET_GROUP_REQUEST nothing to remove for groupindex = "
                            << int(event_obj->steeringGroupIndex);
            return -BML_RET_INVALID_CONFIGURATION;
        }
    }

    auto steering_group_config =
        rdkb_db.get_steering_group_list().find(event_obj->steeringGroupIndex)->second;
    cfg_2 = steering_group_config->get_config_2ghz().get_ap_config();
    cfg_5 = steering_group_config->get_config_5ghz().get_ap_config();

    if (event_obj->remove) {
        if (!rdkb_db.clear_steering_group_config(event_obj->steeringGroupIndex)) {
            LOG(ERROR) << "STEERING_SET_GROUP_REQUEST db configuration failed";
            return -BML_RET_INVALID_CONFIGURATION;
        }
    }

    return BML_RET_OK;
}

void rdkb_wlan_task::send_bml_response(int event, Socket *sd, int32_t ret)
{
    switch (event) {
    case STEERING_EVENT_UNREGISTER:
    case STEERING_EVENT_REGISTER: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE "
                          "message!";
            break;
        }

        response->error_code() = ret;

        //send response to bml
        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    case STEERING_SET_GROUP_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_STEERING_SET_GROUP_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_STEERING_SET_GROUP_RESPONSE message!";
            break;
        }
        LOG(DEBUG) << "sent ACTION_BML_STEERING_SET_GROUP_RESPONSE message!" << int(ret);
        response->error_code() = ret;

        //send response to bml
        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    case STEERING_CLIENT_SET_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_STEERING_CLIENT_SET_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_STEERING_SET_GROUP_RESPONSE message!";
            break;
        }

        response->error_code() = ret;

        //send response to bml
        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    case STEERING_CLIENT_DISCONNECT_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE message!";
            break;
        }

        response->error_code() = ret;

        //send response to bml
        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    case STEERING_RSSI_MEASUREMENT_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE message!";
            break;
        }

        response->error_code() = ret;

        //send response to bml
        message_com::send_cmdu(sd, cmdu_tx);
        break;
    }
    default: {
        TASK_LOG(ERROR) << "UNKNOWN event was received, event = " << int(event);
        break;
    }
    }
    return;
}
void rdkb_wlan_task::add_pending_events(int event, Socket *bml_sd, uint32_t amount)
{
    // TODO - fix to use tasks infrastructure WLANRTSYS-TBD
    for (uint32_t i = 0; i < amount; i++) {
        sPendingEvent pending_event_value{};
        pending_event_value.bml_sd  = bml_sd;
        pending_event_value.timeout = std::chrono::steady_clock::now() + std::chrono::seconds(1);
        pending_events.insert(std::make_pair(event, pending_event_value));
    }
}

std::pair<bool, Socket *> rdkb_wlan_task::check_for_pending_events(int event)
{
    // TODO - fix to use tasks infrastructure WLANRTSYS-TBD
    std::pair<bool, Socket *> res;
    auto ret      = pending_events.equal_range(event);
    auto distance = std::distance(ret.first, ret.second);
    TASK_LOG(DEBUG) << "event = " << int(event) << " distance = " << int(distance);
    auto it = ret.first;
    if (distance == 0) {
        res = std::make_pair(false, nullptr);
        return res;
    } else if (it->second.timeout < std::chrono::steady_clock::now()) {
        res = std::make_pair(false, it->second.bml_sd);
        pending_events.clear();
        return res;
    } else if (std::distance(ret.first, ret.second) > 1) {
        pending_events.erase(it);
        res = std::make_pair(true, nullptr);
        return res;
    }
    res = std::make_pair(true, it->second.bml_sd);
    pending_events.clear();
    return res;
}

void rdkb_wlan_task::pending_event_check_timeout()
{
    // TODO - fix to use tasks infrastructure WLANRTSYS-TBD
    if (pending_events.empty()) {
        return;
    }

    auto it =
        std::find_if(pending_events.begin(), pending_events.end(),
                     [&](std::pair<int, sPendingEvent> const &event_element) {
                         return (event_element.second.timeout < std::chrono::steady_clock::now());
                     });

    if (it != pending_events.end()) {
        send_bml_response(it->first, it->second.bml_sd, -BML_RET_TIMEOUT);
        pending_events.clear();
    }
}
