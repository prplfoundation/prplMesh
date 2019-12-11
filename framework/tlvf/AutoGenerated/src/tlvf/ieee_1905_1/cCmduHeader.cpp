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

#include <tlvf/ieee_1905_1/cCmduHeader.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

cCmduHeader::cCmduHeader(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cCmduHeader::cCmduHeader(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cCmduHeader::~cCmduHeader() {
}
const uint8_t& cCmduHeader::message_version() {
    return (const uint8_t&)(*m_message_version);
}

const uint8_t& cCmduHeader::reserved() {
    return (const uint8_t&)(*m_reserved);
}

eMessageType& cCmduHeader::message_type() {
    return (eMessageType&)(*m_message_type);
}

uint16_t& cCmduHeader::message_id() {
    return (uint16_t&)(*m_message_id);
}

uint8_t& cCmduHeader::fragment_id() {
    return (uint8_t&)(*m_fragment_id);
}

cCmduHeader::sFlags& cCmduHeader::flags() {
    return (sFlags&)(*m_flags);
}

void cCmduHeader::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_message_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_message_id));
    m_flags->struct_swap();
}

size_t cCmduHeader::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // message_version
    class_size += sizeof(uint8_t); // reserved
    class_size += sizeof(eMessageType); // message_type
    class_size += sizeof(uint16_t); // message_id
    class_size += sizeof(uint8_t); // fragment_id
    class_size += sizeof(sFlags); // flags
    return class_size;
}

bool cCmduHeader::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_message_version = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_message_version = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_reserved = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_reserved = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_message_type = (eMessageType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eMessageType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eMessageType) << ") Failed!";
        return false;
    }
    m_message_id = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_message_id = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_fragment_id = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_fragment_id = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_flags = (sFlags*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sFlags))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sFlags) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_flags->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


