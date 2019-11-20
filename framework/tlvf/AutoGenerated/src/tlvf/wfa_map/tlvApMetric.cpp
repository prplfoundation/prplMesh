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

#include <tlvf/wfa_map/tlvApMetric.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApMetric::tlvApMetric(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvApMetric::tlvApMetric(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvApMetric::~tlvApMetric() {
}
const eTlvTypeMap& tlvApMetric::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApMetric::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvApMetric::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint8_t& tlvApMetric::channel_utilization() {
    return (uint8_t&)(*m_channel_utilization);
}

uint16_t& tlvApMetric::number_of_stas_currently_associated() {
    return (uint16_t&)(*m_number_of_stas_currently_associated);
}

tlvApMetric::sEstimatedService& tlvApMetric::estimated_service_parameters() {
    return (sEstimatedService&)(*m_estimated_service_parameters);
}

uint8_t* tlvApMetric::estimated_service_info_field(size_t idx) {
    if ( (m_estimated_service_info_field_idx__ <= 0) || (m_estimated_service_info_field_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_estimated_service_info_field[idx]);
}

bool tlvApMetric::alloc_estimated_service_info_field(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list estimated_service_info_field, abort!";
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
    uint8_t *src = (uint8_t *)m_estimated_service_info_field;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_estimated_service_info_field_idx__ += count;
    if (!buffPtrIncrementSafe(len)) { return false; }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvApMetric::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_bssid->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_number_of_stas_currently_associated));
    m_estimated_service_parameters->struct_swap();
}

size_t tlvApMetric::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint8_t); // channel_utilization
    class_size += sizeof(uint16_t); // number_of_stas_currently_associated
    class_size += sizeof(sEstimatedService); // estimated_service_parameters
    return class_size;
}

bool tlvApMetric::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_METRIC;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_channel_utilization = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_number_of_stas_currently_associated = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_estimated_service_parameters = (sEstimatedService*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sEstimatedService))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sEstimatedService); }
    if (!m_parse__) { m_estimated_service_parameters->struct_init(); }
    m_estimated_service_info_field = (uint8_t*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&len)); }
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_estimated_service_info_field_idx__ = len/sizeof(uint8_t);
        if (!buffPtrIncrementSafe(len)) { return false; }
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_AP_METRIC) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_METRIC) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


