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
        } __attribute__((packed)) sFlags;
        
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
