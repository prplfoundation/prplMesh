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
    sTlvHeader *tlv = reinterpret_cast<sTlvHeader *>(msg.prevClass()->getBuffPtr());
    return tlv->type;
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
    sTlvHeader *tlv = reinterpret_cast<sTlvHeader *>(msg.prevClass()->getBuffPtr());
    return tlv->length;
}

uint8_t *CmduMessage::getNextTlvData() const
{
    if (!getCmduHeader())
        return nullptr;

    sTlvHeader *tlv = reinterpret_cast<sTlvHeader *>(msg.prevClass()->getBuffPtr());
    return reinterpret_cast<uint8_t *>(tlv) + sizeof(*tlv);
}

eMessageType CmduMessage::getMessageType()
{
    uint16_t msgValue = 0;
    auto cmduhdr      = getCmduHeader();
    msgValue          = (uint16_t)cmduhdr->message_type();
    if (cmduhdr->is_finalized())
        swap_16((uint16_t &)msgValue);

    return (eMessageType)msgValue;
}

uint16_t CmduMessage::getMessageId()
{
    auto cmduhdr = getCmduHeader();
    uint16_t mid = cmduhdr->message_id();

    if (cmduhdr->is_finalized())
        swap_16((uint16_t &)mid);

    return mid;
}
