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

#include <tlvf/wfa_map/tlvRadioOperationRestriction.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvRadioOperationRestriction::tlvRadioOperationRestriction(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvRadioOperationRestriction::tlvRadioOperationRestriction(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvRadioOperationRestriction::~tlvRadioOperationRestriction() {
}
const eTlvTypeMap& tlvRadioOperationRestriction::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvRadioOperationRestriction::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvRadioOperationRestriction::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& tlvRadioOperationRestriction::operating_classes_list_length() {
    return (uint8_t&)(*m_operating_classes_list_length);
}

std::tuple<bool, cRestrictedOperatingClasses&> tlvRadioOperationRestriction::operating_classes_list(size_t idx) {
    bool ret_success = ( (m_operating_classes_list_idx__ > 0) && (m_operating_classes_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    if (*m_operating_classes_list_length > 0) {
        return std::forward_as_tuple(ret_success, *(m_operating_classes_list_vector[ret_idx]));
    }
    else {
        return std::forward_as_tuple(ret_success, *(m_operating_classes_list));
    }
}

std::shared_ptr<cRestrictedOperatingClasses> tlvRadioOperationRestriction::create_operating_classes_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list operating_classes_list, abort!";
        return nullptr;
    }
    size_t len = cRestrictedOperatingClasses::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_operating_classes_list;
        uint8_t *dst = (uint8_t *)m_operating_classes_list + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cRestrictedOperatingClasses>(getBuffPtr(), getBuffRemainingBytes(), m_parse__, m_swap__);
}

bool tlvRadioOperationRestriction::add_operating_classes_list(std::shared_ptr<cRestrictedOperatingClasses> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_operating_classes_list was called before add_operating_classes_list";
        return false;
    }
    if (ptr->getStartBuffPtr() != getBuffPtr()) {
        TLVF_LOG(ERROR) << "Received to entry pointer is different than expected (excepting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes()) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    if (!m_parse__) {
        m_operating_classes_list_idx__++;
        (*m_operating_classes_list_length)++;
    }
    size_t len = ptr->getLen();
    m_operating_classes_list_vector.push_back(ptr);
    m_buff_ptr__ += len;
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvRadioOperationRestriction::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_radio_uid->struct_swap();
    for (size_t i = 0; i < (size_t)*m_operating_classes_list_length; i++){
        std::get<1>(operating_classes_list(i)).class_swap();
    }
}

size_t tlvRadioOperationRestriction::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // operating_classes_list_length
    return class_size;
}

bool tlvRadioOperationRestriction::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_RADIO_OPERATION_RESTRICTION;
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
    m_operating_classes_list = (cRestrictedOperatingClasses*)m_buff_ptr__;
    uint8_t operating_classes_list_length = *m_operating_classes_list_length;
    m_operating_classes_list_idx__ = operating_classes_list_length;
    for (size_t i = 0; i < operating_classes_list_length; i++) {
        if (!add_operating_classes_list(create_operating_classes_list())) { 
            TLVF_LOG(ERROR) << "Failed adding operating_classes_list entry.";
            return false;
        }
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_RADIO_OPERATION_RESTRICTION) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_RADIO_OPERATION_RESTRICTION) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cRestrictedOperatingClasses::cRestrictedOperatingClasses(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cRestrictedOperatingClasses::cRestrictedOperatingClasses(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cRestrictedOperatingClasses::~cRestrictedOperatingClasses() {
}
uint8_t& cRestrictedOperatingClasses::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& cRestrictedOperatingClasses::channel_list_length() {
    return (uint8_t&)(*m_channel_list_length);
}

std::tuple<bool, cRestrictedOperatingClasses::sChannelInfo&> cRestrictedOperatingClasses::channel_list(size_t idx) {
    bool ret_success = ( (m_channel_list_idx__ > 0) && (m_channel_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_channel_list[ret_idx]);
}

bool cRestrictedOperatingClasses::alloc_channel_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list channel_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sChannelInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    if (!m_parse__) {
        uint8_t *src = (uint8_t *)m_channel_list;
        uint8_t *dst = (uint8_t *)m_channel_list + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_channel_list_idx__ += count;
    *m_channel_list_length += count;
    m_buff_ptr__ += len;
    if (!m_parse__) { 
        for (size_t i = m_channel_list_idx__ - count; i < m_channel_list_idx__; i++) { m_channel_list[i].struct_init(); }
    }
    return true;
}

void cRestrictedOperatingClasses::class_swap()
{
    for (size_t i = 0; i < (size_t)*m_channel_list_length; i++){
        m_channel_list[i].struct_swap();
    }
}

size_t cRestrictedOperatingClasses::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // channel_list_length
    return class_size;
}

bool cRestrictedOperatingClasses::init()
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
    m_channel_list = (sChannelInfo*)m_buff_ptr__;
    uint8_t channel_list_length = *m_channel_list_length;
    m_channel_list_idx__ = channel_list_length;
    m_buff_ptr__ += sizeof(sChannelInfo)*(channel_list_length);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


