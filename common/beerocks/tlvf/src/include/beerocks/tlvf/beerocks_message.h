/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_BEEROCKS_MESSAGE_H_
#define _TLVF_BEEROCKS_MESSAGE_H_

#include <beerocks/tlvf/beerocks_header.h>
#include <tlvf/CmduMessageRx.h>
#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <bcl/beerocks_message_structs.h>
#include <bcl/network/socket.h>

#include <type_traits>

#define GET_MESSAGE_POINTER(MSG, TX_B, SIZE_VAR)                                                   \
    (MSG *)(TX_B + SIZE_VAR);                                                                      \
    SIZE_VAR += sizeof(MSG);

#define MTU_SIZE (size_t)1500

namespace beerocks {

class message_com {
public:
    static bool send_data(Socket *sd, const uint8_t *tx_buffer, size_t size);

    template <typename VS_CLASS> static uint16_t get_vs_cmdu_size_on_buffer()
    {
        static const size_t get_vs_cmdu_size_on_buffer_on_buffer =
            sizeof(message::sUdsHeader) + ieee1905_1::cCmduHeader::get_initial_size() +
            ieee1905_1::tlvVendorSpecific::get_initial_size() +
            beerocks_message::cACTION_HEADER::get_initial_size() +
            ieee1905_1::tlvEndOfMessage::get_initial_size();

        return get_vs_cmdu_size_on_buffer_on_buffer + VS_CLASS::get_initial_size();
    }

    static message::sUdsHeader *get_uds_header(ieee1905_1::CmduMessage &cmdu);

    static bool
    intel_oui(std::shared_ptr<ieee1905_1::tlvVendorSpecific> tlv_vendor_specific_header);

    static std::shared_ptr<beerocks_header>
    parse_intel_vs_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    static std::shared_ptr<beerocks_header> get_beerocks_header(ieee1905_1::CmduMessage &cmdu)
    {
        auto tlv = cmdu.msg.getClass<ieee1905_1::tlvVendorSpecific>();
        if (!tlv)
            return nullptr;
        if (!intel_oui(tlv))
            return nullptr;
        return std::dynamic_pointer_cast<beerocks_header>(tlv->getInnerClassList());
    }

    template <class T>
    static std::shared_ptr<beerocks_header>
    add_intel_vs_data(std::shared_ptr<ieee1905_1::tlvVendorSpecific> tlv, uint16_t id = 0)
    {
        // Allocate maximum allowed length for the payload, so it can accommodate variable length
        // data inside the internal TLV list.
        // On finalize(), the buffer is shrunk back to its real size.
        size_t payload_length =
            tlv->getBuffRemainingBytes() - ieee1905_1::tlvEndOfMessage::get_initial_size();
        if (!tlv->alloc_payload(payload_length)) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!"
                      << std::endl;
            return nullptr;
        }
        std::shared_ptr<beerocks_header> hdr =
            std::make_shared<beerocks_header>(tlv->payload(), tlv->payload_length(), false);
        if (!hdr)
            return nullptr;
        if (!hdr->addClass<beerocks_message::cACTION_HEADER>())
            return nullptr;
        if (!hdr->addClass<T>())
            return nullptr;

        beerocks_message::eAction action = beerocks_message::eAction::ACTION_NONE;
        auto action_op                   = hdr->getClass<T>()->get_action_op();
        if (typeid(action_op) == typeid(beerocks_message::eActionOp_1905_VS))
            action = beerocks_message::eAction::ACTION_1905_VS;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_CONTROL))
            action = beerocks_message::eAction::ACTION_CONTROL;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_PLATFORM))
            action = beerocks_message::eAction::ACTION_PLATFORM;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_BACKHAUL))
            action = beerocks_message::eAction::ACTION_BACKHAUL;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_APMANAGER))
            action = beerocks_message::eAction::ACTION_APMANAGER;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_MONITOR))
            action = beerocks_message::eAction::ACTION_MONITOR;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_CLI))
            action = beerocks_message::eAction::ACTION_CLI;
        else if (typeid(action_op) == typeid(beerocks_message::eActionOp_BML))
            action = beerocks_message::eAction::ACTION_BML;

        if (action == beerocks_message::eAction::ACTION_NONE) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!"
                      << std::endl;
            return nullptr;
        }

        hdr->actionhdr()->action()    = (beerocks_message::eAction)action;
        hdr->actionhdr()->action_op() = action_op;
        hdr->actionhdr()->id()        = id;
        tlv->addInnerClassList(hdr);

        return hdr;
    }

    template <class T>
    static std::shared_ptr<T> create_vs_message(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto cmduhdr = cmdu_tx.create(id, ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE);
        if (!cmduhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!"
                      << std::endl;
            return nullptr;
        }

        return add_vs_tlv<T>(cmdu_tx, id);
    }

    template <class T>
    static std::shared_ptr<T> add_vs_tlv(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto tlvhdr = cmdu_tx.add_vs_tlv(ieee1905_1::tlvVendorSpecific::eVendorOUI::OUI_INTEL);
        if (!tlvhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!"
                      << std::endl;
            return nullptr;
        }

        auto beerocks_header = add_intel_vs_data<T>(tlvhdr, id);
        if (!beerocks_header)
            return nullptr;

        // According to C++'03 Standard 14.2/4:
        // When the name of a member template specialization appears after . or -> in a postfix-expression,
        // or after nested-name-specifier in a qualified-id, and the postfix-expression or qualified-id
        // explicitly depends on a template-parameter (14.6.2), the member template name must be prefixed by
        // the keyword template. Otherwise the name is assumed to name a non-template.
        return beerocks_header->template getClass<T>();
    }

    typedef struct sCmduInfo {
        ieee1905_1::eMessageType cmdu_type;
        beerocks_message::eAction intel_vs_action = beerocks_message::eAction::ACTION_NONE;
        uint8_t intel_vs_action_op;
    } sCmduInfo;

    static std::string print_cmdu_types(const message::sUdsHeader *uds_header,
                                        sCmduInfo *cmdu_info = nullptr);
    static std::string print_cmdu_types(ieee1905_1::CmduMessageRx &cmdu_rx,
                                        sCmduInfo *cmdu_info = nullptr);

    static bool send_cmdu(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx,
                          const std::string &dst_mac = std::string(),
                          const std::string &src_mac = std::string());
    static bool forward_cmdu_to_uds(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx,
                                    uint16_t length);
};

} // namespace beerocks

#endif // _TLVF_BEEROCKS_MESSAGE_H_
