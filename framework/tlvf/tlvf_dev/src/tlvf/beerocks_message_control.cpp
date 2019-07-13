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

#include <tlvf/beerocks_message_control.h>
#include <tlvf/tlvflogging.h>
//~include_insert

cACTION_CONTROL_AGENT_PING_RESPONSE::cACTION_CONTROL_AGENT_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_AGENT_PING_RESPONSE::cACTION_CONTROL_AGENT_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
//~class_constractor_cACTION_CONTROL_AGENT_PING_RESPONSE
cACTION_CONTROL_AGENT_PING_RESPONSE::~cACTION_CONTROL_AGENT_PING_RESPONSE() {
}
//~class_distractor_cACTION_CONTROL_AGENT_PING_RESPONSE
uint16_t& cACTION_CONTROL_AGENT_PING_RESPONSE::total() {
    return (uint16_t&)(*m_total);
}

uint16_t& cACTION_CONTROL_AGENT_PING_RESPONSE::seq() {
    return (uint16_t&)(*m_seq);
}

uint16_t& cACTION_CONTROL_AGENT_PING_RESPONSE::size() {
    return (uint16_t&)(*m_size);
}

std::tuple<bool, uint8_t&> cACTION_CONTROL_AGENT_PING_RESPONSE::data(size_t idx) {
    bool ret_success = ( (m_data_idx__ > 0) && (m_data_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_data[ret_idx]);
}

//~class_public_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
void cACTION_CONTROL_AGENT_PING_RESPONSE::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_total));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_seq));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
    //~class_swap_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
}

size_t cACTION_CONTROL_AGENT_PING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // total
    class_size += sizeof(uint16_t); // seq
    class_size += sizeof(uint16_t); // size
    //~class_size_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
    return class_size;
}

bool cACTION_CONTROL_AGENT_PING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_total = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_seq = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_data = (uint8_t*)m_buff_ptr__;
    //~class_init_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

//~class_private_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
bool cACTION_CONTROL_AGENT_PING_RESPONSE::alloc_data(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
//~class_alloc_func_start_cACTION_CONTROL_AGENT_PING_RESPONSE_data
    std::memmove(m_buff_ptr__ + len, m_buff_ptr__, len);
    m_data_idx__ += count;
    m_buff_ptr__ += len;
    return true;
}

//~class_alloc_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE_data
//~code_end_insert

