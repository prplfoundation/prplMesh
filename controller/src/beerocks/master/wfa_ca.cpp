/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "wfa_ca.h"

#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>

#include <easylogging++.h>

#include <algorithm>

using namespace beerocks;

std::string wfa_ca::wfa_ca_status_to_string(eWfaCaStatus status)
{
    switch (status) {
    case eWfaCaStatus::RUNNING:
        return "RUNNING";
    case eWfaCaStatus::INVALID:
        return "INVALID";
    case eWfaCaStatus::ERROR:
        return "ERROR";
    case eWfaCaStatus::COMPLETE:
        return "COMPLETE";
    default:
        LOG(ERROR) << "Status argument doesn't have an enum";
        break;
    }
    return std::string();
}

wfa_ca::eWfaCaCommand wfa_ca::wfa_ca_command_from_string(std::string command)
{
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "CA_GET_VERSION") {
        return eWfaCaCommand::CA_GET_VERSION;
    } else if (command == "DEV_CONFIGURE_IE") {
        return eWfaCaCommand::DEV_CONFIGURE_IE;
    } else if (command == "DEV_GET_FRAME_INFO") {
        return eWfaCaCommand::DEV_GET_FRAME_INFO;
    } else if (command == "DEVICE_GET_INFO") {
        return eWfaCaCommand::DEVICE_GET_INFO;
    } else if (command == "DEV_EXEC_ACTION") {
        return eWfaCaCommand::DEV_EXEC_ACTION;
    } else if (command == "DEV_RESET_DEFAULT") {
        return eWfaCaCommand::DEV_RESET_DEFAULT;
    } else if (command == "DEV_SEND_1905") {
        return eWfaCaCommand::DEV_SEND_1905;
    } else if (command == "DEV_SEND_FRAME") {
        return eWfaCaCommand::DEV_SEND_FRAME;
    } else if (command == "DEV_SET_CONFIG") {
        return eWfaCaCommand::DEV_SET_CONFIG;
    }

    return eWfaCaCommand::WFA_CA_COMMAND_MAX;
}

bool wfa_ca::reply_invalid_error(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx,
                                 eWfaCaStatus status, const std::string &description)
{
    if (status != eWfaCaStatus::INVALID || status != eWfaCaStatus::ERROR) {
        LOG(WARNING) << "function misuse, status is not INVALID or ERROR";
        return false;
    }

    std::string reply_string =
        "status," + wfa_ca_status_to_string(status) + ",errorCode," + description;
    return reply(sd, cmdu_tx, reply_string);
}

bool wfa_ca::reply(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx, const std::string &reply_string)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_WFA_CA_CONTROLLER_RESPONSE>(
            cmdu_tx);

    if (!response) {
        LOG(ERROR) << "Failed building cACTION_BML_WFA_CA_CONTROLLER_RESPONSE message!";
        return false;
    }

    if (!response->set_reply(reply_string)) {
        LOG(ERROR) << "setting reply has failed!";
        return false;
    }

    return message_com::send_cmdu(sd, cmdu_tx);
}

void wfa_ca::handle_wfa_ca_message(
    Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
    task_pool &tasks)
{
    auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_WFA_CA_CONTROLLER_REQUEST>();

    if (!request) {
        LOG(ERROR) << "addClass cACTION_BML_WFA_CA_CONTROLLER_REQUEST failed";
        return;
    }

    std::string command(request->command());

    LOG(DEBUG) << "Received ACTION_BML_WFA_CA_CONTROLLER_REQUEST, command=" << command;

    auto cmd_tokens_vec = string_utils::str_split(command, ',');

    if (cmd_tokens_vec.size() < 1) {
        LOG(ERROR) << "Invalid WFA-CA command";
        reply_invalid_error(sd, cmdu_tx, eWfaCaStatus::INVALID, "empty command");
    }

    auto &command_type_str = cmd_tokens_vec[0];
    auto command_type      = wfa_ca_command_from_string(command_type_str);
    cmd_tokens_vec.erase(cmd_tokens_vec.begin()); // erase command token

    switch (command_type) {
    case eWfaCaCommand::CA_GET_VERSION:
        // TODO
        break;
    case eWfaCaCommand::DEV_CONFIGURE_IE:
        // TODO
        break;
    case eWfaCaCommand::DEV_GET_FRAME_INFO:
        // TODO
        break;
    case eWfaCaCommand::DEVICE_GET_INFO:
        // TODO
        break;
    case eWfaCaCommand::DEV_GET_PARAMETER:
        // TODO
        break;
    case eWfaCaCommand::DEV_RESET_DEFAULT:
        // TODO
        break;
    case eWfaCaCommand::DEV_SEND_1905:
        // TODO
        break;
    case eWfaCaCommand::DEV_SEND_FRAME:
        // TODO
        break;
    case eWfaCaCommand::DEV_SET_CONFIG:
        // TODO
        break;
    default:
        auto err_description = "Invalid WFA-CA command type: '" + command_type_str + "'";
        LOG(ERROR) << err_description;
        reply_invalid_error(sd, cmdu_tx, eWfaCaStatus::INVALID, err_description);
        break;
    }
}
