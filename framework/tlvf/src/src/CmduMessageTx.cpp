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
    msg.reset(false);
    auto cmduhdr = addClass<cCmduHeader>();
    if (!cmduhdr)
        return nullptr;

    cmduhdr->message_type() = message_type;
    cmduhdr->message_id()   = id;
    return cmduhdr;
}

/**
 * @brief load a prefilled cmdu for transmission
 *
 * In certification mode, we are sometimes required to wait for UCC
 * command which triggers a completion of a certain flow by sending a
 * CMDU which would normally be sent without external intervention.
 * In these cases, prplMesh prepares the CMDU for tranmission but doesn't
 * actually send it - it saves the buffer contents in the certification_tx_buffer
 * instead which contains the entire CMDU in network byte order ready to be sent.
 * Sending the CMDU on UCC trigger first "loads" the certification_tx_buffer
 * into a new CmduMessageTx then sends it.
 * This API does the loading of the buffer into a this CmduMessageTx by first calling
 * the parse() API which sets all the pointers, then swap the contents back to network
 * byte order since on parse we swap to host byte order, and returns the cCmduHeader
 * shared_ptr so the MID can be set by the caller.
 *
 * @return std::shared_ptr<cCmduHeader> 
 */
std::shared_ptr<cCmduHeader> CmduMessageTx::load()
{
    parse();
    msg.swap(); // swap back
    return msg.getClass<cCmduHeader>();
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
