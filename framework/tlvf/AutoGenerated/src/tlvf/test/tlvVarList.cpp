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

#include <tlvf/test/tlvVarList.h>
#include <tlvf/tlvflogging.h>

tlvTestVarList::tlvTestVarList(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvTestVarList::tlvTestVarList(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvTestVarList::~tlvTestVarList() {
}
const uint8_t& tlvTestVarList::type() {
    return (const uint8_t&)(*m_type);
}

uint16_t& tlvTestVarList::var0() {
    return (uint16_t&)(*m_var0);
}

const uint16_t& tlvTestVarList::length() {
    return (const uint16_t&)(*m_length);
}

uint16_t& tlvTestVarList::simple_list_length() {
    return (uint16_t&)(*m_simple_list_length);
}

std::tuple<bool, uint16_t&> tlvTestVarList::simple_list(size_t idx) {
    bool ret_success = ( (m_simple_list_idx__ > 0) && (m_simple_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_simple_list[ret_idx]);
}

bool tlvTestVarList::alloc_simple_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint16_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_simple_list;
        uint8_t *dst = (uint8_t *)m_simple_list + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_complex_list_length = (uint16_t *)((uint8_t *)(m_complex_list_length) + len);
    m_complex_list = (cInner *)((uint8_t *)(m_complex_list) + len);
    m_var1 = (uint16_t *)((uint8_t *)(m_var1) + len);
    m_simple_list_idx__ += count;
    *m_simple_list_length += count;
    m_buff_ptr__ += len;
    return true;
}

uint16_t& tlvTestVarList::complex_list_length() {
    return (uint16_t&)(*m_complex_list_length);
}

std::tuple<bool, cInner&> tlvTestVarList::complex_list(size_t idx) {
    bool ret_success = ( (m_complex_list_idx__ > 0) && (m_complex_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    if (*m_complex_list_length > 0) {
        return std::forward_as_tuple(ret_success, *(m_complex_list_vector[ret_idx]));
    }
    else {
        return std::forward_as_tuple(ret_success, *(m_complex_list));
    }
}

std::shared_ptr<cInner> tlvTestVarList::create_complex_list() {
    size_t len = cInner::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_allocation__ = true;
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_complex_list;
        uint8_t *dst = (uint8_t *)m_complex_list + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cInner>(getBuffPtr(), getBuffRemainingBytes(), m_parse__, m_swap__);
}

bool tlvTestVarList::add_complex_list(std::shared_ptr<cInner> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_complex_list was called before add_complex_list";
        return false;
    }
    if (ptr->getStartBuffPtr() != getBuffPtr()) {
        TLVF_LOG(ERROR) << "Received to entry pointer is different than expected (excepting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes()) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    if (!m_parse__) {
        m_complex_list_idx__++;
        (*m_complex_list_length)++;
    }
    size_t len = ptr->getLen();
    m_var1 = (uint16_t *)((uint8_t *)(m_var1) + len);
    m_complex_list_vector.push_back(ptr);
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

uint16_t& tlvTestVarList::var1() {
    return (uint16_t&)(*m_var1);
}

void tlvTestVarList::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var0));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_simple_list_length));
    for (size_t i = 0; i < (size_t)*m_simple_list_length; i++){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(m_simple_list[i]));
    }
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_complex_list_length));
    for (size_t i = 0; i < (size_t)*m_complex_list_length; i++){
        std::get<1>(complex_list(i)).class_swap();
    }
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var1));
}

size_t tlvTestVarList::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // type
    class_size += sizeof(uint16_t); // var0
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // simple_list_length
    class_size += sizeof(uint16_t); // complex_list_length
    class_size += sizeof(uint16_t); // var1
    return class_size;
}

bool tlvTestVarList::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_type = 0x1;
    else {
            if (*m_type != 0x1) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(0x1) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_var0 = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_simple_list_length = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_simple_list = (uint16_t*)m_buff_ptr__;
    uint16_t simple_list_length = *m_simple_list_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&simple_list_length));
    m_simple_list_idx__ = simple_list_length;
    m_buff_ptr__ += sizeof(uint16_t)*(simple_list_length);
    m_complex_list_length = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_complex_list = (cInner*)m_buff_ptr__;
    uint16_t complex_list_length = *m_complex_list_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&complex_list_length));
    m_complex_list_idx__ = complex_list_length;
    for (size_t i = 0; i < complex_list_length; i++) {
        if (!add_complex_list(create_complex_list())) { 
            TLVF_LOG(ERROR) << "Failed adding complex_list entry.";
            return false;
        }
    }
    m_var1 = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cInner::cInner(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cInner::cInner(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cInner::~cInner() {
}
uint16_t& cInner::list_length() {
    return (uint16_t&)(*m_list_length);
}

std::tuple<bool, uint8_t&> cInner::list(size_t idx) {
    bool ret_success = ( (m_list_idx__ > 0) && (m_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_list[ret_idx]);
}

bool cInner::alloc_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_list;
        uint8_t *dst = (uint8_t *)m_list + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_var1 = (uint32_t *)((uint8_t *)(m_var1) + len);
    m_list_idx__ += count;
    *m_list_length += count;
    m_buff_ptr__ += len;
    return true;
}

uint32_t& cInner::var1() {
    return (uint32_t&)(*m_var1);
}

void cInner::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_list_length));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_var1));
}

size_t cInner::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // list_length
    class_size += sizeof(uint32_t); // var1
    return class_size;
}

bool cInner::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_list_length = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_list = (uint8_t*)m_buff_ptr__;
    uint16_t list_length = *m_list_length;
    tlvf_swap(16, reinterpret_cast<uint8_t*>(&list_length));
    m_list_idx__ = list_length;
    m_buff_ptr__ += sizeof(uint8_t)*(list_length);
    m_var1 = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


