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

    /*
     * TODO
     * should this be removed and replaced by parse_intel_vs_message() ?
     */

    class beerocks_header {
    public:
        beerocks_header(std::unique_ptr<ieee1905_1::TlvList> contents,
                        std::shared_ptr<beerocks_message::cACTION_HEADER> header)
            : m_header(header),m_contents(std::move(contents))
        {}
        beerocks_message::eAction action() { return m_header->action(); }
        uint8_t action_op() { return m_header->action_op(); };
        uint16_t id(){return m_header->id();}
        template <class T> std::shared_ptr<T> addClass()
        {
            return m_contents->addClass<T>();
        }
        template <class T> std::shared_ptr<T> get_vs_class()
        {
            return m_contents->getClass<T>(2);
        }
        std::shared_ptr<beerocks_message::cACTION_HEADER> m_header;
    private:
        std::unique_ptr<ieee1905_1::TlvList> m_contents;
    };

    static std::shared_ptr<beerocks_header>
    parse_intel_vs_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    static std::shared_ptr<beerocks_message::cACTION_HEADER>
    get_vs_class_header(ieee1905_1::CmduMessage &cmdu)
    {
        return std::dynamic_pointer_cast<beerocks_message::cACTION_HEADER>(cmdu.getClass(1));
    }

    template <class T>
    static std::shared_ptr<T> add_intel_vs_data(std::shared_ptr<ieee1905_1::tlvVendorSpecific> vs_tlv, bool swap, uint16_t id = 0)
    {
        beerocks_message::eAction action;
        std::shared_ptr<T> p_class = nullptr;
        // TODO This fixed payload length only works for fixed-size payload.
        // Most of the beerocks messages are fixed size, but we need to find a solution for
        // the others. Perhaps in finalize, like it was done before.
        size_t payload_length = beerocks_message::cACTION_HEADER::get_initial_size() +
                T::get_initial_size();
        if (!vs_tlv->alloc_payload(payload_length)) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }
    
        ieee1905_1::TlvList actions(vs_tlv->payload(), payload_length, false, swap);
        auto actionhdr = actions.addClass<beerocks_message::cACTION_HEADER>();
        if (!actionhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        p_class = actions.addClass<T>();
        if (!p_class) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        auto action_op = p_class->get_action_op();
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
        else
            action = beerocks_message::eAction::ACTION_NONE;

        if (action == beerocks_message::eAction::ACTION_NONE) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        actionhdr->action()    = (beerocks_message::eAction)action;
        actionhdr->action_op() = action_op;
        actionhdr->id()        = id;
        actions.swap();

        return p_class;
    }

    template <class T>
    static std::shared_ptr<T> create_vs_message(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto cmduhdr = cmdu_tx.create(id, ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE);
        if (!cmduhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        return add_vs_tlv<T>(cmdu_tx, id);
    }

    template <class T>
    static std::shared_ptr<T> add_vs_tlv(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto tlvhdr = cmdu_tx.add_vs_tlv(ieee1905_1::tlvVendorSpecific::eVendorOUI::OUI_INTEL);
        if (!tlvhdr) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return nullptr;
        }

        return add_intel_vs_data<T>(tlvhdr, id);
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
