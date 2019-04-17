///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
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
