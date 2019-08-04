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
#include <sstream>

#include "tlvf/ieee_1905_1/tlv1905NeighborDevice.h"
#include "tlvf/ieee_1905_1/tlvLinkMetricQuery.h"
#include "tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h"
#include "tlvf/ieee_1905_1/tlvVendorSpecific.h"
#include "tlvf/ieee_1905_1/tlvWscM2.h"
#include "tlvf/ieee_1905_1/tlvMacAddress.h"
#include "tlvf/wfa_map/tlvApCapability.h"
#include <tlvf/test/tlvVarList.h>

#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <tlvf/wfa_map/tlvApCapability.h>

#include <stdio.h>
#include <algorithm>
#include <iterator>

using namespace ieee1905_1;
using namespace wfa_map;

MAPF_INITIALIZE_LOGGER

using namespace mapf;

void dump_buffer(uint8_t *buffer, size_t len)
{
    for (size_t i = 0; i < len; i += 16) {
        std::ostringstream hexdump;
        for (size_t j = i; j < len && j < i + 16; j++)
            hexdump << std::hex << std::setw(2) << std::setfill('0') << (unsigned)buffer[j]
                    << " ";
        LOG(INFO) << hexdump.str();
    }
}

int test_int_len_list()
{
    int errors                     = 0;
    uint8_t tx_buffer[4096];
    const uint8_t gTlvMacAddress[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    MAPF_INFO(__FUNCTION__ << " start");
    memset(tx_buffer, 0, sizeof(tx_buffer));
    CmduMessageTx msg = CmduMessageTx(tx_buffer, sizeof(tx_buffer));
    auto header = msg.create(0, eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE); //DUMMY Message type
    header->flags().last_fragment_indicator = 1;

    {
        auto tlv = msg.addClass<tlvMacAddress>();
        auto mac = &std::get<1>(tlv->mac(0));
        std::copy_n(gTlvMacAddress, 6, mac);
    }

    msg.finalize(true);
    LOG(INFO) << "TX Buffer (after finalize):";
    dump_buffer(tx_buffer, msg.getMessageLength());

    uint8_t recv_buffer[sizeof(tx_buffer)];
    memcpy(recv_buffer, tx_buffer, sizeof(recv_buffer));

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, sizeof(recv_buffer), true);

    {
        auto tlv = received_message.addClass<tlvMacAddress>();
        if (tlv == nullptr) {
            MAPF_ERR("TLV is NULL");
            errors++;
        }
        auto mac = &std::get<1>(tlv->mac(0));
        if (!std::equal(mac, mac + 6, gTlvMacAddress)) {
            MAPF_ERR("MAC address in received TLV does not match expected result");
            errors++;
        }
    }
    
    MAPF_INFO(__FUNCTION__ << " Finished, errors = " << errors << std::endl);
    return errors;
}

int test_complex_list()
{
    int errors                     = 0;
    const int complex_list_entries = 3;
    uint8_t tx_buffer[4096];

    MAPF_INFO(__FUNCTION__ << " start");

    //START BUILDING THE MESSAGE HERE
    memset(tx_buffer, 0, sizeof(tx_buffer));

    //creating cmdu message class and setting the header
    CmduMessageTx msg = CmduMessageTx(tx_buffer, sizeof(tx_buffer));
    //create method initializes the buffer and returns shared pointer to the message header
    auto header = msg.create(0, eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE);
    header->flags().last_fragment_indicator = 1;
    header->flags().relay_indicator         = 1;

    auto fourthTlv    = msg.addClass<tlvTestVarList>();
    fourthTlv->var0() = 0xa0;
    if (!fourthTlv->alloc_simple_list(2)) {
        MAPF_ERR("Failed to allocate simple list");
        ++errors;
    }
    std::get<1>(fourthTlv->simple_list(0)) = 0x0bb0;
    std::get<1>(fourthTlv->simple_list(1)) = 0x0bb1;

    for (int i = 0; i < complex_list_entries; i++) {
        auto cmplx    = fourthTlv->create_complex_list();
        cmplx->var1() = 0xbbbbaaaa;
        cmplx->alloc_list(3);
        std::get<1>(cmplx->list(0)) = 0xc0;
        std::get<1>(cmplx->list(1)) = 0xc1;
        std::get<1>(cmplx->list(2)) = 0xc2;
        if (!fourthTlv->add_complex_list(cmplx)) {
            LOG(ERROR) << "Failed to add complex list";
            ++errors;
        }
        //test multiple add (should Fail!)
        if (fourthTlv->add_complex_list(cmplx)) {
            LOG(ERROR) << "Could add complex list a second time";
            ++errors;
        }
    }

    auto cmplx    = fourthTlv->create_var1();
    cmplx->var1() = 0xb11b;
    if (!fourthTlv->add_var1(cmplx)) {
        LOG(ERROR) << "Failed to add var1";
        ++errors;
    }
    // Test multiple add - should fail
    if (fourthTlv->add_var1(cmplx)) {
        LOG(ERROR) << "Could add var1 a second time";
        errors++;
    }
    LOG(INFO) << "TLV 4 length " << fourthTlv->length();

    LOG(INFO) << "Finalize";
    //MANDATORY - swaps to little indian.
    msg.finalize(true);

    LOG(INFO) << "TX:";
    for (size_t i = 0; i < msg.getMessageLength(); i += 16) {
        std::ostringstream hexdump;
        for (size_t j = i; j < msg.getMessageLength() && j < i + 16; j++)
            hexdump << std::hex << std::setw(2) << std::setfill('0') << (unsigned)tx_buffer[j]
                    << " ";
        LOG(INFO) << hexdump.str();
    }

    uint8_t recv_buffer[sizeof(tx_buffer)];
    memcpy(recv_buffer, tx_buffer, sizeof(recv_buffer));

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, sizeof(recv_buffer), true);

    auto tlv4 = received_message.addClass<tlvTestVarList>();
    if (tlv4 == nullptr) {
        MAPF_ERR("TLV4 is NULL");
        return ++errors;
    }
    if (tlv4->complex_list_length() != complex_list_entries) {
        MAPF_ERR("Invalid complex list num of entries" << tlv4->complex_list_length());
        return ++errors;
    }
    for (int i = 0; i < complex_list_entries; i++) {
        auto cmplx = std::get<1>(tlv4->complex_list(i));
        if (cmplx.var1() != 0xbbbbaaaa) {
            MAPF_ERR("wrong value for cmplx->var1 " << std::hex << cmplx.var1());
            ++errors;
        }
    }

    if (tlv4->var1()->var1() != 0xb11b) {
        MAPF_ERR("Unexpected var1 value" << tlv4->var1()->var1());
        ++errors;
    }

    MAPF_INFO(__FUNCTION__ << " Finished, errors = " << errors << std::endl);
    return errors;
}

int test_all()
{
    int errors = 0;
    uint8_t tx_buffer[4096];

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

    LOG(DEBUG) << "Start WSC M2";
    auto thirdTlv = msg.addClass<tlvWscM2>();

    thirdTlv->message_type_attr().data = WSC::WSC_MSG_TYPE_M2;
    if (!thirdTlv->set_manufacturer("Intel"))
        return false;

    // All attributes have been initialized to default.
    LOG(DEBUG) << "Create WSC M2 encrypted settings";
    auto encrypted_settings_tx = thirdTlv->create_encrypted_settings();
    if (!encrypted_settings_tx) {
        LOG(ERROR) << "Failed to create encrypted settings";
        return false;
    }

    encrypted_settings_tx->set_ssid("test_ssid");
    encrypted_settings_tx->authentication_type_attr().data = WSC::WSC_AUTH_WPA2; //DUMMY
    encrypted_settings_tx->encryption_type_attr().data     = WSC::WSC_ENCR_AES;
    std::fill(encrypted_settings_tx->network_key_attr().data,
              encrypted_settings_tx->network_key_attr().data +
                  encrypted_settings_tx->network_key_attr().data_length,
              0xaa); //DUMMY
    std::fill(encrypted_settings_tx->key_wrap_auth_attr().data,
              encrypted_settings_tx->key_wrap_auth_attr().data +
                  encrypted_settings_tx->key_wrap_auth_attr().data_length,
              0xff); //DUMMY
    LOG(DEBUG) << "WSC encrypted_settings_tx:" << std::endl
               << "     ssid: " << encrypted_settings_tx->ssid() << std::endl
               << "     authentication_type: "
               << encrypted_settings_tx->authentication_type_attr().data << std::endl
               << "     encryption_type: " << encrypted_settings_tx->encryption_type_attr().data
               << std::endl;
    LOG(DEBUG) << "Add WSC M2 encrypted settings";
    if (!thirdTlv->add_encrypted_settings(encrypted_settings_tx)) {
        LOG(ERROR) << "Failed to add encrypted settings";
        return false;
    }
    LOG(DEBUG) << "Done (WSC M2)";
    MAPF_INFO("TLV LENGTH WSC M2: " << thirdTlv->length());

    auto fourthTlv     = msg.addClass<tlvTestVarList>();
    fourthTlv->var0()  = 0xa0;
    allocation_succeed = fourthTlv->alloc_simple_list(2);
    mapf_assert(allocation_succeed);
    std::get<1>(fourthTlv->simple_list(0)) = 0x0bb0;
    std::get<1>(fourthTlv->simple_list(1)) = 0x0bb1;

    auto cmplx = fourthTlv->create_complex_list();
    cmplx->alloc_list(3);
    std::get<1>(cmplx->list(0)) = 0xc0;
    std::get<1>(cmplx->list(1)) = 0xc1;
    std::get<1>(cmplx->list(2)) = 0xc2;
    cmplx->var1()               = 0xd00d;
    if (!fourthTlv->add_complex_list(cmplx)) {
        LOG(ERROR) << "Failed to add complex list";
        errors++;
    }

    // TODO the complex list doesn't work at the moment if it has more than one element
    // Cfr. #137
    //    cmplx         = fourthTlv->create_complex_list();
    //    cmplx->var1() = 0xd11d;
    //    if (!fourthTlv->add_complex_list(cmplx)) {
    //        LOG(ERROR) << "Failed to add complex list";
    //        errors++;
    //    }
    //    if (fourthTlv->add_complex_list(cmplx)) {
    //        LOG(ERROR) << "Could add complex list a second time";
    //        errors++;
    //    }

    cmplx         = fourthTlv->create_var1();
    cmplx->var1() = 0xeeee;
    if (!fourthTlv->add_var1(cmplx)) {
        LOG(ERROR) << "Failed to add var1";
        errors++;
    }
    if (fourthTlv->add_var1(cmplx)) {
        LOG(ERROR) << "Could add var1 a second time";
        errors++;
    }

    LOG(INFO) << "TLV 4 length " << fourthTlv->length();
    auto unknown = fourthTlv->create_unknown_length_list();
    unknown->alloc_unknown_length_list(10);
    fourthTlv->add_unknown_length_list(unknown);
    LOG(INFO) << "Unknown list size: " << unknown->getLen();
    LOG(INFO) << "Total unknown Length: " << fourthTlv->unknown_length_list_length();
    LOG(INFO) << "TLV 4 length " << fourthTlv->length();

    LOG(INFO) << "Finalize";
    //MANDATORY - swaps to little indian.
    msg.finalize(true);

    LOG(INFO) << "TX:";
    for (size_t i = 0; i < msg.getMessageLength(); i += 16) {
        std::ostringstream hexdump;
        for (size_t j = i; j < msg.getMessageLength() && j < i + 16; j++)
            hexdump << std::hex << std::setw(2) << std::setfill('0') << (unsigned)tx_buffer[j]
                    << " ";
        LOG(INFO) << hexdump.str();
    }

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

            LOG(INFO) << "ADDRESS IS " << (int)address.oct[0];
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

    if (received_message.getNextTlvType(type) && type == eTlvType::TLV_WSC) {
        MAPF_INFO("SUCCESS");
    } else {
        errors++;
    }

    auto tlv3 = received_message.addClass<tlvWscM2>();
    if (tlv3 != nullptr) {
        MAPF_INFO("TLV3 LENGTH AFTER INIT: " << tlv3->length());
    } else {
        MAPF_ERR("TLV3 IS NULL");
        errors++;
    }
    auto encrypted_settings_rx = tlv3->encrypted_settings();
    auto ssid = std::string(encrypted_settings_rx->ssid(), encrypted_settings_rx->ssid_length());
    auto authentication_type = encrypted_settings_rx->authentication_type_attr().data;
    auto encryption_type     = encrypted_settings_rx->encryption_type_attr().data;
    LOG(DEBUG) << "WSC encrypted_settings_rx:" << std::endl
               << "     ssid: " << ssid << std::endl
               << "     authentication_type: " << authentication_type << std::endl
               << "     encryption_type: " << encryption_type << std::endl;

    auto tlv4 = received_message.addClass<tlvTestVarList>();
    if (tlv4 == nullptr) {
        MAPF_ERR("TLV4 is NULL");
        errors++;
    } else {
        if (tlv4->var0() != 0xa0) {
            MAPF_ERR("TLV4 var0 is 0x" << std::hex << tlv4->var0() << " instead of 0xa0");
            errors++;
        }

        if (tlv4->simple_list_length() != 2) {
            MAPF_ERR("TLV4 simple list length is " << unsigned(tlv4->simple_list_length())
                                                   << " instead of 2");
            errors++;
        }
        for (uint8_t list_idx = 0; list_idx < tlv4->simple_list_length(); list_idx++) {
            uint16_t expected = 0x0bb0;
            if (!std::get<0>(tlv4->simple_list(list_idx))) {
                MAPF_ERR("TLV4 has no simple " << list_idx);
                errors++;
            } else {
                auto value = std::get<1>(tlv4->simple_list(list_idx));
                if (value != expected + list_idx) {
                    MAPF_ERR("TLV4 simple ") << list_idx << " has value " << std::hex << value
                                             << " instead of " << std::hex << expected + list_idx;
                    errors++;
                }
            }
        }

        LOG(INFO) << "Total unknown Length: " << tlv4->unknown_length_list_length();
        LOG(INFO) << "TLV 4 length " << tlv4->length();
        // TODO the complex list doesn't work at the moment if it has more than one element
        // Cfr. #137
        //        if (tlv4->complex_list_length() != 2) {
        //            MAPF_ERR("TLV4 complex list length is " << unsigned(tlv4->complex_list_length())
        //                                                    << " instead of 2");
        //            errors++;
        //        }
        //        if (!std::get<0>(tlv4->complex_list(0))) {
        //            MAPF_ERR("TLV4 has no complex 0");
        //            errors++;
        //        } else {
        //            auto cmplx = std::get<1>(tlv4->complex_list(0));
        //            if (cmplx.list_length() != 3) {
        //                MAPF_ERR("TLV4 complex 0 list length is " << unsigned(cmplx.list_length())
        //                                                          << " instead of 3");
        //                errors++;
        //            }
        //            uint8_t expected = 0xc0;
        //            for (uint8_t list_idx = 0; list_idx < cmplx.list_length(); list_idx++) {
        //                if (!std::get<0>(cmplx.list(list_idx))) {
        //                    MAPF_ERR("TLV4 complex 0 has no list[" << list_idx << "]");
        //                    errors++;
        //                } else {
        //                    auto value = std::get<1>(cmplx.list(list_idx));
        //                    if (value != expected + list_idx) {
        //                        MAPF_ERR("TLV4 complex 0 list ")
        //                            << list_idx << " has value " << std::hex << value << " instead of "
        //                            << std::hex << expected + list_idx;
        //                        errors++;
        //                    }
        //                }
        //            }
        //
        //            if (cmplx.var1() != 0xd00d) {
        //                MAPF_ERR("TLV4 complex 0 var1 is " << std::hex << cmplx.var1()
        //                                                   << " instead of 0xd00d");
        //                errors++;
        //            }
        //        }
        //        if (!std::get<0>(tlv4->complex_list(1))) {
        //            MAPF_ERR("TLV4 has no complex 1");
        //            errors++;
        //        } else {
        //            auto cmplx = std::get<1>(tlv4->complex_list(1));
        //            if (cmplx.list_length() != 0) {
        //                MAPF_ERR("TLV4 complex 1 list length is " << unsigned(cmplx.list_length())
        //                                                          << " instead of 0");
        //                errors++;
        //            }
        //            if (cmplx.var1() != 0xd11d) {
        //                MAPF_ERR("TLV4 complex 1 var1 is " << std::hex << cmplx.var1()
        //                                                   << " instead of 0xd11d");
        //                errors++;
        //            }
        //        }

        auto var1 = tlv4->var1();
        if (!var1) {
            MAPF_ERR("TLV4 var1 is not set");
        } else {
            if (var1->list_length() != 0) {
                MAPF_ERR("TLV4 var1 list length is " << unsigned(var1->list_length())
                                                     << " instead of 0");
                errors++;
            }
            if (var1->var1() != 0xeeee) {
                MAPF_ERR("TLV4 var1 var1 is " << std::hex << var1->var1() << " instead of 0xeeee");
                errors++;
            }
        }
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

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("tlvf_test");
    int errors = 0;

    errors += test_int_len_list();
    errors += test_complex_list();
    errors += test_all();

    return errors;
}
