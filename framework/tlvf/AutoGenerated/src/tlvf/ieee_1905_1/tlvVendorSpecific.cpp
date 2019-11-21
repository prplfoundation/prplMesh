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

#include <tlvf/ieee_1905_1/tlvVendorSpecific.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvVendorSpecific::tlvVendorSpecific(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvVendorSpecific::tlvVendorSpecific(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvVendorSpecific::~tlvVendorSpecific() {
}
const eTlvType& tlvVendorSpecific::type() {
    return (const eTlvType&)(*m_type);
}

uint16_t& tlvVendorSpecific::length() {
    return (uint16_t&)(*m_length);
}

sVendorOUI& tlvVendorSpecific::vendor_oui() {
    return (sVendorOUI&)(*m_vendor_oui);
}

uint8_t* tlvVendorSpecific::payload(size_t idx) {
    if ( (m_payload_idx__ <= 0) || (m_payload_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_payload[idx]);
}

bool tlvVendorSpecific::alloc_payload(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list payload, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_payload;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_payload_idx__ += count;
    if (!buffPtrIncrementSafe(len)) { return false; }
    return true;
}

void tlvVendorSpecific::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_vendor_oui->struct_swap();
}

size_t tlvVendorSpecific::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sVendorOUI); // vendor_oui
    return class_size;
}

bool tlvVendorSpecific::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_VENDOR_SPECIFIC;
    if (!buffPtrIncrementSafe(sizeof(eTlvType))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0x3;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_vendor_oui = (sVendorOUI*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVendorOUI))) { return false; }
    if (!m_parse__) { m_vendor_oui->struct_init(); }
    m_payload = (uint8_t*)m_buff_ptr__;
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


