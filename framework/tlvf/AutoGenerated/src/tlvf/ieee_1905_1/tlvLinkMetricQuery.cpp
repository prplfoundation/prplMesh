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

#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvLinkMetricQueryAllNeighbors::tlvLinkMetricQueryAllNeighbors(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvLinkMetricQueryAllNeighbors::tlvLinkMetricQueryAllNeighbors(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvLinkMetricQueryAllNeighbors::~tlvLinkMetricQueryAllNeighbors() {
}
const eTlvType& tlvLinkMetricQueryAllNeighbors::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvLinkMetricQueryAllNeighbors::length() {
    return (const uint16_t&)(*m_length);
}

const eLinkMetricNeighborType& tlvLinkMetricQueryAllNeighbors::neighbor_type() {
    return (const eLinkMetricNeighborType&)(*m_neighbor_type);
}

eLinkMetricsType& tlvLinkMetricQueryAllNeighbors::link_metrics_type() {
    return (eLinkMetricsType&)(*m_link_metrics_type);
}

void tlvLinkMetricQueryAllNeighbors::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(8*sizeof(eLinkMetricsType), reinterpret_cast<uint8_t*>(m_link_metrics_type));
}

bool tlvLinkMetricQueryAllNeighbors::finalize()
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

size_t tlvLinkMetricQueryAllNeighbors::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eLinkMetricNeighborType); // neighbor_type
    class_size += sizeof(eLinkMetricsType); // link_metrics_type
    return class_size;
}

bool tlvLinkMetricQueryAllNeighbors::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_LINK_METRIC_QUERY;
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
    m_neighbor_type = (eLinkMetricNeighborType*)m_buff_ptr__;
    if (!m_parse__) *m_neighbor_type = eLinkMetricNeighborType::ALL_NEIGHBORS;
    if (!buffPtrIncrementSafe(sizeof(eLinkMetricNeighborType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eLinkMetricNeighborType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eLinkMetricNeighborType); }
    m_link_metrics_type = (eLinkMetricsType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eLinkMetricsType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eLinkMetricsType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eLinkMetricsType); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_LINK_METRIC_QUERY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_LINK_METRIC_QUERY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

tlvLinkMetricQuery::tlvLinkMetricQuery(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvLinkMetricQuery::tlvLinkMetricQuery(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvLinkMetricQuery::~tlvLinkMetricQuery() {
}
const eTlvType& tlvLinkMetricQuery::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvLinkMetricQuery::length() {
    return (const uint16_t&)(*m_length);
}

eLinkMetricNeighborType& tlvLinkMetricQuery::neighbor_type() {
    return (eLinkMetricNeighborType&)(*m_neighbor_type);
}

sMacAddr& tlvLinkMetricQuery::mac_al_1905_device() {
    return (sMacAddr&)(*m_mac_al_1905_device);
}

eLinkMetricsType& tlvLinkMetricQuery::link_metrics_type() {
    return (eLinkMetricsType&)(*m_link_metrics_type);
}

void tlvLinkMetricQuery::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(8*sizeof(eLinkMetricNeighborType), reinterpret_cast<uint8_t*>(m_neighbor_type));
    m_mac_al_1905_device->struct_swap();
    tlvf_swap(8*sizeof(eLinkMetricsType), reinterpret_cast<uint8_t*>(m_link_metrics_type));
}

bool tlvLinkMetricQuery::finalize()
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

size_t tlvLinkMetricQuery::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eLinkMetricNeighborType); // neighbor_type
    class_size += sizeof(sMacAddr); // mac_al_1905_device
    class_size += sizeof(eLinkMetricsType); // link_metrics_type
    return class_size;
}

bool tlvLinkMetricQuery::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_LINK_METRIC_QUERY;
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
    m_neighbor_type = (eLinkMetricNeighborType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eLinkMetricNeighborType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eLinkMetricNeighborType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eLinkMetricNeighborType); }
    m_mac_al_1905_device = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_mac_al_1905_device->struct_init(); }
    m_link_metrics_type = (eLinkMetricsType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eLinkMetricsType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eLinkMetricsType) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(eLinkMetricsType); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_LINK_METRIC_QUERY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_LINK_METRIC_QUERY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


