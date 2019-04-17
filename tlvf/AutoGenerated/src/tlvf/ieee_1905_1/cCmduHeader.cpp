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

#include <tlvf/ieee_1905_1/cCmduHeader.h>
#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

cCmduHeader::cCmduHeader(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cCmduHeader::cCmduHeader(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cCmduHeader::~cCmduHeader() {
}
const uint8_t& cCmduHeader::message_version() {
    return (const uint8_t&)(*m_message_version);
}

const uint8_t& cCmduHeader::reserved() {
    return (const uint8_t&)(*m_reserved);
}

eMessageType& cCmduHeader::message_type() {
    return (eMessageType&)(*m_message_type);
}

uint16_t& cCmduHeader::message_id() {
    return (uint16_t&)(*m_message_id);
}

uint8_t& cCmduHeader::fragment_id() {
    return (uint8_t&)(*m_fragment_id);
}

cCmduHeader::sFlags& cCmduHeader::flags() {
    return (sFlags&)(*m_flags);
}

void cCmduHeader::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_message_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_message_id));
    m_flags->struct_swap();
}

size_t cCmduHeader::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // message_version
    class_size += sizeof(uint8_t); // reserved
    class_size += sizeof(eMessageType); // message_type
    class_size += sizeof(uint16_t); // message_id
    class_size += sizeof(uint8_t); // fragment_id
    class_size += sizeof(sFlags); // flags
    return class_size;
}

bool cCmduHeader::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_message_version = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_message_version = 0x0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_reserved = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_reserved = 0x0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_message_type = (eMessageType*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(eMessageType) * 1;
    m_message_id = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_message_id = 0x0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_fragment_id = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_fragment_id = 0x0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
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


