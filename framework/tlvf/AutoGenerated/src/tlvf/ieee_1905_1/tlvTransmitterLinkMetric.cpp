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

#include <tlvf/ieee_1905_1/tlvTransmitterLinkMetric.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvTransmitterLinkMetric::tlvTransmitterLinkMetric(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvTransmitterLinkMetric::tlvTransmitterLinkMetric(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvTransmitterLinkMetric::~tlvTransmitterLinkMetric() {
}
const eTlvType& tlvTransmitterLinkMetric::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvTransmitterLinkMetric::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvTransmitterLinkMetric::reporter_al_mac() {
    return (sMacAddr&)(*m_reporter_al_mac);
}

sMacAddr& tlvTransmitterLinkMetric::neighbor_al_mac() {
    return (sMacAddr&)(*m_neighbor_al_mac);
}

std::tuple<bool, tlvTransmitterLinkMetric::sInterfacePairInfo&> tlvTransmitterLinkMetric::interface_pair_info(size_t idx) {
    bool ret_success = ( (m_interface_pair_info_idx__ > 0) && (m_interface_pair_info_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_interface_pair_info[ret_idx]);
}

bool tlvTransmitterLinkMetric::alloc_interface_pair_info(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list interface_pair_info, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sInterfacePairInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_interface_pair_info;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_interface_pair_info_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_interface_pair_info_idx__ - count; i < m_interface_pair_info_idx__; i++) { m_interface_pair_info[i].struct_init(); }
    }
    return true;
}

void tlvTransmitterLinkMetric::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_reporter_al_mac->struct_swap();
    m_neighbor_al_mac->struct_swap();
    for (size_t i = 0; i < m_interface_pair_info_idx__; i++){
        m_interface_pair_info[i].struct_swap();
    }
}

size_t tlvTransmitterLinkMetric::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // reporter_al_mac
    class_size += sizeof(sMacAddr); // neighbor_al_mac
    return class_size;
}

bool tlvTransmitterLinkMetric::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_TRANSMITTER_LINK_METRIC;
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
    m_reporter_al_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_reporter_al_mac->struct_init(); }
    m_neighbor_al_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_neighbor_al_mac->struct_init(); }
    m_interface_pair_info = (sInterfacePairInfo*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&len)); }
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_interface_pair_info_idx__ = len/sizeof(sInterfacePairInfo);
        if (!buffPtrIncrementSafe(len)) {
            LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
            return false;
        }
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_TRANSMITTER_LINK_METRIC) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_TRANSMITTER_LINK_METRIC) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


