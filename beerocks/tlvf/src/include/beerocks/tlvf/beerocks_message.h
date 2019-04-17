/*
* INTEL CONFIDENTIAL
* Copyright 2018 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*l
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#ifndef _TLVF_BEEROCKS_MESSAGE_H_
#define _TLVF_BEEROCKS_MESSAGE_H_

#include <tlvf/CmduMessageTx.h>
#include <tlvf/CmduMessageRx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <beerocks/tlvf/beerocks_message_header.h>

#include <beerocks/bcl/beerocks_message_structs.h>
#include <beerocks/bcl/network/socket.h>

#include <type_traits>

#define GET_MESSAGE_POINTER(MSG, TX_B, SIZE_VAR) (MSG*)(TX_B + SIZE_VAR); SIZE_VAR += sizeof(MSG);

#define MTU_SIZE (size_t)1500

namespace beerocks {

class message_com
{
    public:
    
    static bool send_data(Socket *sd, const uint8_t *tx_buffer, size_t size);

    template <typename VS_CLASS> 
    static uint16_t get_vs_cmdu_size_on_buffer()
    {
        static const size_t get_vs_cmdu_size_on_buffer_on_buffer = 
            sizeof(message::sUdsHeader)                          +
            ieee1905_1::cCmduHeader::get_initial_size()          + 
            ieee1905_1::tlvVendorSpecific::get_initial_size()    + 
            beerocks_message::cACTION_HEADER::get_initial_size() +
            ieee1905_1::tlvEndOfMessage::get_initial_size();

        return get_vs_cmdu_size_on_buffer_on_buffer + VS_CLASS::get_initial_size();    
    }

    static message::sUdsHeader* get_uds_header(ieee1905_1::CmduMessage& cmdu);

    static bool intel_oui(std::shared_ptr<ieee1905_1::tlvVendorSpecific> tlv_vendor_specific_header);

    /*
     * TODO
     * should this be removed and replaced by parse_intel_vs_message() ?
     */

    static std::shared_ptr<beerocks_message::cACTION_HEADER> parse_intel_vs_message(ieee1905_1::CmduMessageRx& cmdu_rx);

    template <class T>
    static std::shared_ptr<T> get_vs_class(ieee1905_1::CmduMessage& cmdu) {
        return cmdu.getClass<T>(2);
    }

    static std::shared_ptr<beerocks_message::cACTION_HEADER> get_vs_class_header(ieee1905_1::CmduMessage& cmdu) {
        return cmdu.getClass<beerocks_message::cACTION_HEADER>(1);
    }

    template <class T>
    static std::shared_ptr<T> create_vs_message(ieee1905_1::CmduMessageTx& cmdu_tx, uint16_t id = 0) 
    {
        beerocks_message::eAction action;
        std::shared_ptr<T> p_class = nullptr;
        auto p_cmdu_header = cmdu_tx.create(id, ieee1905_1::tlvVendorSpecific::eVendorOUI::OUI_INTEL);
        if(p_cmdu_header) {
            auto p_action_header = cmdu_tx.addClass<beerocks_message::cACTION_HEADER>();
            if(p_action_header) {
                p_class = cmdu_tx.addClass<T>();
                if(p_class) {
                    auto action_op = p_class->get_action_op();
                    if(typeid(action_op)      == typeid(beerocks_message::eActionOp_CONTROL))   action = beerocks_message::eAction::ACTION_CONTROL;
                    else if(typeid(action_op) == typeid(beerocks_message::eActionOp_PLATFORM))  action = beerocks_message::eAction::ACTION_PLATFORM;
                    else if(typeid(action_op) == typeid(beerocks_message::eActionOp_BACKHAUL))  action = beerocks_message::eAction::ACTION_BACKHAUL;
                    else if(typeid(action_op) == typeid(beerocks_message::eActionOp_APMANAGER)) action = beerocks_message::eAction::ACTION_APMANAGER;
                    else if(typeid(action_op) == typeid(beerocks_message::eActionOp_MONITOR))   action = beerocks_message::eAction::ACTION_MONITOR;
                    else if(typeid(action_op) == typeid(beerocks_message::eActionOp_CLI))       action = beerocks_message::eAction::ACTION_CLI;
                    else if(typeid(action_op) == typeid(beerocks_message::eActionOp_BML))       action = beerocks_message::eAction::ACTION_BML;
                    else action = beerocks_message::eAction::ACTION_NONE;
                    if(action != beerocks_message::eAction::ACTION_NONE){
                        p_action_header->action() = (beerocks_message::eAction)action;
                        p_action_header->action_op() = action_op;
                        p_action_header->id() = id;
                        return p_class;
                    } else {
                        // LOG(ERROR) << "action != beerocks_message::eAction::ACTION_NONE -> This shouldn't happen";
                    }
                }
            }
        } else {
            std::cout << "beerocks_message.h[ " << __LINE__ << "]: " << "create vs message failed!" << std::endl; 
        }
        return p_class;
    }

    typedef struct sCmduInfo {
        ieee1905_1::eMessageType cmdu_type;
        beerocks_message::eAction intel_vs_action = beerocks_message::eAction::ACTION_NONE;
        uint8_t intel_vs_action_op;
    } sCmduInfo;
    
    static std::string print_cmdu_types(const message::sUdsHeader* uds_header, sCmduInfo* cmdu_info = nullptr);
    static std::string print_cmdu_types(ieee1905_1::CmduMessageRx& cmdu_rx, sCmduInfo* cmdu_info = nullptr);

    static bool send_cmdu(Socket *sd, ieee1905_1::CmduMessageTx& cmdu_tx, const std::string& dst_mac = std::string(), const std::string& src_mac = std::string());
    static bool forward_cmdu_to_uds(Socket *sd, ieee1905_1::CmduMessageRx& cmdu_rx, uint16_t length);
};

} // namespace beerocks

#endif // _TLVF_BEEROCKS_MESSAGE_H_
