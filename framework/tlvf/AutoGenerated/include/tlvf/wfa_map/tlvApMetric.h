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

#ifndef _TLVF_WFA_MAP_TLVAPMETRIC_H_
#define _TLVF_WFA_MAP_TLVAPMETRIC_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include <asm/byteorder.h>

namespace wfa_map {


class tlvApMetric : public BaseClass
{
    public:
        tlvApMetric(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvApMetric(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvApMetric();

        typedef struct sEstimatedService {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 4;
            uint8_t include_ac_vi : 1;
            uint8_t include_ac_vo : 1;
            uint8_t include_ac_bk : 1;
            uint8_t include_ac_be : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t include_ac_be : 1;
            uint8_t include_ac_bk : 1;
            uint8_t include_ac_vo : 1;
            uint8_t include_ac_vi : 1;
            uint8_t reserved : 4;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
                include_ac_be = 0x1;
            }
        } __attribute__((packed)) sEstimatedService;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& bssid();
        uint8_t& channel_utilization();
        uint16_t& number_of_stas_currently_associated();
        sEstimatedService& estimated_service_parameters();
        size_t estimated_service_info_field_length() { return m_estimated_service_info_field_idx__ * sizeof(uint8_t); }
        uint8_t* estimated_service_info_field(size_t idx = 0);
        bool alloc_estimated_service_info_field(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_bssid = nullptr;
        uint8_t* m_channel_utilization = nullptr;
        uint16_t* m_number_of_stas_currently_associated = nullptr;
        sEstimatedService* m_estimated_service_parameters = nullptr;
        uint8_t* m_estimated_service_info_field = nullptr;
        size_t m_estimated_service_info_field_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPMETRIC_H_
