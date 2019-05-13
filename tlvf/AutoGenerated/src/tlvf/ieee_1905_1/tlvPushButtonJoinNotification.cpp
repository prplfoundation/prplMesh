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

#include <tlvf/ieee_1905_1/tlvPushButtonJoinNotification.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvPushButtonJoinNotification::tlvPushButtonJoinNotification(uint8_t *buff, size_t buff_len,
                                                             bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
tlvPushButtonJoinNotification::tlvPushButtonJoinNotification(std::shared_ptr<BaseClass> base,
                                                             bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
tlvPushButtonJoinNotification::~tlvPushButtonJoinNotification() {}
const eTlvType &tlvPushButtonJoinNotification::type() { return (const eTlvType &)(*m_type); }

const uint16_t &tlvPushButtonJoinNotification::length() { return (const uint16_t &)(*m_length); }

sMacAddress &tlvPushButtonJoinNotification::al_mac_notification_src()
{
    return (sMacAddress &)(*m_al_mac_notification_src);
}

uint16_t &tlvPushButtonJoinNotification::mid_of_the_notification()
{
    return (uint16_t &)(*m_mid_of_the_notification);
}

sMacAddress &tlvPushButtonJoinNotification::transmitter_iface_mac_of_new_device_joined()
{
    return (sMacAddress &)(*m_transmitter_iface_mac_of_new_device_joined);
}

sMacAddress &tlvPushButtonJoinNotification::iface_mac_of_new_device_joined()
{
    return (sMacAddress &)(*m_iface_mac_of_new_device_joined);
}

void tlvPushButtonJoinNotification::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t *>(m_type));
    tlvf_swap(16, reinterpret_cast<uint8_t *>(m_length));
    m_al_mac_notification_src->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t *>(m_mid_of_the_notification));
    m_transmitter_iface_mac_of_new_device_joined->struct_swap();
    m_iface_mac_of_new_device_joined->struct_swap();
}

size_t tlvPushButtonJoinNotification::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType);    // type
    class_size += sizeof(uint16_t);    // length
    class_size += sizeof(sMacAddress); // al_mac_notification_src
    class_size += sizeof(uint16_t);    // mid_of_the_notification
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
    m_type = (eTlvType *)m_buff_ptr__;
    if (!m_parse__)
        *m_type = eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION;
    else {
        eTlvType swapped_type = *m_type;
        if (m_swap__) {
            tlvf_swap(16, reinterpret_cast<uint8_t *>(&swapped_type));
        }
        if (swapped_type != eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: "
                            << int(eTlvType::TLV_PUSH_BUTTON_JOIN_NOTIFICATION)
                            << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvType) * 1;
    m_length = (uint16_t *)m_buff_ptr__;
    if (!m_parse__)
        *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_al_mac_notification_src = (sMacAddress *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if (m_length && !m_parse__) {
        (*m_length) += sizeof(sMacAddress);
    }
    if (!m_parse__) {
        m_al_mac_notification_src->struct_init();
    }
    m_mid_of_the_notification = (uint16_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if (m_length && !m_parse__) {
        (*m_length) += sizeof(uint16_t);
    }
    m_transmitter_iface_mac_of_new_device_joined = (sMacAddress *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if (m_length && !m_parse__) {
        (*m_length) += sizeof(sMacAddress);
    }
    if (!m_parse__) {
        m_transmitter_iface_mac_of_new_device_joined->struct_init();
    }
    m_iface_mac_of_new_device_joined = (sMacAddress *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if (m_length && !m_parse__) {
        (*m_length) += sizeof(sMacAddress);
    }
    if (!m_parse__) {
        m_iface_mac_of_new_device_joined->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}
