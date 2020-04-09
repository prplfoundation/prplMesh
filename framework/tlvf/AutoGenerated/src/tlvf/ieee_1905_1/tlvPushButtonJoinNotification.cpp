///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/ieee_1905_1/tlvPushButtonJoinNotification.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvPushButtonJoinNotification::tlvPushButtonJoinNotification(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvPushButtonJoinNotification::tlvPushButtonJoinNotification(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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

sMacAddr& tlvPushButtonJoinNotification::al_mac_notification_src() {
    return (sMacAddr&)(*m_al_mac_notification_src);
}

uint16_t& tlvPushButtonJoinNotification::mid_of_the_notification() {
    return (uint16_t&)(*m_mid_of_the_notification);
}

sMacAddr& tlvPushButtonJoinNotification::transmitter_iface_mac_of_new_device_joined() {
    return (sMacAddr&)(*m_transmitter_iface_mac_of_new_device_joined);
}

sMacAddr& tlvPushButtonJoinNotification::iface_mac_of_new_device_joined() {
    return (sMacAddr&)(*m_iface_mac_of_new_device_joined);
}

void tlvPushButtonJoinNotification::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_al_mac_notification_src->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_mid_of_the_notification));
    m_transmitter_iface_mac_of_new_device_joined->struct_swap();
    m_iface_mac_of_new_device_joined->struct_swap();
}

bool tlvPushButtonJoinNotification::finalize()
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

size_t tlvPushButtonJoinNotification::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // al_mac_notification_src
    class_size += sizeof(uint16_t); // mid_of_the_notification
    class_size += sizeof(sMacAddr); // transmitter_iface_mac_of_new_device_joined
    class_size += sizeof(sMacAddr); // iface_mac_of_new_device_joined
    return class_size;
}

bool tlvPushButtonJoinNotification::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION;
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
    m_al_mac_notification_src = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_al_mac_notification_src->struct_init(); }
    m_mid_of_the_notification = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint16_t); }
    m_transmitter_iface_mac_of_new_device_joined = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_transmitter_iface_mac_of_new_device_joined->struct_init(); }
    m_iface_mac_of_new_device_joined = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_iface_mac_of_new_device_joined->struct_init(); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


