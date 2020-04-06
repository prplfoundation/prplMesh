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

#include <beerocks/tlvf/beerocks_message_common.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::~cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

sMacAddr& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::bssid() {
    return (sMacAddr&)(*m_bssid);
}

beerocks::message::sRadioCapabilities& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::capabilities() {
    return (beerocks::message::sRadioCapabilities&)(*m_capabilities);
}

int8_t& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

uint8_t* cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::association_frame(size_t idx) {
    if ( (m_association_frame_idx__ == 0) || (m_association_frame_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_association_frame[idx]);
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::set_association_frame(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_association_frame received a null pointer.";
        return false;
    }
    if (!alloc_association_frame(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_association_frame);
    return true;
}
bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::alloc_association_frame(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list association_frame, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_association_frame;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_association_frame_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
    m_bssid->struct_swap();
    m_capabilities->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(beerocks::message::sRadioCapabilities); // capabilities
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::init()
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
    m_capabilities = (beerocks::message::sRadioCapabilities*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sRadioCapabilities))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sRadioCapabilities) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_capabilities->struct_init(); }
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_association_frame = (uint8_t*)m_buff_ptr__;
    m_association_frame_idx__ = getBuffRemainingBytes();
    if (m_parse__) { class_swap(); }
    return true;
}


