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

#ifndef _TLVF_IEEE_1905_1_TLV1905NEIGHBORDEVICE_H_
#define _TLVF_IEEE_1905_1_TLV1905NEIGHBORDEVICE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddress.h"
#include <tuple>

namespace ieee1905_1 {


class tlv1905NeighborDevice : public BaseClass
{
    public:
        tlv1905NeighborDevice(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlv1905NeighborDevice(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlv1905NeighborDevice();

        enum eBridgesExist: uint8_t {
            NO_BRIDGES_EXIST = 0x0,
            AT_LEAST_ONE_BRIDGES_EXIST = 0x80,
        };
        
        typedef struct sMacAl1905Device {
            sMacAddress mac;
            eBridgesExist bridges_exist;
            void struct_swap(){
                mac.struct_swap();
            }
            void struct_init(){
                mac.struct_init();
            }
        } sMacAl1905Device;
        
        const eTlvType& type();
        const uint16_t& length();
        sMacAddress& mac_local_iface();
        std::tuple<bool, sMacAl1905Device&> mac_al_1905_device(size_t idx);
        bool alloc_mac_al_1905_device(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddress* m_mac_local_iface = nullptr;
        sMacAl1905Device* m_mac_al_1905_device = nullptr;
        size_t m_mac_al_1905_device_idx__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLV1905NEIGHBORDEVICE_H_
