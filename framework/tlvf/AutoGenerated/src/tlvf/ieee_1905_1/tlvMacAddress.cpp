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

#include <tlvf/ieee_1905_1/tlvMacAddress.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvMacAddress::tlvMacAddress(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvMacAddress::tlvMacAddress(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvMacAddress::~tlvMacAddress() {
}
const eTlvType& tlvMacAddress::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvMacAddress::length() {
    return (const uint16_t&)(*m_length);
}

std::tuple<bool, uint8_t&> tlvMacAddress::mac(size_t idx) {
    bool ret_success = ( (m_mac_idx__ > 0) && (m_mac_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_mac[ret_idx]);
}

void tlvMacAddress::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t tlvMacAddress::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += 6 * sizeof(uint8_t); // mac
    return class_size;
}

bool tlvMacAddress::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_MAC_ADDRESS;
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_mac = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(uint8_t) * 6);
    m_mac_idx__  = 6;
    if (!m_parse__) {
        if (m_length) { (*m_length) += (sizeof(uint8_t) * 6); }
        for (size_t i = 0; i < 6; i++){
            m_mac[i] = 0x0;
        }
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_MAC_ADDRESS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_MAC_ADDRESS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


