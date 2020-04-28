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

#include <beerocks/tlvf/channels_list.h>
#include <tlvf/tlvflogging.h>

cChannelsList::cChannelsList(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cChannelsList::cChannelsList(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cChannelsList::~cChannelsList() {
}
uint8_t& cChannelsList::supported_channels_length() {
    return (uint8_t&)(*m_supported_channels_length);
}

std::tuple<bool, beerocks::message::sWifiChannel&> cChannelsList::supported_channels(size_t idx) {
    bool ret_success = ( (m_supported_channels_idx__ > 0) && (m_supported_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_supported_channels[ret_idx]);
}

bool cChannelsList::alloc_supported_channels(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list supported_channels, abort!";
        return false;
    }
    size_t len = sizeof(beerocks::message::sWifiChannel) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_supported_channels[*m_supported_channels_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_supported_channels_idx__ += count;
    *m_supported_channels_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_supported_channels_idx__ - count; i < m_supported_channels_idx__; i++) { m_supported_channels[i].struct_init(); }
    }
    return true;
}

void cChannelsList::class_swap()
{
    for (size_t i = 0; i < (size_t)*m_supported_channels_length; i++){
        m_supported_channels[i].struct_swap();
    }
}

bool cChannelsList::finalize()
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

size_t cChannelsList::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // supported_channels_length
    return class_size;
}

bool cChannelsList::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_supported_channels_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_supported_channels = (beerocks::message::sWifiChannel*)m_buff_ptr__;
    uint8_t supported_channels_length = *m_supported_channels_length;
    m_supported_channels_idx__ = supported_channels_length;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sWifiChannel) * (supported_channels_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sWifiChannel) * (supported_channels_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


