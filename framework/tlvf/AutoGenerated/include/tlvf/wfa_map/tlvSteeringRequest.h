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

#ifndef _TLVF_WFA_MAP_TLVSTEERINGREQUEST_H_
#define _TLVF_WFA_MAP_TLVSTEERINGREQUEST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include <asm/byteorder.h>

namespace wfa_map {


class tlvSteeringRequest : public BaseClass
{
    public:
        tlvSteeringRequest(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvSteeringRequest(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvSteeringRequest();

        typedef struct sRequestFlags {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 5;
            uint8_t btm_abridged_bit : 1;
            uint8_t btm_disassociation_imminent_bit : 1;
            uint8_t request_mode : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t request_mode : 1;
            uint8_t btm_disassociation_imminent_bit : 1;
            uint8_t btm_abridged_bit : 1;
            uint8_t reserved : 5;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
                reserved = 0x0;
            }
        } __attribute__((packed)) sRequestFlags;
        
        enum eRequestMode {
            REQUEST_IS_A_STEERING_OPPORTUNITY = 0x0,
            REQUEST_IS_A_STEERING_MANDATE_TO_TRIGGER_STEERING = 0x1,
        };
        
        typedef struct sTargetBssidInfo {
            //Wildcard BSSID is represented by FF:FF:FF:FF:FF:FF.
            sMacAddr target_bssid;
            uint8_t target_bss_operating_class;
            uint8_t target_bss_channel_number;
            void struct_swap(){
                target_bssid.struct_swap();
            }
            void struct_init(){
            }
        } __attribute__((packed)) sTargetBssidInfo;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& bssid();
        sRequestFlags& request_flags();
        //Time period in seconds (from reception of the Steering Request message) for which the request is valid.
        //If Request Mode bit is 1, then this field is ignored 
        uint16_t& steering_opportunity_window_sec();
        //Time period in TUs of the disassociation timer in the BTM Request.
        uint16_t& btm_disassociation_timer_ms();
        uint8_t& sta_list_length();
        std::tuple<bool, sMacAddr&> sta_list(size_t idx);
        bool alloc_sta_list(size_t count = 1);
        uint8_t& target_bssid_list_length();
        std::tuple<bool, sTargetBssidInfo&> target_bssid_list(size_t idx);
        bool alloc_target_bssid_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_bssid = nullptr;
        sRequestFlags* m_request_flags = nullptr;
        uint16_t* m_steering_opportunity_window_sec = nullptr;
        uint16_t* m_btm_disassociation_timer_ms = nullptr;
        uint8_t* m_sta_list_length = nullptr;
        sMacAddr* m_sta_list = nullptr;
        size_t m_sta_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_target_bssid_list_length = nullptr;
        sTargetBssidInfo* m_target_bssid_list = nullptr;
        size_t m_target_bssid_list_idx__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVSTEERINGREQUEST_H_
