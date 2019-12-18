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

CmduMessageTx::CmduMessageTx(uint8_t *buff, size_t buff_len) : CmduMessage(buff, buff_len) {}

CmduMessageTx::~CmduMessageTx() {}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, eMessageType message_type)
{
    // parse - false since this is for TX
    // swap - false since on TX we do swap in Finalize(swap_needed)
    tlvs.reset(false, false);
    auto cmduhdr = tlvs.addClass<cCmduHeader>();
    if (!cmduhdr)
        return nullptr;

    cmduhdr->message_type() = message_type;
    cmduhdr->message_id()   = id;
    return cmduhdr;
}

std::shared_ptr<cCmduHeader> CmduMessageTx::load()
{
    LOG(DEBUG) << "CmduMessageTx::load";
    tlvs.reset(true, false);
    return addClass<cCmduHeader>();
}

std::shared_ptr<tlvVendorSpecific> CmduMessageTx::add_vs_tlv(tlvVendorSpecific::eVendorOUI voui)
{
    if (!getCmduHeader())
        return nullptr;

    auto tlv = addClass<tlvVendorSpecific>();
    if (!tlv)
        return nullptr;

    //set vendor specific oui
    tlv->vendor_oui() = (uint32_t)voui;

    return tlv;
}

bool CmduMessageTx::finalize(bool swap_needed) { return tlvs.finalize(swap_needed); }
