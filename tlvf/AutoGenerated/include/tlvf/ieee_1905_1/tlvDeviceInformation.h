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

#ifndef _TLVF_IEEE_1905_1_TLVDEVICEINFORMATION_H_
#define _TLVF_IEEE_1905_1_TLVDEVICEINFORMATION_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddress.h"
#include <tuple>
#include "tlvf/ieee_1905_1/eMediaType.h"

namespace ieee1905_1 {


class tlvDeviceInformation : public BaseClass
{
    public:
        tlvDeviceInformation(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvDeviceInformation(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvDeviceInformation();

        typedef struct sInfo {
            sMacAddress mac;
            eMediaType media_type;
            uint8_t media_info_length;
            eMediaType media_info;
            void struct_swap(){
                mac.struct_swap();
            }
            void struct_init(){
                mac.struct_init();
            }
        } sInfo;
        
        const eTlvType& type();
        const uint16_t& length();
        sMacAddress& mac();
        uint8_t& info_length();
        std::tuple<bool, sInfo&> info(size_t idx);
        bool alloc_info(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddress* m_mac = nullptr;
        uint8_t* m_info_length = nullptr;
        sInfo* m_info = nullptr;
        size_t m_info_idx__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVDEVICEINFORMATION_H_
