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

#include <beerocks/tlvf/beerocks_message_header.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_HEADER::cACTION_HEADER(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_HEADER::cACTION_HEADER(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_HEADER::~cACTION_HEADER() {
}
const uint32_t& cACTION_HEADER::magic() {
    return (const uint32_t&)(*m_magic);
}

const uint8_t& cACTION_HEADER::version() {
    return (const uint8_t&)(*m_version);
}

eAction& cACTION_HEADER::action() {
    return (eAction&)(*m_action);
}

uint8_t& cACTION_HEADER::action_op() {
    return (uint8_t&)(*m_action_op);
}

uint8_t& cACTION_HEADER::direction() {
    return (uint8_t&)(*m_direction);
}

sMacAddr& cACTION_HEADER::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

uint8_t& cACTION_HEADER::last() {
    return (uint8_t&)(*m_last);
}

uint16_t& cACTION_HEADER::id() {
    return (uint16_t&)(*m_id);
}

uint16_t& cACTION_HEADER::length() {
    return (uint16_t&)(*m_length);
}

void cACTION_HEADER::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_magic));
    m_radio_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_id));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t cACTION_HEADER::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // magic
    class_size += sizeof(uint8_t); // version
    class_size += sizeof(eAction); // action
    class_size += sizeof(uint8_t); // action_op
    class_size += sizeof(uint8_t); // direction
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(uint8_t); // last
    class_size += sizeof(uint16_t); // id
    class_size += sizeof(uint16_t); // length
    return class_size;
}

bool cACTION_HEADER::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_magic = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_magic = beerocks::message::MESSAGE_MAGIC;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    m_version = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_version = beerocks::message::MESSAGE_VERSION;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_action = (eAction*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eAction))) { return false; }
    m_action_op = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_direction = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_direction = 0x1;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_last = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_last = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_id = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


