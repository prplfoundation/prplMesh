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

#ifndef _TLVF_IEEE_1905_1_TLVDEVICEBRIDGINGCAPABILITY_H_
#define _TLVF_IEEE_1905_1_TLVDEVICEBRIDGINGCAPABILITY_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <tuple>
#include <vector>
#include "tlvf/common/sMacAddress.h"

namespace ieee1905_1 {

class cMacList;

class tlvDeviceBridgingCapability : public BaseClass
{
    public:
        tlvDeviceBridgingCapability(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvDeviceBridgingCapability(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvDeviceBridgingCapability();

        const eTlvType& type();
        const uint16_t& length();
        uint8_t& bridging_tuples_list_length();
        std::tuple<bool, cMacList&> bridging_tuples_list(size_t idx);
        std::shared_ptr<cMacList> create_bridging_tuples_list();
        bool add_bridging_tuples_list(std::shared_ptr<cMacList> ptr);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_bridging_tuples_list_length = nullptr;
        cMacList* m_bridging_tuples_list = nullptr;
        size_t m_bridging_tuples_list_idx__ = 0;
        std::vector<std::shared_ptr<cMacList>> m_bridging_tuples_list_vector;
        bool m_lock_allocation__ = false;
};

class cMacList : public BaseClass
{
    public:
        cMacList(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cMacList(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cMacList();

        uint8_t& mac_list_length();
        std::tuple<bool, sMacAddress&> mac_list(size_t idx);
        bool alloc_mac_list(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_mac_list_length = nullptr;
        sMacAddress* m_mac_list = nullptr;
        size_t m_mac_list_idx__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVDEVICEBRIDGINGCAPABILITY_H_
