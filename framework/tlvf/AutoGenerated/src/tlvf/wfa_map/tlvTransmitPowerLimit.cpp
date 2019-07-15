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

#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvTransmitPowerLimit::tlvTransmitPowerLimit(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvTransmitPowerLimit::tlvTransmitPowerLimit(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvTransmitPowerLimit::~tlvTransmitPowerLimit() {
}
const eTlvTypeMap& tlvTransmitPowerLimit::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvTransmitPowerLimit::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvTransmitPowerLimit::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvTransmitPowerLimit::transmit_power_limit_dbm() {
    return (uint8_t&)(*m_transmit_power_limit_dbm);
}

void tlvTransmitPowerLimit::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
}

size_t tlvTransmitPowerLimit::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // transmit_power_limit_dbm
    return class_size;
}

bool tlvTransmitPowerLimit::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT;
    else {
            if (*m_type != eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT) << ", received value: " << int(*m_type);
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
    m_transmit_power_limit_dbm = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


