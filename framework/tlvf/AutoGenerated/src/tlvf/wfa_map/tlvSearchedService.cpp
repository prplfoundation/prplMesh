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

#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvSearchedService::tlvSearchedService(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvSearchedService::tlvSearchedService(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvSearchedService::~tlvSearchedService() {
}
const eTlvTypeMap& tlvSearchedService::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvSearchedService::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvSearchedService::searched_service_list_length() {
    return (uint8_t&)(*m_searched_service_list_length);
}

std::tuple<bool, tlvSearchedService::eSearchedService&> tlvSearchedService::searched_service_list(size_t idx) {
    bool ret_success = ( (m_searched_service_list_idx__ > 0) && (m_searched_service_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_searched_service_list[ret_idx]);
}

bool tlvSearchedService::alloc_searched_service_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list searched_service_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(eSearchedService) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_searched_service_list[*m_searched_service_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_searched_service_list_idx__ += count;
    *m_searched_service_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvSearchedService::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t tlvSearchedService::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // searched_service_list_length
    return class_size;
}

bool tlvSearchedService::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_SEARCHED_SERVICE;
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
    m_searched_service_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_searched_service_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_searched_service_list = (eSearchedService*)m_buff_ptr__;
    uint8_t searched_service_list_length = *m_searched_service_list_length;
    m_searched_service_list_idx__ = searched_service_list_length;
    if (!buffPtrIncrementSafe(sizeof(eSearchedService) * (searched_service_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eSearchedService) * (searched_service_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_SEARCHED_SERVICE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_SEARCHED_SERVICE) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


