///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WFA_MAP_TLVSTEERINGPOLICY_H_
#define _TLVF_WFA_MAP_TLVSTEERINGPOLICY_H_

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

namespace wfa_map {


class tlvSteeringPolicy : public BaseClass
{
    public:
        tlvSteeringPolicy(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvSteeringPolicy(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvSteeringPolicy();

        enum eSteeringPolicy: uint8_t {
            AGENT_INITIATED_STEERING_DISALLOWED = 0x0,
            AGENT_INITIATED_RSSI_BASED_STEERING_MANDATED = 0x1,
            AGENT_INITIATED_RSSI_BASED_STEERING_ALLOWED = 0x2,
        };
        
        typedef struct sRadioApControlPolicy {
            sMacAddr radio_ap_mac;
            eSteeringPolicy steering_policy;
            uint8_t channel_utilization_threshold;
            uint8_t rssi_steering_threshold;
            void struct_swap(){
                radio_ap_mac.struct_swap();
                tlvf_swap(8*sizeof(eSteeringPolicy), reinterpret_cast<uint8_t*>(&steering_policy));
            }
            void struct_init(){
                radio_ap_mac.struct_init();
            }
        } __attribute__((packed)) sRadioApControlPolicy;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& local_steering_disallowed_sta_list_length();
        std::tuple<bool, sMacAddr&> local_steering_disallowed_sta_list(size_t idx);
        bool alloc_local_steering_disallowed_sta_list(size_t count = 1);
        uint8_t& btm_steering_disallowed_sta_list_length();
        std::tuple<bool, sMacAddr&> btm_steering_disallowed_sta_list(size_t idx);
        bool alloc_btm_steering_disallowed_sta_list(size_t count = 1);
        uint8_t& radio_ap_control_policy_list_length();
        std::tuple<bool, sRadioApControlPolicy&> radio_ap_control_policy_list(size_t idx);
        bool alloc_radio_ap_control_policy_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_local_steering_disallowed_sta_list_length = nullptr;
        sMacAddr* m_local_steering_disallowed_sta_list = nullptr;
        size_t m_local_steering_disallowed_sta_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_btm_steering_disallowed_sta_list_length = nullptr;
        sMacAddr* m_btm_steering_disallowed_sta_list = nullptr;
        size_t m_btm_steering_disallowed_sta_list_idx__ = 0;
        uint8_t* m_radio_ap_control_policy_list_length = nullptr;
        sRadioApControlPolicy* m_radio_ap_control_policy_list = nullptr;
        size_t m_radio_ap_control_policy_list_idx__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVSTEERINGPOLICY_H_
