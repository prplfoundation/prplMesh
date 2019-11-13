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
#include "tlvf/ieee_1905_1/tlvMacAddress.h"
#include "tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h"
#include "tlvf/ieee_1905_1/tlvUnknown.h"
#include "tlvf/ieee_1905_1/tlvVendorSpecific.h"
#include "tlvf/ieee_1905_1/tlvWscM1.h"
#include "tlvf/ieee_1905_1/tlvWscM2.h"
#include "tlvf/wfa_map/tlvApCapability.h"
#include <tlvf/test/tlvVarList.h>

#include <mapf/common/encryption.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <mapf/common/utils.h>
#include <tlvf/wfa_map/tlvApCapability.h>

#include <algorithm>
#include <iterator>
#include <stdio.h>

using namespace ieee1905_1;
using namespace wfa_map;

MAPF_INITIALIZE_LOGGER

using namespace mapf;

int test_int_len_list()
{
    int errors = 0;
    uint8_t tx_buffer[1024];
    const uint8_t gTlvMacAddress[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    MAPF_INFO(__FUNCTION__ << " start");
    memset(tx_buffer, 0, sizeof(tx_buffer));
    {
        auto tlv = tlvMacAddress(tx_buffer, sizeof(tx_buffer), false, true);
        std::copy_n(gTlvMacAddress, 6, tlv.mac().oct);
        tlv.class_swap(); //finalize
        LOG(DEBUG) << "TX: " << std::endl << utils::dump_buffer(tx_buffer, tlv.getLen());
    }

    uint8_t rx_buffer[sizeof(tx_buffer)];
    memcpy(rx_buffer, tx_buffer, sizeof(rx_buffer));
    {
        auto tlv = tlvMacAddress(tx_buffer, sizeof(tx_buffer), true, true);
        auto mac = tlv.mac();
        if (!std::equal(mac.oct, mac.oct + 6, gTlvMacAddress)) {
            MAPF_ERR("MAC address in received TLV does not match expected result");
            errors++;
        }
        LOG(DEBUG) << "RX: " << std::endl << utils::dump_buffer(rx_buffer, tlv.getLen());
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
        errors++;
    }
    *(fourthTlv->simple_list(0)) = 0x0bb0;
    *(fourthTlv->simple_list(1)) = 0x0bb1;

    if (true == fourthTlv->set_test_string("1234567890")) {
        LOG(ERROR) << "FAIL test maximum size string";
        errors++;
    }
    // test allocation of correct length (less then max)
    if (false == fourthTlv->set_test_string("1234567")) {
        LOG(ERROR) << "FAIL test normal size string";
        errors++;
    }

    for (int i = 0; i < complex_list_entries; i++) {
        auto cmplx    = fourthTlv->create_complex_list();
        cmplx->var1() = 0xbbbbaaaa;
        cmplx->alloc_list(3);
        *(cmplx->list(0)) = 0xc0;
        *(cmplx->list(1)) = 0xc1;
        *(cmplx->list(2)) = 0xc2;
        if (!fourthTlv->add_complex_list(cmplx)) {
            LOG(ERROR) << "Failed to add complex list";
            errors++;
        }
        //test multiple add (should Fail!)
        if (fourthTlv->add_complex_list(cmplx)) {
            LOG(ERROR) << "Could add complex list a second time";
            errors++;
        }
    }

    fourthTlv->var2() = 0xabababab;
    auto cmplx        = fourthTlv->create_var1();
    cmplx->var1()     = 0xb11b;
    cmplx->set_unknown_length_list_inner("prplMesh");
    if (!fourthTlv->add_var1(cmplx)) {
        LOG(ERROR) << "Failed to add var1";
        errors++;
    }
    // Test multiple add - should fail
    if (fourthTlv->add_var1(cmplx)) {
        LOG(ERROR) << "Could add var1 a second time";
        errors++;
    }

    LOG(DEBUG) << "TLV 4 length " << fourthTlv->length();

    LOG(DEBUG) << "Finalize";
    //MANDATORY - swaps to little indian.
    msg.finalize(true);

    LOG(DEBUG) << "TX: " << std::endl << utils::dump_buffer(tx_buffer, msg.getMessageLength());

    uint8_t recv_buffer[sizeof(tx_buffer)];
    memcpy(recv_buffer, tx_buffer, sizeof(recv_buffer));

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, sizeof(recv_buffer), true);

    auto tlv4 = received_message.addClass<tlvTestVarList>();
    if (tlv4 == nullptr) {
        MAPF_ERR("TLV4 is NULL");
        return ++errors;
    }
    if (tlv4->test_string_str().compare("1234567")) {
        MAPF_ERR("FAIL, expected  \"1234567\", received " << tlv4->test_string_str());
        errors++;
    }
    if (tlv4->complex_list_length() != complex_list_entries) {
        MAPF_ERR("Invalid complex list num of entries" << tlv4->complex_list_length());
        return ++errors;
    }
    for (int i = 0; i < complex_list_entries; i++) {
        auto cmplx = std::get<1>(tlv4->complex_list(i));
        if (cmplx.var1() != 0xbbbbaaaa) {
            MAPF_ERR("wrong value for cmplx->var1 " << std::hex << cmplx.var1());
            errors++;
        }
    }

    if (tlv4->var1()->var1() != 0xb11b) {
        MAPF_ERR("Unexpected var1 value " << tlv4->var1()->var1());
        errors++;
    }

    if (tlv4->var2() != 0xabababab) {
        MAPF_ERR("Unexpected var2 value " << tlv4->var2());
        errors++;
    }

    auto str = std::string(tlv4->var1()->unknown_length_list_inner(),
                           tlv4->var1()->unknown_length_list_inner_length());
    if (str.compare("prplMesh")) {
        MAPF_ERR("unknown length list failure - expected \"prplMesh\", received " << str);
        errors++;
    }

    MAPF_INFO(__FUNCTION__ << " Finished, errors = " << errors << std::endl);
    return errors;
}

bool add_encrypted_settings(std::shared_ptr<tlvWscM2> m2, uint8_t *keywrapkey, uint8_t *iv)
{
    // Encrypted settings
    // Encrypted settings are the ConfigData + IV. First create the ConfigData,
    // Then copy it to the encrypted data, add an IV and encrypt.
    // Finally, add HMAC
    uint8_t buf[1024];
    WSC::cConfigData config_data(buf, sizeof(buf), false, true);
    config_data.set_ssid("test_ssid");
    config_data.authentication_type_attr().data = WSC::eWscAuth::WSC_AUTH_WPA2; //DUMMY
    config_data.encryption_type_attr().data     = WSC::eWscEncr::WSC_ENCR_AES;
    config_data.set_network_key("test1234");

    LOG(DEBUG) << "WSC config_data:" << std::endl
               << "     ssid: " << config_data.ssid() << std::endl
               << "     authentication_type: " << int(config_data.authentication_type_attr().data)
               << std::endl
               << "     network_key: "
               << std::string(config_data.network_key(), config_data.network_key_length())
               << std::endl
               << "     network_key length: " << int(config_data.network_key_length()) << std::endl
               << "     encryption_type: " << int(config_data.encryption_type_attr().data)
               << std::endl;
    config_data.class_swap();

    int datalen                  = config_data.getLen();
    int cipherlen                = datalen + 16;
    uint8_t encrypted[cipherlen] = {0};
    std::copy_n(config_data.getStartBuffPtr(), config_data.getLen(), encrypted);
    if (!mapf::encryption::aes_encrypt(keywrapkey, iv, encrypted, datalen, encrypted, cipherlen)) {
        LOG(DEBUG) << "aes encrypt";
        return false;
    }

    std::fill(m2->authenticator().data, m2->authenticator().data + m2->authenticator().data_length,
              0xbb);

    auto encrypted_settings = m2->create_encrypted_settings();
    encrypted_settings->alloc_encrypted_settings(cipherlen);
    m2->add_encrypted_settings(encrypted_settings);
    std::copy_n(encrypted, cipherlen, encrypted_settings->encrypted_settings());
    std::copy_n(iv, sizeof(iv), encrypted_settings->iv());
    LOG(DEBUG) << "encrypted settings length: " << encrypted_settings->getLen();
    LOG(DEBUG) << "encrypted settings buffer: " << std::endl
               << utils::dump_buffer((uint8_t *)encrypted_settings->encrypted_settings(),
                                     encrypted_settings->encrypted_settings_length());

    LOG(DEBUG) << "authenticator type:" << m2->authenticator().attribute_type;
    LOG(DEBUG) << "authenticator length: " << m2->authenticator().data_length;
    LOG(DEBUG) << "authenticator buffer: " << std::endl
               << utils::dump_buffer((uint8_t *)m2->authenticator().data,
                                     m2->authenticator().data_length);

    return true;
}

bool parse_encrypted_settings(std::shared_ptr<tlvWscM2> m2, uint8_t *keywrapkey, uint8_t *iv)
{
    auto encrypted_settings = m2->encrypted_settings();
    mapf::encryption::aes_decrypt(keywrapkey, iv,
                                  (uint8_t *)encrypted_settings->encrypted_settings(),
                                  encrypted_settings->encrypted_settings_length());
    LOG(DEBUG) << "type: " << encrypted_settings->type();
    LOG(DEBUG) << "encrypted settings length: " << encrypted_settings->getLen();
    LOG(DEBUG) << "encrypted settings buffer: " << std::endl
               << utils::dump_buffer((uint8_t *)encrypted_settings->encrypted_settings(),
                                     encrypted_settings->encrypted_settings_length());

    uint8_t buf[encrypted_settings->encrypted_settings_length()];
    std::copy_n(encrypted_settings->encrypted_settings(),
                encrypted_settings->encrypted_settings_length(), buf);
    WSC::cConfigData config_data(buf, sizeof(buf), true, true);

    LOG(DEBUG) << "WSC config_data:" << std::endl
               << "     ssid: " << config_data.ssid() << std::endl
               << "     authentication_type: " << int(config_data.authentication_type_attr().data)
               << std::endl
               << "     encryption_type: " << int(config_data.encryption_type_attr().data)
               << std::endl;
    LOG(DEBUG) << "authenticator type:" << m2->authenticator().attribute_type;
    LOG(DEBUG) << "authenticator length:" << m2->authenticator().data_length;
    LOG(DEBUG) << "authenticator buffer: " << std::endl
               << utils::dump_buffer((uint8_t *)m2->authenticator().data,
                                     m2->authenticator().data_length);
    return true;
}

int test_parser()
{
    int errors = 0;
    uint8_t tx_buffer[4096];
    //creating cmdu message class and setting the header
    CmduMessageTx msg = CmduMessageTx(tx_buffer, sizeof(tx_buffer));

    //create method initializes the buffer and returns shared pointer to the message header
    auto header = msg.create(0, eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE);
    header->flags().last_fragment_indicator = 1;
    header->flags().relay_indicator         = 1;

    auto tlv1 = msg.addClass<tlvNon1905neighborDeviceList>();
    auto tlv2 = msg.addClass<tlvLinkMetricQuery>();
    auto tlv3 = msg.addClass<tlvWscM1>();

    LOG(DEBUG) << "Finalize";
    msg.finalize(true);

    LOG(DEBUG) << "TX: " << std::endl << utils::dump_buffer(tx_buffer, msg.getMessageLength());

    uint8_t recv_buffer[sizeof(tx_buffer)];
    memcpy(recv_buffer, tx_buffer, sizeof(recv_buffer));

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, sizeof(recv_buffer), true, true);
    auto tlv1_ = received_message.getClass<tlvNon1905neighborDeviceList>();
    if (!tlv1_)
        errors++;
    auto tlv2_ = received_message.getClass<tlvLinkMetricQuery>();
    if (!tlv2_)
        errors++;
    auto tlv3_ = received_message.getClass<tlvWscM1>();
    if (!tlv3_)
        errors++;

    MAPF_INFO(__FUNCTION__ << " Finished, errors = " << errors << std::endl);
    return errors;
}

int test_all()
{
    int errors = 0;
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

    //NOTE: I used random TLVs for the example, don't expect a standard IEEE1905 message
    MAPF_DBG("CLASS SIZE: " << sizeof(tlvNon1905neighborDeviceList));
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
        MAPF_DBG("WRITE 1 : " << (int)address.oct[3]);
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
        MAPF_DBG("WRITE 2 : " << (int)address.oct[3]);
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

    MAPF_DBG("TLV LENGTH START: " << firstTlv->length());
    auto secondTlv            = msg.addClass<tlvLinkMetricQuery>(); // another tlv for the example
    secondTlv->link_metrics() = tlvLinkMetricQuery::eLinkMetricsType::RX_LINK_METRICS_ONLY;

    LOG(DEBUG) << "Start WSC M2";
    auto thirdTlv = msg.addClass<tlvWscM2>();

    thirdTlv->message_type_attr().data = WSC::WSC_MSG_TYPE_M2;
    if (!thirdTlv->set_manufacturer("Intel"))
        return false;

    /**
     * @brief Diffie helman key exchange
     * 
     */
    uint8_t key1[192];
    uint8_t key2[192];
    unsigned key1_length = sizeof(key1);
    unsigned key2_length = sizeof(key2);
    std::fill(key1, key1 + key1_length, 1);
    std::fill(key2, key2 + key2_length, 2);
    mapf::encryption::diffie_hellman m1;
    mapf::encryption::diffie_hellman m2;
    // diffie-helman key-exchange
    m1.compute_key(key1, key1_length, m2.pubkey(), m2.pubkey_length());
    m2.compute_key(key2, key2_length, m1.pubkey(), m1.pubkey_length());
    key1_length    = sizeof(key1);
    uint8_t mac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    uint8_t authkey[32];
    uint8_t keywrapkey[16];
    uint8_t iv[16];
    mapf::encryption::create_iv(iv, sizeof(iv));
    wps_calculate_keys(m1, key1, key1_length, m1.nonce(), mac, m2.nonce(), authkey, keywrapkey);
    if (!add_encrypted_settings(thirdTlv, keywrapkey, iv)) {
        MAPF_ERR("add encrypted settings failed");
        return false;
    }
    LOG(DEBUG) << "Done (WSC M2)";
    MAPF_DBG("TLV LENGTH WSC M2: " << thirdTlv->length());

    auto mactlv                     = msg.addClass<tlvMacAddress>();
    const uint8_t gTlvMacAddress[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    std::copy_n(gTlvMacAddress, 6, mactlv->mac().oct);

    MAPF_DBG("TLV LENGHT MAC: " << mactlv->length());

    auto fourthTlv     = msg.addClass<tlvTestVarList>();
    fourthTlv->var0()  = 0xa0;
    allocation_succeed = fourthTlv->alloc_simple_list(2);
    mapf_assert(allocation_succeed);
    *(fourthTlv->simple_list(0)) = 0x0bb0;
    *(fourthTlv->simple_list(1)) = 0x0bb1;

    auto cmplx = fourthTlv->create_complex_list();
    cmplx->alloc_list(3);
    *(cmplx->list(0)) = 0xc0;
    *(cmplx->list(1)) = 0xc1;
    *(cmplx->list(2)) = 0xc2;
    cmplx->var1()     = 0xd00d;
    cmplx->alloc_list();
    *(cmplx->list(3)) = 0xc3;
    cmplx->alloc_list();
    *(cmplx->list(4)) = 0xc4;
    cmplx->alloc_list();
    *(cmplx->list(5)) = 0xc5;
    if (!fourthTlv->add_complex_list(cmplx)) { //first entry
        LOG(ERROR) << "Failed to add complex list";
        errors++;
    }

    // TODO the complex list doesn't work at the moment if it has more than one element
    // Cfr. #137
    cmplx         = fourthTlv->create_complex_list();
    cmplx->var1() = 0xa11ad11d;
    if (!fourthTlv->add_complex_list(cmplx)) { //second entry
        LOG(ERROR) << "Failed to add complex list";
        errors++;
    }
    if (fourthTlv->add_complex_list(cmplx)) {
        LOG(ERROR) << "Could add complex list a second time";
        errors++;
    }
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

    LOG(DEBUG) << "TLV 4 length " << fourthTlv->length();
    auto unknown    = fourthTlv->create_unknown_length_list();
    unknown->var1() = 0xbbbbaaaa;
    fourthTlv->add_unknown_length_list(unknown);
    LOG(DEBUG) << "Unknown list size: " << unknown->getLen();
    LOG(DEBUG) << "Total unknown Length: " << fourthTlv->unknown_length_list_length();
    LOG(DEBUG) << "TLV 4 length " << fourthTlv->length();

    LOG(DEBUG) << "Total Message length=" << int(msg.getMessageLength());

    LOG(DEBUG) << "Finalize";
    //MANDATORY - swaps to little indian.
    msg.finalize(true);

    LOG(DEBUG) << "TX: " << std::endl << utils::dump_buffer(tx_buffer, msg.getMessageLength());

    uint8_t recv_buffer[sizeof(tx_buffer)];
    memcpy(recv_buffer, tx_buffer, sizeof(recv_buffer));

    CmduMessageRx received_message;
    received_message.parse(recv_buffer, sizeof(recv_buffer), true);

    eTlvType type;
    if (received_message.getNextTlvType(type) &&
        type == eTlvType::TLV_NON_1905_NEIGHBOR_DEVICE_LIST) {
        MAPF_DBG("SUCCESS");
    }

    MAPF_DBG("size: " << received_message.getNextTlvLength());

    auto tlv1 = received_message.addClass<tlvNon1905neighborDeviceList>();
    if (tlv1 != nullptr) {
        MAPF_DBG("LENGTH AFTER INIT: " << tlv1->length());
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

            LOG(DEBUG) << "ADDRESS IS " << (int)address.oct[0];
        } else {
            MAPF_ERR("TLV DOES NOT EXIST");
            errors++;
        }
    } else {
        MAPF_ERR("TLV1 IS NULL");
        errors++;
    }

    if (received_message.getNextTlvType(type) && type == eTlvType::TLV_LINK_METRIC_QUERY) {
        MAPF_DBG("SUCCESS");
    } else {
        errors++;
    }

    MAPF_DBG("size: " << received_message.getNextTlvLength());

    auto tlv2 = received_message.addClass<tlvLinkMetricQuery>();
    if (tlv2 != nullptr) {
        MAPF_DBG("TLV2 LENGTH AFTER INIT: " << tlv2->length());
    } else {
        MAPF_ERR("TLV2 IS NULL");
        errors++;
    }

    if (received_message.getNextTlvType(type) && type == eTlvType::TLV_WSC) {
        MAPF_DBG("SUCCESS");
    } else {
        errors++;
    }

    auto tlv3 = received_message.addClass<tlvWscM2>();
    if (tlv3 != nullptr) {
        MAPF_DBG("TLV3 LENGTH AFTER INIT: " << tlv3->length());
    } else {
        MAPF_ERR("TLV3 IS NULL");
        errors++;
    }
    if (!parse_encrypted_settings(tlv3, keywrapkey, iv)) {
        MAPF_ERR("TLV3 parse encrypted settings failed");
        errors++;
    }

    auto tlvunknown = received_message.addClass<tlvUnknown>();
    if (tlvunknown != nullptr) {
        LOG(DEBUG) << "TLVUnknown Type: " << int(tlvunknown->type());

        LOG(DEBUG) << "TLVUnknown data:";
        for (uint8_t data_idx = 0; data_idx < tlvunknown->data_length(); data_idx++) {
            LOG(DEBUG) << " " << int(tlvunknown->data()[data_idx]);
        }
    } else {
        MAPF_ERR("TLVUnknown is NULL");
        errors++;
    }
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
            if (!tlv4->simple_list(list_idx)) {
                MAPF_ERR("TLV4 has no simple " << list_idx);
                errors++;
            } else {
                auto value = tlv4->simple_list(list_idx);
                if (*value != expected + list_idx) {
                    MAPF_ERR("TLV4 simple ") << list_idx << " has value " << std::hex << *value
                                             << " instead of " << std::hex << expected + list_idx;
                    errors++;
                }
            }
        }

        LOG(DEBUG) << "Total unknown Length: " << tlv4->unknown_length_list_length();
        if (tlv4->unknown_length_list_length() != fourthTlv->unknown_length_list_length()) {
            MAPF_ERR("TLV 4 unknown length list length mismatch");
            errors++;
        }

        LOG(DEBUG) << "TLV 4 length " << tlv4->length();
        // TODO the complex list doesn't work at the moment if it has more than one element
        // Cfr. #137
        if (tlv4->complex_list_length() != 2) {
            MAPF_ERR("TLV4 complex list length is " << unsigned(tlv4->complex_list_length())
                                                    << " instead of 2");
            errors++;
        }

        if (!std::get<0>(tlv4->complex_list(0))) {
            MAPF_ERR("TLV4 has no complex 0");
            errors++;
        } else {
            auto cmplx = std::get<1>(tlv4->complex_list(0));
            if (cmplx.list_length() != 6) {
                MAPF_ERR("TLV4 complex 0 list length is " << unsigned(cmplx.list_length())
                                                          << " instead of 6");
                errors++;
            }
            uint8_t expected = 0xc0;
            for (uint8_t list_idx = 0; list_idx < cmplx.list_length(); list_idx++) {
                if (!cmplx.list(list_idx)) {
                    MAPF_ERR("TLV4 complex 0 has no list[" << list_idx << "]");
                    errors++;
                } else {
                    auto value = cmplx.list(list_idx);
                    if (*value != expected + list_idx) {
                        MAPF_ERR("TLV4 complex 0 list ")
                            << list_idx << " has value " << std::hex << *value << " instead of "
                            << std::hex << expected + list_idx;
                        errors++;
                    }
                }
            }

            if (cmplx.var1() != 0xd00d) {
                MAPF_ERR("TLV4 complex 0 var1 is " << std::hex << cmplx.var1()
                                                   << " instead of 0xd00d");
                errors++;
            }
        }
        if (!std::get<0>(tlv4->complex_list(1))) {
            MAPF_ERR("TLV4 has no complex 1");
            errors++;
        } else {
            auto cmplx = std::get<1>(tlv4->complex_list(1));
            if (cmplx.list_length() != 0) {
                MAPF_ERR("TLV4 complex 1 list length is " << unsigned(cmplx.list_length())
                                                          << " instead of 0");
                errors++;
            }
            if (cmplx.var1() != 0xa11ad11d) {
                MAPF_ERR("TLV4 complex 1 var1 is " << std::hex << cmplx.var1()
                                                   << " instead of 0xa11ad11d");
                errors++;
            }
        }

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
    if (!invmsg.parse(invalidBuffer, invalidBufferSize, false)) {
        MAPF_DBG("HEADER PROTECTION SUCCESS");
    }

    if (!received_message.getNextTlvType(type)) {
        MAPF_DBG("TYPE PROTECTION SUCCESS");
    }
    auto invptr = invmsg.addClass<tlv1905NeighborDevice>();
    if (invptr == nullptr) {
        MAPF_DBG("PROTECTION SUCCESS");
    } else {
        MAPF_ERR("PROTECTION FAILED");
        errors++;
    }

    MAPF_INFO(__FUNCTION__ << " Finished, errors = " << errors << std::endl);
    return errors;
}

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("tlvf_test");
    int errors = 0;

    MAPF_INFO(__FUNCTION__ << " Starting tests");
    errors += test_int_len_list();
    errors += test_complex_list();
    errors += test_all();
    errors += test_parser();
    MAPF_INFO(__FUNCTION__ << " Finished, errors = " << errors << std::endl);
    return errors;
}
