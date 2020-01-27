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

tlvTestVarList::tlvTestVarList(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvTestVarList::tlvTestVarList(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvTestVarList::~tlvTestVarList() {
}
const uint8_t& tlvTestVarList::type() {
    return (const uint8_t&)(*m_type);
}

const uint16_t& tlvTestVarList::length() {
    return (const uint16_t&)(*m_length);
}

uint16_t& tlvTestVarList::var0() {
    return (uint16_t&)(*m_var0);
}

uint8_t& tlvTestVarList::simple_list_length() {
    return (uint8_t&)(*m_simple_list_length);
}

uint16_t* tlvTestVarList::simple_list(size_t idx) {
    if ( (m_simple_list_idx__ <= 0) || (m_simple_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_simple_list[idx]);
}

bool tlvTestVarList::alloc_simple_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list simple_list, abort!";
        return false;
    }
    size_t len = sizeof(uint16_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_simple_list[*m_simple_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_test_string_length = (uint8_t *)((uint8_t *)(m_test_string_length) + len);
    m_test_string = (char *)((uint8_t *)(m_test_string) + len);
    m_complex_list_length = (uint8_t *)((uint8_t *)(m_complex_list_length) + len);
    m_complex_list = (cInner *)((uint8_t *)(m_complex_list) + len);
    m_var1 = (cInner *)((uint8_t *)(m_var1) + len);
    m_var3 = (cInner *)((uint8_t *)(m_var3) + len);
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len);
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len);
    m_simple_list_idx__ += count;
    *m_simple_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

uint8_t& tlvTestVarList::test_string_length() {
    return (uint8_t&)(*m_test_string_length);
}

std::string tlvTestVarList::test_string_str() {
    char *test_string_ = test_string();
    if (!test_string_) { return std::string(); }
    return std::string(test_string_, m_test_string_idx__);
}

char* tlvTestVarList::test_string(size_t length) {
    if( (m_test_string_idx__ <= 0) || (m_test_string_idx__ < length) ) {
        TLVF_LOG(ERROR) << "test_string length is smaller than requested length";
        return nullptr;
    }
    if (m_test_string_idx__ > 8 )  {
        TLVF_LOG(ERROR) << "Invalid length -  " << m_test_string_idx__ << " elements (max length is " << 8 << ")";
        return nullptr;
    }
    return ((char*)m_test_string);
}

bool tlvTestVarList::set_test_string(const std::string& str) { return set_test_string(str.c_str(), str.size()); }
bool tlvTestVarList::set_test_string(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_test_string received a null pointer.";
        return false;
    }
    if (!alloc_test_string(size)) { return false; }
    std::copy(str, str + size, m_test_string);
    return true;
}
bool tlvTestVarList::alloc_test_string(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list test_string, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (count > 8 )  {
        TLVF_LOG(ERROR) << "Can't allocate " << count << " elements (max length is " << 8 << ")";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_test_string[*m_test_string_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_complex_list_length = (uint8_t *)((uint8_t *)(m_complex_list_length) + len);
    m_complex_list = (cInner *)((uint8_t *)(m_complex_list) + len);
    m_var1 = (cInner *)((uint8_t *)(m_var1) + len);
    m_var3 = (cInner *)((uint8_t *)(m_var3) + len);
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len);
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len);
    m_test_string_idx__ += count;
    *m_test_string_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

uint8_t& tlvTestVarList::complex_list_length() {
    return (uint8_t&)(*m_complex_list_length);
}

std::tuple<bool, cInner&> tlvTestVarList::complex_list(size_t idx) {
    bool ret_success = ( (m_complex_list_idx__ > 0) && (m_complex_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_complex_list_vector[ret_idx]));
}

std::shared_ptr<cInner> tlvTestVarList::create_complex_list() {
    if (m_lock_order_counter__ > 2) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list complex_list, abort!";
        return nullptr;
    }
    size_t len = cInner::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 2;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_complex_list;
    if (m_complex_list_idx__ > 0) {
        src = (uint8_t *)m_complex_list_vector[m_complex_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_var1 = (cInner *)((uint8_t *)(m_var1) + len);
    m_var3 = (cInner *)((uint8_t *)(m_var3) + len);
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len);
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len);
    return std::make_shared<cInner>(src, getBuffRemainingBytes(src), m_parse__);
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
    uint8_t *src = (uint8_t *)m_complex_list;
    if (m_complex_list_idx__ > 0) {
        src = (uint8_t *)m_complex_list_vector[m_complex_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_complex_list_idx__++;
    if (!m_parse__) { (*m_complex_list_length)++; }
    size_t len = ptr->getLen();
    m_var1 = (cInner *)((uint8_t *)(m_var1) + len - ptr->get_initial_size());
    m_var3 = (cInner *)((uint8_t *)(m_var3) + len - ptr->get_initial_size());
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len - ptr->get_initial_size());
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len - ptr->get_initial_size());
    m_complex_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

bool tlvTestVarList::isPostInitSucceeded() {
    if (!m_var1_init) {
        TLVF_LOG(ERROR) << "var1 is not initialized";
        return false;
    }
    if (!m_var3_init) {
        TLVF_LOG(ERROR) << "var3 is not initialized";
        return false;
    }
    return true; 
}

std::shared_ptr<cInner> tlvTestVarList::create_var1() {
    if (m_lock_order_counter__ > 3) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list var1, abort!";
        return nullptr;
    }
    size_t len = cInner::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 3;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_var1;
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_var3 = (cInner *)((uint8_t *)(m_var3) + len);
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len);
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len);
    return std::make_shared<cInner>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvTestVarList::add_var1(std::shared_ptr<cInner> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_var1 was called before add_var1";
        return false;
    }
    uint8_t *src = (uint8_t *)m_var1;
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_var1_init = true;
    size_t len = ptr->getLen();
    m_var3 = (cInner *)((uint8_t *)(m_var3) + len - ptr->get_initial_size());
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len - ptr->get_initial_size());
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len - ptr->get_initial_size());
    m_var1_ptr = ptr;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

std::shared_ptr<cInner> tlvTestVarList::create_var3() {
    if (m_lock_order_counter__ > 4) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list var3, abort!";
        return nullptr;
    }
    size_t len = cInner::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 4;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_var3;
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len);
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len);
    return std::make_shared<cInner>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvTestVarList::add_var3(std::shared_ptr<cInner> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_var3 was called before add_var3";
        return false;
    }
    uint8_t *src = (uint8_t *)m_var3;
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_var3_init = true;
    size_t len = ptr->getLen();
    m_var2 = (uint32_t *)((uint8_t *)(m_var2) + len - ptr->get_initial_size());
    m_unknown_length_list = (cInner *)((uint8_t *)(m_unknown_length_list) + len - ptr->get_initial_size());
    m_var3_ptr = ptr;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

uint32_t& tlvTestVarList::var2() {
    return (uint32_t&)(*m_var2);
}

size_t tlvTestVarList::unknown_length_list_length()
{
    size_t len = 0;
    for (size_t i = 0; i < m_unknown_length_list_idx__; i++) {
        len += m_unknown_length_list_vector[i]->getLen();
    }
    return len;
}

std::tuple<bool, cInner&> tlvTestVarList::unknown_length_list(size_t idx) {
    bool ret_success = ( (m_unknown_length_list_idx__ > 0) && (m_unknown_length_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_unknown_length_list_vector[ret_idx]));
}

std::shared_ptr<cInner> tlvTestVarList::create_unknown_length_list() {
    if (m_lock_order_counter__ > 5) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list unknown_length_list, abort!";
        return nullptr;
    }
    size_t len = cInner::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 5;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_unknown_length_list;
    if (m_unknown_length_list_idx__ > 0) {
        src = (uint8_t *)m_unknown_length_list_vector[m_unknown_length_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cInner>(getBuffPtr(), getBuffRemainingBytes(), m_parse__);
}

bool tlvTestVarList::add_unknown_length_list(std::shared_ptr<cInner> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_unknown_length_list was called before add_unknown_length_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_unknown_length_list;
    if (m_unknown_length_list_idx__ > 0) {
        src = (uint8_t *)m_unknown_length_list_vector[m_unknown_length_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_unknown_length_list_idx__++;
    size_t len = ptr->getLen();
    m_unknown_length_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvTestVarList::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var0));
    for (size_t i = 0; i < (size_t)*m_simple_list_length; i++){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&m_simple_list[i]));
    }
    for (size_t i = 0; i < (size_t)*m_complex_list_length; i++){
        std::get<1>(complex_list(i)).class_swap();
    }
    if (m_var1_ptr) { m_var1_ptr->class_swap(); }
    if (m_var3_ptr) { m_var3_ptr->class_swap(); }
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_var2));
    for (size_t i = 0; i < m_unknown_length_list_idx__; i++){
        std::get<1>(unknown_length_list(i)).class_swap();
    }
}

bool tlvTestVarList::finalize()
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

size_t tlvTestVarList::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // var0
    class_size += sizeof(uint8_t); // simple_list_length
    class_size += sizeof(uint8_t); // test_string_length
    class_size += sizeof(uint8_t); // complex_list_length
    class_size += sizeof(uint32_t); // var2
    return class_size;
}

bool tlvTestVarList::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_type = 0xff;
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
    m_var0 = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_simple_list_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_simple_list = (uint16_t*)m_buff_ptr__;
    uint8_t simple_list_length = *m_simple_list_length;
    m_simple_list_idx__ = simple_list_length;
    if (!buffPtrIncrementSafe(sizeof(uint16_t) * (simple_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) * (simple_list_length) << ") Failed!";
        return false;
    }
    m_test_string_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_test_string = (char*)m_buff_ptr__;
    uint8_t test_string_length = *m_test_string_length;
    m_test_string_idx__ = test_string_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (test_string_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (test_string_length) << ") Failed!";
        return false;
    }
    m_complex_list_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_complex_list = (cInner*)m_buff_ptr__;
    uint8_t complex_list_length = *m_complex_list_length;
    m_complex_list_idx__ = 0;
    for (size_t i = 0; i < complex_list_length; i++) {
        auto complex_list = create_complex_list();
        if (!complex_list) {
            TLVF_LOG(ERROR) << "create_complex_list() failed";
            return false;
        }
        if (!add_complex_list(complex_list)) {
            TLVF_LOG(ERROR) << "add_complex_list() failed";
            return false;
        }
        // swap back since complex_list will be swapped as part of the whole class swap
        complex_list->class_swap();
    }
    m_var1 = (cInner*)m_buff_ptr__;
    if (m_parse__) {
        auto var1 = create_var1();
        if (!var1) {
            TLVF_LOG(ERROR) << "create_var1() failed";
            return false;
        }
        if (!add_var1(var1)) {
            TLVF_LOG(ERROR) << "add_var1() failed";
            return false;
        }
        // swap back since var1 will be swapped as part of the whole class swap
        var1->class_swap();
    }
    m_var3 = (cInner*)m_buff_ptr__;
    if (m_parse__) {
        auto var3 = create_var3();
        if (!var3) {
            TLVF_LOG(ERROR) << "create_var3() failed";
            return false;
        }
        if (!add_var3(var3)) {
            TLVF_LOG(ERROR) << "add_var3() failed";
            return false;
        }
        // swap back since var3 will be swapped as part of the whole class swap
        var3->class_swap();
    }
    m_var2 = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_unknown_length_list = (cInner*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&len));
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        while (len > 0) {
            if (len < cInner::get_initial_size()) {
                TLVF_LOG(ERROR) << "Invalid length (unknown_length_list)";
                return false;
            }
            auto unknown_length_list = create_unknown_length_list();
            if (!unknown_length_list) {
                TLVF_LOG(ERROR) << "create_unknown_length_list() failed";
                return false;
            }
            if (!add_unknown_length_list(unknown_length_list)) {
                TLVF_LOG(ERROR) << "add_unknown_length_list() failed";
                return false;
            }
            // swap back since unknown_length_list will be swapped as part of the whole class swap
            unknown_length_list->class_swap();
            len -= unknown_length_list->getLen();
        }
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != 0xff) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(0xff) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cInner::cInner(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cInner::cInner(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cInner::~cInner() {
}
const uint16_t& cInner::type() {
    return (const uint16_t&)(*m_type);
}

const uint16_t& cInner::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& cInner::list_length() {
    return (uint8_t&)(*m_list_length);
}

uint8_t* cInner::list(size_t idx) {
    if ( (m_list_idx__ <= 0) || (m_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_list[idx]);
}

bool cInner::alloc_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list list, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_list[*m_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_var1 = (uint32_t *)((uint8_t *)(m_var1) + len);
    m_unknown_length_list_inner = (char *)((uint8_t *)(m_unknown_length_list_inner) + len);
    m_list_idx__ += count;
    *m_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

uint32_t& cInner::var1() {
    return (uint32_t&)(*m_var1);
}

std::string cInner::unknown_length_list_inner_str() {
    char *unknown_length_list_inner_ = unknown_length_list_inner();
    if (!unknown_length_list_inner_) { return std::string(); }
    return std::string(unknown_length_list_inner_, m_unknown_length_list_inner_idx__);
}

char* cInner::unknown_length_list_inner(size_t length) {
    if( (m_unknown_length_list_inner_idx__ <= 0) || (m_unknown_length_list_inner_idx__ < length) ) {
        TLVF_LOG(ERROR) << "unknown_length_list_inner length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_unknown_length_list_inner);
}

bool cInner::set_unknown_length_list_inner(const std::string& str) { return set_unknown_length_list_inner(str.c_str(), str.size()); }
bool cInner::set_unknown_length_list_inner(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_unknown_length_list_inner received a null pointer.";
        return false;
    }
    if (!alloc_unknown_length_list_inner(size)) { return false; }
    std::copy(str, str + size, m_unknown_length_list_inner);
    return true;
}
bool cInner::alloc_unknown_length_list_inner(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list unknown_length_list_inner, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)m_unknown_length_list_inner;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_unknown_length_list_inner_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void cInner::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_var1));
}

bool cInner::finalize()
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

size_t cInner::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // list_length
    class_size += sizeof(uint32_t); // var1
    return class_size;
}

bool cInner::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_type = 0x1;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_list_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_list = (uint8_t*)m_buff_ptr__;
    uint8_t list_length = *m_list_length;
    m_list_idx__ = list_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (list_length) << ") Failed!";
        return false;
    }
    m_var1 = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_unknown_length_list_inner = (char*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&len));
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_unknown_length_list_inner_idx__ = len/sizeof(char);
        if (!buffPtrIncrementSafe(len)) {
            LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
            return false;
        }
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != 0x1) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(0x1) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


