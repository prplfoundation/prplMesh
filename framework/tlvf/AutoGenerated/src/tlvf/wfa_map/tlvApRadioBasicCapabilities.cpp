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

tlvApRadioBasicCapabilities::tlvApRadioBasicCapabilities(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvApRadioBasicCapabilities::tlvApRadioBasicCapabilities(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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
    if (*m_operating_classes_info_list_length > 0) {
        return std::forward_as_tuple(ret_success, *(m_operating_classes_info_list_vector[ret_idx]));
    }
    else {
        return std::forward_as_tuple(ret_success, *(m_operating_classes_info_list));
    }
}

std::shared_ptr<cOperatingClassesInfo> tlvApRadioBasicCapabilities::create_operating_classes_info_list() {
    size_t len = cOperatingClassesInfo::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_allocation__ = true;
    if (!m_parse__)
        std::memmove(m_buff_ptr__ + len, m_buff_ptr__, getBuffRemainingBytes() - len);
    return std::make_shared<cOperatingClassesInfo>(getBuffPtr(), getBuffRemainingBytes(), m_parse__, m_swap__);
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
    if (ptr->getStartBuffPtr() != getBuffPtr()) {
        TLVF_LOG(ERROR) << "Received to entry pointer is different than expected (excepting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes()) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    if (!m_parse__) {
        m_operating_classes_info_list_idx__++;
        (*m_operating_classes_info_list_length)++;
    }
    size_t len = ptr->getLen();
    if (!m_parse__)
        std::memmove(m_buff_ptr__ + len, m_buff_ptr__, getBuffRemainingBytes() - len);
    m_operating_classes_info_list_vector.push_back(ptr);
    m_buff_ptr__ += len;
    if(m_length){ (*m_length) += len; }
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
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES;
    else {
            if (*m_type != eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES) << ", received value: " << int(*m_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_maximum_number_of_bsss_supported = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_info_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_operating_classes_info_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_info_list = (cOperatingClassesInfo*)m_buff_ptr__;
    m_operating_classes_info_list_idx__ = *m_operating_classes_info_list_length;
    for (size_t i = 0; i < *m_operating_classes_info_list_length; i++) {
        if (!add_operating_classes_info_list(create_operating_classes_info_list())) { 
            TLVF_LOG(ERROR) << "Failed adding operating_classes_info_list entry.";
            return false;
        }
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cOperatingClassesInfo::cOperatingClassesInfo(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cOperatingClassesInfo::cOperatingClassesInfo(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

std::tuple<bool, uint8_t&> cOperatingClassesInfo::statically_non_operable_channels_list(size_t idx) {
    bool ret_success = ( (m_statically_non_operable_channels_list_idx__ > 0) && (m_statically_non_operable_channels_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_statically_non_operable_channels_list[ret_idx]);
}

bool cOperatingClassesInfo::alloc_statically_non_operable_channels_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__)
        std::memmove(m_buff_ptr__ + len, m_buff_ptr__, getBuffRemainingBytes() - len);
    m_statically_non_operable_channels_list_idx__ += count;
    *m_statically_non_operable_channels_list_length += count;
    m_buff_ptr__ += len;
    return true;
}

void cOperatingClassesInfo::class_swap()
{
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
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_operating_class = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_maximum_transmit_power_dbm = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_statically_non_operable_channels_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_statically_non_operable_channels_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_statically_non_operable_channels_list = (uint8_t*)m_buff_ptr__;
    m_statically_non_operable_channels_list_idx__ = *m_statically_non_operable_channels_list_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_statically_non_operable_channels_list_length);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


