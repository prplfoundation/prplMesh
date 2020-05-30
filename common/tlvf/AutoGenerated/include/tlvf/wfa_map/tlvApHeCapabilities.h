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

#ifndef _TLVF_WFA_MAP_TLVAPHECAPABILITIES_H_
#define _TLVF_WFA_MAP_TLVAPHECAPABILITIES_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include <asm/byteorder.h>

namespace wfa_map {


class tlvApHeCapabilities : public BaseClass
{
    public:
        tlvApHeCapabilities(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvApHeCapabilities(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvApHeCapabilities();

        typedef struct sFlags1 {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t he_support_160mhz : 1;
            uint8_t he_support_80_80mhz : 1;
            uint8_t max_num_of_supported_rx_spatial_streams : 3;
            uint8_t max_num_of_supported_tx_spatial_streams : 3;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t max_num_of_supported_tx_spatial_streams : 3;
            uint8_t max_num_of_supported_rx_spatial_streams : 3;
            uint8_t he_support_80_80mhz : 1;
            uint8_t he_support_160mhz : 1;
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
            uint8_t reserved : 1;
            uint8_t dl_ofdm_capable : 1;
            uint8_t ul_ofdm_capable : 1;
            uint8_t dl_mu_mimo_and_ofdm_capable : 1;
            uint8_t ul_mu_mimo_and_ofdm_capable : 1;
            uint8_t ul_mu_mimo_capable : 1;
            uint8_t mu_beamformer_capable : 1;
            uint8_t su_beamformer_capable : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t su_beamformer_capable : 1;
            uint8_t mu_beamformer_capable : 1;
            uint8_t ul_mu_mimo_capable : 1;
            uint8_t ul_mu_mimo_and_ofdm_capable : 1;
            uint8_t dl_mu_mimo_and_ofdm_capable : 1;
            uint8_t ul_ofdm_capable : 1;
            uint8_t dl_ofdm_capable : 1;
            uint8_t reserved : 1;
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
        uint8_t& supported_he_mcs_length();
        uint8_t* supported_he_mcs(size_t idx = 0);
        bool set_supported_he_mcs(const void* buffer, size_t size);
        bool alloc_supported_he_mcs(size_t count = 1);
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
        uint8_t* m_supported_he_mcs_length = nullptr;
        uint8_t* m_supported_he_mcs = nullptr;
        size_t m_supported_he_mcs_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sFlags1* m_flags1 = nullptr;
        sFlags2* m_flags2 = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPHECAPABILITIES_H_
