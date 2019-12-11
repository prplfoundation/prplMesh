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

#include <tlvf/ieee_1905_1/tlvUnknown.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvUnknown::tlvUnknown(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvUnknown::tlvUnknown(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvUnknown::~tlvUnknown() {
}
uint8_t& tlvUnknown::type() {
    return (uint8_t&)(*m_type);
}

const uint16_t& tlvUnknown::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t* tlvUnknown::data(size_t idx) {
    if ( (m_data_idx__ <= 0) || (m_data_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_data[idx]);
}

bool tlvUnknown::alloc_data(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list data, abort!";
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
    uint8_t *src = (uint8_t *)m_data;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_data_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvUnknown::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t tlvUnknown::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // type
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool tlvUnknown::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_data = (uint8_t*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&len)); }
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_data_idx__ = len/sizeof(uint8_t);
        if (!buffPtrIncrementSafe(len)) {
            LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
            return false;
        }
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


