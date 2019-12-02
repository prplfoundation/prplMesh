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

#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

CmduMessageTx::CmduMessageTx(uint8_t *buff, size_t buff_len) : CmduMessage(buff, buff_len) 
{
    buff_len = buff_len;
}

CmduMessageTx::~CmduMessageTx() {}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, eMessageType message_type)
{
    LOG(DEBUG)<<"CmduMessageTx::create";
    if (!m_buff || !m_buff_len)
        return nullptr;

    memset(m_buff, 0, m_buff_len);
    reset();
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, kCmduHeaderLength);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    m_cmdu_header->message_type() = message_type;
    m_cmdu_header->message_id()   = id;
    return m_cmdu_header;
}

std::shared_ptr<cCmduHeader> CmduMessageTx::load()
{
    LOG(DEBUG)<<"CmduMessageTx::load";
    if (!m_buff || !m_buff_len)
        return nullptr;
    
    reset();
    
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, ieee1905_1::CmduMessage::kCmduHeaderLength);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    return m_cmdu_header;
}

std::shared_ptr<tlvVendorSpecific> CmduMessageTx::add_vs_tlv(tlvVendorSpecific::eVendorOUI voui)
{
    if (!m_buff || !m_buff_len)
        return nullptr;

    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    auto tlv = addClass<tlvVendorSpecific>();
    if (!tlv) {
        m_cmdu_header.reset();
        m_cmdu_header = nullptr;
        return nullptr;
    }
    //set vendor specific oui
    tlv->vendor_oui() = (uint32_t)voui;

    return tlv;
}

bool CmduMessageTx::finalize(bool swap_needed)
{

    if (!m_cmdu_header)
        return false; 

    if (!tlvs.addClass<tlvEndOfMessage>())
        return false;

    if (swap_needed)
        swap();

    return true;
}
