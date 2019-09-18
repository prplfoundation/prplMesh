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

#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvDeviceInformation::tlvDeviceInformation(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvDeviceInformation::tlvDeviceInformation(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvDeviceInformation::~tlvDeviceInformation() {
}
const eTlvType& tlvDeviceInformation::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvDeviceInformation::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvDeviceInformation::mac() {
    return (sMacAddr&)(*m_mac);
}

uint8_t& tlvDeviceInformation::info_length() {
    return (uint8_t&)(*m_info_length);
}

std::tuple<bool, tlvDeviceInformation::sInfo&> tlvDeviceInformation::info(size_t idx) {
    bool ret_success = ( (m_info_idx__ > 0) && (m_info_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_info[ret_idx]);
}

bool tlvDeviceInformation::alloc_info(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list info, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_info[*m_info_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_info_idx__ += count;
    *m_info_length += count;
    if (!buffPtrIncrementSafe(len)) { return false; }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_info_idx__ - count; i < m_info_idx__; i++) { m_info[i].struct_init(); }
    }
    return true;
}

void tlvDeviceInformation::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_mac->struct_swap();
    for (size_t i = 0; i < (size_t)*m_info_length; i++){
        m_info[i].struct_swap();
    }
}

size_t tlvDeviceInformation::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(uint8_t); // info_length
    return class_size;
}

bool tlvDeviceInformation::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_DEVICE_INFORMATION;
    if (!buffPtrIncrementSafe(sizeof(eTlvType))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_mac->struct_init(); }
    m_info_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_info_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_info = (sInfo*)m_buff_ptr__;
    uint8_t info_length = *m_info_length;
    m_info_idx__ = info_length;
    if (!buffPtrIncrementSafe(sizeof(sInfo)*(info_length))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_DEVICE_INFORMATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_DEVICE_INFORMATION) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


