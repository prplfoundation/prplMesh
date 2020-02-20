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

#include <tlvf/ieee_1905_1/tlvDeviceBridgingCapability.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvDeviceBridgingCapability::tlvDeviceBridgingCapability(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvDeviceBridgingCapability::tlvDeviceBridgingCapability(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvDeviceBridgingCapability::~tlvDeviceBridgingCapability() {
}
const eTlvType& tlvDeviceBridgingCapability::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvDeviceBridgingCapability::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvDeviceBridgingCapability::bridging_tuples_list_length() {
    return (uint8_t&)(*m_bridging_tuples_list_length);
}

std::tuple<bool, cMacList&> tlvDeviceBridgingCapability::bridging_tuples_list(size_t idx) {
    bool ret_success = ( (m_bridging_tuples_list_idx__ > 0) && (m_bridging_tuples_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_bridging_tuples_list_vector[ret_idx]));
}

std::shared_ptr<cMacList> tlvDeviceBridgingCapability::create_bridging_tuples_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list bridging_tuples_list, abort!";
        return nullptr;
    }
    size_t len = cMacList::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_bridging_tuples_list;
    if (m_bridging_tuples_list_idx__ > 0) {
        src = (uint8_t *)m_bridging_tuples_list_vector[m_bridging_tuples_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cMacList>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvDeviceBridgingCapability::add_bridging_tuples_list(std::shared_ptr<cMacList> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_bridging_tuples_list was called before add_bridging_tuples_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_bridging_tuples_list;
    if (m_bridging_tuples_list_idx__ > 0) {
        src = (uint8_t *)m_bridging_tuples_list_vector[m_bridging_tuples_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_bridging_tuples_list_idx__++;
    if (!m_parse__) { (*m_bridging_tuples_list_length)++; }
    size_t len = ptr->getLen();
    m_bridging_tuples_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvDeviceBridgingCapability::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    for (size_t i = 0; i < (size_t)*m_bridging_tuples_list_length; i++){
        std::get<1>(bridging_tuples_list(i)).class_swap();
    }
}

bool tlvDeviceBridgingCapability::finalize()
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

size_t tlvDeviceBridgingCapability::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // bridging_tuples_list_length
    return class_size;
}

bool tlvDeviceBridgingCapability::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_DEVICE_BRIDGING_CAPABILITY;
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
    m_bridging_tuples_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_bridging_tuples_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bridging_tuples_list = (cMacList*)m_buff_ptr__;
    uint8_t bridging_tuples_list_length = *m_bridging_tuples_list_length;
    m_bridging_tuples_list_idx__ = 0;
    for (size_t i = 0; i < bridging_tuples_list_length; i++) {
        auto bridging_tuples_list = create_bridging_tuples_list();
        if (!bridging_tuples_list) {
            TLVF_LOG(ERROR) << "create_bridging_tuples_list() failed";
            return false;
        }
        if (!add_bridging_tuples_list(bridging_tuples_list)) {
            TLVF_LOG(ERROR) << "add_bridging_tuples_list() failed";
            return false;
        }
        // swap back since bridging_tuples_list will be swapped as part of the whole class swap
        bridging_tuples_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_DEVICE_BRIDGING_CAPABILITY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_DEVICE_BRIDGING_CAPABILITY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cMacList::cMacList(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cMacList::cMacList(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cMacList::~cMacList() {
}
uint8_t& cMacList::mac_list_length() {
    return (uint8_t&)(*m_mac_list_length);
}

std::tuple<bool, sMacAddr&> cMacList::mac_list(size_t idx) {
    bool ret_success = ( (m_mac_list_idx__ > 0) && (m_mac_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_mac_list[ret_idx]);
}

bool cMacList::alloc_mac_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list mac_list, abort!";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_mac_list[*m_mac_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_mac_list_idx__ += count;
    *m_mac_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_mac_list_idx__ - count; i < m_mac_list_idx__; i++) { m_mac_list[i].struct_init(); }
    }
    return true;
}

void cMacList::class_swap()
{
    for (size_t i = 0; i < (size_t)*m_mac_list_length; i++){
        m_mac_list[i].struct_swap();
    }
}

bool cMacList::finalize()
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

size_t cMacList::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // mac_list_length
    return class_size;
}

bool cMacList::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_mac_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_mac_list = (sMacAddr*)m_buff_ptr__;
    uint8_t mac_list_length = *m_mac_list_length;
    m_mac_list_idx__ = mac_list_length;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr) * (mac_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) * (mac_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


