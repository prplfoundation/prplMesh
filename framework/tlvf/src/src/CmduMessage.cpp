/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessage.h>

using namespace ieee1905_1;

eMessageType CmduMessage::getMessageType()
{
    uint16_t msgValue = 0;
    auto cmduhdr      = getCmduHeader();
    msgValue          = (uint16_t)cmduhdr->message_type();
    if (cmduhdr->is_finalized())
        swap_16(msgValue);

    return (eMessageType)msgValue;
}

uint16_t CmduMessage::getMessageId()
{
    auto cmduhdr = getCmduHeader();
    uint16_t mid = cmduhdr->message_id();

    if (cmduhdr->is_finalized())
        swap_16(mid);

    return mid;
}

void CmduMessage::setMessageId(uint16_t mid)
{
    auto cmduhdr = getCmduHeader();

    if (cmduhdr->is_finalized())
        swap_16(mid);
    cmduhdr->message_id() = mid;
}
