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

#include <tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvNon1905neighborDeviceList::tlvNon1905neighborDeviceList(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvNon1905neighborDeviceList::tlvNon1905neighborDeviceList(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvNon1905neighborDeviceList::~tlvNon1905neighborDeviceList() {
}
const eTlvType& tlvNon1905neighborDeviceList::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvNon1905neighborDeviceList::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvNon1905neighborDeviceList::mac_local_iface() {
    return (sMacAddr&)(*m_mac_local_iface);
}

std::tuple<bool, sMacAddr&> tlvNon1905neighborDeviceList::mac_non_1905_device(size_t idx) {
    bool ret_success = ( (m_mac_non_1905_device_idx__ > 0) && (m_mac_non_1905_device_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_mac_non_1905_device[ret_idx]);
}

bool tlvNon1905neighborDeviceList::alloc_mac_non_1905_device(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
//TLVF_TODO: enable call to memmove
    m_mac_non_1905_device_idx__ += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_mac_non_1905_device_idx__ - count; i < m_mac_non_1905_device_idx__; i++) { m_mac_non_1905_device[i].struct_init(); }
    }
    return true;
}

void tlvNon1905neighborDeviceList::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_mac_local_iface->struct_swap();
    for (size_t i = 0; i < m_mac_non_1905_device_idx__; i++){
        m_mac_non_1905_device[i].struct_swap();
    }
}

size_t tlvNon1905neighborDeviceList::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // mac_local_iface
    return class_size;
}

bool tlvNon1905neighborDeviceList::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_NON_1905_NEIGHBOR_DEVICE_LIST;
    else {
            if (*m_type != eTlvType::TLV_NON_1905_NEIGHBOR_DEVICE_LIST) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_NON_1905_NEIGHBOR_DEVICE_LIST) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_mac_local_iface = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_mac_local_iface->struct_init(); }
    m_mac_non_1905_device = (sMacAddr*)m_buff_ptr__;
    if (m_length && m_parse__) {
        size_t len = *m_length;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&len)); }
        len -= (m_buff_ptr__ - kMinimumLength - m_buff__);
        m_mac_non_1905_device_idx__ = len/sizeof(sMacAddr);
        m_buff_ptr__ += len;
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


