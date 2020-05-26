///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
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
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_capabilities = reinterpret_cast<beerocks::message::sRadioCapabilities*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sRadioCapabilities))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sRadioCapabilities) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_capabilities->struct_init(); }
    m_disconnect_reason = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_disconnect_source = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_disconnect_type = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

tlvVsChannelScanRequest::tlvVsChannelScanRequest(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvVsChannelScanRequest::tlvVsChannelScanRequest(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvVsChannelScanRequest::~tlvVsChannelScanRequest() {
}
sMacAddr& tlvVsChannelScanRequest::mac() {
    return (sMacAddr&)(*m_mac);
}

int32_t& tlvVsChannelScanRequest::dwell_time_msec() {
    return (int32_t&)(*m_dwell_time_msec);
}

uint8_t& tlvVsChannelScanRequest::channel_pool_length() {
    return (uint8_t&)(*m_channel_pool_length);
}

uint8_t* tlvVsChannelScanRequest::channel_pool(size_t idx) {
    if ( (m_channel_pool_idx__ == 0) || (m_channel_pool_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_channel_pool[idx]);
}

bool tlvVsChannelScanRequest::set_channel_pool(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_channel_pool received a null pointer.";
        return false;
    }
    if (!alloc_channel_pool(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_channel_pool);
    return true;
}
bool tlvVsChannelScanRequest::alloc_channel_pool(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list channel_pool, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_channel_pool[*m_channel_pool_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_channel_pool_idx__ += count;
    *m_channel_pool_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void tlvVsChannelScanRequest::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_1905_VS), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_dwell_time_msec));
}

bool tlvVsChannelScanRequest::finalize()
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

size_t tlvVsChannelScanRequest::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(int32_t); // dwell_time_msec
    class_size += sizeof(uint8_t); // channel_pool_length
    return class_size;
}

bool tlvVsChannelScanRequest::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_dwell_time_msec = reinterpret_cast<int32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    m_channel_pool_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_channel_pool_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_channel_pool = (uint8_t*)m_buff_ptr__;
    uint8_t channel_pool_length = *m_channel_pool_length;
    m_channel_pool_idx__ = channel_pool_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (channel_pool_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (channel_pool_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


