///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvAssociatedStaTrafficStats.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvAssociatedStaTrafficStats::tlvAssociatedStaTrafficStats(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvAssociatedStaTrafficStats::tlvAssociatedStaTrafficStats(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvAssociatedStaTrafficStats::~tlvAssociatedStaTrafficStats() {
}
const eTlvTypeMap& tlvAssociatedStaTrafficStats::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvAssociatedStaTrafficStats::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvAssociatedStaTrafficStats::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

uint32_t& tlvAssociatedStaTrafficStats::byte_sent() {
    return (uint32_t&)(*m_byte_sent);
}

uint32_t& tlvAssociatedStaTrafficStats::byte_recived() {
    return (uint32_t&)(*m_byte_recived);
}

uint32_t& tlvAssociatedStaTrafficStats::packets_sent() {
    return (uint32_t&)(*m_packets_sent);
}

uint32_t& tlvAssociatedStaTrafficStats::packets_recived() {
    return (uint32_t&)(*m_packets_recived);
}

uint32_t& tlvAssociatedStaTrafficStats::tx_packets_error() {
    return (uint32_t&)(*m_tx_packets_error);
}

uint32_t& tlvAssociatedStaTrafficStats::rx_packets_error() {
    return (uint32_t&)(*m_rx_packets_error);
}

uint32_t& tlvAssociatedStaTrafficStats::retransmission_count() {
    return (uint32_t&)(*m_retransmission_count);
}

void tlvAssociatedStaTrafficStats::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_sta_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_byte_sent));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_byte_recived));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_packets_sent));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_packets_recived));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_tx_packets_error));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_rx_packets_error));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_retransmission_count));
}

bool tlvAssociatedStaTrafficStats::finalize()
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

size_t tlvAssociatedStaTrafficStats::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // sta_mac
    class_size += sizeof(uint32_t); // byte_sent
    class_size += sizeof(uint32_t); // byte_recived
    class_size += sizeof(uint32_t); // packets_sent
    class_size += sizeof(uint32_t); // packets_recived
    class_size += sizeof(uint32_t); // tx_packets_error
    class_size += sizeof(uint32_t); // rx_packets_error
    class_size += sizeof(uint32_t); // retransmission_count
    return class_size;
}

bool tlvAssociatedStaTrafficStats::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_ASSOCIATED_STA_TRAFFIC_STATS;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_sta_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    m_byte_sent = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_byte_recived = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_packets_sent = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_packets_recived = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_tx_packets_error = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_rx_packets_error = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_retransmission_count = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_ASSOCIATED_STA_TRAFFIC_STATS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_ASSOCIATED_STA_TRAFFIC_STATS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


