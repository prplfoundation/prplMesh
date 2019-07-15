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

#ifndef _TLVF_WFA_MAP_TLVOPERATINGCHANNELREPORT_H_
#define _TLVF_WFA_MAP_TLVOPERATINGCHANNELREPORT_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>

namespace wfa_map {


class tlvOperatingChannelReport : public BaseClass
{
    public:
        tlvOperatingChannelReport(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvOperatingChannelReport(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvOperatingChannelReport();

        typedef struct sOperatingClasses {
            uint8_t operating_class;
            uint8_t channel_number;
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sOperatingClasses;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        uint8_t& operating_classes_list_length();
        std::tuple<bool, sOperatingClasses&> operating_classes_list(size_t idx);
        bool alloc_operating_classes_list(size_t count = 1);
        //Current Transmit Power EIRP representing the current nominal transmit power.
        //The field is coded as a 2's complement signed integer in units of decibels relative to 1 mW (dBm).
        //This value is less than or equal to the Maximum Transmit Power specified in the AP Radio Basic Capabilities TLV for the current operating class.    
        int8_t& current_transmit_power();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        uint8_t* m_operating_classes_list_length = nullptr;
        sOperatingClasses* m_operating_classes_list = nullptr;
        size_t m_operating_classes_list_idx__ = 0;
        int8_t* m_current_transmit_power = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVOPERATINGCHANNELREPORT_H_
