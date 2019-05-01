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

#ifndef _TLVF_WFA_MAP_TLVAPCAPABILITY_H_
#define _TLVF_WFA_MAP_TLVAPCAPABILITY_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include <asm/byteorder.h>

namespace wfa_map {


class tlvApCapability : public BaseClass
{
    public:
        tlvApCapability(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvApCapability(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvApCapability();

        typedef struct sValue {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 5;
            uint8_t support_agent_initiated_rssi_based_steering : 1;
            uint8_t support_unassociated_sta_link_metrics_on_non_operating_bssid : 1;
            uint8_t support_unassociated_sta_link_metrics_on_operating_bssid : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t support_unassociated_sta_link_metrics_on_operating_bssid : 1;
            uint8_t support_unassociated_sta_link_metrics_on_non_operating_bssid : 1;
            uint8_t support_agent_initiated_rssi_based_steering : 1;
            uint8_t reserved : 5;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
                reserved = 0x0;
            }
        } sValue;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sValue& value();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sValue* m_value = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPCAPABILITY_H_
