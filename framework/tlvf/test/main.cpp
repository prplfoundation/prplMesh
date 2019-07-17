/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "tlvf/CmduMessageRx.h"
#include "tlvf/CmduMessageTx.h"
#include <cstring>
#include <iostream>

#include "tlvf/ieee_1905_1/tlv1905NeighborDevice.h"
#include "tlvf/ieee_1905_1/tlvLinkMetricQuery.h"
#include "tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h"
#include "tlvf/ieee_1905_1/tlvVendorSpecific.h"
#include "tlvf/ieee_1905_1/tlvWscM2.h"
#include "tlvf/wfa_map/tlvApCapability.h"

#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <tlvf/wfa_map/tlvApCapability.h>

#include <stdio.h>

using namespace ieee1905_1;
using namespace wfa_map;

MAPF_INITIALIZE_LOGGER

using namespace mapf;

int main(int argc, char *argv[])
{
    int errors = 0;
    mapf::Logger::Instance().LoggerInit("TLVF example");
    uint8_t tx_buffer[1000];

    //START BUILDING THE MESSAGE HERE
    memset(tx_buffer, 0, sizeof(tx_buffer));

    //creating cmdu message class and setting the header
    CmduMessageTx msg = CmduMessageTx(tx_buffer, sizeof(tx_buffer));

    //create method initializes the buffer and returns shared pointer to the message header
    auto header = msg.create(0, eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE);
    header->flags().last_fragment_indicator = 1;
    header->flags().relay_indicator         = 1;

    //NOTE: I used random TLVs for the example, don't expect a standard IEEE1905 message
    MAPF_INFO("CLASS SIZE: " << sizeof(tlvNon1905neighborDeviceList));
    auto firstTlv           = msg.addClass<tlvNon1905neighborDeviceList>();
    bool allocation_succeed = firstTlv->alloc_mac_non_1905_device(
        3); //3 mac addresses for the example, can be any number, only limited by the buffer size
    mapf_assert(
        allocation_succeed ==
        true); //false is returned in case that there isn't enough space on the buffer for the allocation

    auto first_mac = firstTlv->mac_non_1905_device(
        0); //get the first mac address struct in this tlv. returns a <bool,sMacAddr&> collection.
    bool isExist = std::get<0>(
        first_mac); //checking the first parameter, boolean, if the address in this index exists
    if (isExist) {
        auto &address =
            std::get<1>(first_mac); //knowing that the address exists, get the address struct
        address.oct[0] = 0x00;
        address.oct[1] = 0x01;
        address.oct[2] = 0x02;
        address.oct[3] = 0x03;
        address.oct[4] = 0x04;
        address.oct[5] = 0x05;
        MAPF_INFO("WRITE 1 : " << (int)address.oct[3]);
    }

    auto second_mac = firstTlv->mac_non_1905_device(
        1); //get the second mac address struct in this tlv. returns a bool-sMacAddr& collection.
    isExist = std::get<0>(
        second_mac); //checking the first parameter, boolean, if the address in this index exists
    if (isExist) {
        auto &address =
            std::get<1>(second_mac); //knowing that the address exists, get the address struct
        address.oct[0] = 0x05;
        address.oct[1] = 0x05;
        address.oct[2] = 0x05;
        address.oct[3] = 0x05;
        address.oct[4] = 0x05;
        address.oct[5] = 0x05;
        MAPF_INFO("WRITE 2 : " << (int)address.oct[3]);
    }

    auto third_mac = firstTlv->mac_non_1905_device(
        2); //get the third mac address struct in this tlv. returns a bool-sMacAddr& collection.
    isExist = std::get<0>(
        third_mac); //checking the first parameter, boolean, if the address in this index exists
    if (isExist) {
        auto &address =
            std::get<1>(third_mac); //knowing that the address exists, get the address struct
        address.oct[0] = 0xFF;
        address.oct[1] = 0xFF;
        address.oct[2] = 0xFF;
        address.oct[3] = 0xFF;
        address.oct[4] = 0xFF;
        address.oct[5] = 0xFF;

        // Remove "unused variable" warning
        (void)address;
    }

    MAPF_INFO("TLV LENGTH START: " << firstTlv->length());
    auto secondTlv            = msg.addClass<tlvLinkMetricQuery>(); // another tlv for the example
    secondTlv->link_metrics() = tlvLinkMetricQuery::eLinkMetricsType::RX_LINK_METRICS_ONLY;

    auto thirdTlv = msg.addClass<tlvWscM2>();
    // All attributes have been initialized to default.
    memcpy(thirdTlv->encrypted_settings_attr().ssid_attr.data, "test", 4);

    //MANDATORY - swaps to little indian.
    msg.finalize(true);

    // Temporary for checking correctness
    std::cout << "TX: " << std::endl;
    for (size_t i = 0; i < msg.getMessageLength(); i++) {
        if (i % 16 == 0)
            std::cout << std::endl;
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)tx_buffer[i] << " ";
    }
    std::cout << std::endl;

    uint8_t recv_buffer[sizeof(tx_buffer)];
    memcpy(recv_buffer, tx_buffer, sizeof(recv_buffer));

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, sizeof(recv_buffer), true);

    eTlvType type;
    if (received_message.getNextTlvType(type) &&
        type == eTlvType::TLV_NON_1905_NEIGHBOR_DEVICE_LIST) {
        MAPF_INFO("SUCCESS");
    }

    MAPF_INFO("size: " << received_message.getNextTlvLength());

    auto tlv1 = received_message.addClass<tlvNon1905neighborDeviceList>();
    if (tlv1 != nullptr) {
        MAPF_INFO("LENGTH AFTER INIT: " << tlv1->length());
        //tlv1->alloc_mac_non_1905_device(3);

        auto mac2 = tlv1->mac_non_1905_device(
            2); //get the second mac address struct in this tlv. returns a bool-sMacAddr& collection.
        isExist = std::get<0>(
            mac2); //checking the first parameter, boolean, if the address in this index exists
        if (isExist) {
            auto address =
                std::get<1>(mac2); //knowing that the address exists, get the address struct
                                   /*       address.oct[0] = 0x05;
            address.oct[1] = 0x05;
            address.oct[2] = 0x05;
            address.oct[3] = 0x05;
            address.oct[4] = 0x05;
            address.oct[5] = 0x05;*/

            std::cout << "ADDRESS IS " << (int)address.oct[0] << std::endl;
        } else {
            MAPF_ERR("TLV DOES NOT EXIST");
            errors++;
        }
    } else {
        MAPF_ERR("TLV1 IS NULL");
        errors++;
    }

    if (received_message.getNextTlvType(type) && type == eTlvType::TLV_LINK_METRIC_QUERY) {
        MAPF_INFO("SUCCESS");
    } else {
        errors++;
    }

    MAPF_INFO("size: " << received_message.getNextTlvLength());

    auto tlv2 = received_message.addClass<tlvLinkMetricQuery>();
    if (tlv2 != nullptr) {
        MAPF_INFO("TLV2 LENGTH AFTER INIT: " << tlv2->length());
    } else {
        MAPF_ERR("TLV2 IS NULL");
        errors++;
    }

    auto tlv3 = received_message.addClass<tlvWscM2>();
    if (tlv3 != nullptr) {
        MAPF_INFO("TLV3 LENGTH AFTER INIT: " << tlv3->length());
    } else {
        MAPF_ERR("TLV3 IS NULL");
        errors++;
    }

    int invalidBufferSize = 26;
    uint8_t invalidBuffer[invalidBufferSize];
    memcpy(invalidBuffer, recv_buffer, 26);

    CmduMessageRx invmsg;
    auto invheader = invmsg.parse(invalidBuffer, invalidBufferSize, false);
    if (invheader == nullptr) {
        MAPF_INFO("HEADER PROTECTION SUCCESS");
    }

    if (!received_message.getNextTlvType(type)) {
        MAPF_INFO("TYPE PROTECTION SUCCESS");
    }
    auto invptr = invmsg.addClass<tlv1905NeighborDevice>();
    if (invptr == nullptr) {
        MAPF_INFO("PROTECTION SUCCESS");
    } else {
        MAPF_ERR("PROTECTION FAILED");
        errors++;
    }

    return errors;
}
