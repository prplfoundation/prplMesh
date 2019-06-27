/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

using namespace ieee1905_1;

CmduMessageTx::CmduMessageTx(uint8_t *buff, size_t buff_len) : CmduMessage()
{
    m_buff     = buff;
    m_buff_len = buff_len;
}

CmduMessageTx::~CmduMessageTx() {}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, eMessageType message_type)
{
    if (!m_buff || !m_buff_len)
        return nullptr;

    m_parse = false;
    memset(m_buff, 0, m_buff_len);
    reset();
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, m_buff_len);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    m_cmdu_header->message_type() = message_type;
    m_cmdu_header->message_id()   = id;
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
    uint32_t voui_uint32 = (uint32_t)voui;
    for (int i = 0; i < 3; i++) {
        auto ref = tlv->vendor_oui(i);
        if (std::get<0>(ref))
            std::get<1>(ref) = uint8_t(voui_uint32);
        voui_uint32 >>= 8;
    }

    return tlv;
}

bool CmduMessageTx::finalize(bool swap_needed)
{
    if (m_finalized)
        return true;

    if (m_parse || !m_cmdu_header)
        return false; //return false if parsing was set or if cmdu_header was not set

    if (m_cmdu_header->message_type() == eMessageType::VENDOR_SPECIFIC_MESSAGE &&
        getClassCount() > 1) {
        tlvVendorSpecific *tlv = dynamic_cast<tlvVendorSpecific *>(getClass(0).get());
        if (tlv != nullptr) {
            tlv->length() = getMessageLength() - (kCmduHeaderLength + kTlvHeaderLength);
        }
    }

    if (!addClass<tlvEndOfMessage>())
        return false;

    if (swap_needed)
        swap();

    m_finalized = true;
    return true;
}
