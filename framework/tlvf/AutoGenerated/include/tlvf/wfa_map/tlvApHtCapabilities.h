///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WFA_MAP_TLVAPHTCAPABILITIES_H_
#define _TLVF_WFA_MAP_TLVAPHTCAPABILITIES_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <asm/byteorder.h>

namespace wfa_map {


class tlvApHtCapabilities : public BaseClass
{
    public:
        tlvApHtCapabilities(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvApHtCapabilities(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvApHtCapabilities();

        typedef struct sFalgs {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 1;
            uint8_t ht_support_40mhz : 1;
            uint8_t short_gi_support_40mhz : 1;
            uint8_t short_gi_support_20mhz : 1;
            uint8_t max_num_of_supported_rx_spatial_streams : 2;
            uint8_t max_num_of_supported_tx_spatial_streams : 2;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t max_num_of_supported_tx_spatial_streams : 2;
            uint8_t max_num_of_supported_rx_spatial_streams : 2;
            uint8_t short_gi_support_20mhz : 1;
            uint8_t short_gi_support_40mhz : 1;
            uint8_t ht_support_40mhz : 1;
            uint8_t reserved : 1;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sFalgs;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        sFalgs& flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        sFalgs* m_flags = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPHTCAPABILITIES_H_
