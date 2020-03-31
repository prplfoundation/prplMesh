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

#include <tlvf/wfa_map/tlvBeaconMetricsQuery.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvBeaconMetricsQuery::tlvBeaconMetricsQuery(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvBeaconMetricsQuery::tlvBeaconMetricsQuery(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvBeaconMetricsQuery::~tlvBeaconMetricsQuery() {
}
const eTlvTypeMap& tlvBeaconMetricsQuery::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvBeaconMetricsQuery::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvBeaconMetricsQuery::associated_sta_mac() {
    return (sMacAddr&)(*m_associated_sta_mac);
}

uint8_t& tlvBeaconMetricsQuery::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& tlvBeaconMetricsQuery::channel_number() {
    return (uint8_t&)(*m_channel_number);
}

sMacAddr& tlvBeaconMetricsQuery::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint8_t& tlvBeaconMetricsQuery::reporting_detail_value() {
    return (uint8_t&)(*m_reporting_detail_value);
}

uint8_t& tlvBeaconMetricsQuery::ssid_length() {
    return (uint8_t&)(*m_ssid_length);
}

std::string tlvBeaconMetricsQuery::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* tlvBeaconMetricsQuery::ssid(size_t length) {
    if( (m_ssid_idx__ == 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool tlvBeaconMetricsQuery::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool tlvBeaconMetricsQuery::set_ssid(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_ssid received a null pointer.";
        return false;
    }
    if (!alloc_ssid(size)) { return false; }
    std::copy(str, str + size, m_ssid);
    return true;
}
bool tlvBeaconMetricsQuery::alloc_ssid(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ssid, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_ssid[*m_ssid_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_ap_channel_reports_list_length = (uint8_t *)((uint8_t *)(m_ap_channel_reports_list_length) + len);
    m_ap_channel_reports_list = (sApChannelReports *)((uint8_t *)(m_ap_channel_reports_list) + len);
    m_elemnt_id_list_length = (uint8_t *)((uint8_t *)(m_elemnt_id_list_length) + len);
    m_elemnt_id_list = (uint8_t *)((uint8_t *)(m_elemnt_id_list) + len);
    m_ssid_idx__ += count;
    *m_ssid_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

uint8_t& tlvBeaconMetricsQuery::ap_channel_reports_list_length() {
    return (uint8_t&)(*m_ap_channel_reports_list_length);
}

std::tuple<bool, tlvBeaconMetricsQuery::sApChannelReports&> tlvBeaconMetricsQuery::ap_channel_reports_list(size_t idx) {
    bool ret_success = ( (m_ap_channel_reports_list_idx__ > 0) && (m_ap_channel_reports_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_ap_channel_reports_list[ret_idx]);
}

bool tlvBeaconMetricsQuery::alloc_ap_channel_reports_list(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ap_channel_reports_list, abort!";
        return false;
    }
    size_t len = sizeof(sApChannelReports) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_ap_channel_reports_list[*m_ap_channel_reports_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_elemnt_id_list_length = (uint8_t *)((uint8_t *)(m_elemnt_id_list_length) + len);
    m_elemnt_id_list = (uint8_t *)((uint8_t *)(m_elemnt_id_list) + len);
    m_ap_channel_reports_list_idx__ += count;
    *m_ap_channel_reports_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_ap_channel_reports_list_idx__ - count; i < m_ap_channel_reports_list_idx__; i++) { m_ap_channel_reports_list[i].struct_init(); }
    }
    return true;
}

uint8_t& tlvBeaconMetricsQuery::elemnt_id_list_length() {
    return (uint8_t&)(*m_elemnt_id_list_length);
}

uint8_t* tlvBeaconMetricsQuery::elemnt_id_list(size_t idx) {
    if ( (m_elemnt_id_list_idx__ == 0) || (m_elemnt_id_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_elemnt_id_list[idx]);
}

bool tlvBeaconMetricsQuery::set_elemnt_id_list(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_elemnt_id_list received a null pointer.";
        return false;
    }
    if (!alloc_elemnt_id_list(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_elemnt_id_list);
    return true;
}
bool tlvBeaconMetricsQuery::alloc_elemnt_id_list(size_t count) {
    if (m_lock_order_counter__ > 2) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list elemnt_id_list, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 2;
    uint8_t *src = (uint8_t *)&m_elemnt_id_list[*m_elemnt_id_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_elemnt_id_list_idx__ += count;
    *m_elemnt_id_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvBeaconMetricsQuery::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_associated_sta_mac->struct_swap();
    m_bssid->struct_swap();
    for (size_t i = 0; i < (size_t)*m_ap_channel_reports_list_length; i++){
        m_ap_channel_reports_list[i].struct_swap();
    }
}

bool tlvBeaconMetricsQuery::finalize()
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

size_t tlvBeaconMetricsQuery::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // associated_sta_mac
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // channel_number
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint8_t); // reporting_detail_value
    class_size += sizeof(uint8_t); // ssid_length
    class_size += sizeof(uint8_t); // ap_channel_reports_list_length
    class_size += sizeof(uint8_t); // elemnt_id_list_length
    return class_size;
}

bool tlvBeaconMetricsQuery::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_BEACON_METRICS_QUERY;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_associated_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_associated_sta_mac->struct_init(); }
    m_operating_class = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_channel_number = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_reporting_detail_value = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_ssid_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_ssid = (char*)m_buff_ptr__;
    uint8_t ssid_length = *m_ssid_length;
    m_ssid_idx__ = ssid_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (ssid_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (ssid_length) << ") Failed!";
        return false;
    }
    m_ap_channel_reports_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_ap_channel_reports_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_ap_channel_reports_list = (sApChannelReports*)m_buff_ptr__;
    uint8_t ap_channel_reports_list_length = *m_ap_channel_reports_list_length;
    m_ap_channel_reports_list_idx__ = ap_channel_reports_list_length;
    if (!buffPtrIncrementSafe(sizeof(sApChannelReports) * (ap_channel_reports_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelReports) * (ap_channel_reports_list_length) << ") Failed!";
        return false;
    }
    m_elemnt_id_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_elemnt_id_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_elemnt_id_list = (uint8_t*)m_buff_ptr__;
    uint8_t elemnt_id_list_length = *m_elemnt_id_list_length;
    m_elemnt_id_list_idx__ = elemnt_id_list_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (elemnt_id_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (elemnt_id_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_BEACON_METRICS_QUERY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_BEACON_METRICS_QUERY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


