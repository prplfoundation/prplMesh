/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "tlvf/CmduMessageTx.h"
#include "tlvf/CmduMessageRx.h"
#include <cstring>
#include <iostream>

#include "tlvf/ieee_1905_1/tlv1905NeighborDevice.h"
#include "tlvf/ieee_1905_1/tlvLinkMetricQuery.h"
#include "tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h"
#include "tlvf/ieee_1905_1/tlvVendorSpecific.h"
#include "tlvf/wfa_map/tlvApCapability.h"

#include <mapf/common/logger.h>
#include <mapf/common/poller.h>
#include <mapf/local_bus.h>
#include <mapf/transport/ieee1905_transport.h>
#include <tlvf/wfa_map/tlvApCapability.h>

#include <stdio.h>

using namespace ieee1905_1;
using namespace wfa_map;

MAPF_INITIALIZE_LOGGER

using namespace mapf;

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("TLVF example");

    MAPF_INFO("initializing local bus interface");
    LocalBusInterface bus(Context::Instance());
    bus.Init();

    //START BUILDING THE MESSAGE HERE
    CmduTxMessage cmdu_tx_msg;

    //setting metadata, not related to tlvf
    memcpy(cmdu_tx_msg.metadata()->dst, "\x01\x80\xc2\x00\x00\x13", 6);
    cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
    cmdu_tx_msg.metadata()->msg_type   = 0x8019;
    cmdu_tx_msg.metadata()->length =
        256; //buffer size, needs to be bigger for longer messages, otherwise segfault or memory leak might occur
    memset(cmdu_tx_msg.data(), 0x00, cmdu_tx_msg.metadata()->length);

    //creating cmdu message class and setting the header
    CmduMessageTx msg = CmduMessageTx(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length);

    //create method initializes the buffer and returns shared pointer to the message header
    auto header = msg.create(0, eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE);
    header->flags().last_fragment_indicator = 1;
    header->flags().relay_indicator         = 1;

    //NOTE: I used random TLVs for the example, don't expect a standard IEEE1905 message
    MAPF_ERR("CLASS SIZE: " << sizeof(tlvNon1905neighborDeviceList));
    auto firstTlv           = msg.addClass<tlvNon1905neighborDeviceList>();
    bool allocation_succeed = firstTlv->alloc_mac_non_1905_device(
        3); //3 mac addresses for the example, can be any number, only limited by the buffer size
    mapf_assert(
        allocation_succeed ==
        true); //false is returned in case that there isn't enough space on the buffer for the allocation

    auto first_mac = firstTlv->mac_non_1905_device(
        0); //get the first mac address struct in this tlv. returns a <bool,sMacAddress&> collection.
    bool isExist = std::get<0>(
        first_mac); //checking the first parameter, boolean, if the address in this index exists
    if (isExist) {
        auto &address =
            std::get<1>(first_mac); //knowing that the address exists, get the address struct
        address.mac[0] = 0x00;
        address.mac[1] = 0x01;
        address.mac[2] = 0x02;
        address.mac[3] = 0x03;
        address.mac[4] = 0x04;
        address.mac[5] = 0x05;
        MAPF_ERR("WRITE 1 : " << (int)address.mac[3]);
    }

    auto second_mac = firstTlv->mac_non_1905_device(
        1); //get the second mac address struct in this tlv. returns a bool-sMacAddress& collection.
    isExist = std::get<0>(
        second_mac); //checking the first parameter, boolean, if the address in this index exists
    if (isExist) {
        auto &address =
            std::get<1>(second_mac); //knowing that the address exists, get the address struct
        address.mac[0] = 0x05;
        address.mac[1] = 0x05;
        address.mac[2] = 0x05;
        address.mac[3] = 0x05;
        address.mac[4] = 0x05;
        address.mac[5] = 0x05;
        MAPF_ERR("WRITE 2 : " << (int)address.mac[3]);
    }

    auto third_mac = firstTlv->mac_non_1905_device(
        2); //get the third mac address struct in this tlv. returns a bool-sMacAddress& collection.
    isExist = std::get<0>(
        third_mac); //checking the first parameter, boolean, if the address in this index exists
    if (isExist) {
        auto &address =
            std::get<1>(third_mac); //knowing that the address exists, get the address struct
        address.mac[0] = 0xFF;
        address.mac[1] = 0xFF;
        address.mac[2] = 0xFF;
        address.mac[3] = 0xFF;
        address.mac[4] = 0xFF;
        address.mac[5] = 0xFF;

        // Remove "unused variable" warning
        (void)address;
    }

    MAPF_ERR("TLV LENGTH START: " << firstTlv->length());
    auto secondTlv = msg.addClass<tlvLinkMetricQuery>(); // another tlv for the example
    auto metrics   = secondTlv->link_metrics();
    metrics        = tlvLinkMetricQuery::eLinkMetricsType::RX_LINK_METRICS_ONLY;

    // Remove "unused variable" warning
    (void)metrics;

    //MANDATORY - swaps to little indian.
    msg.finalize(true);
    uint8_t recv_buffer[256];
    memcpy(recv_buffer, cmdu_tx_msg.data(), 256);
    MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
    bus.publisher().Send(cmdu_tx_msg);

    MAPF_INFO("DONE");

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, 256, true);

    eTlvType type;
    if (received_message.getNextTlvType(type) &&
        type == eTlvType::TLV_NON_1905_NEIGHBOR_DEVICE_LIST) {
        MAPF_ERR("SUCCESS");
    }

    MAPF_ERR("size: " << received_message.getNextTlvLength());

    auto tlv1 = received_message.addClass<tlvNon1905neighborDeviceList>();
    if (tlv1 != nullptr) {
        MAPF_ERR("LENGTH AFTER INIT: " << tlv1->length());
        //tlv1->alloc_mac_non_1905_device(3);

        auto mac2 = tlv1->mac_non_1905_device(
            2); //get the second mac address struct in this tlv. returns a bool-sMacAddress& collection.
        isExist = std::get<0>(
            mac2); //checking the first parameter, boolean, if the address in this index exists
        if (isExist) {
            auto address =
                std::get<1>(mac2); //knowing that the address exists, get the address struct
                                   /*       address.mac[0] = 0x05;
            address.mac[1] = 0x05;
            address.mac[2] = 0x05;
            address.mac[3] = 0x05;
            address.mac[4] = 0x05;
            address.mac[5] = 0x05;*/

            std::cout << "ADDRESS IS " << (int)address.mac[0] << std::endl;
        } else {
            MAPF_ERR("TLV DOES NOT EXIST");
        }
    } else {
        MAPF_ERR("TLV1 IS NULL");
    }

    if (received_message.getNextTlvType(type) && type == eTlvType::TLV_LINK_METRIC_QUERY) {
        MAPF_ERR("SUCCESS");
    }

    MAPF_ERR("size: " << received_message.getNextTlvLength());

    int invalidBufferSize = 26;
    uint8_t invalidBuffer[invalidBufferSize];
    memcpy(invalidBuffer, recv_buffer, 26);

    CmduMessageRx invmsg;
    auto invheader     = invmsg.parse(invalidBuffer, invalidBufferSize, false);
    if (invheader == nullptr) {
        MAPF_ERR("HEADER PROTECTION SUCCESS");
    }

    if (!received_message.getNextTlvType(type)) {
        MAPF_ERR("TYPE PROTECTION SUCCESS");
    }
    auto invptr = invmsg.addClass<tlv1905NeighborDevice>();
    if (invptr == nullptr) {
        MAPF_ERR("PROTECTION SUCCESS");
    }

    return 0;
}
