///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
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

sMacAddress& tlvChannelPreference::radio_uid() {
    return (sMacAddress&)(*m_radio_uid);
}

uint8_t& tlvChannelPreference::operating_classes_list_length() {
    return (uint8_t&)(*m_operating_classes_list_length);
}

std::tuple<bool, cOperatingClasses&> tlvChannelPreference::operating_classes_list(size_t idx) {
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

std::shared_ptr<cOperatingClasses> tlvChannelPreference::create_operating_classes_list() {
    size_t len = cOperatingClasses::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_allocation__ = true;
    return std::make_shared<cOperatingClasses>(getBuffPtr(), getBuffRemainingBytes(), m_parse__, m_swap__);
}

bool tlvChannelPreference::add_operating_classes_list(std::shared_ptr<cOperatingClasses> ptr) {
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
    if(m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvChannelPreference::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_type));
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
    class_size += sizeof(sMacAddress); // radio_uid
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
    else {
        eTlvTypeMap swapped_type = *m_type;
        if (m_swap__) { tlvf_swap(16, reinterpret_cast<uint8_t*>(&swapped_type)); }
            if (swapped_type != eTlvTypeMap::TLV_CHANNEL_PREFERENCE) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_CHANNEL_PREFERENCE) << ", received value: " << int(swapped_type);
            return false;
        }
    }
    m_buff_ptr__ += sizeof(eTlvTypeMap) * 1;
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_radio_uid = (sMacAddress*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddress) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddress); }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_operating_classes_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_operating_classes_list_length = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_operating_classes_list = (cOperatingClasses*)m_buff_ptr__;
    m_operating_classes_list_idx__ = *m_operating_classes_list_length;
    for (size_t i = 0; i < *m_operating_classes_list_length; i++) {
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
    return true;
}

cOperatingClasses::cOperatingClasses(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cOperatingClasses::cOperatingClasses(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cOperatingClasses::~cOperatingClasses() {
}
uint8_t& cOperatingClasses::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& cOperatingClasses::channel_list_length() {
    return (uint8_t&)(*m_channel_list_length);
}

std::tuple<bool, uint8_t&> cOperatingClasses::channel_list(size_t idx) {
    bool ret_success = ( (m_channel_list_idx__ > 0) && (m_channel_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_channel_list[ret_idx]);
}

bool cOperatingClasses::alloc_channel_list(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
//TLVF_TODO: enable call to memmove
    m_channel_list_idx__ += count;
    *m_channel_list_length += count;
    m_buff_ptr__ += len;
    return true;
}

cOperatingClasses::sFlags& cOperatingClasses::flags() {
    return (sFlags&)(*m_flags);
}

void cOperatingClasses::class_swap()
{
    m_flags->struct_swap();
}

size_t cOperatingClasses::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // channel_list_length
    class_size += sizeof(sFlags); // flags
    return class_size;
}

bool cOperatingClasses::init()
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
    m_channel_list_idx__ = *m_channel_list_length;
    m_buff_ptr__ += sizeof(uint8_t)*(*m_channel_list_length);
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


