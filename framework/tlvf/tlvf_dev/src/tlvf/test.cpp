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

#include <tlvf/test.h>
#include <tlvf/tlvflogging.h>
//~include_insert

using namespace ieee1905_1;

cInnerClass::cInnerClass(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cInnerClass::cInnerClass(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
//~class_constractor_cInnerClass
cInnerClass::~cInnerClass() {
}
//~class_distractor_cInnerClass
const uint8_t& cInnerClass::var1() {
    return (const uint8_t&)(*m_var1);
}

uint16_t& cInnerClass::list1_length() {
    return (uint16_t&)(*m_list1_length);
}

std::tuple<bool, uint8_t&> cInnerClass::list1(size_t idx) {
    bool ret_success = ( (m_list1_idx__ > 0) && (m_list1_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_list1[ret_idx]);
}

bool cInnerClass::alloc_list1(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_var2 = (uint16_t *)((uint8_t *)(m_var2) + len);
    m_list2_length = (uint16_t *)((uint8_t *)(m_list2_length) + len);
    m_list2 = (uint8_t *)((uint8_t *)(m_list2) + len);
    m_var3 = (uint32_t *)((uint8_t *)(m_var3) + len);
    m_var4 = (uint8_t *)((uint8_t *)(m_var4) + len);
//~class_alloc_func_insert_cInnerClass_alloc_list1
    m_list1_idx__ += count;
    *m_list1_length += count;
    m_buff_ptr__ += len;
    return true;
}

uint16_t& cInnerClass::var2() {
    return (uint16_t&)(*m_var2);
}

uint16_t& cInnerClass::list2_length() {
    return (uint16_t&)(*m_list2_length);
}

std::tuple<bool, uint8_t&> cInnerClass::list2(size_t idx) {
    bool ret_success = ( (m_list2_idx__ > 0) && (m_list2_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_list2[ret_idx]);
}

bool cInnerClass::alloc_list2(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_var3 = (uint32_t *)((uint8_t *)(m_var3) + len);
    m_var4 = (uint8_t *)((uint8_t *)(m_var4) + len);
//~class_alloc_func_insert_cInnerClass_alloc_list2
    m_list2_idx__ += count;
    *m_test2_length += count;
    m_buff_ptr__ += len;
    return true;
}

uint32_t& cInnerClass::var3() {
    return (uint32_t&)(*m_var3);
}

std::tuple<bool, uint8_t&> cInnerClass::var4(size_t idx) {
    bool ret_success = ( (m_var4_idx__ > 0) && (m_var4_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_var4[ret_idx]);
}

//~class_public_func_insert_cInnerClass
void cInnerClass::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_list1_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var2));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_list2_length));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_var3));
    //~class_swap_func_insert_cInnerClass
}

size_t cInnerClass::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // var1
    class_size += sizeof(uint16_t); // list1_length
    class_size += sizeof(uint16_t); // var2
    class_size += sizeof(uint16_t); // list2_length
    class_size += sizeof(uint32_t); // var3
    class_size += 3 * sizeof(uint8_t); // var4
    //~class_size_func_insert_cInnerClass
    return class_size;
}

bool cInnerClass::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_var1 = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_var1 = 0x1;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_list1_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_list1_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_list1 = (uint8_t*)m_buff_ptr__;
    m_list1_idx__ = *m_list1_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_list1_length);
    m_var2 = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_var2 = 0xe;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_list2_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_list2_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_list2 = (uint8_t*)m_buff_ptr__;
    m_list2_idx__ = *m_test2_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_test2_length);
    m_var3 = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_var3 = 0xa;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_var4 = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(uint8_t) * 3);
    m_var4_idx__  = 3;
    //~class_init_func_insert_cInnerClass
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

//~class_private_func_insert_cInnerClass
cOuterClass::cOuterClass(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cOuterClass::cOuterClass(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
//~class_constractor_cOuterClass
cOuterClass::~cOuterClass() {
}
//~class_distractor_cOuterClass
const uint32_t& cOuterClass::var1() {
    return (const uint32_t&)(*m_var1);
}

uint16_t& cOuterClass::var2_length() {
    return (uint16_t&)(*m_var2_length);
}

std::tuple<bool, cInnerClass&> cOuterClass::var2_list(size_t idx) {
    bool ret_success = ( (m_var2_list_idx__ > 0) && (m_var2_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    if (*m_var2_list_length > 0) {
        return std::forward_as_tuple(ret_success, *(m_var2_list_vector[ret_idx]));
    }
    else {
        return std::forward_as_tuple(ret_success, *(m_var2_list));
    }
}

std::shared_ptr<cInnerClass> cOuterClass::create_var2_list() {
    size_t len = cInnerClass::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_allocation__ = true;
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_var3_length = (uint16_t *)((uint8_t *)(m_var3_length) + len);
    m_var3_list = (uint8_t *)((uint8_t *)(m_var3_list) + len);
    m_list_dynamic = (char *)((uint8_t *)(m_list_dynamic) + len);
//~class_alloc_func_insert_cOuterClass_create_var2_list
    return std::make_shared<cInnerClass>(getBuffPtr(), getBuffRemainingBytes(), m_parse__, m_swap__);
}

bool cOuterClass::add_var2_list(std::shared_ptr<cInnerClass> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_var2_list was called before add_var2_list";
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
        m_var2_list_idx__++;
        (*m_var2_length)++;
    }
    size_t len = ptr->getLen();
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_var3_length = (uint16_t *)((uint8_t *)(m_var3_length) + len);
    m_var3_list = (uint8_t *)((uint8_t *)(m_var3_list) + len);
    m_list_dynamic = (char *)((uint8_t *)(m_list_dynamic) + len);
//~class_alloc_func_insert_cOuterClass_add_var2_list
    m_var2_list_vector.push_back(ptr);
    m_buff_ptr__ += len;
    m_lock_allocation__ = false;
    return true;
}

uint16_t& cOuterClass::var3_length() {
    return (uint16_t&)(*m_var3_length);
}

std::tuple<bool, uint8_t&> cOuterClass::var3_list(size_t idx) {
    bool ret_success = ( (m_var3_list_idx__ > 0) && (m_var3_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_var3_list[ret_idx]);
}

bool cOuterClass::alloc_var3_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_list_dynamic = (char *)((uint8_t *)(m_list_dynamic) + len);
//~class_alloc_func_insert_cOuterClass_alloc_var3_list
    m_var3_list_idx__ += count;
    *m_var3_length += count;
    m_buff_ptr__ += len;
    return true;
}

char* cOuterClass::list_dynamic(size_t length) {
    if( (m_list_dynamic_idx__ <= 0) || (m_list_dynamic_idx__ < length) ) {
        TLVF_LOG(ERROR) << "list_dynamic length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_list_dynamic);
}

bool cOuterClass::set_list_dynamic(std::string& str) {
    return set_list_dynamic(const_cast<std::string&>(str));
}
bool cOuterClass::set_list_dynamic(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_list_dynamic received an empty string.";
        return false;
    }
    if (!alloc_list_dynamic(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_list_dynamic, str.c_str(), str_size + 1);
    return true;
}
bool cOuterClass::set_list_dynamic(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_list_dynamic received an empty string.";
        return false;
    }
    if (!alloc_list_dynamic(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_list_dynamic, str, size + 1);
    m_list_dynamic[size] = '\0';
    return true;
}
bool cOuterClass::alloc_list_dynamic(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_list_dynamic_idx__ += count;
    m_buff_ptr__ += len;
    return true;
}

//~class_public_func_insert_cOuterClass
void cOuterClass::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_var1));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var2_length));
    for (size_t i = 0; i < (size_t)*m_var2_length; i++){
        std::get<1>(var2_list(i)).class_swap();
    }
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var3_length));
    //~class_swap_func_insert_cOuterClass
}

size_t cOuterClass::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // var1
    class_size += sizeof(uint16_t); // var2_length
    class_size += sizeof(uint16_t); // var3_length
    //~class_size_func_insert_cOuterClass
    return class_size;
}

bool cOuterClass::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_var1 = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_var1 = 0x8;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_var2_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_var2_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_var2_list = (cInnerClass*)m_buff_ptr__;
    m_var2_list_idx__ = *m_var2_length;
    for (size_t i = 0; i < *m_var2_length; i++) {
        if (!add_var2_list(create_var2_list())) { 
            TLVF_LOG(ERROR) << "Failed adding var2_list entry.";
            return false;
        }
    }
    m_var3_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_var3_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_var3_list = (uint8_t*)m_buff_ptr__;
    m_var3_list_idx__ = *m_var3_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_var3_length);
    m_list_dynamic = (char*)m_buff_ptr__;
    //~class_init_func_insert_cOuterClass
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

//~class_private_func_insert_cOuterClass
//~code_end_insert

