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

#ifndef _TLVF_WFA_MAP_TLVCHANNELPREFERENCE_H_
#define _TLVF_WFA_MAP_TLVCHANNELPREFERENCE_H_

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
#include <vector>
#include <asm/byteorder.h>

namespace wfa_map {

class cPreferenceOperatingClasses;

class tlvChannelPreference : public BaseClass
{
    public:
        tlvChannelPreference(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvChannelPreference(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvChannelPreference();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        uint8_t& operating_classes_list_length();
        std::tuple<bool, cPreferenceOperatingClasses&> operating_classes_list(size_t idx);
        std::shared_ptr<cPreferenceOperatingClasses> create_operating_classes_list();
        bool add_operating_classes_list(std::shared_ptr<cPreferenceOperatingClasses> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        uint8_t* m_operating_classes_list_length = nullptr;
        cPreferenceOperatingClasses* m_operating_classes_list = nullptr;
        size_t m_operating_classes_list_idx__ = 0;
        std::vector<std::shared_ptr<cPreferenceOperatingClasses>> m_operating_classes_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cPreferenceOperatingClasses : public BaseClass
{
    public:
        cPreferenceOperatingClasses(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cPreferenceOperatingClasses(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cPreferenceOperatingClasses();

        enum eReasonCode {
            UNSPECIFIED = 0x0,
            PROXIMATE_NON_802_11_INTERFERER_IN_LOCAL_ENVIRONMENT = 0x1,
            INTRA_NETWORK_802_11_OBSS_INTERFERENCE_MANAGEMENT = 0x2,
            EXTERNAL_NETWORK_802_11_OBSS_INTERFERENCE_MANAGEMENT = 0x3,
            REDUCED_COVERAGE_LIMITED_TRANSMIT_POWER = 0x4,
            REDUCED_THROUGHPUT_LIMITED_CHANNEL_BANDWIDTH = 0x5,
            IN_DEVICE_INTERFERER_WITHIN_AP = 0x6,
            OPERATION_DISALLOWED_DUE_TO_RADAR_DETECTION_ON_A_DFS_CHANNEL = 0x7,
            OPERATION_WOULD_PREVENT_BACKHAUL_OPERATION_USING_SHARED_RADIO = 0x8,
            IMMEDIATE_OPERATION_POSSIBLE_ON_A_DFS_CHANNEL_CAC_HAS_BEEN_RUN__CHANNEL_HAS_BEEN_CLEARED_FOR_USE = 0x9,
            DFS_CHANNEL_STATE_UNKNOWN_CAC_HAS_NOT_RUN = 0xa,
        };
        
        typedef struct sFlags {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            eReasonCode reason_code : 4;
            uint8_t preference : 4;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t preference : 4;
            eReasonCode reason_code : 4;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sFlags;
        
        uint8_t& operating_class();
        uint8_t& channel_list_length();
        uint8_t* channel_list(size_t idx = 0);
        bool alloc_channel_list(size_t count = 1);
        sFlags& flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_operating_class = nullptr;
        uint8_t* m_channel_list_length = nullptr;
        uint8_t* m_channel_list = nullptr;
        size_t m_channel_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sFlags* m_flags = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCHANNELPREFERENCE_H_
