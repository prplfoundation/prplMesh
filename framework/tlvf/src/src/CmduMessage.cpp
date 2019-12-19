/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessage.h>

using namespace ieee1905_1;

int CmduMessage::getNextTlvType() const
{
    if (!getCmduHeader())
        return -1;
    uint8_t *tlv = msg.getClassVector().back()->getBuffPtr();
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
    uint8_t *tlv = msg.getClassVector().back()->getBuffPtr();
    return *(uint16_t *)(tlv + sizeof(uint8_t));
}

uint8_t *CmduMessage::getNextTlvData() const
{
    if (!getCmduHeader())
        return nullptr;

    uint8_t *tlv = msg.getClassVector().back()->getBuffPtr();
    return tlv + kTlvHeaderLength;
}

eMessageType CmduMessage::getMessageType()
{
    uint16_t msgValue = 0;

    msgValue = (uint16_t)getCmduHeader()->message_type();
    if (!msg.is_swapped()) {
        swap_16((uint16_t &)msgValue);
    }

    return (eMessageType)msgValue;
}

uint16_t CmduMessage::getMessageId()
{
    uint16_t mid = getCmduHeader()->message_id();

    if (!msg.is_swapped()) {
        swap_16((uint16_t &)mid);
    }

    return mid;
}
