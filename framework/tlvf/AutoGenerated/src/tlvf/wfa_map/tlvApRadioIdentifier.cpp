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

#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApRadioIdentifier::tlvApRadioIdentifier(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvApRadioIdentifier::tlvApRadioIdentifier(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvApRadioIdentifier::~tlvApRadioIdentifier() {
}
const eTlvTypeMap& tlvApRadioIdentifier::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApRadioIdentifier::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvApRadioIdentifier::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

void tlvApRadioIdentifier::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
}

size_t tlvApRadioIdentifier::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    return class_size;
}

bool tlvApRadioIdentifier::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER;
    else {
            if (*m_type != eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


