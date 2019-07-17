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

#include <tlvf/ieee_1905_1/tlvWscM2.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvWscM2::tlvWscM2(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvWscM2::tlvWscM2(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvWscM2::~tlvWscM2() {
}
const eTlvType& tlvWscM2::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvWscM2::length() {
    return (const uint16_t&)(*m_length);
}

WSC::sWscAttrVersion& tlvWscM2::version_attr() {
    return (WSC::sWscAttrVersion&)(*m_version_attr);
}

WSC::sWscAttrMessageType& tlvWscM2::message_type_attr() {
    return (WSC::sWscAttrMessageType&)(*m_message_type_attr);
}

WSC::sWscAttrEnroleeNonce& tlvWscM2::enrolee_nonce_attr() {
    return (WSC::sWscAttrEnroleeNonce&)(*m_enrolee_nonce_attr);
}

WSC::sWscAttrRegistrarNonce& tlvWscM2::registrar_nonce_attr() {
    return (WSC::sWscAttrRegistrarNonce&)(*m_registrar_nonce_attr);
}

WSC::sWscAttrUuidR& tlvWscM2::uuid_r_attr() {
    return (WSC::sWscAttrUuidR&)(*m_uuid_r_attr);
}

WSC::sWscAttrPublicKey& tlvWscM2::public_key_attr() {
    return (WSC::sWscAttrPublicKey&)(*m_public_key_attr);
}

WSC::sWscAttrAuthenticationTypeFlags& tlvWscM2::authentication_type_flags_attr() {
    return (WSC::sWscAttrAuthenticationTypeFlags&)(*m_authentication_type_flags_attr);
}

WSC::sWscAttrEncryptionTypeFlags& tlvWscM2::encryption_type_flags_attr() {
    return (WSC::sWscAttrEncryptionTypeFlags&)(*m_encryption_type_flags_attr);
}

WSC::sWscAttrConnectionTypeFlags& tlvWscM2::connection_type_flags_attr() {
    return (WSC::sWscAttrConnectionTypeFlags&)(*m_connection_type_flags_attr);
}

WSC::sWscAttrConfigurationMethods& tlvWscM2::configuration_methods_attr() {
    return (WSC::sWscAttrConfigurationMethods&)(*m_configuration_methods_attr);
}

WSC::eWscAttributes& tlvWscM2::manufacturer_type() {
    return (WSC::eWscAttributes&)(*m_manufacturer_type);
}

uint16_t& tlvWscM2::manufacturer_length() {
    return (uint16_t&)(*m_manufacturer_length);
}

char* tlvWscM2::manufacturer(size_t length) {
    if( (m_manufacturer_idx__ <= 0) || (m_manufacturer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "manufacturer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_manufacturer);
}

bool tlvWscM2::set_manufacturer(std::string& str) {
    return set_manufacturer(const_cast<std::string&>(str));
}
bool tlvWscM2::set_manufacturer(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_manufacturer received an empty string.";
        return false;
    }
    if (!alloc_manufacturer(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_manufacturer, str.c_str(), str_size + 1);
    return true;
}
bool tlvWscM2::set_manufacturer(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_manufacturer received an empty string.";
        return false;
    }
    if (!alloc_manufacturer(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_manufacturer, str, size + 1);
    m_manufacturer[size] = '\0';
    return true;
}
bool tlvWscM2::alloc_manufacturer(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_manufacturer;
        uint8_t *dst = (uint8_t *)m_manufacturer + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::memmove(dst, src, move_length);
    }
    m_model_name_type = (WSC::eWscAttributes *)((uint8_t *)(m_model_name_type) + len);
    m_model_name_length = (uint16_t *)((uint8_t *)(m_model_name_length) + len);
    m_model_name = (char *)((uint8_t *)(m_model_name) + len);
    m_model_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_model_number_type) + len);
    m_model_number_length = (uint16_t *)((uint8_t *)(m_model_number_length) + len);
    m_model_number = (char *)((uint8_t *)(m_model_number) + len);
    m_serial_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_serial_number_type) + len);
    m_serial_number_length = (uint16_t *)((uint8_t *)(m_serial_number_length) + len);
    m_serial_number = (char *)((uint8_t *)(m_serial_number) + len);
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_extensions_attr = (WSC::sWscAttrVendorExtension *)((uint8_t *)(m_vendor_extensions_attr) + len);
    m_encrypted_settings_attr = (WSC::sWscAttrEncryptedSettings *)((uint8_t *)(m_encrypted_settings_attr) + len);
    m_manufacturer_idx__ += count;
    *m_manufacturer_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::eWscAttributes& tlvWscM2::model_name_type() {
    return (WSC::eWscAttributes&)(*m_model_name_type);
}

uint16_t& tlvWscM2::model_name_length() {
    return (uint16_t&)(*m_model_name_length);
}

char* tlvWscM2::model_name(size_t length) {
    if( (m_model_name_idx__ <= 0) || (m_model_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model_name);
}

bool tlvWscM2::set_model_name(std::string& str) {
    return set_model_name(const_cast<std::string&>(str));
}
bool tlvWscM2::set_model_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_model_name received an empty string.";
        return false;
    }
    if (!alloc_model_name(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_model_name, str.c_str(), str_size + 1);
    return true;
}
bool tlvWscM2::set_model_name(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_model_name received an empty string.";
        return false;
    }
    if (!alloc_model_name(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_model_name, str, size + 1);
    m_model_name[size] = '\0';
    return true;
}
bool tlvWscM2::alloc_model_name(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_model_name;
        uint8_t *dst = (uint8_t *)m_model_name + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::memmove(dst, src, move_length);
    }
    m_model_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_model_number_type) + len);
    m_model_number_length = (uint16_t *)((uint8_t *)(m_model_number_length) + len);
    m_model_number = (char *)((uint8_t *)(m_model_number) + len);
    m_serial_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_serial_number_type) + len);
    m_serial_number_length = (uint16_t *)((uint8_t *)(m_serial_number_length) + len);
    m_serial_number = (char *)((uint8_t *)(m_serial_number) + len);
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_extensions_attr = (WSC::sWscAttrVendorExtension *)((uint8_t *)(m_vendor_extensions_attr) + len);
    m_encrypted_settings_attr = (WSC::sWscAttrEncryptedSettings *)((uint8_t *)(m_encrypted_settings_attr) + len);
    m_model_name_idx__ += count;
    *m_model_name_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::eWscAttributes& tlvWscM2::model_number_type() {
    return (WSC::eWscAttributes&)(*m_model_number_type);
}

uint16_t& tlvWscM2::model_number_length() {
    return (uint16_t&)(*m_model_number_length);
}

char* tlvWscM2::model_number(size_t length) {
    if( (m_model_number_idx__ <= 0) || (m_model_number_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model_number length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model_number);
}

bool tlvWscM2::set_model_number(std::string& str) {
    return set_model_number(const_cast<std::string&>(str));
}
bool tlvWscM2::set_model_number(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_model_number received an empty string.";
        return false;
    }
    if (!alloc_model_number(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_model_number, str.c_str(), str_size + 1);
    return true;
}
bool tlvWscM2::set_model_number(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_model_number received an empty string.";
        return false;
    }
    if (!alloc_model_number(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_model_number, str, size + 1);
    m_model_number[size] = '\0';
    return true;
}
bool tlvWscM2::alloc_model_number(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_model_number;
        uint8_t *dst = (uint8_t *)m_model_number + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::memmove(dst, src, move_length);
    }
    m_serial_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_serial_number_type) + len);
    m_serial_number_length = (uint16_t *)((uint8_t *)(m_serial_number_length) + len);
    m_serial_number = (char *)((uint8_t *)(m_serial_number) + len);
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_extensions_attr = (WSC::sWscAttrVendorExtension *)((uint8_t *)(m_vendor_extensions_attr) + len);
    m_encrypted_settings_attr = (WSC::sWscAttrEncryptedSettings *)((uint8_t *)(m_encrypted_settings_attr) + len);
    m_model_number_idx__ += count;
    *m_model_number_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::eWscAttributes& tlvWscM2::serial_number_type() {
    return (WSC::eWscAttributes&)(*m_serial_number_type);
}

uint16_t& tlvWscM2::serial_number_length() {
    return (uint16_t&)(*m_serial_number_length);
}

char* tlvWscM2::serial_number(size_t length) {
    if( (m_serial_number_idx__ <= 0) || (m_serial_number_idx__ < length) ) {
        TLVF_LOG(ERROR) << "serial_number length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_serial_number);
}

bool tlvWscM2::set_serial_number(std::string& str) {
    return set_serial_number(const_cast<std::string&>(str));
}
bool tlvWscM2::set_serial_number(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_serial_number received an empty string.";
        return false;
    }
    if (!alloc_serial_number(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_serial_number, str.c_str(), str_size + 1);
    return true;
}
bool tlvWscM2::set_serial_number(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_serial_number received an empty string.";
        return false;
    }
    if (!alloc_serial_number(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_serial_number, str, size + 1);
    m_serial_number[size] = '\0';
    return true;
}
bool tlvWscM2::alloc_serial_number(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_serial_number;
        uint8_t *dst = (uint8_t *)m_serial_number + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::memmove(dst, src, move_length);
    }
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_extensions_attr = (WSC::sWscAttrVendorExtension *)((uint8_t *)(m_vendor_extensions_attr) + len);
    m_encrypted_settings_attr = (WSC::sWscAttrEncryptedSettings *)((uint8_t *)(m_encrypted_settings_attr) + len);
    m_serial_number_idx__ += count;
    *m_serial_number_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::sWscAttrPrimaryDeviceType& tlvWscM2::primary_device_type_attr() {
    return (WSC::sWscAttrPrimaryDeviceType&)(*m_primary_device_type_attr);
}

WSC::sWscAttrRfBands& tlvWscM2::rf_bands_attr() {
    return (WSC::sWscAttrRfBands&)(*m_rf_bands_attr);
}

WSC::sWscAttrAssociationState& tlvWscM2::association_state_attr() {
    return (WSC::sWscAttrAssociationState&)(*m_association_state_attr);
}

WSC::sWscAttrConfigurationError& tlvWscM2::configuration_error_attr() {
    return (WSC::sWscAttrConfigurationError&)(*m_configuration_error_attr);
}

WSC::sWscAttrDevicePasswordID& tlvWscM2::device_password_id_attr() {
    return (WSC::sWscAttrDevicePasswordID&)(*m_device_password_id_attr);
}

WSC::sWscAttrOsVersion& tlvWscM2::os_version_attr() {
    return (WSC::sWscAttrOsVersion&)(*m_os_version_attr);
}

WSC::sWscAttrVendorExtension& tlvWscM2::vendor_extensions_attr() {
    return (WSC::sWscAttrVendorExtension&)(*m_vendor_extensions_attr);
}

WSC::sWscAttrEncryptedSettings& tlvWscM2::encrypted_settings_attr() {
    return (WSC::sWscAttrEncryptedSettings&)(*m_encrypted_settings_attr);
}

void tlvWscM2::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_version_attr->struct_swap();
    m_message_type_attr->struct_swap();
    m_enrolee_nonce_attr->struct_swap();
    m_registrar_nonce_attr->struct_swap();
    m_uuid_r_attr->struct_swap();
    m_public_key_attr->struct_swap();
    m_authentication_type_flags_attr->struct_swap();
    m_encryption_type_flags_attr->struct_swap();
    m_connection_type_flags_attr->struct_swap();
    m_configuration_methods_attr->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_manufacturer_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_manufacturer_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_name_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_name_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_number_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_number_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_serial_number_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_serial_number_length));
    m_primary_device_type_attr->struct_swap();
    m_rf_bands_attr->struct_swap();
    m_association_state_attr->struct_swap();
    m_configuration_error_attr->struct_swap();
    m_device_password_id_attr->struct_swap();
    m_os_version_attr->struct_swap();
    m_vendor_extensions_attr->struct_swap();
    m_encrypted_settings_attr->struct_swap();
}

size_t tlvWscM2::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(WSC::sWscAttrVersion); // version_attr
    class_size += sizeof(WSC::sWscAttrMessageType); // message_type_attr
    class_size += sizeof(WSC::sWscAttrEnroleeNonce); // enrolee_nonce_attr
    class_size += sizeof(WSC::sWscAttrRegistrarNonce); // registrar_nonce_attr
    class_size += sizeof(WSC::sWscAttrUuidR); // uuid_r_attr
    class_size += sizeof(WSC::sWscAttrPublicKey); // public_key_attr
    class_size += sizeof(WSC::sWscAttrAuthenticationTypeFlags); // authentication_type_flags_attr
    class_size += sizeof(WSC::sWscAttrEncryptionTypeFlags); // encryption_type_flags_attr
    class_size += sizeof(WSC::sWscAttrConnectionTypeFlags); // connection_type_flags_attr
    class_size += sizeof(WSC::sWscAttrConfigurationMethods); // configuration_methods_attr
    class_size += sizeof(WSC::eWscAttributes); // manufacturer_type
    class_size += sizeof(uint16_t); // manufacturer_length
    class_size += sizeof(WSC::eWscAttributes); // model_name_type
    class_size += sizeof(uint16_t); // model_name_length
    class_size += sizeof(WSC::eWscAttributes); // model_number_type
    class_size += sizeof(uint16_t); // model_number_length
    class_size += sizeof(WSC::eWscAttributes); // serial_number_type
    class_size += sizeof(uint16_t); // serial_number_length
    class_size += sizeof(WSC::sWscAttrPrimaryDeviceType); // primary_device_type_attr
    class_size += sizeof(WSC::sWscAttrRfBands); // rf_bands_attr
    class_size += sizeof(WSC::sWscAttrAssociationState); // association_state_attr
    class_size += sizeof(WSC::sWscAttrConfigurationError); // configuration_error_attr
    class_size += sizeof(WSC::sWscAttrDevicePasswordID); // device_password_id_attr
    class_size += sizeof(WSC::sWscAttrOsVersion); // os_version_attr
    class_size += sizeof(WSC::sWscAttrVendorExtension); // vendor_extensions_attr
    class_size += sizeof(WSC::sWscAttrEncryptedSettings); // encrypted_settings_attr
    return class_size;
}

bool tlvWscM2::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_WSC;
    else {
            if (*m_type != eTlvType::TLV_WSC) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_WSC) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_version_attr = (WSC::sWscAttrVersion*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrVersion) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrVersion); }
    if (!m_parse__) { m_version_attr->struct_init(); }
    m_message_type_attr = (WSC::sWscAttrMessageType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrMessageType) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrMessageType); }
    if (!m_parse__) { m_message_type_attr->struct_init(); }
    m_enrolee_nonce_attr = (WSC::sWscAttrEnroleeNonce*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrEnroleeNonce) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrEnroleeNonce); }
    if (!m_parse__) { m_enrolee_nonce_attr->struct_init(); }
    m_registrar_nonce_attr = (WSC::sWscAttrRegistrarNonce*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrRegistrarNonce) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrRegistrarNonce); }
    if (!m_parse__) { m_registrar_nonce_attr->struct_init(); }
    m_uuid_r_attr = (WSC::sWscAttrUuidR*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrUuidR) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrUuidR); }
    if (!m_parse__) { m_uuid_r_attr->struct_init(); }
    m_public_key_attr = (WSC::sWscAttrPublicKey*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrPublicKey) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrPublicKey); }
    if (!m_parse__) { m_public_key_attr->struct_init(); }
    m_authentication_type_flags_attr = (WSC::sWscAttrAuthenticationTypeFlags*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrAuthenticationTypeFlags) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrAuthenticationTypeFlags); }
    if (!m_parse__) { m_authentication_type_flags_attr->struct_init(); }
    m_encryption_type_flags_attr = (WSC::sWscAttrEncryptionTypeFlags*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrEncryptionTypeFlags) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrEncryptionTypeFlags); }
    if (!m_parse__) { m_encryption_type_flags_attr->struct_init(); }
    m_connection_type_flags_attr = (WSC::sWscAttrConnectionTypeFlags*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrConnectionTypeFlags) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrConnectionTypeFlags); }
    if (!m_parse__) { m_connection_type_flags_attr->struct_init(); }
    m_configuration_methods_attr = (WSC::sWscAttrConfigurationMethods*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrConfigurationMethods) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrConfigurationMethods); }
    if (!m_parse__) { m_configuration_methods_attr->struct_init(); }
    m_manufacturer_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_manufacturer_type = WSC::ATTR_MANUFACTURER;
    m_buff_ptr__ += sizeof(WSC::eWscAttributes) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_manufacturer_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_manufacturer_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_manufacturer = (char*)m_buff_ptr__;
    uint16_t manufacturer_length = *m_manufacturer_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&manufacturer_length));
    m_manufacturer_idx__ = manufacturer_length;
    m_buff_ptr__ += sizeof(char)*(manufacturer_length);
    m_model_name_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_model_name_type = WSC::ATTR_MODEL_NAME;
    m_buff_ptr__ += sizeof(WSC::eWscAttributes) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_model_name_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_model_name_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_model_name = (char*)m_buff_ptr__;
    uint16_t model_name_length = *m_model_name_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&model_name_length));
    m_model_name_idx__ = model_name_length;
    m_buff_ptr__ += sizeof(char)*(model_name_length);
    m_model_number_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_model_number_type = WSC::ATTR_MODEL_NUMBER;
    m_buff_ptr__ += sizeof(WSC::eWscAttributes) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_model_number_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_model_number_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_model_number = (char*)m_buff_ptr__;
    uint16_t model_number_length = *m_model_number_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&model_number_length));
    m_model_number_idx__ = model_number_length;
    m_buff_ptr__ += sizeof(char)*(model_number_length);
    m_serial_number_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_serial_number_type = WSC::ATTR_SERIAL_NUMBER;
    m_buff_ptr__ += sizeof(WSC::eWscAttributes) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_serial_number_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_serial_number_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_serial_number = (char*)m_buff_ptr__;
    uint16_t serial_number_length = *m_serial_number_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&serial_number_length));
    m_serial_number_idx__ = serial_number_length;
    m_buff_ptr__ += sizeof(char)*(serial_number_length);
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrPrimaryDeviceType) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrPrimaryDeviceType); }
    if (!m_parse__) { m_primary_device_type_attr->struct_init(); }
    m_rf_bands_attr = (WSC::sWscAttrRfBands*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrRfBands) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrRfBands); }
    if (!m_parse__) { m_rf_bands_attr->struct_init(); }
    m_association_state_attr = (WSC::sWscAttrAssociationState*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrAssociationState) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrAssociationState); }
    if (!m_parse__) { m_association_state_attr->struct_init(); }
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrConfigurationError) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrConfigurationError); }
    if (!m_parse__) { m_configuration_error_attr->struct_init(); }
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrDevicePasswordID) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrDevicePasswordID); }
    if (!m_parse__) { m_device_password_id_attr->struct_init(); }
    m_os_version_attr = (WSC::sWscAttrOsVersion*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrOsVersion) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrOsVersion); }
    if (!m_parse__) { m_os_version_attr->struct_init(); }
    m_vendor_extensions_attr = (WSC::sWscAttrVendorExtension*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrVendorExtension) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrVendorExtension); }
    if (!m_parse__) { m_vendor_extensions_attr->struct_init(); }
    m_encrypted_settings_attr = (WSC::sWscAttrEncryptedSettings*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sWscAttrEncryptedSettings) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrEncryptedSettings); }
    if (!m_parse__) { m_encrypted_settings_attr->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


