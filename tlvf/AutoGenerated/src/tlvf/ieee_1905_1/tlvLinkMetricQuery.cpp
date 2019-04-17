///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
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


