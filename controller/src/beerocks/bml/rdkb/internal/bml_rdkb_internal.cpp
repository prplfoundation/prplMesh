/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_rdkb_internal.h"

#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

#define RESPONSE_TIMEOUT (5000) // 5 seconds

using namespace beerocks;
using namespace net;

int bml_rdkb_internal::steering_set_group(uint32_t steeringGroupIndex,
                                          BML_STEERING_AP_CONFIG *cfg_2,
                                          BML_STEERING_AP_CONFIG *cfg_5)
{
    LOG(DEBUG) << "bml_rdkb_internal::steering_set_group - entry";
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr && !connect_to_master()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<int> prmRdkbWlan;
    m_prmRdkbWlan  = &prmRdkbWlan;
    int iOpTimeout = RESPONSE_TIMEOUT; // Default timeout

    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_STEERING_SET_GROUP_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_AP_SET_CONFIG message!";
        return (-BML_RET_OP_FAILED);
    }

    if (cfg_2 && cfg_5) {
        request->remove() = 0;

        std::copy_n(cfg_2->bssid, sizeof(sMacAddr::oct), request->cfg_2().bssid.oct);
        request->cfg_2().utilCheckIntervalSec   = cfg_2->utilCheckIntervalSec;
        request->cfg_2().utilAvgCount           = cfg_2->utilAvgCount;
        request->cfg_2().inactCheckIntervalSec  = cfg_2->inactCheckIntervalSec;
        request->cfg_2().inactCheckThresholdSec = cfg_2->inactCheckThresholdSec;

        std::copy_n(cfg_5->bssid, sizeof(sMacAddr::oct), request->cfg_5().bssid.oct);
        request->cfg_5().utilCheckIntervalSec   = cfg_5->utilCheckIntervalSec;
        request->cfg_5().utilAvgCount           = cfg_5->utilAvgCount;
        request->cfg_5().inactCheckIntervalSec  = cfg_5->inactCheckIntervalSec;
        request->cfg_5().inactCheckThresholdSec = cfg_5->inactCheckThresholdSec;

    } else if (!cfg_2 && !cfg_5) {
        request->remove() = 1;
    } else {
        LOG(ERROR) << "invalid argument only one cfg_ is null ptr!";
        return (-BML_RET_INVALID_ARGS);
    }

    request->steeringGroupIndex() = steeringGroupIndex;

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending cACTION_BML_AP_SET_CONFIG message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmRdkbWlan.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Get response
    iRet = prmRdkbWlan.get_value();

    m_prmRdkbWlan = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
    }

    return (iRet);
}

int bml_rdkb_internal::steering_client_set(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                           const BML_MAC_ADDR client_mac,
                                           BML_STEERING_CLIENT_CONFIG *config)
{
    LOG(DEBUG) << "bml_rdkb_internal::steering_client_set - entry";

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr && !connect_to_master()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<int> prmRdkbWlan;
    m_prmRdkbWlan  = &prmRdkbWlan;
    int iOpTimeout = RESPONSE_TIMEOUT; // Default timeout

    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_STEERING_CLIENT_SET_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_STEERING_CLIENT_SET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->steeringGroupIndex() = steeringGroupIndex;
    std::copy_n(bssid, sizeof(sMacAddr::oct), request->bssid().oct);
    std::copy_n(client_mac, sizeof(sMacAddr::oct), request->client_mac().oct);
    request->remove() = 1;
    if (config) {
        request->config().snrProbeHWM      = config->snrProbeHWM;
        request->config().snrProbeLWM      = config->snrProbeLWM;
        request->config().snrAuthHWM       = config->snrAuthHWM;
        request->config().snrAuthLWM       = config->snrAuthLWM;
        request->config().snrInactXing     = config->snrInactXing;
        request->config().snrHighXing      = config->snrHighXing;
        request->config().snrLowXing       = config->snrLowXing;
        request->config().authRejectReason = config->authRejectReason;
        request->remove()                  = 0;
    }

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending cACTION_BML_STEERING_CLIENT_SET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmRdkbWlan.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Get response
    iRet = prmRdkbWlan.get_value();

    m_prmRdkbWlan = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
    }

    return (iRet);
}

int bml_rdkb_internal::steering_event_register(BML_EVENT_CB pCB)
{
    LOG(DEBUG) << "bml_rdkb_internal::steering_event_register - entry";

    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr && !connect_to_master()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    if ((m_cbSteeringEvent == nullptr) && (pCB == nullptr)) {
        LOG(WARNING) << "Event callback function was NOT registered...";
        return (-BML_RET_OP_FAILED);
    }

    m_cbSteeringEvent = pCB;

    // Initialize the promise for receiving the response
    beerocks::promise<int> prmRdkbWlan;
    m_prmRdkbWlan  = &prmRdkbWlan;
    int iOpTimeout = RESPONSE_TIMEOUT; // Default timeout

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_STEERING_EVENT_REGISTER message!";
        return (-BML_RET_OP_FAILED);
    }

    if (pCB == nullptr) {
        request->unregister() = 1;
        LOG(DEBUG) << "Steering events unregister";
    } else {
        request->unregister() = 0;
        LOG(DEBUG) << "Steering events register";
    }

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending cACTION_BML_STEERING_EVENT_REGISTER message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmRdkbWlan.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Get response
    iRet = prmRdkbWlan.get_value();

    m_prmRdkbWlan = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
    }

    return (iRet);
}

int bml_rdkb_internal::steering_client_measure(uint32_t steeringGroupIndex,
                                               const BML_MAC_ADDR bssid,
                                               const BML_MAC_ADDR client_mac)
{
    LOG(DEBUG) << "bml_rdkb_internal::steering_client_measure - entry";
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr && !connect_to_master()) {
        return (-BML_RET_CONNECT_FAIL);
    }
    // Initialize the promise for receiving the response
    beerocks::promise<int> prmRdkbWlan;
    m_prmRdkbWlan  = &prmRdkbWlan;
    int iOpTimeout = RESPONSE_TIMEOUT; // Default timeout

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    std::copy_n(client_mac, BML_MAC_ADDR_LEN, request->client_mac().oct);
    request->steeringGroupIndex() = steeringGroupIndex;
    std::copy_n(bssid, sizeof(sMacAddr::oct), request->bssid().oct);

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmRdkbWlan.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Get response
    iRet = prmRdkbWlan.get_value();

    m_prmRdkbWlan = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
    }

    return (iRet);
}

int bml_rdkb_internal::steering_client_disconnect(uint32_t steeringGroupIndex,
                                                  const BML_MAC_ADDR bssid,
                                                  const BML_MAC_ADDR client_mac,
                                                  BML_DISCONNECT_TYPE type, uint32_t reason)
{
    LOG(DEBUG) << "bml_rdkb_internal::steering_client_disconnect - entry";
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr && !connect_to_master()) {
        return (-BML_RET_CONNECT_FAIL);
    }
    // Initialize the promise for receiving the response
    beerocks::promise<int> prmRdkbWlan;
    m_prmRdkbWlan  = &prmRdkbWlan;
    int iOpTimeout = RESPONSE_TIMEOUT; // Default timeout

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    std::copy_n(client_mac, BML_MAC_ADDR_LEN, request->client_mac().oct);
    request->steeringGroupIndex() = steeringGroupIndex;
    std::copy_n(bssid, sizeof(sMacAddr::oct), request->bssid().oct);
    request->type()   = beerocks_message::eDisconnectType(type);
    request->reason() = reason;

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmRdkbWlan.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Get response
    iRet = prmRdkbWlan.get_value();

    m_prmRdkbWlan = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
    }

    return (iRet);
}

bool bml_rdkb_internal::handle_steering_event_update(uint8_t *data_buffer)
{

    BML_EVENT *event = (BML_EVENT *)data_buffer;
    event->ctx       = this;

    //should not happen
    if (!m_cbSteeringEvent) {
        LOG(ERROR) << "steering event arrived although callback not registered ,event - "
                   << event->type;
        return (false);
    }

    auto size = sizeof(BML_EVENT);
    switch (event->type) {
    case BML_EVENT_TYPE_STEERING: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_STEERING, data_buffer, size);
        break;
    }
    default: {
        LOG(ERROR) << "undefined event type: " << event->type;
        return (false);
    }
    }

    m_cbSteeringEvent(event);
    return (true);
}

bool bml_rdkb_internal::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    int ret = process_cmdu_header(beerocks_header->m_header, beerocks_header->m_cmdu_rx);
    if (ret == BML_RET_OP_FAILED) {
        LOG(ERROR) << "bml_rdkb_internal::process_cmdu_header failed !";
    } else if (ret == BML_RET_OP_NOT_SUPPORTED) {
        LOG(ERROR) << "bml_rdkb_internal::process_cmdu_header return code is "
                      "BML_RET_OP_NOT_SUPPORTED, aborting!";
    }
    return (ret == BML_RET_OK);
}

int bml_rdkb_internal::process_cmdu_header(cmdu_vs_action_header_t beerocks_header,
                                           ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto action    = beerocks_header->action();
    auto action_op = beerocks_header->action_op();
    int ret;

    if (action == beerocks_message::ACTION_BML) {
        //uint32_t num_of_nodes;
        // Process BML messages
        switch (action_op) {
        case beerocks_message::ACTION_BML_STEERING_EVENTS_UPDATE: {
            auto response = beerocks_header->addClass<beerocks_message::cACTION_BML_EVENTS_UPDATE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_EVENTS_UPDATE failed";
                return BML_RET_OP_FAILED;
            }

            auto buffer = response->buffer(0);
            if (!buffer) {
                LOG(ERROR) << "get buffer has failed";
                return BML_RET_OP_FAILED;
            }

            handle_steering_event_update((uint8_t *)buffer);
        } break;
        case beerocks_message::ACTION_BML_STEERING_SET_GROUP_RESPONSE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_STEERING_SET_GROUP_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_STEERING_SET_GROUP_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            // Signal any waiting threads
            if (m_prmRdkbWlan) {
                m_prmRdkbWlan->set_value(response->error_code());
                m_prmRdkbWlan = nullptr;
            } else {
                LOG(WARNING) << "Received ACTION_BML_STEERING_SET_GROUP_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_STEERING_CLIENT_SET_RESPONSE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_SET_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_SET_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            // Signal any waiting threads
            if (m_prmRdkbWlan) {
                m_prmRdkbWlan->set_value(response->error_code());
                m_prmRdkbWlan = nullptr;
            } else {
                LOG(WARNING) << "Received ACTION_BML_STEERING_CLIENT_SET_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE: {
            auto response = cmdu_rx.addClass<
                beerocks_message::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE>();
            if (!response) {
                LOG(ERROR)
                    << "addClass cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            // Signal any waiting threads
            if (m_prmRdkbWlan) {
                m_prmRdkbWlan->set_value(response->error_code());
                m_prmRdkbWlan = nullptr;
            } else {
                LOG(WARNING) << "Received ACTION_BML_STEERING_CLIENT_SET_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE: {
            auto response =
                cmdu_rx
                    .addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            // Signal any waiting threads
            if (m_prmRdkbWlan) {
                m_prmRdkbWlan->set_value(response->error_code());
                m_prmRdkbWlan = nullptr;
            } else {
                LOG(WARNING) << "Received ACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            // Signal any waiting threads
            if (m_prmRdkbWlan) {
                m_prmRdkbWlan->set_value(response->error_code());
                m_prmRdkbWlan = nullptr;
            } else {
                LOG(WARNING) << "Received ACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;
        default: {
            if ((ret = bml_internal::process_cmdu_header(beerocks_header->m_header, beerocks_header->m_cmdu_rx)) != BML_RET_OK) {
                LOG(WARNING) << "unhandled extended cmdu header action bml type 0x" << std::hex
                             << action_op;
                return ret;
            }
        }
        }
    } else {
        if ((ret = bml_internal::process_cmdu_header(beerocks_header->m_header, beerocks_header->m_cmdu_rx)) != BML_RET_OK) {
            LOG(WARNING) << "unhandled extended cmdu header action type 0x" << std::hex << action;
            return ret;
        }
    }
    return BML_RET_OK;
}
