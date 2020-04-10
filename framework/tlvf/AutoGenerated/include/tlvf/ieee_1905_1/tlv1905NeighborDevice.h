///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_IEEE_1905_1_TLV1905NEIGHBORDEVICE_H_
#define _TLVF_IEEE_1905_1_TLV1905NEIGHBORDEVICE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>

namespace ieee1905_1 {


class tlv1905NeighborDevice : public BaseClass
{
    public:
        tlv1905NeighborDevice(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlv1905NeighborDevice(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlv1905NeighborDevice();

        enum eBridgesExist: uint8_t {
            NO_BRIDGES_EXIST = 0x0,
            AT_LEAST_ONE_BRIDGES_EXIST = 0x80,
        };
        
        typedef struct sMacAl1905Device {
            sMacAddr mac;
            eBridgesExist bridges_exist;
            void struct_swap(){
                mac.struct_swap();
                tlvf_swap(8*sizeof(eBridgesExist), reinterpret_cast<uint8_t*>(&bridges_exist));
            }
            void struct_init(){
                mac.struct_init();
            }
        } __attribute__((packed)) sMacAl1905Device;
        
        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& mac_local_iface();
        size_t mac_al_1905_device_length() { return m_mac_al_1905_device_idx__ * sizeof(tlv1905NeighborDevice::sMacAl1905Device); }
        std::tuple<bool, sMacAl1905Device&> mac_al_1905_device(size_t idx);
        bool alloc_mac_al_1905_device(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_mac_local_iface = nullptr;
        sMacAl1905Device* m_mac_al_1905_device = nullptr;
        size_t m_mac_al_1905_device_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLV1905NEIGHBORDEVICE_H_
