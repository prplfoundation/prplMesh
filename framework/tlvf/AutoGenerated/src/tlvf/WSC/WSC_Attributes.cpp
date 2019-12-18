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

cConfigData::cConfigData(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cConfigData::cConfigData(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
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

bool cConfigData::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
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
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ssid_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_type = ATTR_SSID;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_ssid_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_ssid = (char*)m_buff_ptr__;
    uint16_t ssid_length = *m_ssid_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&ssid_length)); }
    m_ssid_idx__ = ssid_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (ssid_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (ssid_length) << ") Failed!";
        return false;
    }
    m_authentication_type_attr = (sWscAttrAuthenticationType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrAuthenticationType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWscAttrAuthenticationType) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_authentication_type_attr->struct_init(); }
    m_encryption_type_attr = (sWscAttrEncryptionType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrEncryptionType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWscAttrEncryptionType) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_encryption_type_attr->struct_init(); }
    m_network_key_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_network_key_type = ATTR_NETWORK_KEY;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_network_key_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_network_key_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_network_key = (char*)m_buff_ptr__;
    uint16_t network_key_length = *m_network_key_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&network_key_length)); }
    m_network_key_idx__ = network_key_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (network_key_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (network_key_length) << ") Failed!";
        return false;
    }
    m_bssid_attr = (sWscAttrBssid*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrBssid))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWscAttrBssid) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid_attr->struct_init(); }
    m_multiap_attr = (sWscAttrVendorExtMultiAp*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWscAttrVendorExtMultiAp))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWscAttrVendorExtMultiAp) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_multiap_attr->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrEncryptedSettings::cWscAttrEncryptedSettings(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrEncryptedSettings::cWscAttrEncryptedSettings(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrEncryptedSettings::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrEncryptedSettings::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
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
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = eWscAttributes::ATTR_ENCR_SETTINGS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_iv = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (WSC_ENCRYPTED_SETTINGS_IV_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (WSC_ENCRYPTED_SETTINGS_IV_LENGTH) << ") Failed!";
        return false;
    }
    m_iv_idx__  = WSC_ENCRYPTED_SETTINGS_IV_LENGTH;
    if (!m_parse__) {
        if (m_length) { (*m_length) += (sizeof(char) * WSC_ENCRYPTED_SETTINGS_IV_LENGTH); }
    }
    m_encrypted_settings = (char*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&len));
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_encrypted_settings_idx__ = len/sizeof(char);
        if (!buffPtrIncrementSafe(len)) {
            LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
            return false;
        }
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eWscAttributes::ATTR_ENCR_SETTINGS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eWscAttributes::ATTR_ENCR_SETTINGS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cWscVendorExtWfa::cWscVendorExtWfa(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscVendorExtWfa::cWscVendorExtWfa(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscVendorExtWfa::~cWscVendorExtWfa() {
}
eWscAttributes& cWscVendorExtWfa::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscVendorExtWfa::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& cWscVendorExtWfa::vendor_id_0() {
    return (uint8_t&)(*m_vendor_id_0);
}

uint8_t& cWscVendorExtWfa::vendor_id_1() {
    return (uint8_t&)(*m_vendor_id_1);
}

uint8_t& cWscVendorExtWfa::vendor_id_2() {
    return (uint8_t&)(*m_vendor_id_2);
}

uint8_t* cWscVendorExtWfa::vs_data(size_t idx) {
    if ( (m_vs_data_idx__ <= 0) || (m_vs_data_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_vs_data[idx]);
}

bool cWscVendorExtWfa::alloc_vs_data(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list vs_data, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_vs_data;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_vs_data_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscVendorExtWfa::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscVendorExtWfa::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscVendorExtWfa::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // vendor_id_0
    class_size += sizeof(uint8_t); // vendor_id_1
    class_size += sizeof(uint8_t); // vendor_id_2
    return class_size;
}

bool cWscVendorExtWfa::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_VENDOR_EXTENSION;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_vendor_id_0 = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_vendor_id_0 = WSC_VENDOR_ID_WFA_1;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_vendor_id_1 = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_vendor_id_1 = WSC_VENDOR_ID_WFA_2;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_vendor_id_2 = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_vendor_id_2 = WSC_VENDOR_ID_WFA_3;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_vs_data = (uint8_t*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&len));
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_vs_data_idx__ = len/sizeof(uint8_t);
        if (!buffPtrIncrementSafe(len)) {
            LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
            return false;
        }
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrVersion::cWscAttrVersion(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrVersion::cWscAttrVersion(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrVersion::~cWscAttrVersion() {
}
eWscAttributes& cWscAttrVersion::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrVersion::length() {
    return (const uint16_t&)(*m_length);
}

eWscValues8& cWscAttrVersion::data() {
    return (eWscValues8&)(*m_data);
}

void cWscAttrVersion::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrVersion::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrVersion::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscValues8); // data
    return class_size;
}

bool cWscAttrVersion::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_VERSION;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_data = (eWscValues8*)m_buff_ptr__;
    if (!m_parse__) *m_data = WSC_VERSION;
    if (!buffPtrIncrementSafe(sizeof(eWscValues8))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscValues8) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscValues8); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrMessageType::cWscAttrMessageType(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrMessageType::cWscAttrMessageType(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrMessageType::~cWscAttrMessageType() {
}
eWscAttributes& cWscAttrMessageType::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrMessageType::length() {
    return (const uint16_t&)(*m_length);
}

eWscMessageType& cWscAttrMessageType::msg_type() {
    return (eWscMessageType&)(*m_msg_type);
}

void cWscAttrMessageType::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrMessageType::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrMessageType::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscMessageType); // msg_type
    return class_size;
}

bool cWscAttrMessageType::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_MSG_TYPE;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_msg_type = (eWscMessageType*)m_buff_ptr__;
    if (!m_parse__) *m_msg_type = WSC_MSG_TYPE_INVALID;
    if (!buffPtrIncrementSafe(sizeof(eWscMessageType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscMessageType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscMessageType); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrEnrolleeNonce::cWscAttrEnrolleeNonce(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrEnrolleeNonce::cWscAttrEnrolleeNonce(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrEnrolleeNonce::~cWscAttrEnrolleeNonce() {
}
eWscAttributes& cWscAttrEnrolleeNonce::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrEnrolleeNonce::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t* cWscAttrEnrolleeNonce::nonce(size_t idx) {
    if ( (m_nonce_idx__ <= 0) || (m_nonce_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_nonce[idx]);
}

void cWscAttrEnrolleeNonce::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrEnrolleeNonce::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrEnrolleeNonce::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += WSC_NONCE_LENGTH * sizeof(uint8_t); // nonce
    return class_size;
}

bool cWscAttrEnrolleeNonce::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_ENROLLEE_NONCE;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_nonce = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (WSC_NONCE_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (WSC_NONCE_LENGTH) << ") Failed!";
        return false;
    }
    m_nonce_idx__  = WSC_NONCE_LENGTH;
    if (!m_parse__) {
        if (m_length) { (*m_length) += (sizeof(uint8_t) * WSC_NONCE_LENGTH); }
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrPublicKey::cWscAttrPublicKey(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrPublicKey::cWscAttrPublicKey(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrPublicKey::~cWscAttrPublicKey() {
}
eWscAttributes& cWscAttrPublicKey::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrPublicKey::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t* cWscAttrPublicKey::public_key(size_t idx) {
    if ( (m_public_key_idx__ <= 0) || (m_public_key_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_public_key[idx]);
}

void cWscAttrPublicKey::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrPublicKey::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrPublicKey::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += WSC_PUBLIC_KEY_LENGTH * sizeof(uint8_t); // public_key
    return class_size;
}

bool cWscAttrPublicKey::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_PUBLIC_KEY;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_public_key = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (WSC_PUBLIC_KEY_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (WSC_PUBLIC_KEY_LENGTH) << ") Failed!";
        return false;
    }
    m_public_key_idx__  = WSC_PUBLIC_KEY_LENGTH;
    if (!m_parse__) {
        if (m_length) { (*m_length) += (sizeof(uint8_t) * WSC_PUBLIC_KEY_LENGTH); }
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrAuthenticationTypeFlags::cWscAttrAuthenticationTypeFlags(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrAuthenticationTypeFlags::cWscAttrAuthenticationTypeFlags(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrAuthenticationTypeFlags::~cWscAttrAuthenticationTypeFlags() {
}
eWscAttributes& cWscAttrAuthenticationTypeFlags::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrAuthenticationTypeFlags::length() {
    return (const uint16_t&)(*m_length);
}

uint16_t& cWscAttrAuthenticationTypeFlags::auth_type_flags() {
    return (uint16_t&)(*m_auth_type_flags);
}

void cWscAttrAuthenticationTypeFlags::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_auth_type_flags));
}

bool cWscAttrAuthenticationTypeFlags::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrAuthenticationTypeFlags::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // auth_type_flags
    return class_size;
}

bool cWscAttrAuthenticationTypeFlags::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_AUTH_TYPE_FLAGS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_auth_type_flags = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_auth_type_flags = uint16_t(eWscAuth::WSC_AUTH_OPEN) | uint16_t(eWscAuth::WSC_AUTH_WPA2PSK);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrEncryptionTypeFlags::cWscAttrEncryptionTypeFlags(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrEncryptionTypeFlags::cWscAttrEncryptionTypeFlags(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrEncryptionTypeFlags::~cWscAttrEncryptionTypeFlags() {
}
eWscAttributes& cWscAttrEncryptionTypeFlags::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrEncryptionTypeFlags::length() {
    return (const uint16_t&)(*m_length);
}

uint16_t& cWscAttrEncryptionTypeFlags::encr_type_flags() {
    return (uint16_t&)(*m_encr_type_flags);
}

void cWscAttrEncryptionTypeFlags::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_encr_type_flags));
}

bool cWscAttrEncryptionTypeFlags::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrEncryptionTypeFlags::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // encr_type_flags
    return class_size;
}

bool cWscAttrEncryptionTypeFlags::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_ENCR_TYPE_FLAGS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_encr_type_flags = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_encr_type_flags = uint16_t(eWscEncr::WSC_ENCR_NONE) | uint16_t(eWscEncr::WSC_ENCR_AES);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrConnectionTypeFlags::cWscAttrConnectionTypeFlags(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrConnectionTypeFlags::cWscAttrConnectionTypeFlags(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrConnectionTypeFlags::~cWscAttrConnectionTypeFlags() {
}
eWscAttributes& cWscAttrConnectionTypeFlags::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrConnectionTypeFlags::length() {
    return (const uint16_t&)(*m_length);
}

eWscConn& cWscAttrConnectionTypeFlags::conn_type_flags() {
    return (eWscConn&)(*m_conn_type_flags);
}

void cWscAttrConnectionTypeFlags::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrConnectionTypeFlags::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrConnectionTypeFlags::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscConn); // conn_type_flags
    return class_size;
}

bool cWscAttrConnectionTypeFlags::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_CONN_TYPE_FLAGS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_conn_type_flags = (eWscConn*)m_buff_ptr__;
    if (!m_parse__) *m_conn_type_flags = WSC_CONN_ESS;
    if (!buffPtrIncrementSafe(sizeof(eWscConn))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscConn) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscConn); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrConfigurationMethods::cWscAttrConfigurationMethods(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrConfigurationMethods::cWscAttrConfigurationMethods(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrConfigurationMethods::~cWscAttrConfigurationMethods() {
}
eWscAttributes& cWscAttrConfigurationMethods::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrConfigurationMethods::length() {
    return (const uint16_t&)(*m_length);
}

uint16_t& cWscAttrConfigurationMethods::conf_methods() {
    return (uint16_t&)(*m_conf_methods);
}

void cWscAttrConfigurationMethods::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_conf_methods));
}

bool cWscAttrConfigurationMethods::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrConfigurationMethods::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // conf_methods
    return class_size;
}

bool cWscAttrConfigurationMethods::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_CONFIG_METHODS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_conf_methods = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_conf_methods = (WSC_CONFIG_PHY_PUSHBUTTON | WSC_CONFIG_VIRT_PUSHBUTTON);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrManufacturer::cWscAttrManufacturer(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrManufacturer::cWscAttrManufacturer(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrManufacturer::~cWscAttrManufacturer() {
}
eWscAttributes& cWscAttrManufacturer::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrManufacturer::length() {
    return (const uint16_t&)(*m_length);
}

std::string cWscAttrManufacturer::manufacturer_str() {
    char *manufacturer_ = manufacturer();
    if (!manufacturer_) { return std::string(); }
    return std::string(manufacturer_, m_manufacturer_idx__);
}

char* cWscAttrManufacturer::manufacturer(size_t length) {
    if( (m_manufacturer_idx__ <= 0) || (m_manufacturer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "manufacturer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_manufacturer);
}

bool cWscAttrManufacturer::set_manufacturer(const std::string& str) { return set_manufacturer(str.c_str(), str.size()); }
bool cWscAttrManufacturer::set_manufacturer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_manufacturer received an empty string.";
        return false;
    }
    if (!alloc_manufacturer(size)) { return false; }
    std::copy(str, str + size, m_manufacturer);
    return true;
}
bool cWscAttrManufacturer::alloc_manufacturer(size_t count) {
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
    uint8_t *src = (uint8_t *)&m_manufacturer[*m_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_manufacturer_idx__ += count;
    *m_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrManufacturer::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrManufacturer::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrManufacturer::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool cWscAttrManufacturer::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_MANUFACTURER;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_manufacturer = (char*)m_buff_ptr__;
    uint16_t length = *m_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&length)); }
    m_manufacturer_idx__ = length;
    if (!buffPtrIncrementSafe(sizeof(char) * (length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrModelName::cWscAttrModelName(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrModelName::cWscAttrModelName(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrModelName::~cWscAttrModelName() {
}
eWscAttributes& cWscAttrModelName::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrModelName::length() {
    return (const uint16_t&)(*m_length);
}

std::string cWscAttrModelName::model_str() {
    char *model_ = model();
    if (!model_) { return std::string(); }
    return std::string(model_, m_model_idx__);
}

char* cWscAttrModelName::model(size_t length) {
    if( (m_model_idx__ <= 0) || (m_model_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model);
}

bool cWscAttrModelName::set_model(const std::string& str) { return set_model(str.c_str(), str.size()); }
bool cWscAttrModelName::set_model(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_model received an empty string.";
        return false;
    }
    if (!alloc_model(size)) { return false; }
    std::copy(str, str + size, m_model);
    return true;
}
bool cWscAttrModelName::alloc_model(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list model, abort!";
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
    uint8_t *src = (uint8_t *)&m_model[*m_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_model_idx__ += count;
    *m_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrModelName::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrModelName::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrModelName::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool cWscAttrModelName::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_MODEL_NAME;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_model = (char*)m_buff_ptr__;
    uint16_t length = *m_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&length)); }
    m_model_idx__ = length;
    if (!buffPtrIncrementSafe(sizeof(char) * (length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrModelNumber::cWscAttrModelNumber(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrModelNumber::cWscAttrModelNumber(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrModelNumber::~cWscAttrModelNumber() {
}
eWscAttributes& cWscAttrModelNumber::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrModelNumber::length() {
    return (const uint16_t&)(*m_length);
}

std::string cWscAttrModelNumber::model_number_str() {
    char *model_number_ = model_number();
    if (!model_number_) { return std::string(); }
    return std::string(model_number_, m_model_number_idx__);
}

char* cWscAttrModelNumber::model_number(size_t length) {
    if( (m_model_number_idx__ <= 0) || (m_model_number_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model_number length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model_number);
}

bool cWscAttrModelNumber::set_model_number(const std::string& str) { return set_model_number(str.c_str(), str.size()); }
bool cWscAttrModelNumber::set_model_number(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_model_number received an empty string.";
        return false;
    }
    if (!alloc_model_number(size)) { return false; }
    std::copy(str, str + size, m_model_number);
    return true;
}
bool cWscAttrModelNumber::alloc_model_number(size_t count) {
    if (m_lock_order_counter__ > 0) {;
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
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_model_number[*m_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_model_number_idx__ += count;
    *m_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrModelNumber::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrModelNumber::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrModelNumber::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool cWscAttrModelNumber::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_MODEL_NUMBER;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_model_number = (char*)m_buff_ptr__;
    uint16_t length = *m_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&length)); }
    m_model_number_idx__ = length;
    if (!buffPtrIncrementSafe(sizeof(char) * (length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrSerialNumber::cWscAttrSerialNumber(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrSerialNumber::cWscAttrSerialNumber(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrSerialNumber::~cWscAttrSerialNumber() {
}
eWscAttributes& cWscAttrSerialNumber::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrSerialNumber::length() {
    return (const uint16_t&)(*m_length);
}

std::string cWscAttrSerialNumber::model_number_str() {
    char *model_number_ = model_number();
    if (!model_number_) { return std::string(); }
    return std::string(model_number_, m_model_number_idx__);
}

char* cWscAttrSerialNumber::model_number(size_t length) {
    if( (m_model_number_idx__ <= 0) || (m_model_number_idx__ < length) ) {
        TLVF_LOG(ERROR) << "model_number length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_model_number);
}

bool cWscAttrSerialNumber::set_model_number(const std::string& str) { return set_model_number(str.c_str(), str.size()); }
bool cWscAttrSerialNumber::set_model_number(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_model_number received an empty string.";
        return false;
    }
    if (!alloc_model_number(size)) { return false; }
    std::copy(str, str + size, m_model_number);
    return true;
}
bool cWscAttrSerialNumber::alloc_model_number(size_t count) {
    if (m_lock_order_counter__ > 0) {;
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
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_model_number[*m_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_model_number_idx__ += count;
    *m_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrSerialNumber::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrSerialNumber::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrSerialNumber::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool cWscAttrSerialNumber::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_SERIAL_NUMBER;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_model_number = (char*)m_buff_ptr__;
    uint16_t length = *m_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&length)); }
    m_model_number_idx__ = length;
    if (!buffPtrIncrementSafe(sizeof(char) * (length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrPrimaryDeviceType::cWscAttrPrimaryDeviceType(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrPrimaryDeviceType::cWscAttrPrimaryDeviceType(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrPrimaryDeviceType::~cWscAttrPrimaryDeviceType() {
}
eWscAttributes& cWscAttrPrimaryDeviceType::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrPrimaryDeviceType::length() {
    return (const uint16_t&)(*m_length);
}

uint16_t& cWscAttrPrimaryDeviceType::category_id() {
    return (uint16_t&)(*m_category_id);
}

uint32_t& cWscAttrPrimaryDeviceType::oui() {
    return (uint32_t&)(*m_oui);
}

uint16_t& cWscAttrPrimaryDeviceType::sub_category_id() {
    return (uint16_t&)(*m_sub_category_id);
}

void cWscAttrPrimaryDeviceType::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_category_id));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_oui));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_sub_category_id));
}

bool cWscAttrPrimaryDeviceType::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrPrimaryDeviceType::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // category_id
    class_size += sizeof(uint32_t); // oui
    class_size += sizeof(uint16_t); // sub_category_id
    return class_size;
}

bool cWscAttrPrimaryDeviceType::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_PRIMARY_DEV_TYPE;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_category_id = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_category_id = WSC_DEV_NETWORK_INFRA;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_oui = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_oui = 0x50f204;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_sub_category_id = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrDeviceName::cWscAttrDeviceName(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrDeviceName::cWscAttrDeviceName(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrDeviceName::~cWscAttrDeviceName() {
}
eWscAttributes& cWscAttrDeviceName::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrDeviceName::length() {
    return (const uint16_t&)(*m_length);
}

std::string cWscAttrDeviceName::device_name_str() {
    char *device_name_ = device_name();
    if (!device_name_) { return std::string(); }
    return std::string(device_name_, m_device_name_idx__);
}

char* cWscAttrDeviceName::device_name(size_t length) {
    if( (m_device_name_idx__ <= 0) || (m_device_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "device_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_device_name);
}

bool cWscAttrDeviceName::set_device_name(const std::string& str) { return set_device_name(str.c_str(), str.size()); }
bool cWscAttrDeviceName::set_device_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_device_name received an empty string.";
        return false;
    }
    if (!alloc_device_name(size)) { return false; }
    std::copy(str, str + size, m_device_name);
    return true;
}
bool cWscAttrDeviceName::alloc_device_name(size_t count) {
    if (m_lock_order_counter__ > 0) {;
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
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_device_name[*m_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_device_name_idx__ += count;
    *m_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cWscAttrDeviceName::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrDeviceName::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrDeviceName::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool cWscAttrDeviceName::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_DEV_NAME;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_device_name = (char*)m_buff_ptr__;
    uint16_t length = *m_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&length)); }
    m_device_name_idx__ = length;
    if (!buffPtrIncrementSafe(sizeof(char) * (length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrRfBands::cWscAttrRfBands(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrRfBands::cWscAttrRfBands(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrRfBands::~cWscAttrRfBands() {
}
eWscAttributes& cWscAttrRfBands::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrRfBands::length() {
    return (const uint16_t&)(*m_length);
}

eWscRfBands& cWscAttrRfBands::bands() {
    return (eWscRfBands&)(*m_bands);
}

void cWscAttrRfBands::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cWscAttrRfBands::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrRfBands::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscRfBands); // bands
    return class_size;
}

bool cWscAttrRfBands::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_RF_BANDS;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_bands = (eWscRfBands*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eWscRfBands))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscRfBands) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscRfBands); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrAssociationState::cWscAttrAssociationState(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrAssociationState::cWscAttrAssociationState(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrAssociationState::~cWscAttrAssociationState() {
}
eWscAttributes& cWscAttrAssociationState::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrAssociationState::length() {
    return (const uint16_t&)(*m_length);
}

eWscAssoc& cWscAttrAssociationState::assoc_state() {
    return (eWscAssoc&)(*m_assoc_state);
}

void cWscAttrAssociationState::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_assoc_state));
}

bool cWscAttrAssociationState::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrAssociationState::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscAssoc); // assoc_state
    return class_size;
}

bool cWscAttrAssociationState::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_ASSOC_STATE;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_assoc_state = (eWscAssoc*)m_buff_ptr__;
    if (!m_parse__) *m_assoc_state = WSC_ASSOC_NOT_ASSOC;
    if (!buffPtrIncrementSafe(sizeof(eWscAssoc))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAssoc) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscAssoc); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrDevicePasswordID::cWscAttrDevicePasswordID(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrDevicePasswordID::cWscAttrDevicePasswordID(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrDevicePasswordID::~cWscAttrDevicePasswordID() {
}
eWscAttributes& cWscAttrDevicePasswordID::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrDevicePasswordID::length() {
    return (const uint16_t&)(*m_length);
}

eWscValues16& cWscAttrDevicePasswordID::pw() {
    return (eWscValues16&)(*m_pw);
}

void cWscAttrDevicePasswordID::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_pw));
}

bool cWscAttrDevicePasswordID::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrDevicePasswordID::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscValues16); // pw
    return class_size;
}

bool cWscAttrDevicePasswordID::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_DEV_PASSWORD_ID;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_pw = (eWscValues16*)m_buff_ptr__;
    if (!m_parse__) *m_pw = DEV_PW_PUSHBUTTON;
    if (!buffPtrIncrementSafe(sizeof(eWscValues16))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscValues16) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscValues16); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrConfigurationError::cWscAttrConfigurationError(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrConfigurationError::cWscAttrConfigurationError(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrConfigurationError::~cWscAttrConfigurationError() {
}
eWscAttributes& cWscAttrConfigurationError::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrConfigurationError::length() {
    return (const uint16_t&)(*m_length);
}

eWscValues16& cWscAttrConfigurationError::cfg_err() {
    return (eWscValues16&)(*m_cfg_err);
}

void cWscAttrConfigurationError::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_cfg_err));
}

bool cWscAttrConfigurationError::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrConfigurationError::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscValues16); // cfg_err
    return class_size;
}

bool cWscAttrConfigurationError::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_CONFIG_ERROR;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_cfg_err = (eWscValues16*)m_buff_ptr__;
    if (!m_parse__) *m_cfg_err = WSC_CFG_NO_ERROR;
    if (!buffPtrIncrementSafe(sizeof(eWscValues16))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscValues16) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscValues16); }
    if (m_parse__) { class_swap(); }
    return true;
}

cWscAttrOsVersion::cWscAttrOsVersion(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cWscAttrOsVersion::cWscAttrOsVersion(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cWscAttrOsVersion::~cWscAttrOsVersion() {
}
eWscAttributes& cWscAttrOsVersion::type() {
    return (eWscAttributes&)(*m_type);
}

const uint16_t& cWscAttrOsVersion::length() {
    return (const uint16_t&)(*m_length);
}

uint32_t& cWscAttrOsVersion::os_version() {
    return (uint32_t&)(*m_os_version);
}

void cWscAttrOsVersion::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_os_version));
}

bool cWscAttrOsVersion::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cWscAttrOsVersion::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint32_t); // os_version
    return class_size;
}

bool cWscAttrOsVersion::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_type = ATTR_OS_VERSION;
    if (!buffPtrIncrementSafe(sizeof(eWscAttributes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eWscAttributes) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_os_version = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_os_version = 0x80000001;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    if (m_parse__) { class_swap(); }
    return true;
}


