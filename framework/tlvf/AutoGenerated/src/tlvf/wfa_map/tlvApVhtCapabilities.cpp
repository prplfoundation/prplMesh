///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvApVhtCapabilities.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApVhtCapabilities::tlvApVhtCapabilities(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvApVhtCapabilities::tlvApVhtCapabilities(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvApVhtCapabilities::~tlvApVhtCapabilities() {
}
const eTlvTypeMap& tlvApVhtCapabilities::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApVhtCapabilities::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvApVhtCapabilities::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint16_t& tlvApVhtCapabilities::supported_vht_tx_mcs() {
    return (uint16_t&)(*m_supported_vht_tx_mcs);
}

uint16_t& tlvApVhtCapabilities::supported_vht_rx_mcs() {
    return (uint16_t&)(*m_supported_vht_rx_mcs);
}

tlvApVhtCapabilities::sFlags1& tlvApVhtCapabilities::flags1() {
    return (sFlags1&)(*m_flags1);
}

tlvApVhtCapabilities::sFlags2& tlvApVhtCapabilities::flags2() {
    return (sFlags2&)(*m_flags2);
}

void tlvApVhtCapabilities::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_supported_vht_tx_mcs));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_supported_vht_rx_mcs));
    m_flags1->struct_swap();
    m_flags2->struct_swap();
}

bool tlvApVhtCapabilities::finalize()
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
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t tlvApVhtCapabilities::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint16_t); // supported_vht_tx_mcs
    class_size += sizeof(uint16_t); // supported_vht_rx_mcs
    class_size += sizeof(sFlags1); // flags1
    class_size += sizeof(sFlags2); // flags2
    return class_size;
}

bool tlvApVhtCapabilities::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_VHT_CAPABILITIES;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_supported_vht_tx_mcs = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_supported_vht_rx_mcs = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_flags1 = (sFlags1*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sFlags1))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sFlags1) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sFlags1); }
    if (!m_parse__) { m_flags1->struct_init(); }
    m_flags2 = (sFlags2*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sFlags2))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sFlags2) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sFlags2); }
    if (!m_parse__) { m_flags2->struct_init(); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_AP_VHT_CAPABILITIES) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_VHT_CAPABILITIES) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


