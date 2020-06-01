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

#include <tlvf/wfa_map/tlvChannelScanResult.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvChannelScanResult::tlvChannelScanResult(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvChannelScanResult::tlvChannelScanResult(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvChannelScanResult::~tlvChannelScanResult() {
}
const eTlvTypeMap& tlvChannelScanResult::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvChannelScanResult::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvChannelScanResult::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvChannelScanResult::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& tlvChannelScanResult::channel() {
    return (uint8_t&)(*m_channel);
}

tlvChannelScanResult::eScanStatus& tlvChannelScanResult::success() {
    return (eScanStatus&)(*m_success);
}

uint8_t& tlvChannelScanResult::timestamp_length() {
    return (uint8_t&)(*m_timestamp_length);
}

uint8_t* tlvChannelScanResult::timestamp(size_t idx) {
    if ( (m_timestamp_idx__ == 0) || (m_timestamp_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_timestamp[idx]);
}

bool tlvChannelScanResult::set_timestamp(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_timestamp received a null pointer.";
        return false;
    }
    if (!alloc_timestamp(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_timestamp);
    return true;
}
bool tlvChannelScanResult::alloc_timestamp(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list timestamp, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_timestamp[*m_timestamp_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_utilization = (uint8_t *)((uint8_t *)(m_utilization) + len);
    m_noise = (uint8_t *)((uint8_t *)(m_noise) + len);
    m_neighbors_list_length = (uint16_t *)((uint8_t *)(m_neighbors_list_length) + len);
    m_neighbors_list = (sNeighbors *)((uint8_t *)(m_neighbors_list) + len);
    m_aggregate_scan_duration = (uint32_t *)((uint8_t *)(m_aggregate_scan_duration) + len);
    m_scan_type = (eScanType *)((uint8_t *)(m_scan_type) + len);
    m_timestamp_idx__ += count;
    *m_timestamp_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

uint8_t& tlvChannelScanResult::utilization() {
    return (uint8_t&)(*m_utilization);
}

uint8_t& tlvChannelScanResult::noise() {
    return (uint8_t&)(*m_noise);
}

uint16_t& tlvChannelScanResult::neighbors_list_length() {
    return (uint16_t&)(*m_neighbors_list_length);
}

std::tuple<bool, tlvChannelScanResult::sNeighbors&> tlvChannelScanResult::neighbors_list(size_t idx) {
    bool ret_success = ( (m_neighbors_list_idx__ > 0) && (m_neighbors_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_neighbors_list[ret_idx]);
}

bool tlvChannelScanResult::alloc_neighbors_list(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list neighbors_list, abort!";
        return false;
    }
    size_t len = sizeof(sNeighbors) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_neighbors_list[*m_neighbors_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_aggregate_scan_duration = (uint32_t *)((uint8_t *)(m_aggregate_scan_duration) + len);
    m_scan_type = (eScanType *)((uint8_t *)(m_scan_type) + len);
    m_neighbors_list_idx__ += count;
    *m_neighbors_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_neighbors_list_idx__ - count; i < m_neighbors_list_idx__; i++) { m_neighbors_list[i].struct_init(); }
    }
    return true;
}

uint32_t& tlvChannelScanResult::aggregate_scan_duration() {
    return (uint32_t&)(*m_aggregate_scan_duration);
}

tlvChannelScanResult::eScanType& tlvChannelScanResult::scan_type() {
    return (eScanType&)(*m_scan_type);
}

void tlvChannelScanResult::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    tlvf_swap(8*sizeof(eScanStatus), reinterpret_cast<uint8_t*>(m_success));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_neighbors_list_length));
    for (size_t i = 0; i < m_neighbors_list_idx__; i++){
        m_neighbors_list[i].struct_swap();
    }
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_aggregate_scan_duration));
    tlvf_swap(8*sizeof(eScanType), reinterpret_cast<uint8_t*>(m_scan_type));
}

bool tlvChannelScanResult::finalize()
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

size_t tlvChannelScanResult::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(eScanStatus); // success
    class_size += sizeof(uint8_t); // timestamp_length
    class_size += sizeof(uint8_t); // utilization
    class_size += sizeof(uint8_t); // noise
    class_size += sizeof(uint16_t); // neighbors_list_length
    class_size += sizeof(uint32_t); // aggregate_scan_duration
    class_size += sizeof(eScanType); // scan_type
    return class_size;
}

bool tlvChannelScanResult::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_CHANNEL_SCAN_RESULT;
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
    m_radio_uid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_operating_class = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_success = reinterpret_cast<eScanStatus*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(eScanStatus))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eScanStatus) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eScanStatus); }
    m_timestamp_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_timestamp_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_timestamp = (uint8_t*)m_buff_ptr__;
    uint8_t timestamp_length = *m_timestamp_length;
    m_timestamp_idx__ = timestamp_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (timestamp_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (timestamp_length) << ") Failed!";
        return false;
    }
    m_utilization = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_noise = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_neighbors_list_length = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!m_parse__) *m_neighbors_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_neighbors_list = (sNeighbors*)m_buff_ptr__;
    uint16_t neighbors_list_length = *m_neighbors_list_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&neighbors_list_length)); }
    m_neighbors_list_idx__ = neighbors_list_length;
    if (!buffPtrIncrementSafe(sizeof(sNeighbors) * (neighbors_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNeighbors) * (neighbors_list_length) << ") Failed!";
        return false;
    }
    m_aggregate_scan_duration = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint32_t); }
    m_scan_type = reinterpret_cast<eScanType*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(eScanType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eScanType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eScanType); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_CHANNEL_SCAN_RESULT) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_CHANNEL_SCAN_RESULT) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


