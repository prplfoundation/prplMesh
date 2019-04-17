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

#ifndef _TLVF_IEEE_1905_1_CCMDUHEADER_H_
#define _TLVF_IEEE_1905_1_CCMDUHEADER_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eMessageType.h"
#include <asm/byteorder.h>

namespace ieee1905_1 {


class cCmduHeader : public BaseClass
{
    public:
        cCmduHeader(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cCmduHeader(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cCmduHeader();

        typedef struct sFlags {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 6;
            uint8_t relay_indicator : 1;
            uint8_t last_fragment_indicator : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t last_fragment_indicator : 1;
            uint8_t relay_indicator : 1;
            uint8_t reserved : 6;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
                last_fragment_indicator = 0x1;
                relay_indicator = 0x0;
                reserved = 0x0;
            }
        } sFlags;
        
        const uint8_t& message_version();
        const uint8_t& reserved();
        eMessageType& message_type();
        uint16_t& message_id();
        uint8_t& fragment_id();
        sFlags& flags();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_message_version = nullptr;
        uint8_t* m_reserved = nullptr;
        eMessageType* m_message_type = nullptr;
        uint16_t* m_message_id = nullptr;
        uint8_t* m_fragment_id = nullptr;
        sFlags* m_flags = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_CCMDUHEADER_H_
