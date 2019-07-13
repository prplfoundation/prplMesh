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

tlvVendorSpecific::tlvVendorSpecific(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvVendorSpecific::tlvVendorSpecific(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
//~class_constractor_tlvVendorSpecific
tlvVendorSpecific::~tlvVendorSpecific() {
}
//~class_distractor_tlvVendorSpecific
const uint8_t& tlvVendorSpecific::type() {
    return (const uint8_t&)(*m_type);
}

uint16_t& tlvVendorSpecific::length() {
    return (uint16_t&)(*m_length);
}

uint16_t& tlvVendorSpecific::test_length() {
    return (uint16_t&)(*m_test_length);
}

std::tuple<bool, uint8_t&> tlvVendorSpecific::test_list(size_t idx) {
    bool ret_success = ( (m_test_list_idx__ > 0) && (m_test_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_test_list[ret_idx]);
}

uint16_t& tlvVendorSpecific::test_uint16() {
    return (uint16_t&)(*m_test_uint16);
}

uint16_t& tlvVendorSpecific::test2_length() {
    return (uint16_t&)(*m_test2_length);
}

std::tuple<bool, uint8_t&> tlvVendorSpecific::test2_list(size_t idx) {
    bool ret_success = ( (m_test2_list_idx__ > 0) && (m_test2_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_test2_list[ret_idx]);
}

uint32_t& tlvVendorSpecific::test_uint32() {
    return (uint32_t&)(*m_test_uint32);
}

std::tuple<bool, uint8_t&> tlvVendorSpecific::vendor_oui(size_t idx) {
    bool ret_success = ( (m_vendor_oui_idx__ > 0) && (m_vendor_oui_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_vendor_oui[ret_idx]);
}

//~class_public_func_insert_tlvVendorSpecific
void tlvVendorSpecific::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_test_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_test_uint16));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_test2_length));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_test_uint32));
    //~class_swap_func_insert_tlvVendorSpecific
}

size_t tlvVendorSpecific::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint16_t); // test_length
    class_size += sizeof(uint16_t); // test_uint16
    class_size += sizeof(uint16_t); // test2_length
    class_size += sizeof(uint32_t); // test_uint32
    class_size += 3 * sizeof(uint8_t); // vendor_oui
    //~class_size_func_insert_tlvVendorSpecific
    return class_size;
}

bool tlvVendorSpecific::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_type = 0x1;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0x3;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_test_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_test_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_test_list = (uint8_t*)m_buff_ptr__;
    m_test_list_idx__ = *m_test_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_test_length);
    m_test_uint16 = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_test_uint16 = 0xe;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_test2_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_test2_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_test2_list = (uint8_t*)m_buff_ptr__;
    m_test2_list_idx__ = *m_test2_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_test2_length);
    m_test_uint32 = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_test_uint32 = 0xa;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_vendor_oui = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(uint8_t) * 3);
    m_vendor_oui_idx__  = 3;
    //~class_init_func_insert_tlvVendorSpecific
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

//~class_private_func_insert_tlvVendorSpecific
bool tlvVendorSpecific::alloc_test_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_test_uint16 = (uint16_t *)((uint8_t *)(m_test_uint16) + len);
    m_test2_length = (uint16_t *)((uint8_t *)(m_test2_length) + len);
    m_test2_list = (uint8_t *)((uint8_t *)(m_test2_list) + len);
    m_test_uint32 = (uint32_t *)((uint8_t *)(m_test_uint32) + len);
    m_vendor_oui = (uint8_t *)((uint8_t *)(m_vendor_oui) + len);
//~class_alloc_func_start_tlvVendorSpecific_test_list
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_test_list_idx__ += count;
    *m_test_length += count;
    m_buff_ptr__ += len;
    return true;
}

//~class_alloc_func_insert_tlvVendorSpecific_test_list
bool tlvVendorSpecific::alloc_test2_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_test_uint32 = (uint32_t *)((uint8_t *)(m_test_uint32) + len);
    m_vendor_oui = (uint8_t *)((uint8_t *)(m_vendor_oui) + len);
//~class_alloc_func_start_tlvVendorSpecific_test2_list
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_test2_list_idx__ += count;
    *m_test2_length += count;
    m_buff_ptr__ += len;
    return true;
}

//~class_alloc_func_insert_tlvVendorSpecific_test2_list
//~code_end_insert

