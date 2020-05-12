///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvSteeringPolicy.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvSteeringPolicy::tlvSteeringPolicy(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvSteeringPolicy::tlvSteeringPolicy(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvSteeringPolicy::~tlvSteeringPolicy() {
}
const eTlvTypeMap& tlvSteeringPolicy::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvSteeringPolicy::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvSteeringPolicy::local_steering_disallowed_sta_list_length() {
    return (uint8_t&)(*m_local_steering_disallowed_sta_list_length);
}

std::tuple<bool, sMacAddr&> tlvSteeringPolicy::local_steering_disallowed_sta_list(size_t idx) {
    bool ret_success = ( (m_local_steering_disallowed_sta_list_idx__ > 0) && (m_local_steering_disallowed_sta_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_local_steering_disallowed_sta_list[ret_idx]);
}

bool tlvSteeringPolicy::alloc_local_steering_disallowed_sta_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list local_steering_disallowed_sta_list, abort!";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_local_steering_disallowed_sta_list[*m_local_steering_disallowed_sta_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_btm_steering_disallowed_sta_list_length = (uint8_t *)((uint8_t *)(m_btm_steering_disallowed_sta_list_length) + len);
    m_btm_steering_disallowed_sta_list = (sMacAddr *)((uint8_t *)(m_btm_steering_disallowed_sta_list) + len);
    m_radio_ap_control_policy_list_length = (uint8_t *)((uint8_t *)(m_radio_ap_control_policy_list_length) + len);
    m_radio_ap_control_policy_list = (sRadioApControlPolicy *)((uint8_t *)(m_radio_ap_control_policy_list) + len);
    m_local_steering_disallowed_sta_list_idx__ += count;
    *m_local_steering_disallowed_sta_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_local_steering_disallowed_sta_list_idx__ - count; i < m_local_steering_disallowed_sta_list_idx__; i++) { m_local_steering_disallowed_sta_list[i].struct_init(); }
    }
    return true;
}

uint8_t& tlvSteeringPolicy::btm_steering_disallowed_sta_list_length() {
    return (uint8_t&)(*m_btm_steering_disallowed_sta_list_length);
}

std::tuple<bool, sMacAddr&> tlvSteeringPolicy::btm_steering_disallowed_sta_list(size_t idx) {
    bool ret_success = ( (m_btm_steering_disallowed_sta_list_idx__ > 0) && (m_btm_steering_disallowed_sta_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_btm_steering_disallowed_sta_list[ret_idx]);
}

bool tlvSteeringPolicy::alloc_btm_steering_disallowed_sta_list(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list btm_steering_disallowed_sta_list, abort!";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_btm_steering_disallowed_sta_list[*m_btm_steering_disallowed_sta_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_radio_ap_control_policy_list_length = (uint8_t *)((uint8_t *)(m_radio_ap_control_policy_list_length) + len);
    m_radio_ap_control_policy_list = (sRadioApControlPolicy *)((uint8_t *)(m_radio_ap_control_policy_list) + len);
    m_btm_steering_disallowed_sta_list_idx__ += count;
    *m_btm_steering_disallowed_sta_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_btm_steering_disallowed_sta_list_idx__ - count; i < m_btm_steering_disallowed_sta_list_idx__; i++) { m_btm_steering_disallowed_sta_list[i].struct_init(); }
    }
    return true;
}

uint8_t& tlvSteeringPolicy::radio_ap_control_policy_list_length() {
    return (uint8_t&)(*m_radio_ap_control_policy_list_length);
}

std::tuple<bool, tlvSteeringPolicy::sRadioApControlPolicy&> tlvSteeringPolicy::radio_ap_control_policy_list(size_t idx) {
    bool ret_success = ( (m_radio_ap_control_policy_list_idx__ > 0) && (m_radio_ap_control_policy_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_radio_ap_control_policy_list[ret_idx]);
}

bool tlvSteeringPolicy::alloc_radio_ap_control_policy_list(size_t count) {
    if (m_lock_order_counter__ > 2) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list radio_ap_control_policy_list, abort!";
        return false;
    }
    size_t len = sizeof(sRadioApControlPolicy) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 2;
    uint8_t *src = (uint8_t *)&m_radio_ap_control_policy_list[*m_radio_ap_control_policy_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_radio_ap_control_policy_list_idx__ += count;
    *m_radio_ap_control_policy_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_radio_ap_control_policy_list_idx__ - count; i < m_radio_ap_control_policy_list_idx__; i++) { m_radio_ap_control_policy_list[i].struct_init(); }
    }
    return true;
}

void tlvSteeringPolicy::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    for (size_t i = 0; i < m_local_steering_disallowed_sta_list_idx__; i++){
        m_local_steering_disallowed_sta_list[i].struct_swap();
    }
    for (size_t i = 0; i < m_btm_steering_disallowed_sta_list_idx__; i++){
        m_btm_steering_disallowed_sta_list[i].struct_swap();
    }
    for (size_t i = 0; i < m_radio_ap_control_policy_list_idx__; i++){
        m_radio_ap_control_policy_list[i].struct_swap();
    }
}

bool tlvSteeringPolicy::finalize()
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

size_t tlvSteeringPolicy::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // local_steering_disallowed_sta_list_length
    class_size += sizeof(uint8_t); // btm_steering_disallowed_sta_list_length
    class_size += sizeof(uint8_t); // radio_ap_control_policy_list_length
    return class_size;
}

bool tlvSteeringPolicy::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_STEERING_POLICY;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_local_steering_disallowed_sta_list_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_local_steering_disallowed_sta_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_local_steering_disallowed_sta_list = (sMacAddr*)m_buff_ptr__;
    uint8_t local_steering_disallowed_sta_list_length = *m_local_steering_disallowed_sta_list_length;
    m_local_steering_disallowed_sta_list_idx__ = local_steering_disallowed_sta_list_length;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr) * (local_steering_disallowed_sta_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) * (local_steering_disallowed_sta_list_length) << ") Failed!";
        return false;
    }
    m_btm_steering_disallowed_sta_list_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_btm_steering_disallowed_sta_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_btm_steering_disallowed_sta_list = (sMacAddr*)m_buff_ptr__;
    uint8_t btm_steering_disallowed_sta_list_length = *m_btm_steering_disallowed_sta_list_length;
    m_btm_steering_disallowed_sta_list_idx__ = btm_steering_disallowed_sta_list_length;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr) * (btm_steering_disallowed_sta_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) * (btm_steering_disallowed_sta_list_length) << ") Failed!";
        return false;
    }
    m_radio_ap_control_policy_list_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_radio_ap_control_policy_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_radio_ap_control_policy_list = (sRadioApControlPolicy*)m_buff_ptr__;
    uint8_t radio_ap_control_policy_list_length = *m_radio_ap_control_policy_list_length;
    m_radio_ap_control_policy_list_idx__ = radio_ap_control_policy_list_length;
    if (!buffPtrIncrementSafe(sizeof(sRadioApControlPolicy) * (radio_ap_control_policy_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRadioApControlPolicy) * (radio_ap_control_policy_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_STEERING_POLICY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_STEERING_POLICY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


