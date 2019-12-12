/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_BEEROCKS_MESSAGE_H_
#define _TLVF_BEEROCKS_MESSAGE_H_


#include <beerocks/tlvf/beerocks_message_header.h>
#include <tlvf/CmduMessageRx.h>
#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/TlvList.h>

#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_backport.h>
#include <bcl/network/socket.h>

#include <type_traits>
#include <algorithm>

#define GET_MESSAGE_POINTER(MSG, TX_B, SIZE_VAR)                                                   \
    (MSG *)(TX_B + SIZE_VAR);                                                                      \
    SIZE_VAR += sizeof(MSG);

#define MTU_SIZE (size_t)1500

namespace beerocks {

class beerocks_header {
    public:
        beerocks_header(std::unique_ptr<ieee1905_1::TlvList> contents)
            : actions(std::move(contents))
        {}
        beerocks_message::eAction action() { return actionhdr()->action(); }
        uint8_t action_op() { return actionhdr()->action_op(); };
        uint16_t id() { return actionhdr()->id(); }
        template <class T> std::shared_ptr<T> addClass()
        {
            return actions->addClass<T>();
        }
        template <class T> std::shared_ptr<T> getClass()
        {
            return actions->getClass<T>();
        }
        std::shared_ptr<beerocks_message::cACTION_HEADER> actionhdr()
        {
            return actions->getClass<beerocks_message::cACTION_HEADER>();
        }
        std::unique_ptr<ieee1905_1::TlvList> actions;
    };

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

    /*
     * TODO
     * should this be removed and replaced by parse_intel_vs_message() ?
     */

    static std::shared_ptr<beerocks_header>
    parse_intel_vs_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    static std::shared_ptr<beerocks_message::cACTION_HEADER>
    get_vs_class_header(ieee1905_1::CmduMessageTx &cmdu_tx)
    {
        auto beerocks_header = cmdu_tx.header;
        if (!beerocks_header) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }
        return beerocks_header->actionhdr();
    }

    template <class T>
    static bool add_intel_vs_data(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0, bool resize = false)
    {
        auto vs_tlv = cmdu_tx.getClass<ieee1905_1::tlvVendorSpecific>();
        if (!vs_tlv) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return false;
        }
        // TODO This fixed payload length only works for fixed-size payload.
        // Most of the beerocks messages are fixed size, but we need to find a solution for
        // the others. Perhaps in finalize, like it was done before.
        // So the solution here for now is to allocate the whole buffer excluding the EOM TLV size which will
        // be added in Finalize. This however is done only for vendor specific *CMDUs* until a proper solution
        // is added to the Finalize() API which will shrink the buffer back to its real size.
        // The reason we do this for VS messages only is because VS TLVs which are added as part of a standard CMDU
        // can't be this big cause fragmantaion in the ieee transport is done on TLV boundary, and CMDU size cannot be
        // larget then the MTU.
        // resize is used only for VS CMDUs so that it won't be used for VS TLVs which are part of a 1905 CMDU, such as
        // WSC autoconfiguration.
        size_t payload_length;
        if (resize) {
            const size_t max_vs_tlv_length = ieee1905_1::CmduMessage::kMaxCmduLength -
                                             ieee1905_1::CmduMessage::kCmduHeaderLength -
                                             ieee1905_1::tlvVendorSpecific::get_initial_size() -
                                             ieee1905_1::tlvEndOfMessage::get_initial_size();
            payload_length = std::min(vs_tlv->getBuffRemainingBytes() -
                                      ieee1905_1::tlvEndOfMessage::get_initial_size(),
                                      max_vs_tlv_length);
        } else {
            payload_length = beerocks_message::cACTION_HEADER::get_initial_size() + T::get_initial_size();
        }
        if (!vs_tlv->alloc_payload(payload_length)) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return false;
        }
    
        std::unique_ptr<ieee1905_1::TlvList> actions =
        std::make_unique<ieee1905_1::TlvList>(vs_tlv->payload(), payload_length, false, false);
        if (!actions)
            return false;
        std::shared_ptr<beerocks_header> hdr = std::make_shared<beerocks_header>(std::move(actions));
        if (!hdr)
            return false;
        if (!hdr->addClass<beerocks_message::cACTION_HEADER>())
            return false;
        if (!hdr->addClass<T>())
            return false;

        beerocks_message::eAction action = beerocks_message::eAction::ACTION_NONE;
        auto action_op = hdr->getClass<T>()->get_action_op();
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
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return false;
        }

        hdr->actionhdr()->action()    = (beerocks_message::eAction)action;
        hdr->actionhdr()->action_op() = action_op;
        hdr->actionhdr()->id()        = id;
        cmdu_tx.header = hdr;

        return true;
    }

    template <class T>
    static std::shared_ptr<T> create_vs_message(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto cmduhdr = cmdu_tx.create(id, ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE);
        if (!cmduhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        return add_vs_tlv<T>(cmdu_tx, id, true);
    }

    template <class T>
    static std::shared_ptr<T> add_vs_tlv(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0, bool resize = false)
    {
        auto tlvhdr = cmdu_tx.add_vs_tlv(ieee1905_1::tlvVendorSpecific::eVendorOUI::OUI_INTEL);
        if (!tlvhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        if (!add_intel_vs_data<T>(cmdu_tx, id, resize))
            return nullptr;

        auto beerocks_header = get_beerocks_header(cmdu_tx);
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
