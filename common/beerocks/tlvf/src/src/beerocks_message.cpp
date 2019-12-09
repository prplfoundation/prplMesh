/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <beerocks/tlvf/beerocks_message.h>

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

using namespace beerocks;

bool message_com::send_data(Socket *sd, const uint8_t *tx_buffer, size_t size)
{
    if (tx_buffer == nullptr) {
        LOG(ERROR) << "tx_buffer is null!";
        return false;
    }
    if (sd == nullptr) {
        LOG(ERROR) << "send_data(): sd is nullptr";
        return false;
    }
    if (size == 0) {
        LOG(ERROR) << "send_data(), size=0";
        return false;
    }
    ssize_t written_size = sd->writeBytes(tx_buffer, size);
    if (written_size < 0) {
        LOG(ERROR) << "writeBytes() failed, error=" << strerror(errno);
        return false;
    } else if (written_size != (ssize_t)size) {
        LOG(ERROR) << "writeBytes() failed, written " << written_size << " out of " << size
                   << " bytes";
        return false;
    }
    return true;
}

message::sUdsHeader *message_com::get_uds_header(ieee1905_1::CmduMessage &cmdu)
{
    if (cmdu.getMessageLength())
        return (message::sUdsHeader *)(cmdu.getMessageBuff() - sizeof(message::sUdsHeader));
    LOG(ERROR) << "cmdu is not initialized!";
    return nullptr;
}

bool message_com::intel_oui(
    std::shared_ptr<ieee1905_1::tlvVendorSpecific> tlv_vendor_specific_header)
{
    return tlv_vendor_specific_header->vendor_oui() ==
           ieee1905_1::tlvVendorSpecific::eVendorOUI::OUI_INTEL;
}

std::shared_ptr<beerocks::beerocks_header>
message_com::parse_intel_vs_message(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (!cmdu_rx.getMessageLength()) {
        LOG(ERROR) << "cmdu is not initialized!";
    }
    auto tlv_header = cmdu_rx.addClass<ieee1905_1::tlvVendorSpecific>();
    if (!tlv_header)
        return nullptr;
    if (!intel_oui(tlv_header))
        return nullptr;
    
    std::unique_ptr<ieee1905_1::TlvList> vs_contents =
        std::make_unique<ieee1905_1::TlvList>(tlv_header->payload(), tlv_header->getLen(), true, cmdu_rx.swap_needed());
    if (!vs_contents)
        return nullptr;
    auto header = vs_contents->addClass<beerocks_message::cACTION_HEADER>();
    if (!header)
        return nullptr;
    return std::make_shared<beerocks_header>(std::move(vs_contents));
}

std::string message_com::print_cmdu_types(const message::sUdsHeader *uds_header,
                                          sCmduInfo *cmdu_info)
{
    ieee1905_1::CmduMessageRx cmdu_rx((uint8_t *)uds_header + sizeof(message::sUdsHeader), uds_header->length);

    auto cmdu_header = cmdu_rx.parse(uds_header->swap_needed);

    if (!cmdu_header) {
        LOG(ERROR) << "cmdu parse failed!";
        return std::string();
    }

    return print_cmdu_types(cmdu_rx, cmdu_info);
}

std::string message_com::print_cmdu_types(ieee1905_1::CmduMessageRx &cmdu_rx, sCmduInfo *cmdu_info)
{
    std::string info;
    auto uds_header = get_uds_header(cmdu_rx);
    if (!uds_header) {
        LOG(ERROR) << "uds_header=nullptr";
        return std::string();
    }
    auto message_type = cmdu_rx.getMessageType();
    if (cmdu_info)
        cmdu_info->cmdu_type = message_type;
    info                     = "cmdu_type=" + std::to_string(int(message_type));
    if (message_type == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
        auto tlv_header = cmdu_rx.addClass<ieee1905_1::tlvVendorSpecific>();
        if (!intel_oui(tlv_header))
            return std::string();
        if (!tlv_header) {
            LOG(ERROR) << "addClass<tlvVendorSpecific> failed!";
            return info;
        }
        ieee1905_1::TlvList actions(tlv_header->payload(), tlv_header->payload_length(), true, true);
        auto beerocks_header = actions.addClass<beerocks_message::cACTION_HEADER>();
        if (!beerocks_header) {
            LOG(ERROR) << "addClass<cACTION_HEADER> failed!";
            return info;
        }

        if (cmdu_info) {
            cmdu_info->intel_vs_action    = beerocks_header->action();
            cmdu_info->intel_vs_action_op = beerocks_header->action_op();
        }

        info += ", INTEL_VS: action=" + std::to_string(beerocks_header->action()) + ", action_op=" +
                std::to_string(beerocks_header->action_op());
    }

    if (uds_header->swap_needed) {
        // swap back
        cmdu_rx.swap();
    }
    return info;
}

bool message_com::send_cmdu(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx,
                            const std::string &dst_mac, const std::string &src_mac)
{
    auto uds_header = get_uds_header(cmdu_tx);

    if (!uds_header) {
        LOG(ERROR) << "uds_header=nullptr";
        return false;
    }

    bool swap = !dst_mac.empty();

    if (!cmdu_tx.finalize(swap)) {
        LOG(ERROR) << "finalize failed -> " << print_cmdu_types(uds_header);
        LOG(DEBUG) << "hex_dump(" + std::to_string(cmdu_tx.getMessageLength()) + "):" << std::endl
                   << utils::dump_buffer((uint8_t *)(cmdu_tx.getMessageBuff()
                                         - sizeof(message::sUdsHeader)),
                                         cmdu_tx.getMessageLength() + sizeof(message::sUdsHeader));
        return false;
    }

    // update src & dst bridge mac on uds_header
    // swap is true if dst_mac is not empty so no need to verify it
    if (swap) {
        if (src_mac.empty()) {
            LOG(ERROR) << "src_mac is empty!";
            return false;
        }

        net::network_utils::mac_from_string(uds_header->src_bridge_mac, src_mac);
        net::network_utils::mac_from_string(uds_header->dst_bridge_mac, dst_mac);
    } else {
        memset(uds_header->src_bridge_mac, 0, sizeof(message::sUdsHeader::src_bridge_mac));
        memset(uds_header->dst_bridge_mac, 0, sizeof(message::sUdsHeader::dst_bridge_mac));
    }

    uds_header->length      = cmdu_tx.getMessageLength();
    uds_header->swap_needed = false; //swap;

    return send_data(sd, cmdu_tx.getMessageBuff() - sizeof(message::sUdsHeader),
                     uds_header->length + sizeof(message::sUdsHeader));
}

bool message_com::forward_cmdu_to_uds(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx,
                                      uint16_t length)
{
    return message_com::send_data(sd, cmdu_rx.getMessageBuff() - sizeof(message::sUdsHeader),
                                  length + sizeof(message::sUdsHeader));
}
