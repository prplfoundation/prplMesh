///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_1905_VS_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_1905_VS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "beerocks/bcl/beerocks_message_structs.h"
#include "beerocks/tlvf/beerocks_message_action.h"

namespace beerocks_message {


class tlvVsClientAssociationEvent : public BaseClass
{
    public:
        tlvVsClientAssociationEvent(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvVsClientAssociationEvent(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvVsClientAssociationEvent();

        static eActionOp_1905_VS get_action_op(){
            return (eActionOp_1905_VS)(ACTION_TLV_VENDOR_SPECIFIC);
        }
        sMacAddr& mac();
        sMacAddr& bssid();
        int8_t& vap_id();
        //relevant only on connect event
        beerocks::message::sRadioCapabilities& capabilities();
        //relevant only on disconnect event
        uint8_t& disconnect_reason();
        //relevant only on disconnect event
        uint8_t& disconnect_source();
        //relevant only on disconnect event
        uint8_t& disconnect_type();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_1905_VS* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        sMacAddr* m_bssid = nullptr;
        int8_t* m_vap_id = nullptr;
        beerocks::message::sRadioCapabilities* m_capabilities = nullptr;
        uint8_t* m_disconnect_reason = nullptr;
        uint8_t* m_disconnect_source = nullptr;
        uint8_t* m_disconnect_type = nullptr;
};

class tlvVsClientAssociationControlRequest : public BaseClass
{
    public:
        tlvVsClientAssociationControlRequest(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvVsClientAssociationControlRequest(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvVsClientAssociationControlRequest();

        static eActionOp_1905_VS get_action_op(){
            return (eActionOp_1905_VS)(ACTION_TLV_VENDOR_SPECIFIC);
        }
        beerocks::net::sIpv4Addr& ipv4();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_1905_VS* m_action_op = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_1905_VS_H_
