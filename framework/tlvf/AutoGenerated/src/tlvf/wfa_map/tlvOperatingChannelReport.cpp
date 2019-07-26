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

#include <tlvf/wfa_map/tlvOperatingChannelReport.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvOperatingChannelReport::tlvOperatingChannelReport(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvOperatingChannelReport::tlvOperatingChannelReport(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvOperatingChannelReport::~tlvOperatingChannelReport() {
}
const eTlvTypeMap& tlvOperatingChannelReport::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvOperatingChannelReport::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvOperatingChannelReport::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvOperatingChannelReport::operating_classes_list_length() {
    return (uint8_t&)(*m_operating_classes_list_length);
}

std::tuple<bool, tlvOperatingChannelReport::sOperatingClasses&> tlvOperatingChannelReport::operating_classes_list(size_t idx) {
    bool ret_success = ( (m_operating_classes_list_idx__ > 0) && (m_operating_classes_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_operating_classes_list[ret_idx]);
}

bool tlvOperatingChannelReport::alloc_operating_classes_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list operating_classes_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sOperatingClasses) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_operating_classes_list;
    uint8_t *dst = (uint8_t *)m_operating_classes_list + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_current_transmit_power = (int8_t *)((uint8_t *)(m_current_transmit_power) + len);
    m_operating_classes_list_idx__ += count;
    *m_operating_classes_list_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_operating_classes_list_idx__ - count; i < m_operating_classes_list_idx__; i++) { m_operating_classes_list[i].struct_init(); }
    }
    return true;
}

int8_t& tlvOperatingChannelReport::current_transmit_power() {
    return (int8_t&)(*m_current_transmit_power);
}

void tlvOperatingChannelReport::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    for (size_t i = 0; i < (size_t)*m_operating_classes_list_length; i++){
        m_operating_classes_list[i].struct_swap();
    }
}

size_t tlvOperatingChannelReport::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // operating_classes_list_length
    class_size += sizeof(int8_t); // current_transmit_power
    return class_size;
}

bool tlvOperatingChannelReport::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_OPERATING_CHANNEL_REPORT;
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_operating_classes_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_operating_classes_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_list = (sOperatingClasses*)m_buff_ptr__;
    uint8_t operating_classes_list_length = *m_operating_classes_list_length;
    m_operating_classes_list_idx__ = operating_classes_list_length;
    m_buff_ptr__ += sizeof(sOperatingClasses)*(operating_classes_list_length);
    m_current_transmit_power = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(int8_t); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_OPERATING_CHANNEL_REPORT) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_OPERATING_CHANNEL_REPORT) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


