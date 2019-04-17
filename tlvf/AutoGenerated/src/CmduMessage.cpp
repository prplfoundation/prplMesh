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

#include <tlvf/CmduMessage.h>

using namespace ieee1905_1;

CmduMessage::CmduMessage()
{
}

CmduMessage::~CmduMessage()
{
}

std::shared_ptr<cCmduHeader> CmduMessage::getCmduHeader() const
{
    return m_cmdu_header;
}

std::shared_ptr<BaseClass> CmduMessage::getClass(size_t idx) const
{
    if (m_class_vector.size() > idx) {
        return std::shared_ptr<BaseClass>(m_class_vector.at(idx));
    } else {
        return nullptr;
    }
}

bool CmduMessage::getNextTlvType(eTlvType& tlvType) const
{
    uint16_t tlvValue = 0;
    if (!m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() < kTlvHeaderLength)) {
        return false;
    }

    if (m_class_vector.size() == 0) {
        tlvValue = *((uint16_t*)m_cmdu_header->getBuffPtr());
    } else {
        tlvValue = *((uint16_t*)m_class_vector.back()->getBuffPtr());
    }
    if (m_swap) {
        swap_16((uint16_t&)tlvValue);
    }
    if (eTlvTypeValidate::check(tlvValue)) {
        tlvType = (eTlvType)tlvValue;
    }

    return true;
}

uint16_t CmduMessage::getNextTlvLength() const
{
    uint16_t tlvLength;
    if (m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() > kTlvHeaderLength)) {
        if (m_class_vector.size() == 0) {
            tlvLength = *((uint16_t*)(m_cmdu_header->getBuffPtr()+sizeof(uint16_t)));
        } else {
            tlvLength = *((uint16_t*)(m_class_vector.back()->getBuffPtr()+sizeof(uint16_t)));
        }

        if (m_swap) {
            swap_16((uint16_t&)tlvLength);
        }
    } else {
        tlvLength = 0;
    }
    return tlvLength;
}

size_t CmduMessage::getClassCount() const
{
    return m_class_vector.size();
}

const std::vector<std::shared_ptr<BaseClass>>& CmduMessage::getClassVector() const
{
    return (const std::vector<std::shared_ptr<BaseClass>>&)m_class_vector;
}

size_t CmduMessage::getMessageLength() const
{
    if (m_cmdu_header == nullptr) return 0;
    size_t msg_len = m_cmdu_header->getLen();
    for(auto const& c: m_class_vector) {
        msg_len += c->getLen();
    }
    return msg_len;
}

size_t CmduMessage::getMessageBuffLength() const
{
    return m_buff_len;
}

uint8_t* CmduMessage::getMessageBuff() const 
{
    return (m_cmdu_header ? m_cmdu_header->getStartBuffPtr() : nullptr);
}

void CmduMessage::swap()
{
    if (!m_cmdu_header) return;

    m_cmdu_header->class_swap(); // the header isn't part of the m_class_vector, so we should swap it separately

    // call all tlv finalize functions
    for(auto const& c: m_class_vector) {
        c->class_swap();
    }

    m_swapped = !m_swapped;
}

void CmduMessage::reset()
{
    m_finalized = false;
    m_swapped = false;
    m_swap = false;
    if (m_cmdu_header) m_cmdu_header.reset();
    for(auto & c: m_class_vector) {
        c.reset();
    }
    m_class_vector.clear();
}

eMessageType CmduMessage::getMessageType()
{
    uint16_t msgValue = 0;
    
    msgValue = (uint16_t)getCmduHeader()->message_type();
    if (m_swap && !m_swapped) {
        swap_16((uint16_t&)msgValue);
    }
    
    return (eMessageType)msgValue;
} 

