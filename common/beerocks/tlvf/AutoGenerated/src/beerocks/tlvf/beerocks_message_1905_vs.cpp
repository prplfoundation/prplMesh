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

#include <beerocks/tlvf/beerocks_message_1905_vs.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

tlvVsClientAssociationEvent::tlvVsClientAssociationEvent(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvVsClientAssociationEvent::tlvVsClientAssociationEvent(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvVsClientAssociationEvent::~tlvVsClientAssociationEvent() {
}
sMacAddr& tlvVsClientAssociationEvent::mac() {
    return (sMacAddr&)(*m_mac);
}

sMacAddr& tlvVsClientAssociationEvent::bssid() {
    return (sMacAddr&)(*m_bssid);
}

int8_t& tlvVsClientAssociationEvent::vap_id() {
    return (int8_t&)(*m_vap_id);
}

beerocks::message::sRadioCapabilities& tlvVsClientAssociationEvent::capabilities() {
    return (beerocks::message::sRadioCapabilities&)(*m_capabilities);
}

uint8_t& tlvVsClientAssociationEvent::disconnect_reason() {
    return (uint8_t&)(*m_disconnect_reason);
}

uint8_t& tlvVsClientAssociationEvent::disconnect_source() {
    return (uint8_t&)(*m_disconnect_source);
}

uint8_t& tlvVsClientAssociationEvent::disconnect_type() {
    return (uint8_t&)(*m_disconnect_type);
}

void tlvVsClientAssociationEvent::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_1905_VS), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_bssid->struct_swap();
    m_capabilities->struct_swap();
}

bool tlvVsClientAssociationEvent::finalize()
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
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t tlvVsClientAssociationEvent::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(beerocks::message::sRadioCapabilities); // capabilities
    class_size += sizeof(uint8_t); // disconnect_reason
    class_size += sizeof(uint8_t); // disconnect_source
    class_size += sizeof(uint8_t); // disconnect_type
    return class_size;
}

bool tlvVsClientAssociationEvent::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_capabilities = (beerocks::message::sRadioCapabilities*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sRadioCapabilities))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sRadioCapabilities) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_capabilities->struct_init(); }
    m_disconnect_reason = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_disconnect_source = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_disconnect_type = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


