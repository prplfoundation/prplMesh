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
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "tlvf/WSC/eWscLengths.h"
#include "tlvf/WSC/eWscVendorId.h"
#include "tlvf/WSC/eWscVendorExt.h"
#include "tlvf/WSC/eWscDev.h"
#include "tlvf/common/sMacAddr.h"

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
    sMacAddr data;
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
        category_id = WSC_DEV_NETWORK_INFRA;
        oui = 0x50f204;
    }
} __attribute__((packed)) sWscAttrPrimaryDeviceType;

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

typedef struct sWscAttrVendorExtension {
    eWscAttributes attribute_type;
    uint16_t data_length;
    uint8_t vendor_id_0;
    uint8_t vendor_id_1;
    uint8_t vendor_id_2;
    uint8_t subelement_id;
    uint8_t subelement_length;
    uint8_t subelement_value;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_VENDOR_EXTENSION;
        data_length = WSC_VENDOR_EXTENSIONS_LENGTH;
        vendor_id_0 = WSC_VENDOR_ID_WFA_1;
        vendor_id_1 = WSC_VENDOR_ID_WFA_2;
        vendor_id_2 = WSC_VENDOR_ID_WFA_3;
        subelement_id = 0x6;
        subelement_length = 0x1;
        subelement_value = TEARDOWN;
    }
} __attribute__((packed)) sWscAttrVendorExtension;

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
    sMacAddr data;
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

typedef struct sWscAttrAuthenticator {
    eWscAttributes attribute_type;
    uint16_t data_length;
    char data[WSC_AUTHENTICATOR_LENGTH];
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
    }
    void struct_init(){
        attribute_type = ATTR_AUTHENTICATOR;
        data_length = WSC_AUTHENTICATOR_LENGTH;
    }
} __attribute__((packed)) sWscAttrAuthenticator;


class cWscAttrEncryptedSettings : public BaseClass
{
    public:
        cWscAttrEncryptedSettings(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cWscAttrEncryptedSettings(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cWscAttrEncryptedSettings();

        const eWscAttributes& type();
        const uint16_t& length();
        eWscAttributes& ssid_type();
        uint16_t& ssid_length();
        char* ssid(size_t length = 0);
        bool set_ssid(std::string& str);
        bool set_ssid(const std::string& str);
        bool set_ssid(char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        sWscAttrAuthenticationType& authentication_type_attr();
        sWscAttrEncryptionType& encryption_type_attr();
        sWscAttrNetworkKey& network_key_attr();
        sWscAttrBssid& bssid_attr();
        sWscAttrKeyWrapAuthenticator& key_wrap_auth_attr();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscAttributes* m_ssid_type = nullptr;
        uint16_t* m_ssid_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sWscAttrAuthenticationType* m_authentication_type_attr = nullptr;
        sWscAttrEncryptionType* m_encryption_type_attr = nullptr;
        sWscAttrNetworkKey* m_network_key_attr = nullptr;
        sWscAttrBssid* m_bssid_attr = nullptr;
        sWscAttrKeyWrapAuthenticator* m_key_wrap_auth_attr = nullptr;
};

}; // close namespace: WSC

#endif //_TLVF/WSC_WSC_ATTRIBUTES_H_
