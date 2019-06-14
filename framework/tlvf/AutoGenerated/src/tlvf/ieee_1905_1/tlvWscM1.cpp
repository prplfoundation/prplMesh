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

#include <tlvf/ieee_1905_1/tlvWscM1.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvWscM1::tlvWscM1(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvWscM1::tlvWscM1(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvWscM1::~tlvWscM1() {
}
const eTlvType& tlvWscM1::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvWscM1::length() {
    return (const uint16_t&)(*m_length);
}

WSC::sM1& tlvWscM1::M1Frame() {
    return (WSC::sM1&)(*m_M1Frame);
}

void tlvWscM1::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_M1Frame->struct_swap();
}

size_t tlvWscM1::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(WSC::sM1); // M1Frame
    return class_size;
}

bool tlvWscM1::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_WSC;
    else {
            if (*m_type != eTlvType::TLV_WSC) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_WSC) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_M1Frame = (WSC::sM1*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(WSC::sM1) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(WSC::sM1); }
    if (!m_parse__) { m_M1Frame->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


