/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_wifi_credentials_update_task.h"
#include "../son_actions.h"

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace son;

bml_wifi_credentials_update_task::bml_wifi_credentials_update_task(
    db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_, Socket *sd_,
    const beerocks_message::sWifiCredentials &credentials_)
    : task(), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_), bml_client_socket(sd_),
      credentials(credentials_)
{
}

void bml_wifi_credentials_update_task::work()
{

    if ((state == states::WAIT_FOR_NETWORK_DOWN || state == states::WAIT_FOR_NETWORK_RECOVERY) &&
        (std::chrono::steady_clock::now() > network_recovery_timeout)) {
        LOG(ERROR) << "network recovery timeout error";
        state = states::END;
    }

    switch (state) {
    case states::START: {
        database.assign_bml_config_update_task_id(id);
        LOG(TRACE) << "START case in bml_wifi_credentials_update_task with ID:: " << id;

        backhaul_manager_slaves = database.get_all_backhaul_manager_slaves();
        auto ires               = database.get_all_connected_ires();
        for (auto &ire : ires) {
            network_platforms[ire] = true;
        }
        all_slaves = database.get_active_hostaps();
        all_slaves.insert(backhaul_manager_slaves.begin(), backhaul_manager_slaves.end());

        if (!pending_credentials_requests.empty()) {
            auto pending_credentials = pending_credentials_requests.front();
            pending_credentials_requests.pop();
            credentials       = pending_credentials.cred;
            bml_client_socket = pending_credentials.sd;
        }

        state = states::PREPARE;
        break;
    }

    case states::PREPARE: {
        // sending all slavess the new credentials
        LOG(TRACE) << "PREPARE case in bml_wifi_credentials_update_task";
        for (auto &slave : all_slaves) {
            LOG(TRACE) << "Sending PREPARE request to mac:: " << slave << ", ID: " << id;
            Socket *sd = database.get_node_socket(slave);
            if (sd != nullptr && sd->isOpen()) {
                auto request = message_com::create_vs_message<
                    beerocks_message::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST>(
                    cmdu_tx, id);
                if (request == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return;
                }
                request->params() = credentials;
                son_actions::send_cmdu_to_agent(sd, cmdu_tx, slave);
                add_pending_mac(
                    slave,
                    beerocks_message::ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE);
                LOG(DEBUG) << "send ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST to "
                           << slave << " and added to pending macs";
            } else {
                LOG(WARNING) << "Unable to send message to mac:: " << slave
                             << "  socket is nullptr";
            }
        }

        set_responses_timeout(PREPARE_TIMEOUT_MSC);
        state = states::PRE_COMMIT;
        break;
    }
    case states::PRE_COMMIT: {
        // update credentials in db of all slaves
        LOG(TRACE) << "PRE_COMMIT case in bml_wifi_credentials_update_task";
        timedout_macs.clear();

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST>(cmdu_tx,
                                                                                          id);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }

        for (auto &slave : all_slaves) {

            Socket *ire_socket = database.get_node_socket(slave);

            LOG(DEBUG) << "Sending PRE_COMMIT for the IRE:: " << slave << ", ID: " << id;
            if (ire_socket != nullptr && ire_socket->isOpen()) {
                son_actions::send_cmdu_to_agent(ire_socket, cmdu_tx, slave);
                add_pending_mac(
                    slave,
                    beerocks_message::ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE);
                LOG(DEBUG) << "Request send to " << slave << " and has been added for pending macs";
            } else {
                LOG(WARNING) << "Unable to send message to mac:: " << slave
                             << "  socket is nullptr";
            }
        }
        set_responses_timeout(PRE_COMMIT_TIMEOUT_MSC);
        state = states::COMMIT;
        break;
    }
    case states::COMMIT: {
        // if got here, all slaves returned response to the pre-commit request
        // when receiving commit request, every slave will preform reset and update with the new credentials
        LOG(TRACE) << "COMMIT case in bml_wifi_credentials_update_task";
        timedout_macs.clear();

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST>(cmdu_tx, id);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }

        for (auto &slave : all_slaves) {
            Socket *ire_socket = database.get_node_socket(slave);
            LOG(DEBUG) << "Sending COMMIT for the slave:: " << slave;
            if (ire_socket != nullptr && ire_socket->isOpen()) {
                son_actions::send_cmdu_to_agent(ire_socket, cmdu_tx, slave);
            } else {
                LOG(WARNING) << "Unable to send message to mac:: " << slave
                             << "  socket is nullptr";
            }
        }

        // return bml response ok
        send_bml_response(0);

        network_recovery_timeout =
            std::chrono::steady_clock::now() +
            std::chrono::seconds(database.config.credentials_change_timeout_sec);
        LOG(WARNING) << "set task timeout to " << database.config.credentials_change_timeout_sec
                     << " seconds";
        LOG(DEBUG) << "goto: WAIT_FOR_NETWORK_DOWN";
        state = states::WAIT_FOR_NETWORK_DOWN;
        break;
    }
    case states::WAIT_FOR_NETWORK_DOWN: {

        int non_oper_platform_count = 0;
        for (auto &platform : network_platforms) {
            if (platform.second) { //platform marked as operational on the task
                auto oper_state = database.get_node_operational_state(platform.first);
                if (oper_state == -1) {
                    LOG(ERROR) << "get_node_operational_state() returned error";
                    //TODO: send error code and finish the task
                    send_bml_response(1);
                    state = states::END;
                } else if (!oper_state) {
                    LOG(DEBUG) << "platform " << platform.first << " marked as non operational";
                    platform.second = false;
                    non_oper_platform_count++;
                }
            } else {
                non_oper_platform_count++;
            }
        }

        if (non_oper_platform_count == (int)network_platforms.size()) {
            LOG(DEBUG) << "goto: WAIT_FOR_NETWORK_OPERATIONAL";
            state = states::WAIT_FOR_NETWORK_RECOVERY;
        }
        break;
    }
    case states::WAIT_FOR_NETWORK_RECOVERY: {

        int oper_platform_count = 0;
        for (auto &platform : network_platforms) {
            if (!platform.second) { //platform marked as non operational on the task
                auto oper_state = database.get_node_operational_state(platform.first);
                if (oper_state == -1) {
                    LOG(ERROR) << "get_node_operational_state() returned error";
                    //TODO: send error code and finish the task
                    send_bml_response(1);
                    state = states::END;
                } else if (oper_state) {
                    LOG(DEBUG) << "platform " << platform.first << " marked as operational";
                    platform.second = true;
                    oper_platform_count++;
                }
            } else {
                oper_platform_count++;
            }
        }

        if (oper_platform_count == (int)network_platforms.size()) {
            LOG(DEBUG) << "Network has been successfully recovered!";
            state = states::END;
        }
        break;
    }

    // This state will help to clear the credentials send to IREs so that in future COMMIT message
    // can be verified against blank SSID
    case states::ABORT: {
        LOG(TRACE) << "ABORT case in bml_wifi_credentials_update_task";

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST>(cmdu_tx, id);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }

        std::set<std::string> non_timedout_mac;
        std::set_difference(backhaul_manager_slaves.begin(), backhaul_manager_slaves.end(),
                            timedout_macs.begin(), timedout_macs.end(),
                            std::inserter(timedout_macs, timedout_macs.end()));

        for (auto &slave : non_timedout_mac) {
            Socket *ire_socket = database.get_node_socket(slave);
            if (ire_socket != nullptr && ire_socket->isOpen()) {
                son_actions::send_cmdu_to_agent(ire_socket, cmdu_tx, slave);
            } else {
                LOG(WARNING) << "Unable to send message to mac:: " << slave
                             << "  socket is nullptr";
            }
        }

        state = states::END;
        break;
    }

    case states::END: {
        if (!pending_credentials_requests.empty()) {
            LOG(DEBUG) << "There is a pending credentials change request";
            LOG(DEBUG) << "goto: states::START";
            state = states::START;
        } else {
            LOG(DEBUG) << "Task has been successfully finished!";
            finish();
        }
        break;
    }
    default: {
        LOG(WARNING) << "Default case in bml_wifi_credentials_update_task";
        break;
    }
    }
}

void bml_wifi_credentials_update_task::handle_responses_timeout(
    std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
{
    LOG(INFO) << "bml_wifi_credentials_update_task:handle_messages_timeout Enter";

    //rollback to previous credentials to all non-timed out slaves
    for (auto macs : timed_out_macs) {
        timedout_macs.insert(macs.first);
    }
    send_bml_response(1);

    switch (state) {
    case states::PREPARE: {
        LOG(TRACE) << "PREPARE case in bml_config_update_reponse_timeout";
        break;
    }
    case states::PRE_COMMIT: {
        LOG(TRACE) << "COMMIT case in bml_config_update_reponse_timeout";
        break;
    }
    default: {
        LOG(WARNING) << "Default case in bml_config_update_reponse_timeout";
        break;
    }
    }

    state = states::ABORT;
    return;
}

void bml_wifi_credentials_update_task::handle_event(int event_type, void *obj)
{
    switch (event_type) {
    case NEW_CREDENTIALS_REQUEST: {
        auto event_obj = (new_credential_event_t *)obj;
        TASK_LOG(DEBUG) << "NEW_CREDENTIALS_REQUEST event was received";
        pending_credentials_requests.push(*event_obj);
        break;
    }
    default: {
        TASK_LOG(ERROR) << "UNKNOWN event was received";
        break;
    }
    }
}

// TODO: Use proper error_codes
void bml_wifi_credentials_update_task::send_bml_response(uint8_t error_code)
{
    LOG(TRACE) << "Send ACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE";
    auto response = message_com::create_vs_message<
        beerocks_message::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE>(cmdu_tx);

    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE message!";
        return;
    }
    response->error_code() = error_code;
    if (bml_client_socket != nullptr && bml_client_socket->isOpen()) {
        message_com::send_cmdu(bml_client_socket, cmdu_tx);
    } else {
        LOG(ERROR) << "Unable to release BML socket is nullptr";
    }
    return;
}
