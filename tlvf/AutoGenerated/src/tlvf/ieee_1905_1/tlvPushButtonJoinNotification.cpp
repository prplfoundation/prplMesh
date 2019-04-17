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

#include <tlvf/ieee_1905_1/tlvPushButtonJoinNotification.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvPushButtonJoinNotification::tlvPushButtonJoinNotification(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvPushButtonJoinNotification::tlvPushButtonJoinNotification(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvPushButtonJoinNotification::~tlvPushButtonJoinNotification() {
}
const eTlvType& tlvPushButtonJoinNotification::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvPushButtonJoinNotification::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddress& tlvPushButtonJoinNotification::al_mac_notification_src() {
    return (sMacAddress&)(*m_al_mac_notification_src);
}

uint16_t& tlvPushButtonJoinNotification::mid_of_the_notification() {
    return (uint16_t&)(*m_mid_of_the_notification);
}

sMacAddress& tlvPushButtonJoinNotification::transmitter_iface_mac_of_new_device_joined() {
    return (sMacAddress&)(*m_transmitter_iface_mac_of_new_device_joined);
}

sMacAddress& tlvPushButtonJoinNotification::iface_mac_of_new_device_joined() {
    return (sMacAddress&)(*m_iface_mac_of_new_device_joined);
}

void tlvPushButtonJoinNotification::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_al_mac_notification_src->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_mid_of_the_notification));
    m_transmitter_iface_mac_of_new_device_joined->struct_swap();
    m_iface_mac_of_new_device_joined->struct_swap();
}

size_t tlvPushButtonJoinNotification::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddress); // al_mac_notification_src
    class_size += sizeof(uint16_t); // mid_of_the_notification
    class_size += sizeof(sMacAddress); // transmitter_iface_mac_of_new_device_joined
    class_size += sizeof(sMacAddress); // iface_mac_of_new_device_joined
    return class_size;
}

bool tlvPushButtonJoinNotification::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION;
    else {
        eTlvType swapped_type = *m_type;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&swapped_type)); }
            if (swapped_type != eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION) << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_al_mac_notification_src = (sMacAddress*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddress); }
    if (!m_parse__) { m_al_mac_notification_src->struct_init(); }
    m_mid_of_the_notification = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_transmitter_iface_mac_of_new_device_joined = (sMacAddress*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddress); }
    if (!m_parse__) { m_transmitter_iface_mac_of_new_device_joined->struct_init(); }
    m_iface_mac_of_new_device_joined = (sMacAddress*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddress); }
    if (!m_parse__) { m_iface_mac_of_new_device_joined->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


