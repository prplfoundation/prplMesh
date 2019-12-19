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

#include <tlvf/ieee_1905_1/tlvWscM1.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvWscM1::tlvWscM1(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvWscM1::tlvWscM1(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvWscM1::~tlvWscM1() {
}
const eTlvType& tlvWscM1::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvWscM1::length() {
    return (const uint16_t&)(*m_length);
}

WSC::sWscAttrVersion& tlvWscM1::version_attr() {
    return (WSC::sWscAttrVersion&)(*m_version_attr);
}

WSC::sWscAttrMessageType& tlvWscM1::message_type_attr() {
    return (WSC::sWscAttrMessageType&)(*m_message_type_attr);
}

WSC::sWscAttrUuidE& tlvWscM1::uuid_e_attr() {
    return (WSC::sWscAttrUuidE&)(*m_uuid_e_attr);
}

WSC::sWscAttrMac& tlvWscM1::mac_attr() {
    return (WSC::sWscAttrMac&)(*m_mac_attr);
}

WSC::sWscAttrEnroleeNonce& tlvWscM1::enrolee_nonce_attr() {
    return (WSC::sWscAttrEnroleeNonce&)(*m_enrolee_nonce_attr);
}

WSC::sWscAttrPublicKey& tlvWscM1::public_key_attr() {
    return (WSC::sWscAttrPublicKey&)(*m_public_key_attr);
}

WSC::sWscAttrAuthenticationTypeFlags& tlvWscM1::authentication_type_flags_attr() {
    return (WSC::sWscAttrAuthenticationTypeFlags&)(*m_authentication_type_flags_attr);
}

WSC::sWscAttrEncryptionTypeFlags& tlvWscM1::encryption_type_flags_attr() {
    return (WSC::sWscAttrEncryptionTypeFlags&)(*m_encryption_type_flags_attr);
}

WSC::sWscAttrConnectionTypeFlags& tlvWscM1::connection_type_flags_attr() {
    return (WSC::sWscAttrConnectionTypeFlags&)(*m_connection_type_flags_attr);
}

WSC::sWscAttrConfigurationMethods& tlvWscM1::configuration_methods_attr() {
    return (WSC::sWscAttrConfigurationMethods&)(*m_configuration_methods_attr);
}

WSC::sWscAttrWscState& tlvWscM1::wsc_state_attr() {
    return (WSC::sWscAttrWscState&)(*m_wsc_state_attr);
}

WSC::eWscAttributes& tlvWscM1::manufacturer_type() {
    return (WSC::eWscAttributes&)(*m_manufacturer_type);
}

uint16_t& tlvWscM1::manufacturer_length() {
    return (uint16_t&)(*m_manufacturer_length);
}

std::string tlvWscM1::manufacturer_str() {
    char *manufacturer_ = manufacturer();
    if (!manufacturer_) { return std::string(); }
    return std::string(manufacturer_, m_manufacturer_idx__);
}

char* tlvWscM1::manufacturer(size_t length) {
    if( (m_manufacturer_idx__ <= 0) || (m_manufacturer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "manufacturer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_manufacturer);
}

bool tlvWscM1::set_manufacturer(const std::string& str) { return set_manufacturer(str.c_str(), str.size()); }
bool tlvWscM1::set_manufacturer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_manufacturer received an empty string.";
        return false;
    }
    if (!alloc_manufacturer(size)) { return false; }
    std::copy(str, str + size, m_manufacturer);
    return true;
}
bool tlvWscM1::alloc_manufacturer(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list manufacturer, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_manufacturer[*m_manufacturer_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
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
    m_device_name_type = (WSC::eWscAttributes *)((uint8_t *)(m_device_name_type) + len);
    m_device_name_length = (uint16_t *)((uint8_t *)(m_device_name_length) + len);
    m_device_name = (char *)((uint8_t *)(m_device_name) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_ext = (WSC::cWscVendorExtWfa *)((uint8_t *)(m_vendor_ext) + len);
    m_manufacturer_idx__ += count;
    *m_manufacturer_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::eWscAttributes& tlvWscM1::model_name_type() {
    return (WSC::eWscAttributes&)(*m_model_name_type);
}

uint16_t& tlvWscM1::model_name_length() {
    return (uint16_t&)(*m_model_name_length);
}

std::string tlvWscM1::model_name_str() {
    char *model_name_ = model_name();
    if (!model_name_) { return std::string(); }
    return std::string(model_name_, m_model_name_idx__);
}

char* tlvWscM1::model_name(size_t length) {
    if( (m_model_name_idx__ <= 0) || (m_model_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model_name);
}

bool tlvWscM1::set_model_name(const std::string& str) { return set_model_name(str.c_str(), str.size()); }
bool tlvWscM1::set_model_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_model_name received an empty string.";
        return false;
    }
    if (!alloc_model_name(size)) { return false; }
    std::copy(str, str + size, m_model_name);
    return true;
}
bool tlvWscM1::alloc_model_name(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list model_name, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_model_name[*m_model_name_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_model_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_model_number_type) + len);
    m_model_number_length = (uint16_t *)((uint8_t *)(m_model_number_length) + len);
    m_model_number = (char *)((uint8_t *)(m_model_number) + len);
    m_serial_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_serial_number_type) + len);
    m_serial_number_length = (uint16_t *)((uint8_t *)(m_serial_number_length) + len);
    m_serial_number = (char *)((uint8_t *)(m_serial_number) + len);
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_device_name_type = (WSC::eWscAttributes *)((uint8_t *)(m_device_name_type) + len);
    m_device_name_length = (uint16_t *)((uint8_t *)(m_device_name_length) + len);
    m_device_name = (char *)((uint8_t *)(m_device_name) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_ext = (WSC::cWscVendorExtWfa *)((uint8_t *)(m_vendor_ext) + len);
    m_model_name_idx__ += count;
    *m_model_name_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::eWscAttributes& tlvWscM1::model_number_type() {
    return (WSC::eWscAttributes&)(*m_model_number_type);
}

uint16_t& tlvWscM1::model_number_length() {
    return (uint16_t&)(*m_model_number_length);
}

std::string tlvWscM1::model_number_str() {
    char *model_number_ = model_number();
    if (!model_number_) { return std::string(); }
    return std::string(model_number_, m_model_number_idx__);
}

char* tlvWscM1::model_number(size_t length) {
    if( (m_model_number_idx__ <= 0) || (m_model_number_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model_number length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model_number);
}

bool tlvWscM1::set_model_number(const std::string& str) { return set_model_number(str.c_str(), str.size()); }
bool tlvWscM1::set_model_number(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_model_number received an empty string.";
        return false;
    }
    if (!alloc_model_number(size)) { return false; }
    std::copy(str, str + size, m_model_number);
    return true;
}
bool tlvWscM1::alloc_model_number(size_t count) {
    if (m_lock_order_counter__ > 2) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list model_number, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 2;
    uint8_t *src = (uint8_t *)&m_model_number[*m_model_number_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_serial_number_type = (WSC::eWscAttributes *)((uint8_t *)(m_serial_number_type) + len);
    m_serial_number_length = (uint16_t *)((uint8_t *)(m_serial_number_length) + len);
    m_serial_number = (char *)((uint8_t *)(m_serial_number) + len);
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_device_name_type = (WSC::eWscAttributes *)((uint8_t *)(m_device_name_type) + len);
    m_device_name_length = (uint16_t *)((uint8_t *)(m_device_name_length) + len);
    m_device_name = (char *)((uint8_t *)(m_device_name) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_ext = (WSC::cWscVendorExtWfa *)((uint8_t *)(m_vendor_ext) + len);
    m_model_number_idx__ += count;
    *m_model_number_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::eWscAttributes& tlvWscM1::serial_number_type() {
    return (WSC::eWscAttributes&)(*m_serial_number_type);
}

uint16_t& tlvWscM1::serial_number_length() {
    return (uint16_t&)(*m_serial_number_length);
}

std::string tlvWscM1::serial_number_str() {
    char *serial_number_ = serial_number();
    if (!serial_number_) { return std::string(); }
    return std::string(serial_number_, m_serial_number_idx__);
}

char* tlvWscM1::serial_number(size_t length) {
    if( (m_serial_number_idx__ <= 0) || (m_serial_number_idx__ < length) ) {
        TLVF_LOG(ERROR) << "serial_number length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_serial_number);
}

bool tlvWscM1::set_serial_number(const std::string& str) { return set_serial_number(str.c_str(), str.size()); }
bool tlvWscM1::set_serial_number(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_serial_number received an empty string.";
        return false;
    }
    if (!alloc_serial_number(size)) { return false; }
    std::copy(str, str + size, m_serial_number);
    return true;
}
bool tlvWscM1::alloc_serial_number(size_t count) {
    if (m_lock_order_counter__ > 3) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list serial_number, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 3;
    uint8_t *src = (uint8_t *)&m_serial_number[*m_serial_number_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType *)((uint8_t *)(m_primary_device_type_attr) + len);
    m_device_name_type = (WSC::eWscAttributes *)((uint8_t *)(m_device_name_type) + len);
    m_device_name_length = (uint16_t *)((uint8_t *)(m_device_name_length) + len);
    m_device_name = (char *)((uint8_t *)(m_device_name) + len);
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_ext = (WSC::cWscVendorExtWfa *)((uint8_t *)(m_vendor_ext) + len);
    m_serial_number_idx__ += count;
    *m_serial_number_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::sWscAttrPrimaryDeviceType& tlvWscM1::primary_device_type_attr() {
    return (WSC::sWscAttrPrimaryDeviceType&)(*m_primary_device_type_attr);
}

WSC::eWscAttributes& tlvWscM1::device_name_type() {
    return (WSC::eWscAttributes&)(*m_device_name_type);
}

uint16_t& tlvWscM1::device_name_length() {
    return (uint16_t&)(*m_device_name_length);
}

std::string tlvWscM1::device_name_str() {
    char *device_name_ = device_name();
    if (!device_name_) { return std::string(); }
    return std::string(device_name_, m_device_name_idx__);
}

char* tlvWscM1::device_name(size_t length) {
    if( (m_device_name_idx__ <= 0) || (m_device_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "device_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_device_name);
}

bool tlvWscM1::set_device_name(const std::string& str) { return set_device_name(str.c_str(), str.size()); }
bool tlvWscM1::set_device_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_device_name received an empty string.";
        return false;
    }
    if (!alloc_device_name(size)) { return false; }
    std::copy(str, str + size, m_device_name);
    return true;
}
bool tlvWscM1::alloc_device_name(size_t count) {
    if (m_lock_order_counter__ > 4) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list device_name, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 4;
    uint8_t *src = (uint8_t *)&m_device_name[*m_device_name_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_rf_bands_attr = (WSC::sWscAttrRfBands *)((uint8_t *)(m_rf_bands_attr) + len);
    m_association_state_attr = (WSC::sWscAttrAssociationState *)((uint8_t *)(m_association_state_attr) + len);
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID *)((uint8_t *)(m_device_password_id_attr) + len);
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError *)((uint8_t *)(m_configuration_error_attr) + len);
    m_os_version_attr = (WSC::sWscAttrOsVersion *)((uint8_t *)(m_os_version_attr) + len);
    m_vendor_ext = (WSC::cWscVendorExtWfa *)((uint8_t *)(m_vendor_ext) + len);
    m_device_name_idx__ += count;
    *m_device_name_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

WSC::sWscAttrRfBands& tlvWscM1::rf_bands_attr() {
    return (WSC::sWscAttrRfBands&)(*m_rf_bands_attr);
}

WSC::sWscAttrAssociationState& tlvWscM1::association_state_attr() {
    return (WSC::sWscAttrAssociationState&)(*m_association_state_attr);
}

WSC::sWscAttrDevicePasswordID& tlvWscM1::device_password_id_attr() {
    return (WSC::sWscAttrDevicePasswordID&)(*m_device_password_id_attr);
}

WSC::sWscAttrConfigurationError& tlvWscM1::configuration_error_attr() {
    return (WSC::sWscAttrConfigurationError&)(*m_configuration_error_attr);
}

WSC::sWscAttrOsVersion& tlvWscM1::os_version_attr() {
    return (WSC::sWscAttrOsVersion&)(*m_os_version_attr);
}

bool tlvWscM1::isPostInitSucceeded() {
    if (!m_vendor_ext_init) {
        TLVF_LOG(ERROR) << "vendor_ext is not initialized";
        return false;
    }
    return true; 
}

std::shared_ptr<WSC::cWscVendorExtWfa> tlvWscM1::create_vendor_ext() {
    if (m_lock_order_counter__ > 5) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list vendor_ext, abort!";
        return nullptr;
    }
    size_t len = WSC::cWscVendorExtWfa::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 5;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_vendor_ext;
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<WSC::cWscVendorExtWfa>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvWscM1::add_vendor_ext(std::shared_ptr<WSC::cWscVendorExtWfa> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_vendor_ext was called before add_vendor_ext";
        return false;
    }
    uint8_t *src = (uint8_t *)m_vendor_ext;
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_vendor_ext_init = true;
    size_t len = ptr->getLen();
    m_vendor_ext_ptr = ptr;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvWscM1::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_version_attr->struct_swap();
    m_message_type_attr->struct_swap();
    m_uuid_e_attr->struct_swap();
    m_mac_attr->struct_swap();
    m_enrolee_nonce_attr->struct_swap();
    m_public_key_attr->struct_swap();
    m_authentication_type_flags_attr->struct_swap();
    m_encryption_type_flags_attr->struct_swap();
    m_connection_type_flags_attr->struct_swap();
    m_configuration_methods_attr->struct_swap();
    m_wsc_state_attr->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_manufacturer_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_manufacturer_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_name_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_name_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_number_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_model_number_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_serial_number_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_serial_number_length));
    m_primary_device_type_attr->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_device_name_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_device_name_length));
    m_rf_bands_attr->struct_swap();
    m_association_state_attr->struct_swap();
    m_device_password_id_attr->struct_swap();
    m_configuration_error_attr->struct_swap();
    m_os_version_attr->struct_swap();
    if (m_vendor_ext_ptr) { m_vendor_ext_ptr->class_swap(); }
}

size_t tlvWscM1::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(WSC::sWscAttrVersion); // version_attr
    class_size += sizeof(WSC::sWscAttrMessageType); // message_type_attr
    class_size += sizeof(WSC::sWscAttrUuidE); // uuid_e_attr
    class_size += sizeof(WSC::sWscAttrMac); // mac_attr
    class_size += sizeof(WSC::sWscAttrEnroleeNonce); // enrolee_nonce_attr
    class_size += sizeof(WSC::sWscAttrPublicKey); // public_key_attr
    class_size += sizeof(WSC::sWscAttrAuthenticationTypeFlags); // authentication_type_flags_attr
    class_size += sizeof(WSC::sWscAttrEncryptionTypeFlags); // encryption_type_flags_attr
    class_size += sizeof(WSC::sWscAttrConnectionTypeFlags); // connection_type_flags_attr
    class_size += sizeof(WSC::sWscAttrConfigurationMethods); // configuration_methods_attr
    class_size += sizeof(WSC::sWscAttrWscState); // wsc_state_attr
    class_size += sizeof(WSC::eWscAttributes); // manufacturer_type
    class_size += sizeof(uint16_t); // manufacturer_length
    class_size += sizeof(WSC::eWscAttributes); // model_name_type
    class_size += sizeof(uint16_t); // model_name_length
    class_size += sizeof(WSC::eWscAttributes); // model_number_type
    class_size += sizeof(uint16_t); // model_number_length
    class_size += sizeof(WSC::eWscAttributes); // serial_number_type
    class_size += sizeof(uint16_t); // serial_number_length
    class_size += sizeof(WSC::sWscAttrPrimaryDeviceType); // primary_device_type_attr
    class_size += sizeof(WSC::eWscAttributes); // device_name_type
    class_size += sizeof(uint16_t); // device_name_length
    class_size += sizeof(WSC::sWscAttrRfBands); // rf_bands_attr
    class_size += sizeof(WSC::sWscAttrAssociationState); // association_state_attr
    class_size += sizeof(WSC::sWscAttrDevicePasswordID); // device_password_id_attr
    class_size += sizeof(WSC::sWscAttrConfigurationError); // configuration_error_attr
    class_size += sizeof(WSC::sWscAttrOsVersion); // os_version_attr
    return class_size;
}

bool tlvWscM1::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_WSC;
    if (!buffPtrIncrementSafe(sizeof(eTlvType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvType) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_version_attr = (WSC::sWscAttrVersion*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrVersion))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrVersion) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrVersion); }
    if (!m_parse__) { m_version_attr->struct_init(); }
    m_message_type_attr = (WSC::sWscAttrMessageType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrMessageType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrMessageType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrMessageType); }
    if (!m_parse__) { m_message_type_attr->struct_init(); }
    m_uuid_e_attr = (WSC::sWscAttrUuidE*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrUuidE))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrUuidE) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrUuidE); }
    if (!m_parse__) { m_uuid_e_attr->struct_init(); }
    m_mac_attr = (WSC::sWscAttrMac*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrMac))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrMac) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrMac); }
    if (!m_parse__) { m_mac_attr->struct_init(); }
    m_enrolee_nonce_attr = (WSC::sWscAttrEnroleeNonce*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrEnroleeNonce))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrEnroleeNonce) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrEnroleeNonce); }
    if (!m_parse__) { m_enrolee_nonce_attr->struct_init(); }
    m_public_key_attr = (WSC::sWscAttrPublicKey*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrPublicKey))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrPublicKey) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrPublicKey); }
    if (!m_parse__) { m_public_key_attr->struct_init(); }
    m_authentication_type_flags_attr = (WSC::sWscAttrAuthenticationTypeFlags*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrAuthenticationTypeFlags))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrAuthenticationTypeFlags) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrAuthenticationTypeFlags); }
    if (!m_parse__) { m_authentication_type_flags_attr->struct_init(); }
    m_encryption_type_flags_attr = (WSC::sWscAttrEncryptionTypeFlags*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrEncryptionTypeFlags))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrEncryptionTypeFlags) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrEncryptionTypeFlags); }
    if (!m_parse__) { m_encryption_type_flags_attr->struct_init(); }
    m_connection_type_flags_attr = (WSC::sWscAttrConnectionTypeFlags*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrConnectionTypeFlags))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrConnectionTypeFlags) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrConnectionTypeFlags); }
    if (!m_parse__) { m_connection_type_flags_attr->struct_init(); }
    m_configuration_methods_attr = (WSC::sWscAttrConfigurationMethods*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrConfigurationMethods))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrConfigurationMethods) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrConfigurationMethods); }
    if (!m_parse__) { m_configuration_methods_attr->struct_init(); }
    m_wsc_state_attr = (WSC::sWscAttrWscState*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrWscState))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrWscState) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrWscState); }
    if (!m_parse__) { m_wsc_state_attr->struct_init(); }
    m_manufacturer_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_manufacturer_type = WSC::ATTR_MANUFACTURER;
    if (!buffPtrIncrementSafe(sizeof(WSC::eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::eWscAttributes) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_manufacturer_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_manufacturer_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_manufacturer = (char*)m_buff_ptr__;
    uint16_t manufacturer_length = *m_manufacturer_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&manufacturer_length)); }
    m_manufacturer_idx__ = manufacturer_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (manufacturer_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (manufacturer_length) << ") Failed!";
        return false;
    }
    m_model_name_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_model_name_type = WSC::ATTR_MODEL_NAME;
    if (!buffPtrIncrementSafe(sizeof(WSC::eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::eWscAttributes) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_model_name_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_model_name_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_model_name = (char*)m_buff_ptr__;
    uint16_t model_name_length = *m_model_name_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&model_name_length)); }
    m_model_name_idx__ = model_name_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (model_name_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (model_name_length) << ") Failed!";
        return false;
    }
    m_model_number_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_model_number_type = WSC::ATTR_MODEL_NUMBER;
    if (!buffPtrIncrementSafe(sizeof(WSC::eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::eWscAttributes) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_model_number_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_model_number_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_model_number = (char*)m_buff_ptr__;
    uint16_t model_number_length = *m_model_number_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&model_number_length)); }
    m_model_number_idx__ = model_number_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (model_number_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (model_number_length) << ") Failed!";
        return false;
    }
    m_serial_number_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_serial_number_type = WSC::ATTR_SERIAL_NUMBER;
    if (!buffPtrIncrementSafe(sizeof(WSC::eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::eWscAttributes) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_serial_number_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_serial_number_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_serial_number = (char*)m_buff_ptr__;
    uint16_t serial_number_length = *m_serial_number_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&serial_number_length)); }
    m_serial_number_idx__ = serial_number_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (serial_number_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (serial_number_length) << ") Failed!";
        return false;
    }
    m_primary_device_type_attr = (WSC::sWscAttrPrimaryDeviceType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrPrimaryDeviceType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrPrimaryDeviceType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrPrimaryDeviceType); }
    if (!m_parse__) { m_primary_device_type_attr->struct_init(); }
    m_device_name_type = (WSC::eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_device_name_type = WSC::ATTR_DEV_NAME;
    if (!buffPtrIncrementSafe(sizeof(WSC::eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::eWscAttributes) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::eWscAttributes); }
    m_device_name_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_device_name_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_device_name = (char*)m_buff_ptr__;
    uint16_t device_name_length = *m_device_name_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&device_name_length)); }
    m_device_name_idx__ = device_name_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (device_name_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (device_name_length) << ") Failed!";
        return false;
    }
    m_rf_bands_attr = (WSC::sWscAttrRfBands*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrRfBands))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrRfBands) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrRfBands); }
    if (!m_parse__) { m_rf_bands_attr->struct_init(); }
    m_association_state_attr = (WSC::sWscAttrAssociationState*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrAssociationState))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrAssociationState) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrAssociationState); }
    if (!m_parse__) { m_association_state_attr->struct_init(); }
    m_device_password_id_attr = (WSC::sWscAttrDevicePasswordID*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrDevicePasswordID))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrDevicePasswordID) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrDevicePasswordID); }
    if (!m_parse__) { m_device_password_id_attr->struct_init(); }
    m_configuration_error_attr = (WSC::sWscAttrConfigurationError*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrConfigurationError))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrConfigurationError) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrConfigurationError); }
    if (!m_parse__) { m_configuration_error_attr->struct_init(); }
    m_os_version_attr = (WSC::sWscAttrOsVersion*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(WSC::sWscAttrOsVersion))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(WSC::sWscAttrOsVersion) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sWscAttrOsVersion); }
    if (!m_parse__) { m_os_version_attr->struct_init(); }
    m_vendor_ext = (WSC::cWscVendorExtWfa*)m_buff_ptr__;
    if (m_parse__) {
        auto vendor_ext = create_vendor_ext();
        if (!vendor_ext) {
            TLVF_LOG(ERROR) << "create_vendor_ext() failed";
            return false;
        }
        if (!add_vendor_ext(vendor_ext)) {
            TLVF_LOG(ERROR) << "add_vendor_ext() failed";
            return false;
        }
        // swap back since vendor_ext will be swapped as part of the whole class swap
        vendor_ext->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_WSC) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_WSC) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


