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

    static std::shared_ptr<beerocks_header>
    get_beerocks_header(ieee1905_1::CmduMessage &cmdu)
    {
        return cmdu.tlvs.getInnerTlvList<beerocks_header>(ieee1905_1::eTlvType::TLV_VENDOR_SPECIFIC);
    }

    template <class T>
    static bool add_intel_vs_data(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto vs_tlv = cmdu_tx.getClass<ieee1905_1::tlvVendorSpecific>();
        if (!vs_tlv) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return false;
        }
        // Allocate maximum allowed length for the payload, so it can accommodate variable length
        // data inside the internal TLV list.
        // On finalize(), the buffer is shrunk back to its real size.
        size_t payload_length;
        const size_t max_vs_tlv_length = cmdu_tx.max_tlv_length() - ieee1905_1::tlvVendorSpecific::get_initial_size();
        payload_length = std::min(vs_tlv->getBuffRemainingBytes() -
                                    ieee1905_1::tlvEndOfMessage::get_initial_size(),
                                    max_vs_tlv_length);
        if (!vs_tlv->alloc_payload(payload_length)) {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << __FUNCTION__ << " failed!" << std::endl;
            return false;
        }
        std::shared_ptr<beerocks_header> hdr = std::make_shared<beerocks_header>(vs_tlv->payload(),
                                                            vs_tlv->payload_length(), false, false);
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
        cmdu_tx.tlvs.addInnerTlvList(ieee1905_1::eTlvType::TLV_VENDOR_SPECIFIC, hdr);

        return true;
    }

    /**
     * @brief Create a jumbo vs message object
     *
     * create_vs_message preallocates the maximum tlv size for each vendor
     * specific TLV payload, based on the maximum tlv size in CmduMessage.
     *
     * This method allows the creation of "jumbo" CMDUs which are meant to
     * be only sent internally over UDS by setting the maximum TLV size
     * of the CMDU to be bigger than the MTU, based on the actual buffer size.
     *
     * @tparam T beerocks operation template type
     * @param cmdu_tx Transmit CMDU descriptor
     * @param id message id
     * @return std::shared_ptr<T> to the created operation class
     */
    template <class T>
    static std::shared_ptr<T> create_jumbo_vs_message(ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id = 0)
    {
        auto max_tlv_length = cmdu_tx.max_tlv_length();

        cmdu_tx.set_max_tlv_length(cmdu_tx.tlvs.getMessageBuffLength() -
                                   ieee1905_1::CmduMessage::kCmduHeaderLength -
                                   ieee1905_1::tlvEndOfMessage::get_initial_size());
        auto ret = create_vs_message<T>(cmdu_tx, id);
        cmdu_tx.set_max_tlv_length(max_tlv_length);

        return ret;
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

        if (!add_intel_vs_data<T>(cmdu_tx, id))
            return nullptr;

        auto beerocks_header = get_beerocks_header(cmdu_tx);
        if (!beerocks_header)
            return nullptr;

        return beerocks_header->getClass<T>();
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
