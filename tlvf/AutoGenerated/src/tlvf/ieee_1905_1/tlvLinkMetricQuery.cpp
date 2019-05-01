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

#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvLinkMetricQuery::tlvLinkMetricQuery(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvLinkMetricQuery::tlvLinkMetricQuery(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

tlvLinkMetricQuery::eNeighborType& tlvLinkMetricQuery::neighbor_type() {
    return (eNeighborType&)(*m_neighbor_type);
}

sMacAddress& tlvLinkMetricQuery::mac_al_1905_device() {
    return (sMacAddress&)(*m_mac_al_1905_device);
}

tlvLinkMetricQuery::eLinkMetricsType& tlvLinkMetricQuery::link_metrics() {
    return (eLinkMetricsType&)(*m_link_metrics);
}

void tlvLinkMetricQuery::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_mac_al_1905_device->struct_swap();
}

size_t tlvLinkMetricQuery::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(eNeighborType); // neighbor_type
    class_size += sizeof(sMacAddress); // mac_al_1905_device
    class_size += sizeof(eLinkMetricsType); // link_metrics
    return class_size;
}

bool tlvLinkMetricQuery::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_LINK_METRIC_QUERY;
    else {
        eTlvType swapped_type = *m_type;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&swapped_type)); }
            if (swapped_type != eTlvType::TLV_LINK_METRIC_QUERY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_LINK_METRIC_QUERY) << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_neighbor_type = (eNeighborType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(eNeighborType) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(eNeighborType); }
    m_mac_al_1905_device = (sMacAddress*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddress); }
    if (!m_parse__) { m_mac_al_1905_device->struct_init(); }
    m_link_metrics = (eLinkMetricsType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(eLinkMetricsType) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(eLinkMetricsType); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


