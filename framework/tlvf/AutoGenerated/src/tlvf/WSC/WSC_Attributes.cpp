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

#include <tlvf/WSC/WSC_Attributes.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

cConfigData::cConfigData(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cConfigData::cConfigData(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cConfigData::~cConfigData() {
}
eWscAttributes& cConfigData::ssid_type() {
    return (eWscAttributes&)(*m_ssid_type);
}

uint16_t& cConfigData::ssid_length() {
    return (uint16_t&)(*m_ssid_length);
}

std::string cConfigData::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* cConfigData::ssid(size_t length) {
    if( (m_ssid_idx__ <= 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    if (m_ssid_idx__ > WSC_MAX_SSID_LENGTH )  {
        TLVF_LOG(ERROR) << "Invalid length -  " << m_ssid_idx__ << " elements (max length is " << WSC_MAX_SSID_LENGTH << ")";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cConfigData::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool cConfigData::set_ssid(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_ssid received an empty string.";
        return false;
    }
    if (!alloc_ssid(size)) { return false; }
    std::copy(str, str + size, m_ssid);
    return true;
}
bool cConfigData::alloc_ssid(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ssid, abort!";
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
    if (count > WSC_MAX_SSID_LENGTH )  {
        TLVF_LOG(ERROR) << "Can't allocate " << count << " elements (max length is " << WSC_MAX_SSID_LENGTH << ")";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_ssid[*m_ssid_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_authentication_type_attr = (sWscAttrAuthenticationType *)((uint8_t *)(m_authentication_type_attr) + len);
    m_encryption_type_attr = (sWscAttrEncryptionType *)((uint8_t *)(m_encryption_type_attr) + len);
    m_network_key_type = (eWscAttributes *)((uint8_t *)(m_network_key_type) + len);
    m_network_key_length = (uint16_t *)((uint8_t *)(m_network_key_length) + len);
    m_network_key = (char *)((uint8_t *)(m_network_key) + len);
    m_bssid_attr = (sWscAttrBssid *)((uint8_t *)(m_bssid_attr) + len);
    m_multiap_attr = (sWscAttrVendorExtMultiAp *)((uint8_t *)(m_multiap_attr) + len);
    m_ssid_idx__ += count;
    *m_ssid_length += count;
    if (!buffPtrIncrementSafe(len)) { return false; }
    return true;
}

sWscAttrAuthenticationType& cConfigData::authentication_type_attr() {
    return (sWscAttrAuthenticationType&)(*m_authentication_type_attr);
}

sWscAttrEncryptionType& cConfigData::encryption_type_attr() {
    return (sWscAttrEncryptionType&)(*m_encryption_type_attr);
}

eWscAttributes& cConfigData::network_key_type() {
    return (eWscAttributes&)(*m_network_key_type);
}

uint16_t& cConfigData::network_key_length() {
    return (uint16_t&)(*m_network_key_length);
}

std::string cConfigData::network_key_str() {
    char *network_key_ = network_key();
    if (!network_key_) { return std::string(); }
    return std::string(network_key_, m_network_key_idx__);
}

char* cConfigData::network_key(size_t length) {
    if( (m_network_key_idx__ <= 0) || (m_network_key_idx__ < length) ) {
        TLVF_LOG(ERROR) << "network_key length is smaller than requested length";
        return nullptr;
    }
    if (m_network_key_idx__ > WSC_MAX_NETWORK_KEY_LENGTH )  {
        TLVF_LOG(ERROR) << "Invalid length -  " << m_network_key_idx__ << " elements (max length is " << WSC_MAX_NETWORK_KEY_LENGTH << ")";
        return nullptr;
    }
    return ((char*)m_network_key);
}

bool cConfigData::set_network_key(const std::string& str) { return set_network_key(str.c_str(), str.size()); }
bool cConfigData::set_network_key(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_network_key received an empty string.";
        return false;
    }
    if (!alloc_network_key(size)) { return false; }
    std::copy(str, str + size, m_network_key);
    return true;
}
bool cConfigData::alloc_network_key(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list network_key, abort!";
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
    if (count > WSC_MAX_NETWORK_KEY_LENGTH )  {
        TLVF_LOG(ERROR) << "Can't allocate " << count << " elements (max length is " << WSC_MAX_NETWORK_KEY_LENGTH << ")";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_network_key[*m_network_key_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_bssid_attr = (sWscAttrBssid *)((uint8_t *)(m_bssid_attr) + len);
    m_multiap_attr = (sWscAttrVendorExtMultiAp *)((uint8_t *)(m_multiap_attr) + len);
    m_network_key_idx__ += count;
    *m_network_key_length += count;
    if (!buffPtrIncrementSafe(len)) { return false; }
    return true;
}

sWscAttrBssid& cConfigData::bssid_attr() {
    return (sWscAttrBssid&)(*m_bssid_attr);
}

sWscAttrVendorExtMultiAp& cConfigData::multiap_attr() {
    return (sWscAttrVendorExtMultiAp&)(*m_multiap_attr);
}

void cConfigData::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_ssid_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_ssid_length));
    m_authentication_type_attr->struct_swap();
    m_encryption_type_attr->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_network_key_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_network_key_length));
    m_bssid_attr->struct_swap();
    m_multiap_attr->struct_swap();
}

size_t cConfigData::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // ssid_type
    class_size += sizeof(uint16_t); // ssid_length
    class_size += sizeof(sWscAttrAuthenticationType); // authentication_type_attr
    class_size += sizeof(sWscAttrEncryptionType); // encryption_type_attr
    class_size += sizeof(eWscAttributes); // network_key_type
    class_size += sizeof(uint16_t); // network_key_length
    class_size += sizeof(sWscAttrBssid); // bssid_attr
    class_size += sizeof(sWscAttrVendorExtMultiAp); // multiap_attr
    return class_size;
}

bool cConfigData::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ssid_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_type = ATTR_SSID;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) { return false; }
    m_ssid_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_ssid = (char*)m_buff_ptr__;
    uint16_t ssid_length = *m_ssid_length;
    if (m_parse__ && m_swap__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&ssid_length)); }
    m_ssid_idx__ = ssid_length;
    if (!buffPtrIncrementSafe(sizeof(char)*(ssid_length))) { return false; }
    m_authentication_type_attr = (sWscAttrAuthenticationType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrAuthenticationType))) { return false; }
    if (!m_parse__) { m_authentication_type_attr->struct_init(); }
    m_encryption_type_attr = (sWscAttrEncryptionType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrEncryptionType))) { return false; }
    if (!m_parse__) { m_encryption_type_attr->struct_init(); }
    m_network_key_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_network_key_type = ATTR_NETWORK_KEY;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) { return false; }
    m_network_key_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_network_key_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_network_key = (char*)m_buff_ptr__;
    uint16_t network_key_length = *m_network_key_length;
    if (m_parse__ && m_swap__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&network_key_length)); }
    m_network_key_idx__ = network_key_length;
    if (!buffPtrIncrementSafe(sizeof(char)*(network_key_length))) { return false; }
    m_bssid_attr = (sWscAttrBssid*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrBssid))) { return false; }
    if (!m_parse__) { m_bssid_attr->struct_init(); }
    m_multiap_attr = (sWscAttrVendorExtMultiAp*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrVendorExtMultiAp))) { return false; }
    if (!m_parse__) { m_multiap_attr->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cWscAttrEncryptedSettings::cWscAttrEncryptedSettings(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cWscAttrEncryptedSettings::cWscAttrEncryptedSettings(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cWscAttrEncryptedSettings::~cWscAttrEncryptedSettings() {
}
const eWscAttributes& cWscAttrEncryptedSettings::type() {
    return (const eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrEncryptedSettings::length() {
    return (const uint16_t&)(*m_length);
}

std::string cWscAttrEncryptedSettings::iv_str() {
    char *iv_ = iv();
    if (!iv_) { return std::string(); }
    return std::string(iv_, m_iv_idx__);
}

char* cWscAttrEncryptedSettings::iv(size_t length) {
    if( (m_iv_idx__ <= 0) || (m_iv_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iv length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iv);
}

bool cWscAttrEncryptedSettings::set_iv(const std::string& str) { return set_iv(str.c_str(), str.size()); }
bool cWscAttrEncryptedSettings::set_iv(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_iv received an empty string.";
        return false;
    }
    if (size > WSC_ENCRYPTED_SETTINGS_IV_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_iv);
    return true;
}
std::string cWscAttrEncryptedSettings::encrypted_settings_str() {
    char *encrypted_settings_ = encrypted_settings();
    if (!encrypted_settings_) { return std::string(); }
    return std::string(encrypted_settings_, m_encrypted_settings_idx__);
}

char* cWscAttrEncryptedSettings::encrypted_settings(size_t length) {
    if( (m_encrypted_settings_idx__ <= 0) || (m_encrypted_settings_idx__ < length) ) {
        TLVF_LOG(ERROR) << "encrypted_settings length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_encrypted_settings);
}

bool cWscAttrEncryptedSettings::set_encrypted_settings(const std::string& str) { return set_encrypted_settings(str.c_str(), str.size()); }
bool cWscAttrEncryptedSettings::set_encrypted_settings(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_encrypted_settings received an empty string.";
        return false;
    }
    if (!alloc_encrypted_settings(size)) { return false; }
    std::copy(str, str + size, m_encrypted_settings);
    return true;
}
bool cWscAttrEncryptedSettings::alloc_encrypted_settings(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list encrypted_settings, abort!";
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
    uint8_t *src = (uint8_t *)m_encrypted_settings;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_encrypted_settings_idx__ += count;
    if (!buffPtrIncrementSafe(len)) { return false; }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrEncryptedSettings::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t cWscAttrEncryptedSettings::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += WSC_ENCRYPTED_SETTINGS_IV_LENGTH * sizeof(char); // iv
    return class_size;
}

bool cWscAttrEncryptedSettings::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = eWscAttributes::ATTR_ENCR_SETTINGS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_iv = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(WSC_ENCRYPTED_SETTINGS_IV_LENGTH))) { return false; }
    m_iv_idx__  = WSC_ENCRYPTED_SETTINGS_IV_LENGTH;
    if (!m_parse__) {
        if (m_length) { (*m_length) += (sizeof(char) * WSC_ENCRYPTED_SETTINGS_IV_LENGTH); }
    }
    m_encrypted_settings = (char*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&len)); }
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_encrypted_settings_idx__ = len/sizeof(char);
        if (!buffPtrIncrementSafe(len)) { return false; }
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eWscAttributes::ATTR_ENCR_SETTINGS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eWscAttributes::ATTR_ENCR_SETTINGS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


