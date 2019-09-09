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

#ifndef _TLVF_WFA_MAP_TLVAPRADIOBASICCAPABILITIES_H_
#define _TLVF_WFA_MAP_TLVAPRADIOBASICCAPABILITIES_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include <vector>

namespace wfa_map {

class cOperatingClassesInfo;

class tlvApRadioBasicCapabilities : public BaseClass
{
    public:
        tlvApRadioBasicCapabilities(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvApRadioBasicCapabilities(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvApRadioBasicCapabilities();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        uint8_t& maximum_number_of_bsss_supported();
        uint8_t& operating_classes_info_list_length();
        std::tuple<bool, cOperatingClassesInfo&> operating_classes_info_list(size_t idx);
        std::shared_ptr<cOperatingClassesInfo> create_operating_classes_info_list();
        bool add_operating_classes_info_list(std::shared_ptr<cOperatingClassesInfo> ptr);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        uint8_t* m_maximum_number_of_bsss_supported = nullptr;
        uint8_t* m_operating_classes_info_list_length = nullptr;
        cOperatingClassesInfo* m_operating_classes_info_list = nullptr;
        size_t m_operating_classes_info_list_idx__ = 0;
        std::vector<std::shared_ptr<cOperatingClassesInfo>> m_operating_classes_info_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cOperatingClassesInfo : public BaseClass
{
    public:
        cOperatingClassesInfo(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cOperatingClassesInfo(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cOperatingClassesInfo();

        uint8_t& operating_class();
        uint8_t& maximum_transmit_power_dbm();
        uint8_t& statically_non_operable_channels_list_length();
        uint8_t* statically_non_operable_channels_list(size_t idx = 0);
        bool alloc_statically_non_operable_channels_list(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_operating_class = nullptr;
        uint8_t* m_maximum_transmit_power_dbm = nullptr;
        uint8_t* m_statically_non_operable_channels_list_length = nullptr;
        uint8_t* m_statically_non_operable_channels_list = nullptr;
        size_t m_statically_non_operable_channels_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPRADIOBASICCAPABILITIES_H_
