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

#include <tlvf/wfa_map/tlvSupportedService.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvSupportedService::tlvSupportedService(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvSupportedService::tlvSupportedService(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvSupportedService::~tlvSupportedService() {
}
const eTlvTypeMap& tlvSupportedService::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvSupportedService::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvSupportedService::supported_service_list_length() {
    return (uint8_t&)(*m_supported_service_list_length);
}

std::tuple<bool, tlvSupportedService::eSupportedService&> tlvSupportedService::supported_service_list(size_t idx) {
    bool ret_success = ( (m_supported_service_list_idx__ > 0) && (m_supported_service_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_supported_service_list[ret_idx]);
}

bool tlvSupportedService::alloc_supported_service_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list supported_service_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(eSupportedService) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_supported_service_list;
        uint8_t *dst = (uint8_t *)m_supported_service_list + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_supported_service_list_idx__ += count;
    *m_supported_service_list_length += count;
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvSupportedService::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

size_t tlvSupportedService::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // supported_service_list_length
    return class_size;
}

bool tlvSupportedService::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_SUPPORTED_SERVICE;
    else {
            if (*m_type != eTlvTypeMap::TLV_SUPPORTED_SERVICE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_SUPPORTED_SERVICE) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_supported_service_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_supported_service_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_supported_service_list = (eSupportedService*)m_buff_ptr__;
    uint8_t supported_service_list_length = *m_supported_service_list_length;
    m_supported_service_list_idx__ = supported_service_list_length;
    m_buff_ptr__ += sizeof(eSupportedService)*(supported_service_list_length);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


