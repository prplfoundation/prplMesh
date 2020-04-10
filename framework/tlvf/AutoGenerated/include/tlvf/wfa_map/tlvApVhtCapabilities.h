///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WFA_MAP_TLVAPVHTCAPABILITIES_H_
#define _TLVF_WFA_MAP_TLVAPVHTCAPABILITIES_H_

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


class tlvApVhtCapabilities : public BaseClass
{
    public:
        tlvApVhtCapabilities(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvApVhtCapabilities(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvApVhtCapabilities();

        typedef struct sFlags1 {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t short_gi_support_160mhz_and_80_80mhz : 1;
            uint8_t short_gi_support_80mhz : 1;
            uint8_t max_num_of_supported_rx_spatial_streams : 3;
            uint8_t max_num_of_supported_tx_spatial_streams : 3;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t max_num_of_supported_tx_spatial_streams : 3;
            uint8_t max_num_of_supported_rx_spatial_streams : 3;
            uint8_t short_gi_support_80mhz : 1;
            uint8_t short_gi_support_160mhz_and_80_80mhz : 1;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sFlags1;
        
        typedef struct sFlags2 {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 4;
            uint8_t mu_beamformer_capable : 1;
            uint8_t su_beamformer_capable : 1;
            uint8_t vht_support_160mhz : 1;
            uint8_t vht_support_80_80mhz : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t vht_support_80_80mhz : 1;
            uint8_t vht_support_160mhz : 1;
            uint8_t su_beamformer_capable : 1;
            uint8_t mu_beamformer_capable : 1;
            uint8_t reserved : 4;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
                reserved = 0x0;
            }
        } __attribute__((packed)) sFlags2;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        uint16_t& supported_vht_tx_mcs();
        uint16_t& supported_vht_rx_mcs();
        sFlags1& flags1();
        sFlags2& flags2();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        uint16_t* m_supported_vht_tx_mcs = nullptr;
        uint16_t* m_supported_vht_rx_mcs = nullptr;
        sFlags1* m_flags1 = nullptr;
        sFlags2* m_flags2 = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPVHTCAPABILITIES_H_
