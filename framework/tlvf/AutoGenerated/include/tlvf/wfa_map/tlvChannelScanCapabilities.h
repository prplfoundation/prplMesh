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

#ifndef _TLVF_WFA_MAP_TLVCHANNELSCANCAPABILITIES_H_
#define _TLVF_WFA_MAP_TLVCHANNELSCANCAPABILITIES_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include <tuple>
#include <vector>
#include "tlvf/common/sMacAddr.h"
#include <asm/byteorder.h>

namespace wfa_map {

class cRadiosWithScanCapabilities;
class cOperatingClasses;

class tlvChannelScanCapabilities : public BaseClass
{
    public:
        tlvChannelScanCapabilities(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvChannelScanCapabilities(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvChannelScanCapabilities();

        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& radio_list_length();
        std::tuple<bool, cRadiosWithScanCapabilities&> radio_list(size_t idx);
        std::shared_ptr<cRadiosWithScanCapabilities> create_radio_list();
        bool add_radio_list(std::shared_ptr<cRadiosWithScanCapabilities> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_radio_list_length = nullptr;
        cRadiosWithScanCapabilities* m_radio_list = nullptr;
        size_t m_radio_list_idx__ = 0;
        std::vector<std::shared_ptr<cRadiosWithScanCapabilities>> m_radio_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cRadiosWithScanCapabilities : public BaseClass
{
    public:
        cRadiosWithScanCapabilities(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cRadiosWithScanCapabilities(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cRadiosWithScanCapabilities();

        typedef struct sCapabilities {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 5;
            uint8_t scan_impact : 2;
            uint8_t on_boot_only : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t on_boot_only : 1;
            uint8_t scan_impact : 2;
            uint8_t reserved : 5;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sCapabilities;
        
        sMacAddr& radio_uid();
        sCapabilities& capabilities();
        //The minimum interval in seconds between the start of two consecutive channel scans on this radio
        uint32_t& minimum_scan_interval();
        uint8_t& operating_classes_list_length();
        std::tuple<bool, cOperatingClasses&> operating_classes_list(size_t idx);
        std::shared_ptr<cOperatingClasses> create_operating_classes_list();
        bool add_operating_classes_list(std::shared_ptr<cOperatingClasses> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_radio_uid = nullptr;
        sCapabilities* m_capabilities = nullptr;
        uint32_t* m_minimum_scan_interval = nullptr;
        uint8_t* m_operating_classes_list_length = nullptr;
        cOperatingClasses* m_operating_classes_list = nullptr;
        size_t m_operating_classes_list_idx__ = 0;
        std::vector<std::shared_ptr<cOperatingClasses>> m_operating_classes_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cOperatingClasses : public BaseClass
{
    public:
        cOperatingClasses(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cOperatingClasses(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cOperatingClasses();

        uint8_t& operating_class();
        //Number of channels specified in the Channel List. k=0 indicates that the Multi-AP Agent
        //is capable of scanning on all channels in the Operating Class.
        uint8_t& channel_list_length();
        uint8_t* channel_list(size_t idx = 0);
        bool set_channel_list(const void* buffer, size_t size);
        bool alloc_channel_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_operating_class = nullptr;
        uint8_t* m_channel_list_length = nullptr;
        uint8_t* m_channel_list = nullptr;
        size_t m_channel_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCHANNELSCANCAPABILITIES_H_
