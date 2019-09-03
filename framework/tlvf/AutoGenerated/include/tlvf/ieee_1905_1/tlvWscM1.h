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

#ifndef _TLVF_IEEE_1905_1_TLVWSCM1_H_
#define _TLVF_IEEE_1905_1_TLVWSCM1_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "tlvf/WSC/WSC_Attributes.h"

namespace ieee1905_1 {


class tlvWscM1 : public BaseClass
{
    public:
        tlvWscM1(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvWscM1(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvWscM1();

        const eTlvType& type();
        const uint16_t& length();
        WSC::sWscAttrVersion& version_attr();
        WSC::sWscAttrMessageType& message_type_attr();
        WSC::sWscAttrUuidE& uuid_e_attr();
        WSC::sWscAttrMac& mac_attr();
        WSC::sWscAttrEnroleeNonce& enrolee_nonce_attr();
        WSC::sWscAttrPublicKey& public_key_attr();
        WSC::sWscAttrAuthenticationTypeFlags& authentication_type_flags_attr();
        WSC::sWscAttrEncryptionTypeFlags& encryption_type_flags_attr();
        WSC::sWscAttrConnectionTypeFlags& connection_type_flags_attr();
        WSC::sWscAttrConfigurationMethods& configuration_methods_attr();
        WSC::sWscAttrWscState& wsc_state_attr();
        WSC::eWscAttributes& manufacturer_type();
        uint16_t& manufacturer_length();
        std::string manufacturer_str();
        char* manufacturer(size_t length = 0);
        bool set_manufacturer(const std::string& str);
        bool set_manufacturer(const char buffer[], size_t size);
        bool alloc_manufacturer(size_t count = 1);
        WSC::eWscAttributes& model_name_type();
        uint16_t& model_name_length();
        std::string model_name_str();
        char* model_name(size_t length = 0);
        bool set_model_name(const std::string& str);
        bool set_model_name(const char buffer[], size_t size);
        bool alloc_model_name(size_t count = 1);
        WSC::eWscAttributes& model_number_type();
        uint16_t& model_number_length();
        std::string model_number_str();
        char* model_number(size_t length = 0);
        bool set_model_number(const std::string& str);
        bool set_model_number(const char buffer[], size_t size);
        bool alloc_model_number(size_t count = 1);
        WSC::eWscAttributes& serial_number_type();
        uint16_t& serial_number_length();
        std::string serial_number_str();
        char* serial_number(size_t length = 0);
        bool set_serial_number(const std::string& str);
        bool set_serial_number(const char buffer[], size_t size);
        bool alloc_serial_number(size_t count = 1);
        WSC::sWscAttrPrimaryDeviceType& primary_device_type_attr();
        WSC::eWscAttributes& device_name_type();
        uint16_t& device_name_length();
        std::string device_name_str();
        char* device_name(size_t length = 0);
        bool set_device_name(const std::string& str);
        bool set_device_name(const char buffer[], size_t size);
        bool alloc_device_name(size_t count = 1);
        WSC::sWscAttrRfBands& rf_bands_attr();
        WSC::sWscAttrAssociationState& association_state_attr();
        WSC::sWscAttrDevicePasswordID& device_password_id_attr();
        WSC::sWscAttrConfigurationError& configuration_error_attr();
        WSC::sWscAttrOsVersion& os_version_attr();
        WSC::sWscAttrVersion2& version2_attr();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        WSC::sWscAttrVersion* m_version_attr = nullptr;
        WSC::sWscAttrMessageType* m_message_type_attr = nullptr;
        WSC::sWscAttrUuidE* m_uuid_e_attr = nullptr;
        WSC::sWscAttrMac* m_mac_attr = nullptr;
        WSC::sWscAttrEnroleeNonce* m_enrolee_nonce_attr = nullptr;
        WSC::sWscAttrPublicKey* m_public_key_attr = nullptr;
        WSC::sWscAttrAuthenticationTypeFlags* m_authentication_type_flags_attr = nullptr;
        WSC::sWscAttrEncryptionTypeFlags* m_encryption_type_flags_attr = nullptr;
        WSC::sWscAttrConnectionTypeFlags* m_connection_type_flags_attr = nullptr;
        WSC::sWscAttrConfigurationMethods* m_configuration_methods_attr = nullptr;
        WSC::sWscAttrWscState* m_wsc_state_attr = nullptr;
        WSC::eWscAttributes* m_manufacturer_type = nullptr;
        uint16_t* m_manufacturer_length = nullptr;
        char* m_manufacturer = nullptr;
        size_t m_manufacturer_idx__ = 0;
        int m_lock_order_counter__ = 0;
        WSC::eWscAttributes* m_model_name_type = nullptr;
        uint16_t* m_model_name_length = nullptr;
        char* m_model_name = nullptr;
        size_t m_model_name_idx__ = 0;
        WSC::eWscAttributes* m_model_number_type = nullptr;
        uint16_t* m_model_number_length = nullptr;
        char* m_model_number = nullptr;
        size_t m_model_number_idx__ = 0;
        WSC::eWscAttributes* m_serial_number_type = nullptr;
        uint16_t* m_serial_number_length = nullptr;
        char* m_serial_number = nullptr;
        size_t m_serial_number_idx__ = 0;
        WSC::sWscAttrPrimaryDeviceType* m_primary_device_type_attr = nullptr;
        WSC::eWscAttributes* m_device_name_type = nullptr;
        uint16_t* m_device_name_length = nullptr;
        char* m_device_name = nullptr;
        size_t m_device_name_idx__ = 0;
        WSC::sWscAttrRfBands* m_rf_bands_attr = nullptr;
        WSC::sWscAttrAssociationState* m_association_state_attr = nullptr;
        WSC::sWscAttrDevicePasswordID* m_device_password_id_attr = nullptr;
        WSC::sWscAttrConfigurationError* m_configuration_error_attr = nullptr;
        WSC::sWscAttrOsVersion* m_os_version_attr = nullptr;
        WSC::sWscAttrVersion2* m_version2_attr = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVWSCM1_H_
