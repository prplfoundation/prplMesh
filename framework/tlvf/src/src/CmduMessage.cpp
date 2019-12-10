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

CmduMessage::CmduMessage(uint8_t *buff, size_t buff_len)
    : tlvs(buff, buff_len) {}

CmduMessage::~CmduMessage() {}

std::shared_ptr<cCmduHeader> CmduMessage::getCmduHeader() const { return tlvs.getClass<cCmduHeader>(); }

int CmduMessage::getNextTlvType() const
{
    if (!getCmduHeader())
        return -1;
    uint8_t *tlv = tlvs.getClassVector().back()->getBuffPtr();
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
    if (!getCmduHeader())
        return -1;
    uint8_t *tlv = tlvs.getClassVector().back()->getBuffPtr();
    return *(uint16_t *)(tlv + sizeof(uint8_t));
}

uint8_t *CmduMessage::getNextTlvData() const
{
    if (!getCmduHeader())
        return nullptr;

    uint8_t *tlv = tlvs.getClassVector().back()->getBuffPtr();
    return tlv + kTlvHeaderLength;
}

size_t CmduMessage::getMessageLength() const
{
    size_t msg_len = 0;
    for (auto const &c : tlvs.getClassVector()) {
        msg_len += c->getLen();
    }
    return msg_len;
}

size_t CmduMessage::getMessageBuffLength() const { return tlvs.getMessageBuffLength(); }

uint8_t *CmduMessage::getMessageBuff() const { return tlvs.getMessageBuff(); }

void CmduMessage::swap()
{
    tlvs.swap();
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
