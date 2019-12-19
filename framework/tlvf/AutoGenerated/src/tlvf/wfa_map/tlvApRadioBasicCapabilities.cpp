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

#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApRadioBasicCapabilities::tlvApRadioBasicCapabilities(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvApRadioBasicCapabilities::tlvApRadioBasicCapabilities(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvApRadioBasicCapabilities::~tlvApRadioBasicCapabilities() {
}
const eTlvTypeMap& tlvApRadioBasicCapabilities::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApRadioBasicCapabilities::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvApRadioBasicCapabilities::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvApRadioBasicCapabilities::maximum_number_of_bsss_supported() {
    return (uint8_t&)(*m_maximum_number_of_bsss_supported);
}

uint8_t& tlvApRadioBasicCapabilities::operating_classes_info_list_length() {
    return (uint8_t&)(*m_operating_classes_info_list_length);
}

std::tuple<bool, cOperatingClassesInfo&> tlvApRadioBasicCapabilities::operating_classes_info_list(size_t idx) {
    bool ret_success = ( (m_operating_classes_info_list_idx__ > 0) && (m_operating_classes_info_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_operating_classes_info_list_vector[ret_idx]));
}

std::shared_ptr<cOperatingClassesInfo> tlvApRadioBasicCapabilities::create_operating_classes_info_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list operating_classes_info_list, abort!";
        return nullptr;
    }
    size_t len = cOperatingClassesInfo::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_operating_classes_info_list;
    if (m_operating_classes_info_list_idx__ > 0) {
        src = (uint8_t *)m_operating_classes_info_list_vector[m_operating_classes_info_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cOperatingClassesInfo>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvApRadioBasicCapabilities::add_operating_classes_info_list(std::shared_ptr<cOperatingClassesInfo> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_operating_classes_info_list was called before add_operating_classes_info_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_operating_classes_info_list;
    if (m_operating_classes_info_list_idx__ > 0) {
        src = (uint8_t *)m_operating_classes_info_list_vector[m_operating_classes_info_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_operating_classes_info_list_idx__++;
    if (!m_parse__) { (*m_operating_classes_info_list_length)++; }
    size_t len = ptr->getLen();
    m_operating_classes_info_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvApRadioBasicCapabilities::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    for (size_t i = 0; i < (size_t)*m_operating_classes_info_list_length; i++){
        std::get<1>(operating_classes_info_list(i)).class_swap();
    }
}

bool tlvApRadioBasicCapabilities::finalize()
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

size_t tlvApRadioBasicCapabilities::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // maximum_number_of_bsss_supported
    class_size += sizeof(uint8_t); // operating_classes_info_list_length
    return class_size;
}

bool tlvApRadioBasicCapabilities::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES;
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
    m_maximum_number_of_bsss_supported = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_info_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_operating_classes_info_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_info_list = (cOperatingClassesInfo*)m_buff_ptr__;
    uint8_t operating_classes_info_list_length = *m_operating_classes_info_list_length;
    m_operating_classes_info_list_idx__ = 0;
    for (size_t i = 0; i < operating_classes_info_list_length; i++) {
        auto operating_classes_info_list = create_operating_classes_info_list();
        if (!operating_classes_info_list) {
            TLVF_LOG(ERROR) << "create_operating_classes_info_list() failed";
            return false;
        }
        if (!add_operating_classes_info_list(operating_classes_info_list)) {
            TLVF_LOG(ERROR) << "add_operating_classes_info_list() failed";
            return false;
        }
        // swap back since operating_classes_info_list will be swapped as part of the whole class swap
        operating_classes_info_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cOperatingClassesInfo::cOperatingClassesInfo(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cOperatingClassesInfo::cOperatingClassesInfo(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cOperatingClassesInfo::~cOperatingClassesInfo() {
}
uint8_t& cOperatingClassesInfo::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& cOperatingClassesInfo::maximum_transmit_power_dbm() {
    return (uint8_t&)(*m_maximum_transmit_power_dbm);
}

uint8_t& cOperatingClassesInfo::statically_non_operable_channels_list_length() {
    return (uint8_t&)(*m_statically_non_operable_channels_list_length);
}

uint8_t* cOperatingClassesInfo::statically_non_operable_channels_list(size_t idx) {
    if ( (m_statically_non_operable_channels_list_idx__ <= 0) || (m_statically_non_operable_channels_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_statically_non_operable_channels_list[idx]);
}

bool cOperatingClassesInfo::alloc_statically_non_operable_channels_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list statically_non_operable_channels_list, abort!";
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
    uint8_t *src = (uint8_t *)&m_statically_non_operable_channels_list[*m_statically_non_operable_channels_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_statically_non_operable_channels_list_idx__ += count;
    *m_statically_non_operable_channels_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cOperatingClassesInfo::class_swap()
{
}

bool cOperatingClassesInfo::finalize()
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

size_t cOperatingClassesInfo::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // maximum_transmit_power_dbm
    class_size += sizeof(uint8_t); // statically_non_operable_channels_list_length
    return class_size;
}

bool cOperatingClassesInfo::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_operating_class = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_maximum_transmit_power_dbm = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_statically_non_operable_channels_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_statically_non_operable_channels_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_statically_non_operable_channels_list = (uint8_t*)m_buff_ptr__;
    uint8_t statically_non_operable_channels_list_length = *m_statically_non_operable_channels_list_length;
    m_statically_non_operable_channels_list_idx__ = statically_non_operable_channels_list_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (statically_non_operable_channels_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (statically_non_operable_channels_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


