/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

extern "C" {
#pragma GCC diagnostic ignored "-Wattributes"
#include <ugw_msg_api.h>
}

#include "../../../common/utils/utils.h"
#include <bpl/bpl_cfg.h>

#include <mapf/common/logger.h>

#define ERR_UBUS_TIME_OUT -237

namespace beerocks {
namespace bpl {

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void init_msg(Msg_t &xMsg)
{
    // Clear the message
    memset(&xMsg, 0, sizeof(xMsg));

    // Fill in the server and SL names
    utils::copy_string(xMsg.sSerName, "servd", MAX_SERVER_NAME);
    utils::copy_string(xMsg.sSLId, "sl_beerocks", MAX_SL_NAME);

    // OPT_NOTIFY - Notify operation
    // OPT_THREAD_ENV - Consider thread handling in ubus communication
    xMsg.nOperType = OPT_NOTIFY | OPT_THREAD_ENV;
}

static bool send_msg(Msg_t &xMsg)
{
    int nRet = UGW_SUCCESS;

    // Send the message
    nRet = msg_send(&xMsg);
    if (nRet == ERR_UBUS_TIME_OUT) {
        nRet = msg_send(&xMsg);
    }

    if (nRet != UGW_SUCCESS) {
        MAPF_ERR("Failed sending message to sl_beerocks: " << nRet << " For msg_type: "
                                                           << (int)xMsg.nMsgType);

        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool sl_beerocks_notify_fw_mismatch()
{
    Msg_t xMsg;

    // Initialize the message
    init_msg(xMsg);

    // Set the message opcode
    xMsg.nMsgType = BPL_NOTIF_VER_MISMATCH;

    // Since the "msg_send" API requires some data to be sent, we'll just
    // use some dummy data
    uint32_t uiDummyData = 1;

    // Build the message
    xMsg.pMsg     = &uiDummyData;
    xMsg.nMsgSize = sizeof(uint32_t);

    // Send the message
    return send_msg(xMsg);
}

bool sl_beerocks_notify_error(int code, const char *data)
{
    Msg_t xMsg;

    // Initialize the message
    init_msg(xMsg);

    // Set the message opcode
    xMsg.nMsgType = BPL_NOTIF_ERROR;

    // Fill the credentials structure
    BPL_ERROR sError = {0};

    sError.code = code;
    utils::copy_string(sError.data, data, BPL_ERROR_STRING_LEN);

    // Build the message
    xMsg.pMsg     = &sError;
    xMsg.nMsgSize = sizeof(sError);

    // Send the message
    return send_msg(xMsg);
}

bool sl_beerocks_set_wifi_credentials(const int radio_int, const char *ssid, const char *pass,
                                      const char *sec)
{
    Msg_t xMsg;

    // Initialize the message
    init_msg(xMsg);

    // Set the message opcode
    xMsg.nMsgType = BPL_MSG_SET_WIFI_CRED;

    // Fill the credentials structure
    BPL_WIFI_CREDENTIALS sWifiCred = {0};

    utils::copy_string(sWifiCred.ssid, ssid, BPL_SSID_LEN);
    utils::copy_string(sWifiCred.pass, pass, BPL_PASS_LEN);
    utils::copy_string(sWifiCred.sec, sec, BPL_SEC_LEN);

    // Build the message
    xMsg.pMsg     = &sWifiCred;
    xMsg.nMsgSize = sizeof(sWifiCred);

    // Send the message
    return send_msg(xMsg);
}

bool sl_beerocks_set_wifi_iface_state(const char *iface, int op)
{
    Msg_t xMsg;

    MAPF_ERR("sl_beerocks_set_wifi_iface_state iface=" << iface << " op=" << op);

    // Initialize the message
    init_msg(xMsg);

    // Set the message opcode
    xMsg.nMsgType = BPL_MSG_SET_WIFI_IFACE_STATE;

    // Fill the credentials structure
    BPL_WIFI_IFACE_STATE sWifiIfaceState = {0};

    utils::copy_string(sWifiIfaceState.ifname, iface, BPL_IFNAME_LEN);
    sWifiIfaceState.op = op;

    // Build the message
    xMsg.pMsg     = &sWifiIfaceState;
    xMsg.nMsgSize = sizeof(sWifiIfaceState);

    // Send the message
    return send_msg(xMsg);
}

bool sl_beerocks_set_wifi_radio_tx_state(const char *iface, int enable)
{
    Msg_t xMsg;

    MAPF_ERR("sl_beerocks_set_wifi_radio_tx_state iface=" << iface << " enable=" << enable);

    // Initialize the message
    init_msg(xMsg);

    // Set the message opcode
    xMsg.nMsgType = BPL_MSG_SET_WIFI_RADIO_TX_STATE;

    // Fill the credentials structure
    BPL_WIFI_RADIO_TX_STATE sWifiIRadioTxState = {0};

    utils::copy_string(sWifiIRadioTxState.ifname, iface, BPL_IFNAME_LEN);

    sWifiIRadioTxState.enable = enable;

    // Build the message
    xMsg.pMsg     = &sWifiIRadioTxState;
    xMsg.nMsgSize = sizeof(sWifiIRadioTxState);

    // Send the message
    return send_msg(xMsg);
}

bool sl_beerocks_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif)
{
    if (status_notif == nullptr) {
        MAPF_ERR("sl_beerocks_notify_iface_status - Invalid param");
        return false;
    }

    Msg_t xMsg;

    // Initialize the message
    init_msg(xMsg);

    // Set the message status
    xMsg.nMsgType = BPL_MSG_SET_WIFI_INTERFACE_STATUS;

    // Build the message
    xMsg.pMsg     = (void *)status_notif;
    xMsg.nMsgSize = sizeof(BPL_INTERFACE_STATUS_NOTIFICATION);

    // Send the message
    return send_msg(xMsg);
}

bool sl_beerocks_notify_onboarding_completed(const char *ssid, const char *pass, const char *sec,
                                             const char *iface_name, const int success)
{
    Msg_t xMsg;

    // Initialize the message
    init_msg(xMsg);

    // Set the message status
    xMsg.nMsgType = BPL_NOTIF_WPS_COMPLETE;

    // Fill the credentials structure
    struct BPL_NOTIF_WPS_COMPLETE_NOTIFICATION sWpsCompleteNotification = {0};

    utils::copy_string(sWpsCompleteNotification.ssid, ssid, BPL_SSID_LEN);
    utils::copy_string(sWpsCompleteNotification.pass, pass, BPL_PASS_LEN);
    utils::copy_string(sWpsCompleteNotification.sec, sec, BPL_SEC_LEN);
    utils::copy_string(sWpsCompleteNotification.ifname, iface_name, BPL_IFNAME_LEN);
    sWpsCompleteNotification.status = success; //should this be reversed?

    // Build the message
    xMsg.pMsg     = &sWpsCompleteNotification;
    xMsg.nMsgSize = sizeof(sWpsCompleteNotification);

    // Send the message
    return send_msg(xMsg);

    return true;
}

} // namespace bpl
} // namespace beerocks
