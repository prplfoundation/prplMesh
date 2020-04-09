///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvSteeringRequest.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvSteeringRequest::tlvSteeringRequest(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvSteeringRequest::tlvSteeringRequest(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvSteeringRequest::~tlvSteeringRequest() {
}
const eTlvTypeMap& tlvSteeringRequest::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvSteeringRequest::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvSteeringRequest::bssid() {
    return (sMacAddr&)(*m_bssid);
}

tlvSteeringRequest::sRequestFlags& tlvSteeringRequest::request_flags() {
    return (sRequestFlags&)(*m_request_flags);
}

uint16_t& tlvSteeringRequest::steering_opportunity_window_sec() {
    return (uint16_t&)(*m_steering_opportunity_window_sec);
}

uint16_t& tlvSteeringRequest::btm_disassociation_timer_ms() {
    return (uint16_t&)(*m_btm_disassociation_timer_ms);
}

uint8_t& tlvSteeringRequest::sta_list_length() {
    return (uint8_t&)(*m_sta_list_length);
}

std::tuple<bool, sMacAddr&> tlvSteeringRequest::sta_list(size_t idx) {
    bool ret_success = ( (m_sta_list_idx__ > 0) && (m_sta_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_sta_list[ret_idx]);
}

bool tlvSteeringRequest::alloc_sta_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list sta_list, abort!";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_sta_list[*m_sta_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_target_bssid_list_length = (uint8_t *)((uint8_t *)(m_target_bssid_list_length) + len);
    m_target_bssid_list = (sTargetBssidInfo *)((uint8_t *)(m_target_bssid_list) + len);
    m_sta_list_idx__ += count;
    *m_sta_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_sta_list_idx__ - count; i < m_sta_list_idx__; i++) { m_sta_list[i].struct_init(); }
    }
    return true;
}

uint8_t& tlvSteeringRequest::target_bssid_list_length() {
    return (uint8_t&)(*m_target_bssid_list_length);
}

std::tuple<bool, tlvSteeringRequest::sTargetBssidInfo&> tlvSteeringRequest::target_bssid_list(size_t idx) {
    bool ret_success = ( (m_target_bssid_list_idx__ > 0) && (m_target_bssid_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_target_bssid_list[ret_idx]);
}

bool tlvSteeringRequest::alloc_target_bssid_list(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list target_bssid_list, abort!";
        return false;
    }
    size_t len = sizeof(sTargetBssidInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_target_bssid_list[*m_target_bssid_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_target_bssid_list_idx__ += count;
    *m_target_bssid_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_target_bssid_list_idx__ - count; i < m_target_bssid_list_idx__; i++) { m_target_bssid_list[i].struct_init(); }
    }
    return true;
}

void tlvSteeringRequest::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_bssid->struct_swap();
    m_request_flags->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_steering_opportunity_window_sec));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_btm_disassociation_timer_ms));
    for (size_t i = 0; i < (size_t)*m_sta_list_length; i++){
        m_sta_list[i].struct_swap();
    }
    for (size_t i = 0; i < (size_t)*m_target_bssid_list_length; i++){
        m_target_bssid_list[i].struct_swap();
    }
}

bool tlvSteeringRequest::finalize()
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

size_t tlvSteeringRequest::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(sRequestFlags); // request_flags
    class_size += sizeof(uint16_t); // steering_opportunity_window_sec
    class_size += sizeof(uint16_t); // btm_disassociation_timer_ms
    class_size += sizeof(uint8_t); // sta_list_length
    class_size += sizeof(uint8_t); // target_bssid_list_length
    return class_size;
}

bool tlvSteeringRequest::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_STEERING_REQUEST;
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
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_request_flags = (sRequestFlags*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sRequestFlags))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRequestFlags) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sRequestFlags); }
    if (!m_parse__) { m_request_flags->struct_init(); }
    m_steering_opportunity_window_sec = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_btm_disassociation_timer_ms = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_sta_list_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_sta_list = (sMacAddr*)m_buff_ptr__;
    uint8_t sta_list_length = *m_sta_list_length;
    m_sta_list_idx__ = sta_list_length;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr) * (sta_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) * (sta_list_length) << ") Failed!";
        return false;
    }
    m_target_bssid_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_target_bssid_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_target_bssid_list = (sTargetBssidInfo*)m_buff_ptr__;
    uint8_t target_bssid_list_length = *m_target_bssid_list_length;
    m_target_bssid_list_idx__ = target_bssid_list_length;
    if (!buffPtrIncrementSafe(sizeof(sTargetBssidInfo) * (target_bssid_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sTargetBssidInfo) * (target_bssid_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_STEERING_REQUEST) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_STEERING_REQUEST) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


