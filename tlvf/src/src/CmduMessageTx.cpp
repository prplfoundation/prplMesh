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

#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

using namespace ieee1905_1;

CmduMessageTx::CmduMessageTx(uint8_t* buff, size_t buff_len) : CmduMessage()
{
    m_buff = buff;
    m_buff_len = buff_len;
}

CmduMessageTx::~CmduMessageTx()
{
}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, eMessageType message_type)
{
    if (!m_buff || !m_buff_len) return nullptr;
    
    m_parse = false;
    memset(m_buff, 0, m_buff_len);
    reset();
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, m_buff_len);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    } 

    m_cmdu_header->message_type() = message_type;
    m_cmdu_header->message_id() = id;
    return m_cmdu_header;
}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, tlvVendorSpecific::eVendorOUI voui)
{
    if (!m_buff || !m_buff_len) return nullptr;
    
    m_parse = false;
    memset(m_buff, 0, m_buff_len);
    reset();
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, m_buff_len);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    m_cmdu_header->message_type() = eMessageType::VENDOR_SPECIFIC_MESSAGE;
    m_cmdu_header->message_id() = id;
    auto tlv = addClass<tlvVendorSpecific>();
    if (!tlv) {
        m_cmdu_header.reset();
        m_cmdu_header = nullptr;
        return nullptr;
    }
    //set vendor specific oui
    uint32_t voui_uint32 = (uint32_t)voui;
    for(int i=0; i<3; i++){
        auto ref = tlv->vendor_oui(i);
        if(std::get<0>(ref)) std::get<1>(ref) = uint8_t(voui_uint32);
        voui_uint32>>=8;
    }

    return m_cmdu_header;
}

bool CmduMessageTx::finalize(bool swap_needed)
{
    if (m_finalized) return true;
    
    if (m_parse || !m_cmdu_header) return false; //return false if parsing was set or if cmdu_header was not set

    if (m_cmdu_header->message_type() == eMessageType::VENDOR_SPECIFIC_MESSAGE && getClassCount() > 1) {
        tlvVendorSpecific* tlv = dynamic_cast<tlvVendorSpecific*>(getClass(0).get());
        if (tlv != nullptr) {
            tlv->length() = getMessageLength() - (kCmduHeaderLength + kTlvHeaderLength);
        }
    }

    if (!addClass<tlvEndOfMessage>()) return false;

    if (swap_needed) swap();

    m_finalized = true;
    return true;
}

