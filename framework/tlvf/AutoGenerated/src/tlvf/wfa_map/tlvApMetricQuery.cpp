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

#include <tlvf/wfa_map/tlvApMetricQuery.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApMetricQuery::tlvApMetricQuery(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvApMetricQuery::tlvApMetricQuery(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvApMetricQuery::~tlvApMetricQuery() {
}
const eTlvTypeMap& tlvApMetricQuery::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApMetricQuery::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvApMetricQuery::bssid_list_length() {
    return (uint8_t&)(*m_bssid_list_length);
}

std::tuple<bool, sMacAddr&> tlvApMetricQuery::bssid_list(size_t idx) {
    bool ret_success = ( (m_bssid_list_idx__ > 0) && (m_bssid_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_bssid_list[ret_idx]);
}

bool tlvApMetricQuery::alloc_bssid_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list bssid_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_bssid_list[*m_bssid_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_bssid_list_idx__ += count;
    *m_bssid_list_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_bssid_list_idx__ - count; i < m_bssid_list_idx__; i++) { m_bssid_list[i].struct_init(); }
    }
    return true;
}

void tlvApMetricQuery::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    for (size_t i = 0; i < (size_t)*m_bssid_list_length; i++){
        m_bssid_list[i].struct_swap();
    }
}

size_t tlvApMetricQuery::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // bssid_list_length
    return class_size;
}

bool tlvApMetricQuery::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_METRIC_QUERY;
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_bssid_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_bssid_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bssid_list = (sMacAddr*)m_buff_ptr__;
    uint8_t bssid_list_length = *m_bssid_list_length;
    m_bssid_list_idx__ = bssid_list_length;
    m_buff_ptr__ += sizeof(sMacAddr)*(bssid_list_length);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_AP_METRIC_QUERY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_METRIC_QUERY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


