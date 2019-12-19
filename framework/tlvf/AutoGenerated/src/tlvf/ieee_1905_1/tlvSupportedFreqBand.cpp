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

#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvSupportedFreqBand::tlvSupportedFreqBand(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvSupportedFreqBand::tlvSupportedFreqBand(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvSupportedFreqBand::~tlvSupportedFreqBand() {
}
const eTlvType& tlvSupportedFreqBand::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvSupportedFreqBand::length() {
    return (const uint16_t&)(*m_length);
}

tlvSupportedFreqBand::eValue& tlvSupportedFreqBand::value() {
    return (eValue&)(*m_value);
}

void tlvSupportedFreqBand::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t tlvSupportedFreqBand::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eValue); // value
    return class_size;
}

bool tlvSupportedFreqBand::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_SUPPORTED_FREQ_BAND;
    if (!buffPtrIncrementSafe(sizeof(eTlvType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvType) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_value = (eValue*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eValue))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eValue) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eValue); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_SUPPORTED_FREQ_BAND) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_SUPPORTED_FREQ_BAND) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


