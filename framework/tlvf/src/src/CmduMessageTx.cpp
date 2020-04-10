/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <tlvf/tlvflogging.h>

#include <set>

using namespace ieee1905_1;

static void set_relay_indicator(std::shared_ptr<ieee1905_1::cCmduHeader> &cmdu_header)
{
    // Taken from Table 6-4 on IEEE 1905.1-2013
    auto message_type                                         = cmdu_header->message_type();
    const std::set<ieee1905_1::eMessageType> relayed_messages = {
        ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE,
        ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE,
        ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE,
        ieee1905_1::eMessageType::PUSH_BUTTON_EVENT_NOTIFICATION_MESSAGE,
        ieee1905_1::eMessageType::PUSH_BUTTON_JOIN_NOTIFICATION_MESSAGE,
    };

    if (relayed_messages.find(message_type) != relayed_messages.end()) {
        cmdu_header->flags().relay_indicator = true;
    }
}

CmduMessageTx::CmduMessageTx(uint8_t *buff, size_t buff_len) : CmduMessage(buff, buff_len) {}

CmduMessageTx::~CmduMessageTx() {}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, eMessageType message_type)
{
    // parse - false since this is for TX
    msg.reset(false);
    auto cmduhdr = addClass<cCmduHeader>();
    if (!cmduhdr)
        return nullptr;

    cmduhdr->message_type() = message_type;
    cmduhdr->message_id()   = id;

    set_relay_indicator(cmduhdr);

    return cmduhdr;
}

std::shared_ptr<cCmduHeader> CmduMessageTx::load()
{
    msg.reset(true);
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

bool CmduMessageTx::finalize()
{
    if (!addClass<tlvEndOfMessage>())
        return false;
    return msg.finalize();
};
