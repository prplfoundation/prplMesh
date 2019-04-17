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

#include <tlvf/ieee_1905_1/tlvPushButtonEventNotification.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvPushButtonEventNotification::tlvPushButtonEventNotification(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvPushButtonEventNotification::tlvPushButtonEventNotification(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvPushButtonEventNotification::~tlvPushButtonEventNotification() {
}
const eTlvType& tlvPushButtonEventNotification::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvPushButtonEventNotification::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvPushButtonEventNotification::media_type_list_length() {
    return (uint8_t&)(*m_media_type_list_length);
}

std::tuple<bool, tlvPushButtonEventNotification::sMediaType&> tlvPushButtonEventNotification::media_type_list(size_t idx) {
    bool ret_success = ( (m_media_type_list_idx__ > 0) && (m_media_type_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_media_type_list[ret_idx]);
}

bool tlvPushButtonEventNotification::alloc_media_type_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sMediaType) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
//TLVF_TODO: enable call to memmove
    m_media_type_list_idx__ += count;
    *m_media_type_list_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_media_type_list_idx__ - count; i < m_media_type_list_idx__; i++) { m_media_type_list[i].struct_init(); }
    }
    return true;
}

void tlvPushButtonEventNotification::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    for (size_t i = 0; i < (size_t)*m_media_type_list_length; i++){
        m_media_type_list[i].struct_swap();
    }
}

size_t tlvPushButtonEventNotification::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // media_type_list_length
    return class_size;
}

bool tlvPushButtonEventNotification::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_PUSH_BUTTON_EVENT_NOTIFICATION;
    else {
        eTlvType swapped_type = *m_type;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&swapped_type)); }
            if (swapped_type != eTlvType::TLV_PUSH_BUTTON_EVENT_NOTIFICATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_PUSH_BUTTON_EVENT_NOTIFICATION) << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_media_type_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_media_type_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_media_type_list = (sMediaType*)m_buff_ptr__;
    m_media_type_list_idx__ = *m_media_type_list_length;
    m_buff_ptr__ += sizeof(sMediaType)*(*m_media_type_list_length);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


