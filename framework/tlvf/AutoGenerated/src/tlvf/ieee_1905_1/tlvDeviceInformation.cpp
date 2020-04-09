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

#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

tlvDeviceInformation::tlvDeviceInformation(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvDeviceInformation::tlvDeviceInformation(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvDeviceInformation::~tlvDeviceInformation() {
}
const eTlvType& tlvDeviceInformation::type() {
    return (const eTlvType&)(*m_type);
}

const uint16_t& tlvDeviceInformation::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvDeviceInformation::mac() {
    return (sMacAddr&)(*m_mac);
}

uint8_t& tlvDeviceInformation::local_interface_list_length() {
    return (uint8_t&)(*m_local_interface_list_length);
}

std::tuple<bool, cLocalInterfaceInfo&> tlvDeviceInformation::local_interface_list(size_t idx) {
    bool ret_success = ( (m_local_interface_list_idx__ > 0) && (m_local_interface_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_local_interface_list_vector[ret_idx]));
}

std::shared_ptr<cLocalInterfaceInfo> tlvDeviceInformation::create_local_interface_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list local_interface_list, abort!";
        return nullptr;
    }
    size_t len = cLocalInterfaceInfo::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_local_interface_list;
    if (m_local_interface_list_idx__ > 0) {
        src = (uint8_t *)m_local_interface_list_vector[m_local_interface_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cLocalInterfaceInfo>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvDeviceInformation::add_local_interface_list(std::shared_ptr<cLocalInterfaceInfo> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_local_interface_list was called before add_local_interface_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_local_interface_list;
    if (m_local_interface_list_idx__ > 0) {
        src = (uint8_t *)m_local_interface_list_vector[m_local_interface_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_local_interface_list_idx__++;
    if (!m_parse__) { (*m_local_interface_list_length)++; }
    size_t len = ptr->getLen();
    m_local_interface_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvDeviceInformation::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_mac->struct_swap();
    for (size_t i = 0; i < (size_t)*m_local_interface_list_length; i++){
        std::get<1>(local_interface_list(i)).class_swap();
    }
}

bool tlvDeviceInformation::finalize()
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

size_t tlvDeviceInformation::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvType); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(uint8_t); // local_interface_list_length
    return class_size;
}

bool tlvDeviceInformation::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvType*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvType::TLV_DEVICE_INFORMATION;
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
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_mac->struct_init(); }
    m_local_interface_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_local_interface_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_local_interface_list = (cLocalInterfaceInfo*)m_buff_ptr__;
    uint8_t local_interface_list_length = *m_local_interface_list_length;
    m_local_interface_list_idx__ = 0;
    for (size_t i = 0; i < local_interface_list_length; i++) {
        auto local_interface_list = create_local_interface_list();
        if (!local_interface_list) {
            TLVF_LOG(ERROR) << "create_local_interface_list() failed";
            return false;
        }
        if (!add_local_interface_list(local_interface_list)) {
            TLVF_LOG(ERROR) << "add_local_interface_list() failed";
            return false;
        }
        // swap back since local_interface_list will be swapped as part of the whole class swap
        local_interface_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvType::TLV_DEVICE_INFORMATION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvType::TLV_DEVICE_INFORMATION) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cLocalInterfaceInfo::cLocalInterfaceInfo(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cLocalInterfaceInfo::cLocalInterfaceInfo(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cLocalInterfaceInfo::~cLocalInterfaceInfo() {
}
sMacAddr& cLocalInterfaceInfo::mac() {
    return (sMacAddr&)(*m_mac);
}

eMediaType& cLocalInterfaceInfo::media_type() {
    return (eMediaType&)(*m_media_type);
}

uint8_t& cLocalInterfaceInfo::media_info_length() {
    return (uint8_t&)(*m_media_info_length);
}

uint8_t* cLocalInterfaceInfo::media_info(size_t idx) {
    if ( (m_media_info_idx__ == 0) || (m_media_info_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_media_info[idx]);
}

bool cLocalInterfaceInfo::set_media_info(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_media_info received a null pointer.";
        return false;
    }
    if (!alloc_media_info(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_media_info);
    return true;
}
bool cLocalInterfaceInfo::alloc_media_info(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list media_info, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_media_info[*m_media_info_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_media_info_idx__ += count;
    *m_media_info_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cLocalInterfaceInfo::class_swap()
{
    m_mac->struct_swap();
    tlvf_swap(8*sizeof(eMediaType), reinterpret_cast<uint8_t*>(m_media_type));
}

bool cLocalInterfaceInfo::finalize()
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

size_t cLocalInterfaceInfo::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(eMediaType); // media_type
    class_size += sizeof(uint8_t); // media_info_length
    return class_size;
}

bool cLocalInterfaceInfo::init()
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
    m_media_type = (eMediaType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eMediaType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eMediaType) << ") Failed!";
        return false;
    }
    m_media_info_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_media_info_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_media_info = (uint8_t*)m_buff_ptr__;
    uint8_t media_info_length = *m_media_info_length;
    m_media_info_idx__ = media_info_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (media_info_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (media_info_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


