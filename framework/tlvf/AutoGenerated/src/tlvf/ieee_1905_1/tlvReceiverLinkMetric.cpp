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

#include <tlvf/ieee_1905_1/tlvReceiverLinkMetric.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvReceiverLinkMetric::tlvReceiverLinkMetric(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvReceiverLinkMetric::tlvReceiverLinkMetric(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvReceiverLinkMetric::~tlvReceiverLinkMetric() {
}
const eTlvType& tlvReceiverLinkMetric::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvReceiverLinkMetric::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvReceiverLinkMetric::al_mac_of_the_device_that_transmits() {
    return (sMacAddr&)(*m_al_mac_of_the_device_that_transmits);
}

sMacAddr& tlvReceiverLinkMetric::al_mac_of_the_neighbor_whose_link_metric_is_reported_in_this_tlv() {
    return (sMacAddr&)(*m_al_mac_of_the_neighbor_whose_link_metric_is_reported_in_this_tlv);
}

std::tuple<bool, tlvReceiverLinkMetric::sInterfacePairInfo&> tlvReceiverLinkMetric::interface_pair_info(size_t idx) {
    bool ret_success = ( (m_interface_pair_info_idx__ > 0) && (m_interface_pair_info_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_interface_pair_info[ret_idx]);
}

bool tlvReceiverLinkMetric::alloc_interface_pair_info(size_t count) {
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
    m_interface_pair_info_idx__ += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_interface_pair_info_idx__ - count; i < m_interface_pair_info_idx__; i++) { m_interface_pair_info[i].struct_init(); }
    }
    return true;
}

void tlvReceiverLinkMetric::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_al_mac_of_the_device_that_transmits->struct_swap();
    m_al_mac_of_the_neighbor_whose_link_metric_is_reported_in_this_tlv->struct_swap();
    for (size_t i = 0; i < m_interface_pair_info_idx__; i++){
        m_interface_pair_info[i].struct_swap();
    }
}

size_t tlvReceiverLinkMetric::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // al_mac_of_the_device_that_transmits
    class_size += sizeof(sMacAddr); // al_mac_of_the_neighbor_whose_link_metric_is_reported_in_this_tlv
    return class_size;
}

bool tlvReceiverLinkMetric::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_RECEIVER_LINK_METRIC;
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_al_mac_of_the_device_that_transmits = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_al_mac_of_the_device_that_transmits->struct_init(); }
    m_al_mac_of_the_neighbor_whose_link_metric_is_reported_in_this_tlv = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_al_mac_of_the_neighbor_whose_link_metric_is_reported_in_this_tlv->struct_init(); }
    m_interface_pair_info = (sInterfacePairInfo*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&len)); }
        len -= (m_buff_ptr__ - sizeof(*m_type) - sizeof(*m_length) - m_buff__);
        m_interface_pair_info_idx__ = len/sizeof(sInterfacePairInfo);
        m_buff_ptr__ += len;
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_RECEIVER_LINK_METRIC) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_RECEIVER_LINK_METRIC) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


