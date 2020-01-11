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
#include "tlvf/WSC/eWscAuth.h"
#include "tlvf/WSC/eWscEncr.h"
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "tlvf/WSC/eWscValues8.h"
#include "tlvf/WSC/eWscMessageType.h"
#include "tlvf/WSC/eWscConn.h"
#include "tlvf/WSC/eWscRfBands.h"
#include "tlvf/WSC/eWscAssoc.h"
#include "tlvf/WSC/eWscValues16.h"
#include "tlvf/WSC/eWscState.h"
#include "tlvf/WSC/eWscLengths.h"
#include "tlvf/WSC/eWscVendorId.h"
#include "tlvf/WSC/eWscVendorExt.h"
#include "tlvf/WSC/eWscDev.h"
#include "tlvf/common/sMacAddr.h"

namespace WSC {

class cWscAttrEncryptedSettings;
class cWscVendorExtWfa;
class cWscAttrVersion;
class cWscAttrMessageType;
class cWscAttrEnrolleeNonce;
class cWscAttrPublicKey;
class cWscAttrAuthenticationTypeFlags;
class cWscAttrEncryptionTypeFlags;
class cWscAttrConnectionTypeFlags;
class cWscAttrConfigurationMethods;
class cWscAttrManufacturer;
class cWscAttrModelName;
class cWscAttrModelNumber;
class cWscAttrSerialNumber;
class cWscAttrPrimaryDeviceType;
class cWscAttrDeviceName;
class cWscAttrRfBands;
class cWscAttrAssociationState;
class cWscAttrDevicePasswordID;
class cWscAttrConfigurationError;
class cWscAttrOsVersion;
class cWscAttrMac;
class cWscAttrUuidE;
class cWscAttrWscState;
class cWscAttrUuidR;
class cWscAttrAuthenticator;
class cWscAttrRegistrarNonce;
class cWscAttrVersion2;
class cWscAttrSsid;
class cWscAttrAuthenticationType;
class cWscAttrEncryptionType;
class cWscAttrNetworkKey;
typedef struct sWscAttrVersion2 {
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
        subelement_id = 0x0;
        subelement_length = 0x1;
        subelement_value = WSC_VERSION2;
    }
} __attribute__((packed)) sWscAttrVersion2;

typedef struct sWscAttrVendorExtMultiAp {
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
} __attribute__((packed)) sWscAttrVendorExtMultiAp;

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
        data = eWscAuth::WSC_AUTH_WPA2PSK;
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
        data = eWscEncr::WSC_ENCR_AES;
    }
} __attribute__((packed)) sWscAttrEncryptionType;

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


class cConfigData : public BaseClass
{
    public:
        cConfigData(uint8_t* buff, size_t buff_len, bool parse = false);
        cConfigData(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cConfigData();

        eWscAttributes& ssid_type();
        uint16_t& ssid_length();
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        sWscAttrAuthenticationType& authentication_type_attr();
        sWscAttrEncryptionType& encryption_type_attr();
        eWscAttributes& network_key_type();
        uint16_t& network_key_length();
        std::string network_key_str();
        char* network_key(size_t length = 0);
        bool set_network_key(const std::string& str);
        bool set_network_key(const char buffer[], size_t size);
        bool alloc_network_key(size_t count = 1);
        sWscAttrBssid& bssid_attr();
        sWscAttrVendorExtMultiAp& multiap_attr();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_ssid_type = nullptr;
        uint16_t* m_ssid_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sWscAttrAuthenticationType* m_authentication_type_attr = nullptr;
        sWscAttrEncryptionType* m_encryption_type_attr = nullptr;
        eWscAttributes* m_network_key_type = nullptr;
        uint16_t* m_network_key_length = nullptr;
        char* m_network_key = nullptr;
        size_t m_network_key_idx__ = 0;
        sWscAttrBssid* m_bssid_attr = nullptr;
        sWscAttrVendorExtMultiAp* m_multiap_attr = nullptr;
};

class cWscAttrEncryptedSettings : public BaseClass
{
    public:
        cWscAttrEncryptedSettings(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrEncryptedSettings(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEncryptedSettings();

        const eWscAttributes& type();
        const uint16_t& length();
        std::string iv_str();
        char* iv(size_t length = 0);
        bool set_iv(const std::string& str);
        bool set_iv(const char buffer[], size_t size);
        size_t encrypted_settings_length() { return m_encrypted_settings_idx__ * sizeof(char); }
        std::string encrypted_settings_str();
        char* encrypted_settings(size_t length = 0);
        bool set_encrypted_settings(const std::string& str);
        bool set_encrypted_settings(const char buffer[], size_t size);
        bool alloc_encrypted_settings(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_iv = nullptr;
        size_t m_iv_idx__ = 0;
        int m_lock_order_counter__ = 0;
        char* m_encrypted_settings = nullptr;
        size_t m_encrypted_settings_idx__ = 0;
};

class cWscVendorExtWfa : public BaseClass
{
    public:
        cWscVendorExtWfa(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscVendorExtWfa(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscVendorExtWfa();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t& vendor_id_0();
        uint8_t& vendor_id_1();
        uint8_t& vendor_id_2();
        uint8_t& subelement_id();
        uint8_t& subelement_length();
        uint8_t& subelement_value();
        size_t vs_data_length() { return m_vs_data_idx__ * sizeof(uint8_t); }
        uint8_t* vs_data(size_t idx = 0);
        bool alloc_vs_data(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_vendor_id_0 = nullptr;
        uint8_t* m_vendor_id_1 = nullptr;
        uint8_t* m_vendor_id_2 = nullptr;
        uint8_t* m_subelement_id = nullptr;
        uint8_t* m_subelement_length = nullptr;
        uint8_t* m_subelement_value = nullptr;
        uint8_t* m_vs_data = nullptr;
        size_t m_vs_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrVersion : public BaseClass
{
    public:
        cWscAttrVersion(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrVersion(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrVersion();

        eWscAttributes& type();
        const uint16_t& length();
        eWscValues8& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscValues8* m_data = nullptr;
};

class cWscAttrMessageType : public BaseClass
{
    public:
        cWscAttrMessageType(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrMessageType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrMessageType();

        eWscAttributes& type();
        const uint16_t& length();
        eWscMessageType& msg_type();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscMessageType* m_msg_type = nullptr;
};

class cWscAttrEnrolleeNonce : public BaseClass
{
    public:
        cWscAttrEnrolleeNonce(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrEnrolleeNonce(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEnrolleeNonce();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* nonce(size_t idx = 0);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_nonce = nullptr;
        size_t m_nonce_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrPublicKey : public BaseClass
{
    public:
        cWscAttrPublicKey(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrPublicKey(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrPublicKey();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* public_key(size_t idx = 0);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_public_key = nullptr;
        size_t m_public_key_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrAuthenticationTypeFlags : public BaseClass
{
    public:
        cWscAttrAuthenticationTypeFlags(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrAuthenticationTypeFlags(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAuthenticationTypeFlags();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& auth_type_flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_auth_type_flags = nullptr;
};

class cWscAttrEncryptionTypeFlags : public BaseClass
{
    public:
        cWscAttrEncryptionTypeFlags(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrEncryptionTypeFlags(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEncryptionTypeFlags();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& encr_type_flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_encr_type_flags = nullptr;
};

class cWscAttrConnectionTypeFlags : public BaseClass
{
    public:
        cWscAttrConnectionTypeFlags(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrConnectionTypeFlags(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrConnectionTypeFlags();

        eWscAttributes& type();
        const uint16_t& length();
        eWscConn& conn_type_flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscConn* m_conn_type_flags = nullptr;
};

class cWscAttrConfigurationMethods : public BaseClass
{
    public:
        cWscAttrConfigurationMethods(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrConfigurationMethods(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrConfigurationMethods();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& conf_methods();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_conf_methods = nullptr;
};

class cWscAttrManufacturer : public BaseClass
{
    public:
        cWscAttrManufacturer(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrManufacturer(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrManufacturer();

        eWscAttributes& type();
        const uint16_t& length();
        size_t manufacturer_length() { return m_manufacturer_idx__ * sizeof(char); }
        std::string manufacturer_str();
        char* manufacturer(size_t length = 0);
        bool set_manufacturer(const std::string& str);
        bool set_manufacturer(const char buffer[], size_t size);
        bool alloc_manufacturer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_manufacturer = nullptr;
        size_t m_manufacturer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrModelName : public BaseClass
{
    public:
        cWscAttrModelName(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrModelName(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrModelName();

        eWscAttributes& type();
        const uint16_t& length();
        size_t model_length() { return m_model_idx__ * sizeof(char); }
        std::string model_str();
        char* model(size_t length = 0);
        bool set_model(const std::string& str);
        bool set_model(const char buffer[], size_t size);
        bool alloc_model(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_model = nullptr;
        size_t m_model_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrModelNumber : public BaseClass
{
    public:
        cWscAttrModelNumber(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrModelNumber(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrModelNumber();

        eWscAttributes& type();
        const uint16_t& length();
        size_t model_number_length() { return m_model_number_idx__ * sizeof(char); }
        std::string model_number_str();
        char* model_number(size_t length = 0);
        bool set_model_number(const std::string& str);
        bool set_model_number(const char buffer[], size_t size);
        bool alloc_model_number(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_model_number = nullptr;
        size_t m_model_number_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrSerialNumber : public BaseClass
{
    public:
        cWscAttrSerialNumber(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrSerialNumber(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrSerialNumber();

        eWscAttributes& type();
        const uint16_t& length();
        size_t serial_number_length() { return m_serial_number_idx__ * sizeof(char); }
        std::string serial_number_str();
        char* serial_number(size_t length = 0);
        bool set_serial_number(const std::string& str);
        bool set_serial_number(const char buffer[], size_t size);
        bool alloc_serial_number(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_serial_number = nullptr;
        size_t m_serial_number_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrPrimaryDeviceType : public BaseClass
{
    public:
        cWscAttrPrimaryDeviceType(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrPrimaryDeviceType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrPrimaryDeviceType();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& category_id();
        uint32_t& oui();
        uint16_t& sub_category_id();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_category_id = nullptr;
        uint32_t* m_oui = nullptr;
        uint16_t* m_sub_category_id = nullptr;
};

class cWscAttrDeviceName : public BaseClass
{
    public:
        cWscAttrDeviceName(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrDeviceName(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrDeviceName();

        eWscAttributes& type();
        const uint16_t& length();
        size_t device_name_length() { return m_device_name_idx__ * sizeof(char); }
        std::string device_name_str();
        char* device_name(size_t length = 0);
        bool set_device_name(const std::string& str);
        bool set_device_name(const char buffer[], size_t size);
        bool alloc_device_name(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_device_name = nullptr;
        size_t m_device_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrRfBands : public BaseClass
{
    public:
        cWscAttrRfBands(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrRfBands(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrRfBands();

        eWscAttributes& type();
        const uint16_t& length();
        eWscRfBands& bands();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscRfBands* m_bands = nullptr;
};

class cWscAttrAssociationState : public BaseClass
{
    public:
        cWscAttrAssociationState(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrAssociationState(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAssociationState();

        eWscAttributes& type();
        const uint16_t& length();
        eWscAssoc& assoc_state();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscAssoc* m_assoc_state = nullptr;
};

class cWscAttrDevicePasswordID : public BaseClass
{
    public:
        cWscAttrDevicePasswordID(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrDevicePasswordID(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrDevicePasswordID();

        eWscAttributes& type();
        const uint16_t& length();
        eWscValues16& pw();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscValues16* m_pw = nullptr;
};

class cWscAttrConfigurationError : public BaseClass
{
    public:
        cWscAttrConfigurationError(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrConfigurationError(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrConfigurationError();

        eWscAttributes& type();
        const uint16_t& length();
        eWscValues16& cfg_err();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscValues16* m_cfg_err = nullptr;
};

class cWscAttrOsVersion : public BaseClass
{
    public:
        cWscAttrOsVersion(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrOsVersion(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrOsVersion();

        eWscAttributes& type();
        const uint16_t& length();
        uint32_t& os_version();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint32_t* m_os_version = nullptr;
};

class cWscAttrMac : public BaseClass
{
    public:
        cWscAttrMac(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrMac(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrMac();

        eWscAttributes& type();
        const uint16_t& length();
        sMacAddr& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_data = nullptr;
};

class cWscAttrUuidE : public BaseClass
{
    public:
        cWscAttrUuidE(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrUuidE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrUuidE();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrWscState : public BaseClass
{
    public:
        cWscAttrWscState(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrWscState(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrWscState();

        eWscAttributes& type();
        const uint16_t& length();
        eWscState& state();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscState* m_state = nullptr;
};

class cWscAttrUuidR : public BaseClass
{
    public:
        cWscAttrUuidR(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrUuidR(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrUuidR();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrAuthenticator : public BaseClass
{
    public:
        cWscAttrAuthenticator(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrAuthenticator(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAuthenticator();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrRegistrarNonce : public BaseClass
{
    public:
        cWscAttrRegistrarNonce(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrRegistrarNonce(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrRegistrarNonce();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* nonce(size_t idx = 0);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_nonce = nullptr;
        size_t m_nonce_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrVersion2 : public BaseClass
{
    public:
        cWscAttrVersion2(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrVersion2(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrVersion2();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t& vendor_id_0();
        uint8_t& vendor_id_1();
        uint8_t& vendor_id_2();
        uint8_t& subelement_id();
        uint8_t& subelement_length();
        uint8_t& subelement_value();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_vendor_id_0 = nullptr;
        uint8_t* m_vendor_id_1 = nullptr;
        uint8_t* m_vendor_id_2 = nullptr;
        uint8_t* m_subelement_id = nullptr;
        uint8_t* m_subelement_length = nullptr;
        uint8_t* m_subelement_value = nullptr;
};

class cWscAttrSsid : public BaseClass
{
    public:
        cWscAttrSsid(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrSsid(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrSsid();

        eWscAttributes& type();
        const uint16_t& length();
        size_t ssid_length() { return m_ssid_idx__ * sizeof(char); }
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrAuthenticationType : public BaseClass
{
    public:
        cWscAttrAuthenticationType(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrAuthenticationType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAuthenticationType();

        eWscAttributes& type();
        const uint16_t& length();
        eWscAuth& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscAuth* m_data = nullptr;
};

class cWscAttrEncryptionType : public BaseClass
{
    public:
        cWscAttrEncryptionType(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrEncryptionType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEncryptionType();

        eWscAttributes& type();
        const uint16_t& length();
        eWscEncr& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscEncr* m_data = nullptr;
};

class cWscAttrNetworkKey : public BaseClass
{
    public:
        cWscAttrNetworkKey(uint8_t* buff, size_t buff_len, bool parse = false);
        cWscAttrNetworkKey(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrNetworkKey();

        eWscAttributes& type();
        const uint16_t& length();
        size_t key_length() { return m_key_idx__ * sizeof(char); }
        std::string key_str();
        char* key(size_t length = 0);
        bool set_key(const std::string& str);
        bool set_key(const char buffer[], size_t size);
        bool alloc_key(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_key = nullptr;
        size_t m_key_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: WSC

#endif //_TLVF/WSC_WSC_ATTRIBUTES_H_
