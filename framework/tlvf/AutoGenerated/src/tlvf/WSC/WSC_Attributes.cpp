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

eWscAttributes& cWscAttrEncryptedSettings::ssid_type() {
    return (eWscAttributes&)(*m_ssid_type);
}

uint16_t& cWscAttrEncryptedSettings::ssid_length() {
    return (uint16_t&)(*m_ssid_length);
}

char* cWscAttrEncryptedSettings::ssid(size_t length) {
    if( (m_ssid_idx__ <= 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cWscAttrEncryptedSettings::set_ssid(std::string& str) {
    return set_ssid(const_cast<std::string&>(str));
}
bool cWscAttrEncryptedSettings::set_ssid(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_ssid received an empty string.";
        return false;
    }
    if (!alloc_ssid(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_ssid, str.c_str(), str_size + 1);
    return true;
}
bool cWscAttrEncryptedSettings::set_ssid(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_ssid received an empty string.";
        return false;
    }
    if (!alloc_ssid(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_ssid, str, size + 1);
    m_ssid[size] = '\0';
    return true;
}
bool cWscAttrEncryptedSettings::alloc_ssid(size_t count) {
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
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_ssid;
    uint8_t *dst = (uint8_t *)m_ssid + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_authentication_type_attr = (sWscAttrAuthenticationType *)((uint8_t *)(m_authentication_type_attr) + len);
    m_encryption_type_attr = (sWscAttrEncryptionType *)((uint8_t *)(m_encryption_type_attr) + len);
    m_network_key_attr = (sWscAttrNetworkKey *)((uint8_t *)(m_network_key_attr) + len);
    m_bssid_attr = (sWscAttrBssid *)((uint8_t *)(m_bssid_attr) + len);
    m_key_wrap_auth_attr = (sWscAttrKeyWrapAuthenticator *)((uint8_t *)(m_key_wrap_auth_attr) + len);
    m_ssid_idx__ += count;
    *m_ssid_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    return true;
}

sWscAttrAuthenticationType& cWscAttrEncryptedSettings::authentication_type_attr() {
    return (sWscAttrAuthenticationType&)(*m_authentication_type_attr);
}

sWscAttrEncryptionType& cWscAttrEncryptedSettings::encryption_type_attr() {
    return (sWscAttrEncryptionType&)(*m_encryption_type_attr);
}

sWscAttrNetworkKey& cWscAttrEncryptedSettings::network_key_attr() {
    return (sWscAttrNetworkKey&)(*m_network_key_attr);
}

sWscAttrBssid& cWscAttrEncryptedSettings::bssid_attr() {
    return (sWscAttrBssid&)(*m_bssid_attr);
}

sWscAttrKeyWrapAuthenticator& cWscAttrEncryptedSettings::key_wrap_auth_attr() {
    return (sWscAttrKeyWrapAuthenticator&)(*m_key_wrap_auth_attr);
}

void cWscAttrEncryptedSettings::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_ssid_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_ssid_length));
    m_authentication_type_attr->struct_swap();
    m_encryption_type_attr->struct_swap();
    m_network_key_attr->struct_swap();
    m_bssid_attr->struct_swap();
    m_key_wrap_auth_attr->struct_swap();
}

size_t cWscAttrEncryptedSettings::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eWscAttributes); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eWscAttributes); // ssid_type
    class_size += sizeof(uint16_t); // ssid_length
    class_size += sizeof(sWscAttrAuthenticationType); // authentication_type_attr
    class_size += sizeof(sWscAttrEncryptionType); // encryption_type_attr
    class_size += sizeof(sWscAttrNetworkKey); // network_key_attr
    class_size += sizeof(sWscAttrBssid); // bssid_attr
    class_size += sizeof(sWscAttrKeyWrapAuthenticator); // key_wrap_auth_attr
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
    m_buff_ptr__ += sizeof(eWscAttributes) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_ssid_type = (eWscAttributes*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_type = ATTR_SSID;
    m_buff_ptr__ += sizeof(eWscAttributes) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(eWscAttributes); }
    m_ssid_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_ssid = (char*)m_buff_ptr__;
    uint16_t ssid_length = *m_ssid_length;
    if (m_parse__ && m_swap__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&ssid_length)); }
    m_ssid_idx__ = ssid_length;
    m_buff_ptr__ += sizeof(char)*(ssid_length);
    m_authentication_type_attr = (sWscAttrAuthenticationType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWscAttrAuthenticationType) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sWscAttrAuthenticationType); }
    if (!m_parse__) { m_authentication_type_attr->struct_init(); }
    m_encryption_type_attr = (sWscAttrEncryptionType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWscAttrEncryptionType) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sWscAttrEncryptionType); }
    if (!m_parse__) { m_encryption_type_attr->struct_init(); }
    m_network_key_attr = (sWscAttrNetworkKey*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWscAttrNetworkKey) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sWscAttrNetworkKey); }
    if (!m_parse__) { m_network_key_attr->struct_init(); }
    m_bssid_attr = (sWscAttrBssid*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWscAttrBssid) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sWscAttrBssid); }
    if (!m_parse__) { m_bssid_attr->struct_init(); }
    m_key_wrap_auth_attr = (sWscAttrKeyWrapAuthenticator*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWscAttrKeyWrapAuthenticator) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sWscAttrKeyWrapAuthenticator); }
    if (!m_parse__) { m_key_wrap_auth_attr->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
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


