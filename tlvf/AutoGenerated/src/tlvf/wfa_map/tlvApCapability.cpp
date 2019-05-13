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

#include <tlvf/tlvflogging.h>
#include <tlvf/wfa_map/tlvApCapability.h>

using namespace wfa_map;

tlvApCapability::tlvApCapability(uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
tlvApCapability::tlvApCapability(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
tlvApCapability::~tlvApCapability() {}
const eTlvTypeMap &tlvApCapability::type() { return (const eTlvTypeMap &)(*m_type); }

const uint16_t &tlvApCapability::length() { return (const uint16_t &)(*m_length); }

tlvApCapability::sValue &tlvApCapability::value() { return (sValue &)(*m_value); }

void tlvApCapability::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t *>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t *>(m_length));
    m_value->struct_swap();
}

size_t tlvApCapability::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t);    // length
    class_size += sizeof(sValue);      // value
    return class_size;
}

bool tlvApCapability::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap *)m_buff_ptr__;
    if (!m_parse__)
        *m_type = eTlvTypeMap::TLV_AP_CAPABILITY;
    else {
        eTlvTypeMap swapped_type = *m_type;
        if (m_swap__) {
            tlvf_swap(16, reinterpret_cast<uint8_t *>(&swapped_type));
        }
        if (swapped_type != eTlvTypeMap::TLV_AP_CAPABILITY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: "
                            << int(eTlvTypeMap::TLV_AP_CAPABILITY)
                            << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t *)m_buff_ptr__;
    if (!m_parse__)
        *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_value = (sValue *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sValue) * 1;
    if (m_length && !m_parse__) {
        (*m_length) += sizeof(sValue);
    }
    if (!m_parse__) {
        m_value->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}
