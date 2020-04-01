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

#include <tlvf/wfa_map/tlvAssociatedStaLinkMetrics.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvAssociatedStaLinkMetrics::tlvAssociatedStaLinkMetrics(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvAssociatedStaLinkMetrics::tlvAssociatedStaLinkMetrics(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvAssociatedStaLinkMetrics::~tlvAssociatedStaLinkMetrics() {
}
const eTlvTypeMap& tlvAssociatedStaLinkMetrics::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvAssociatedStaLinkMetrics::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvAssociatedStaLinkMetrics::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

uint8_t& tlvAssociatedStaLinkMetrics::bssid_info_list_length() {
    return (uint8_t&)(*m_bssid_info_list_length);
}

std::tuple<bool, tlvAssociatedStaLinkMetrics::sBssidInfo&> tlvAssociatedStaLinkMetrics::bssid_info_list(size_t idx) {
    bool ret_success = ( (m_bssid_info_list_idx__ > 0) && (m_bssid_info_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_bssid_info_list[ret_idx]);
}

bool tlvAssociatedStaLinkMetrics::alloc_bssid_info_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list bssid_info_list, abort!";
        return false;
    }
    size_t len = sizeof(sBssidInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_bssid_info_list[*m_bssid_info_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_bssid_info_list_idx__ += count;
    *m_bssid_info_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_bssid_info_list_idx__ - count; i < m_bssid_info_list_idx__; i++) { m_bssid_info_list[i].struct_init(); }
    }
    return true;
}

void tlvAssociatedStaLinkMetrics::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_sta_mac->struct_swap();
    for (size_t i = 0; i < (size_t)*m_bssid_info_list_length; i++){
        m_bssid_info_list[i].struct_swap();
    }
}

bool tlvAssociatedStaLinkMetrics::finalize()
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

size_t tlvAssociatedStaLinkMetrics::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // sta_mac
    class_size += sizeof(uint8_t); // bssid_info_list_length
    return class_size;
}

bool tlvAssociatedStaLinkMetrics::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_ASSOCIATED_STA_LINK_METRICS;
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
    m_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    m_bssid_info_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_bssid_info_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bssid_info_list = (sBssidInfo*)m_buff_ptr__;
    uint8_t bssid_info_list_length = *m_bssid_info_list_length;
    m_bssid_info_list_idx__ = bssid_info_list_length;
    if (!buffPtrIncrementSafe(sizeof(sBssidInfo) * (bssid_info_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBssidInfo) * (bssid_info_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_ASSOCIATED_STA_LINK_METRICS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_ASSOCIATED_STA_LINK_METRICS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


