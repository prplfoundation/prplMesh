///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_WSC_ATTRIBUTES_H_
#define _TLVF_WSC_WSC_ATTRIBUTES_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "tlvf/WSC/eWscAttributes.h"
#include "tlvf/WSC/eWscValues8.h"
#include "tlvf/WSC/eWscMessageType.h"
#include "tlvf/WSC/eWscConn.h"
#include "tlvf/WSC/eWscState.h"
#include "tlvf/WSC/eWscRfBands.h"
#include "tlvf/WSC/eWscAssoc.h"
#include "tlvf/WSC/eWscValues16.h"
#include "tlvf/WSC/eWscAuth.h"
#include "tlvf/WSC/eWscEncr.h"
#include "tlvf/WSC/eWscLengths.h"
#include "tlvf/common/sMacAddress.h"

namespace WSC {

typedef struct sWscAttrVersion {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscValues8 data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_VERSION;
        data_length = 0x1;
        data = WSC_VERSION;
    }
} __attribute__((packed)) sWscAttrVersion;

typedef struct sWscAttrMessageType {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscMessageType data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_MSG_TYPE;
        data_length = 0x1;
        data = WSC_MSG_TYPE_M1;
    }
} __attribute__((packed)) sWscAttrMessageType;

typedef struct sWscAttrUuidE {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_UUID_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_UUID_E;
        data_length = WSC_UUID_LENGTH;
    }
} __attribute__((packed)) sWscAttrUuidE;

typedef struct sWscAttrUuidR {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_UUID_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_UUID_E;
        data_length = WSC_UUID_LENGTH;
    }
} __attribute__((packed)) sWscAttrUuidR;

typedef struct sWscAttrMac {
    eWscAttributes attribute_type;
    uint16_t data_length;
    sMacAddress data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        data.struct_swap();
    }
    void struct_init(){
        attribute_type = ATTR_MAC_ADDR;
        data_length = WSC_MAC_LENGTH;
        data.struct_init();
    }
} __attribute__((packed)) sWscAttrMac;

typedef struct sWscAttrEnroleeNonce {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_NONCE_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_ENROLLEE_NONCE;
        data_length = WSC_NONCE_LENGTH;
    }
} __attribute__((packed)) sWscAttrEnroleeNonce;

typedef struct sWscAttrRegistrarNonce {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_NONCE_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_REGISTRAR_NONCE;
        data_length = WSC_NONCE_LENGTH;
    }
} __attribute__((packed)) sWscAttrRegistrarNonce;

typedef struct sWscAttrPublicKey {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_PUBLIC_KEY_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_PUBLIC_KEY;
        data_length = WSC_PUBLIC_KEY_LENGTH;
    }
} __attribute__((packed)) sWscAttrPublicKey;

typedef struct sWscAttrAuthenticationTypeFlags {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint16_t data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_AUTH_TYPE_FLAGS;
        data_length = 0x2;
    }
} __attribute__((packed)) sWscAttrAuthenticationTypeFlags;

typedef struct sWscAttrEncryptionTypeFlags {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint16_t data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_ENCR_TYPE_FLAGS;
        data_length = 0x2;
    }
} __attribute__((packed)) sWscAttrEncryptionTypeFlags;

typedef struct sWscAttrConnectionTypeFlags {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscConn data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_CONN_TYPE_FLAGS;
        data_length = 0x1;
        data = WSC_CONN_ESS;
    }
} __attribute__((packed)) sWscAttrConnectionTypeFlags;

typedef struct sWscAttrConfigurationMethods {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint16_t data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_CONFIG_METHODS;
        data_length = 0x2;
        data = (WSC_CONFIG_PHY_PUSHBUTTON | WSC_CONFIG_VIRT_PUSHBUTTON);
    }
} __attribute__((packed)) sWscAttrConfigurationMethods;

typedef struct sWscAttrWscState {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscState data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_WSC_STATE;
        data_length = 0x1;
        data = WSC_STATE_NOT_CONFIGURED;
    }
} __attribute__((packed)) sWscAttrWscState;

typedef struct sWscAttrManufacturer {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_MANUFACTURER_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_MANUFACTURER;
        data_length = WSC_MAX_MANUFACTURER_LENGTH;
    }
} __attribute__((packed)) sWscAttrManufacturer;

typedef struct sWscAttrModelName {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_MODEL_NAME_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_MODEL_NAME;
        data_length = WSC_MAX_MODEL_NAME_LENGTH;
    }
} __attribute__((packed)) sWscAttrModelName;

typedef struct sWscAttrModelNumber {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_MODEL_NUMBER_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_MODEL_NUMBER;
        data_length = WSC_MAX_MODEL_NUMBER_LENGTH;
    }
} __attribute__((packed)) sWscAttrModelNumber;

typedef struct sWscAttrSerialNumber {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_SERIAL_NUMBER_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_SERIAL_NUMBER;
        data_length = WSC_MAX_SERIAL_NUMBER_LENGTH;
    }
} __attribute__((packed)) sWscAttrSerialNumber;

typedef struct sWscAttrPrimaryDeviceType {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint16_t category_id;
    uint32_t oui;
    uint16_t sub_category_id;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&category_id));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&oui));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&sub_category_id));
    }
    void struct_init(){
        attribute_type = ATTR_PRIMARY_DEV_TYPE;
        data_length = WSC_PRIMARY_DEV_TYPE_LENGTH;
    }
} __attribute__((packed)) sWscAttrPrimaryDeviceType;

typedef struct sWscAttrDeviceName {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_DEV_NAME_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_DEV_NAME;
        data_length = WSC_MAX_DEV_NAME_LENGTH;
    }
} __attribute__((packed)) sWscAttrDeviceName;

typedef struct sWscAttrRfBands {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscRfBands data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_RF_BANDS;
        data_length = 0x1;
    }
} __attribute__((packed)) sWscAttrRfBands;

typedef struct sWscAttrAssociationState {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscAssoc data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_ASSOC_STATE;
        data_length = 0x1;
        data = WSC_ASSOC_NOT_ASSOC;
    }
} __attribute__((packed)) sWscAttrAssociationState;

typedef struct sWscAttrDevicePasswordID {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscValues16 data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_DEV_PASSWORD_ID;
        data_length = 0x2;
        data = DEV_PW_PUSHBUTTON;
    }
} __attribute__((packed)) sWscAttrDevicePasswordID;

typedef struct sWscAttrConfigurationError {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscValues16 data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_CONFIG_ERROR;
        data_length = 0x2;
        data = WSC_CFG_NO_ERROR;
    }
} __attribute__((packed)) sWscAttrConfigurationError;

typedef struct sWscAttrOsVersion {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint32_t data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_OS_VERSION;
        data_length = WSC_OS_VERSION_LENGTH;
        data = 0x80000001;
    }
} __attribute__((packed)) sWscAttrOsVersion;

typedef struct sWscAttrVersionExtension {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_VENDOR_EXTENSIONS_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_VENDOR_EXTENSION;
        data_length = WSC_VENDOR_EXTENSIONS_LENGTH;
    }
} __attribute__((packed)) sWscAttrVersionExtension;

typedef struct sWscAttrKeyWrapAuthenticator {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t data[WSC_KEY_WRAP_AUTH_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_KEY_WRAP_AUTH;
        data_length = WSC_KEY_WRAP_AUTH_LENGTH;
    }
} __attribute__((packed)) sWscAttrKeyWrapAuthenticator;

typedef struct sWscAttrSsid {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_SSID_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_SSID;
        data_length = WSC_MAX_SSID_LENGTH;
    }
} __attribute__((packed)) sWscAttrSsid;

typedef struct sWscAttrAuthenticationType {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscAuth data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_AUTH_TYPE;
        data_length = 0x2;
    }
} __attribute__((packed)) sWscAttrAuthenticationType;

typedef struct sWscAttrEncryptionType {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscEncr data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_ENCR_TYPE;
        data_length = 0x2;
    }
} __attribute__((packed)) sWscAttrEncryptionType;

typedef struct sWscAttrNetworkKey {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_MAX_NETWORK_KEY_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_NETWORK_KEY;
        data_length = WSC_MAX_NETWORK_KEY_LENGTH;
    }
} __attribute__((packed)) sWscAttrNetworkKey;

typedef struct sWscAttrBssid {
    eWscAttributes attribute_type;
    uint16_t data_length;
    sMacAddress data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        data.struct_swap();
    }
    void struct_init(){
        attribute_type = ATTR_MAC_ADDR;
        data_length = WSC_MAC_LENGTH;
    }
} __attribute__((packed)) sWscAttrBssid;

typedef struct sWscAttrEncryptedSettings {
    eWscAttributes attribute_type;
    //TODO the value of the data_length should be updated automatically
    //based on the inner tlvs attributes lengths.
    //This will be added after implementing the support in multiple dynamic lists.
    //This attribute's type will be changed into class (instead of struct) and that way 
    //it can contain _length_var that will be updated automatically.
    uint16_t data_length;
    sWscAttrSsid ssid_attr;
    sWscAttrAuthenticationType authentication_type_attr;
    sWscAttrEncryptionType encryption_type_attr;
    sWscAttrNetworkKey network_key_attr;
    sWscAttrBssid bssid_attr;
    sWscAttrKeyWrapAuthenticator key_wrap_auth_attr;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        ssid_attr.struct_swap();
        authentication_type_attr.struct_swap();
        encryption_type_attr.struct_swap();
        network_key_attr.struct_swap();
        bssid_attr.struct_swap();
        key_wrap_auth_attr.struct_swap();
    }
    void struct_init(){
        attribute_type = ATTR_ENCR_SETTINGS;
        ssid_attr.struct_init();
        authentication_type_attr.struct_init();
        encryption_type_attr.struct_init();
        network_key_attr.struct_init();
        bssid_attr.struct_init();
        key_wrap_auth_attr.struct_init();
    }
} __attribute__((packed)) sWscAttrEncryptedSettings;


}; // close namespace: WSC

#endif //_TLVF/WSC_WSC_ATTRIBUTES_H_
