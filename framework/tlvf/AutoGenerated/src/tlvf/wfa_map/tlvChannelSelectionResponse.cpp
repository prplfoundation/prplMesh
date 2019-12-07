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

#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvChannelSelectionResponse::tlvChannelSelectionResponse(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvChannelSelectionResponse::tlvChannelSelectionResponse(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvChannelSelectionResponse::~tlvChannelSelectionResponse() {
}
const eTlvTypeMap& tlvChannelSelectionResponse::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvChannelSelectionResponse::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvChannelSelectionResponse::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

tlvChannelSelectionResponse::eResponseCode& tlvChannelSelectionResponse::response_code() {
    return (eResponseCode&)(*m_response_code);
}

void tlvChannelSelectionResponse::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
}

size_t tlvChannelSelectionResponse::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(eResponseCode); // response_code
    return class_size;
}

bool tlvChannelSelectionResponse::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_CHANNEL_SELECTION_RESPONSE;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_response_code = (eResponseCode*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eResponseCode))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eResponseCode); }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_CHANNEL_SELECTION_RESPONSE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_CHANNEL_SELECTION_RESPONSE) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


