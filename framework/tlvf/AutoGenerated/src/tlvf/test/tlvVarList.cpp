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

uint16_t& tlvTestVarList::var1() {
    return (uint16_t&)(*m_var1);
}

void tlvTestVarList::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var0));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_var1));
}

size_t tlvTestVarList::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // type
    class_size += sizeof(uint16_t); // var0
    class_size += sizeof(uint16_t); // length
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
    m_var1 = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


