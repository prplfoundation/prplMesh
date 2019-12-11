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

#include <tlvf/ieee_1905_1/tlvLinkMetricResultCode.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvLinkMetricResultCode::tlvLinkMetricResultCode(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvLinkMetricResultCode::tlvLinkMetricResultCode(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvLinkMetricResultCode::~tlvLinkMetricResultCode() {
}
const eTlvType& tlvLinkMetricResultCode::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvLinkMetricResultCode::length() {
    return (const uint16_t&)(*m_length);
}

tlvLinkMetricResultCode::eValue& tlvLinkMetricResultCode::value() {
    return (eValue&)(*m_value);
}

void tlvLinkMetricResultCode::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t tlvLinkMetricResultCode::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eValue); // value
    return class_size;
}

bool tlvLinkMetricResultCode::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_LINK_METRIC_RESULT_CODE;
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
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_LINK_METRIC_RESULT_CODE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_LINK_METRIC_RESULT_CODE) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


