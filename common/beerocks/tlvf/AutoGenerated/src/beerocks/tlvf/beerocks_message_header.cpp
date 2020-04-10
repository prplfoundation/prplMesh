///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <beerocks/tlvf/beerocks_message_header.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_HEADER::cACTION_HEADER(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_HEADER::cACTION_HEADER(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eAction), reinterpret_cast<uint8_t*>(m_action));
    m_radio_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_id));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool cACTION_HEADER::finalize()
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
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_version = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_version = beerocks::message::MESSAGE_VERSION;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_action = (eAction*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eAction))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eAction) << ") Failed!";
        return false;
    }
    m_action_op = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_direction = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_direction = 0x1;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_last = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_last = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_id = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


