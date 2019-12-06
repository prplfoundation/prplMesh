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

#include <tlvf/CmduMessage.h>

using namespace ieee1905_1;

const uint16_t CmduMessage::kCmduHeaderLength = 8;
const uint16_t CmduMessage::kTlvHeaderLength = 3;

CmduMessage::CmduMessage(uint8_t *buff, size_t buff_len, bool swap, bool parse)
    : tlvs(buff + kCmduHeaderLength, buff_len - kCmduHeaderLength, parse, swap),
      m_buff(buff), m_buff_len(buff_len), m_swap(swap) {}

CmduMessage::~CmduMessage() {}

std::shared_ptr<cCmduHeader> CmduMessage::getCmduHeader() const { return m_cmdu_header; }

int CmduMessage::getNextTlvType() const
{
    if (!m_cmdu_header)
        return -1;
    uint8_t *tlv = (tlvs.getClassVector().size() == 0) ?
        tlvs.getMessageBuff() : tlvs.getClassVector().back()->getBuffPtr();
    return *tlv;
}

bool CmduMessage::getNextTlvType(eTlvType &tlvType) const
{
    int tlvValue = getNextTlvType();
    if (tlvValue < 0)
        return false;
    tlvType = (eTlvType)tlvValue;
    return eTlvTypeValidate::check(tlvValue);
}

uint16_t CmduMessage::getNextTlvLength() const
{
    if (!m_cmdu_header)
        return -1;
    uint8_t *tlv = (tlvs.getClassVector().size() == 0) ?
        tlvs.getMessageBuff() : tlvs.getClassVector().back()->getBuffPtr();

    return *(uint16_t *)(tlv + sizeof(uint8_t));
}

uint8_t *CmduMessage::getNextTlvData() const
{
    if (!m_cmdu_header)
        return nullptr;

    uint8_t *tlv = (tlvs.getClassVector().size() == 0) ?
        tlvs.getMessageBuff() : tlvs.getClassVector().back()->getBuffPtr();
    return tlv + kTlvHeaderLength;
}

size_t CmduMessage::getMessageLength() const
{
    if (!m_cmdu_header)
        return 0;
    size_t msg_len = m_cmdu_header->getLen();
    for (auto const &c : tlvs.getClassVector()) {
        msg_len += c->getLen();
    }
    return msg_len;
}

size_t CmduMessage::getMessageBuffLength() const { return m_buff_len; }

uint8_t *CmduMessage::getMessageBuff() const { return const_cast<uint8_t*>(m_buff); }

void CmduMessage::swap()
{
    if (!m_cmdu_header)
        return;

    // the header isn't part of the tlvs.getClassVector(), so we should swap it separately
    m_cmdu_header->class_swap();

    // call all tlv swap functions
    tlvs.swap();
}

void CmduMessage::reset()
{
    if (m_cmdu_header)
        m_cmdu_header.reset();
    tlvs.reset();
}

eMessageType CmduMessage::getMessageType()
{
    uint16_t msgValue = 0;

    msgValue = (uint16_t)getCmduHeader()->message_type();
    if (swap_needed() && !is_swapped()) {
        swap_16((uint16_t &)msgValue);
    }

    return (eMessageType)msgValue;
}

uint16_t CmduMessage::getMessageId()
{
    uint16_t mid = getCmduHeader()->message_id();

    if (swap_needed() && !is_swapped()) {
        swap_16((uint16_t &)mid);
    }

    return mid;
}
