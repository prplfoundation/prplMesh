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

#include <tlvf/wfa_map/tlvApHeCapabilities.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApHeCapabilities::tlvApHeCapabilities(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvApHeCapabilities::tlvApHeCapabilities(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvApHeCapabilities::~tlvApHeCapabilities() {
}
const eTlvTypeMap& tlvApHeCapabilities::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApHeCapabilities::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvApHeCapabilities::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvApHeCapabilities::supported_he_mcs_length() {
    return (uint8_t&)(*m_supported_he_mcs_length);
}

uint8_t* tlvApHeCapabilities::supported_he_mcs(size_t idx) {
    if ( (m_supported_he_mcs_idx__ == 0) || (m_supported_he_mcs_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_supported_he_mcs[idx]);
}

bool tlvApHeCapabilities::set_supported_he_mcs(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_supported_he_mcs received a null pointer.";
        return false;
    }
    if (!alloc_supported_he_mcs(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_supported_he_mcs);
    return true;
}
bool tlvApHeCapabilities::alloc_supported_he_mcs(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list supported_he_mcs, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_supported_he_mcs[*m_supported_he_mcs_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_flags1 = (sFlags1 *)((uint8_t *)(m_flags1) + len);
    m_flags2 = (sFlags2 *)((uint8_t *)(m_flags2) + len);
    m_supported_he_mcs_idx__ += count;
    *m_supported_he_mcs_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

tlvApHeCapabilities::sFlags1& tlvApHeCapabilities::flags1() {
    return (sFlags1&)(*m_flags1);
}

tlvApHeCapabilities::sFlags2& tlvApHeCapabilities::flags2() {
    return (sFlags2&)(*m_flags2);
}

void tlvApHeCapabilities::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    m_flags1->struct_swap();
    m_flags2->struct_swap();
}

bool tlvApHeCapabilities::finalize()
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

size_t tlvApHeCapabilities::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // supported_he_mcs_length
    class_size += sizeof(sFlags1); // flags1
    class_size += sizeof(sFlags2); // flags2
    return class_size;
}

bool tlvApHeCapabilities::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_HE_CAPABILITIES;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_supported_he_mcs_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_supported_he_mcs_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_supported_he_mcs = (uint8_t*)m_buff_ptr__;
    uint8_t supported_he_mcs_length = *m_supported_he_mcs_length;
    m_supported_he_mcs_idx__ = supported_he_mcs_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (supported_he_mcs_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (supported_he_mcs_length) << ") Failed!";
        return false;
    }
    m_flags1 = (sFlags1*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sFlags1))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sFlags1) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sFlags1); }
    if (!m_parse__) { m_flags1->struct_init(); }
    m_flags2 = (sFlags2*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sFlags2))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sFlags2) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sFlags2); }
    if (!m_parse__) { m_flags2->struct_init(); }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_AP_HE_CAPABILITIES) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_HE_CAPABILITIES) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


