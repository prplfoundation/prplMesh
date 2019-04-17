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

sMacAddress& tlvDeviceInformation::mac() {
    return (sMacAddress&)(*m_mac);
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
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
//TLVF_TODO: enable call to memmove
    m_info_idx__ += count;
    *m_info_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_info_idx__ - count; i < m_info_idx__; i++) { m_info[i].struct_init(); }
    }
    return true;
}

void tlvDeviceInformation::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
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
    class_size += sizeof(sMacAddress); // mac
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
    else {
        eTlvType swapped_type = *m_type;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&swapped_type)); }
            if (swapped_type != eTlvType::TLV_DEVICE_INFORMATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_DEVICE_INFORMATION) << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_mac = (sMacAddress*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddress); }
    if (!m_parse__) { m_mac->struct_init(); }
    m_info_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_info_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_info = (sInfo*)m_buff_ptr__;
    m_info_idx__ = *m_info_length;
    m_buff_ptr__ += sizeof(sInfo)*(*m_info_length);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


