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

#include <tlvf/wfa_map/tlvBeaconMetricsResponse.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvBeaconMetricsResponse::tlvBeaconMetricsResponse(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvBeaconMetricsResponse::tlvBeaconMetricsResponse(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvBeaconMetricsResponse::~tlvBeaconMetricsResponse() {
}
const eTlvTypeMap& tlvBeaconMetricsResponse::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvBeaconMetricsResponse::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvBeaconMetricsResponse::associated_sta_mac() {
    return (sMacAddr&)(*m_associated_sta_mac);
}

uint8_t& tlvBeaconMetricsResponse::reserved() {
    return (uint8_t&)(*m_reserved);
}

uint8_t& tlvBeaconMetricsResponse::measurement_report_list_length() {
    return (uint8_t&)(*m_measurement_report_list_length);
}

uint8_t* tlvBeaconMetricsResponse::measurement_report_list(size_t idx) {
    if ( (m_measurement_report_list_idx__ == 0) || (m_measurement_report_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_measurement_report_list[idx]);
}

bool tlvBeaconMetricsResponse::set_measurement_report_list(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_measurement_report_list received a null pointer.";
        return false;
    }
    if (!alloc_measurement_report_list(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_measurement_report_list);
    return true;
}
bool tlvBeaconMetricsResponse::alloc_measurement_report_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list measurement_report_list, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_measurement_report_list[*m_measurement_report_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_measurement_report_list_idx__ += count;
    *m_measurement_report_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvBeaconMetricsResponse::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_associated_sta_mac->struct_swap();
}

bool tlvBeaconMetricsResponse::finalize()
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

size_t tlvBeaconMetricsResponse::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // associated_sta_mac
    class_size += sizeof(uint8_t); // reserved
    class_size += sizeof(uint8_t); // measurement_report_list_length
    return class_size;
}

bool tlvBeaconMetricsResponse::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_BEACON_METRICS_RESPONSE;
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
    m_associated_sta_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_associated_sta_mac->struct_init(); }
    m_reserved = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_measurement_report_list_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_measurement_report_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_measurement_report_list = (uint8_t*)m_buff_ptr__;
    uint8_t measurement_report_list_length = *m_measurement_report_list_length;
    m_measurement_report_list_idx__ = measurement_report_list_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (measurement_report_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (measurement_report_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_BEACON_METRICS_RESPONSE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_BEACON_METRICS_RESPONSE) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


