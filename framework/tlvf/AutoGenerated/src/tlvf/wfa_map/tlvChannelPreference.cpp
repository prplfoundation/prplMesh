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

#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvChannelPreference::tlvChannelPreference(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvChannelPreference::tlvChannelPreference(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvChannelPreference::~tlvChannelPreference() {
}
const eTlvTypeMap& tlvChannelPreference::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvChannelPreference::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvChannelPreference::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvChannelPreference::operating_classes_list_length() {
    return (uint8_t&)(*m_operating_classes_list_length);
}

std::tuple<bool, cPreferenceOperatingClasses&> tlvChannelPreference::operating_classes_list(size_t idx) {
    bool ret_success = ( (m_operating_classes_list_idx__ > 0) && (m_operating_classes_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_operating_classes_list_vector[ret_idx]));
}

std::shared_ptr<cPreferenceOperatingClasses> tlvChannelPreference::create_operating_classes_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list operating_classes_list, abort!";
        return nullptr;
    }
    size_t len = cPreferenceOperatingClasses::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_operating_classes_list;
    if (m_operating_classes_list_idx__ > 0) {
        src = (uint8_t *)m_operating_classes_list_vector[m_operating_classes_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cPreferenceOperatingClasses>(src, getBuffRemainingBytes(src), m_parse__, m_swap__);
}

bool tlvChannelPreference::add_operating_classes_list(std::shared_ptr<cPreferenceOperatingClasses> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_operating_classes_list was called before add_operating_classes_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_operating_classes_list;
    if (m_operating_classes_list_idx__ > 0) {
        src = (uint8_t *)m_operating_classes_list_vector[m_operating_classes_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_operating_classes_list_idx__++;
    if (!m_parse__) { (*m_operating_classes_list_length)++; }
    size_t len = ptr->getLen();
    m_operating_classes_list_vector.push_back(ptr);
    m_buff_ptr__ += len;
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvChannelPreference::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    for (size_t i = 0; i < (size_t)*m_operating_classes_list_length; i++){
        std::get<1>(operating_classes_list(i)).class_swap();
    }
}

size_t tlvChannelPreference::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // operating_classes_list_length
    return class_size;
}

bool tlvChannelPreference::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_CHANNEL_PREFERENCE;
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_operating_classes_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_operating_classes_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_list = (cPreferenceOperatingClasses*)m_buff_ptr__;
    uint8_t operating_classes_list_length = *m_operating_classes_list_length;
    m_operating_classes_list_idx__ = 0;
    for (size_t i = 0; i < operating_classes_list_length; i++) {
        auto operating_classes_list = create_operating_classes_list();
        if (!operating_classes_list) {
            TLVF_LOG(ERROR) << "create_operating_classes_list() failed";
            return false;
        }
        if (!add_operating_classes_list(operating_classes_list)) {
            TLVF_LOG(ERROR) << "add_operating_classes_list() failed";
            return false;
        }
        // swap back since operating_classes_list will be swapped as part of the whole class swap
        operating_classes_list->class_swap();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_CHANNEL_PREFERENCE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_CHANNEL_PREFERENCE) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cPreferenceOperatingClasses::cPreferenceOperatingClasses(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cPreferenceOperatingClasses::cPreferenceOperatingClasses(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cPreferenceOperatingClasses::~cPreferenceOperatingClasses() {
}
uint8_t& cPreferenceOperatingClasses::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& cPreferenceOperatingClasses::channel_list_length() {
    return (uint8_t&)(*m_channel_list_length);
}

uint8_t* cPreferenceOperatingClasses::channel_list(size_t idx) {
    if ( (m_channel_list_idx__ > 0) && (m_channel_list_idx__ > idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_channel_list[idx]);
}

bool cPreferenceOperatingClasses::alloc_channel_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list channel_list, abort!";
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
    uint8_t *src = (uint8_t *)&m_channel_list[*m_channel_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_flags = (sFlags *)((uint8_t *)(m_flags) + len);
    m_channel_list_idx__ += count;
    *m_channel_list_length += count;
    m_buff_ptr__ += len;
    return true;
}

cPreferenceOperatingClasses::sFlags& cPreferenceOperatingClasses::flags() {
    return (sFlags&)(*m_flags);
}

void cPreferenceOperatingClasses::class_swap()
{
    m_flags->struct_swap();
}

size_t cPreferenceOperatingClasses::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // channel_list_length
    class_size += sizeof(sFlags); // flags
    return class_size;
}

bool cPreferenceOperatingClasses::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_operating_class = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_channel_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_channel_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_channel_list = (uint8_t*)m_buff_ptr__;
    uint8_t channel_list_length = *m_channel_list_length;
    m_channel_list_idx__ = channel_list_length;
    m_buff_ptr__ += sizeof(uint8_t)*(channel_list_length);
    m_flags = (sFlags*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sFlags) * 1;
    if (!m_parse__) { m_flags->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


