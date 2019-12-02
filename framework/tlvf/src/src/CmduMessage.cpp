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

// CmduMessage::CmduMessage(){};
CmduMessage::CmduMessage(uint8_t *buff, size_t buff_len) : 
    m_buff(buff), m_buff_len(buff_len),
    tlvs(buff + kCmduHeaderLength, buff_len - kCmduHeaderLength) 
{
    buff_len = buff_len;
}

CmduMessage::~CmduMessage() {}

std::shared_ptr<cCmduHeader> CmduMessage::getCmduHeader() const { return m_cmdu_header; }

int CmduMessage::getNextTlvType() const
{
    uint8_t tlvValue = 0;
    if (!m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() < kTlvHeaderLength)) {
        return -1;
    }

    if (tlvs.getClassVector().size() == 0) {
        tlvValue = *m_cmdu_header->getBuffPtr();
    } else {
        tlvValue = *tlvs.getClassVector().back()->getBuffPtr();
    }
    
    return int(tlvValue);
}

bool CmduMessage::getNextTlvType(eTlvType &tlvType) const
{
    uint16_t tlvValue = 0;
    if (!m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() < kTlvHeaderLength)) {
        return false;
    }

    if (tlvs.getClassVector().size() == 0) {
        tlvValue = *m_cmdu_header->getBuffPtr();
    } else {
        tlvValue = *tlvs.getClassVector().back()->getBuffPtr();
    }
    if (eTlvTypeValidate::check(tlvValue)) {
        tlvType = (eTlvType)tlvValue;
        return true;
    } else {
        return false;
    }
}

uint16_t CmduMessage::getNextTlvLength() const
{
    uint16_t tlvLength;
    if (m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() > kTlvHeaderLength)) {
        if (tlvs.getClassVector().size() == 0) {
            tlvLength = *((uint16_t *)(m_cmdu_header->getBuffPtr() + sizeof(uint8_t)));
        } else {
            tlvLength = *((uint16_t *)(tlvs.getClassVector().back()->getBuffPtr() + sizeof(uint8_t)));
        }

        // if (m_swap) {
        //     swap_16((uint16_t &)tlvLength);
        // }
    } else {
        tlvLength = 0;
    }
    return tlvLength;
}

uint8_t *CmduMessage::getNextTlvData() const
{
    uint8_t *tlvData = nullptr;
    if (m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() > kTlvHeaderLength)) {
        if (tlvs.getClassVector().size() == 0) {
            tlvData = m_cmdu_header->getBuffPtr() + sizeof(uint8_t) + sizeof(uint16_t);
        } else {
            tlvData = tlvs.getClassVector().back()->getBuffPtr() + sizeof(uint8_t) + sizeof(uint16_t);
        }
    }
    return tlvData;
}

size_t CmduMessage::getMessageLength() const
{
    if (m_cmdu_header == nullptr)
        return 0;
    size_t msg_len = m_cmdu_header->getLen();
    for (auto const &c : tlvs.getClassVector()) {
        msg_len += c->getLen();
    }
    return msg_len;
}

//TODO change name to getTlvsBuffLen and check where its used and why
size_t CmduMessage::getMessageBuffLength() const { return tlvs.getMessageBuffLength(); }

//TODO broken API - need to fix
uint8_t *CmduMessage::getMessageBuff() const
{
    return (m_cmdu_header ? m_cmdu_header->getStartBuffPtr() : nullptr);
}

void CmduMessage::swap()
{
    if (!m_cmdu_header)
        return;

    m_cmdu_header
        ->class_swap(); // the header isn't part of the tlvs.getClassVector(), so we should swap it separately

    // call all tlv finalize functions
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
