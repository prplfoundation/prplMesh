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

#ifndef _TLVF_IEEE_1905_1_TLVRECEIVERLINKMETRIC_H_
#define _TLVF_IEEE_1905_1_TLVRECEIVERLINKMETRIC_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include "tlvf/ieee_1905_1/eMediaType.h"

namespace ieee1905_1 {


class tlvReceiverLinkMetric : public BaseClass
{
    public:
        tlvReceiverLinkMetric(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvReceiverLinkMetric(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvReceiverLinkMetric();

        typedef struct sLinkMetricInfo {
            //The underlying network technology
            eMediaType intfType;
            uint32_t packet_errors_received;
            //Number of packets received at the
            //interface during the same measurement
            //period used to count packetErrors.
            uint32_t packets_receiveds;
            //If the media type of the link is IEEE 802.11 (8 MSB value of media
            //type as defined in Table 6-12, then this value is the estimated RSSI in dB at the
            //receive side of the Link expressed in dB; otherwise, it is set to 0xFF.    
            uint8_t rssi_db;
            void struct_swap(){
                tlvf_swap(16, reinterpret_cast<uint8_t*>(&intfType));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&packet_errors_received));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&packets_receiveds));
            }
            void struct_init(){
                rssi_db = 0xff;
            }
        } __attribute__((packed)) sLinkMetricInfo;
        
        typedef struct sInterfacePairInfo {
            sMacAddr rc_interface_mac;
            sMacAddr neighbor_interface_mac;
            sLinkMetricInfo link_metric_info;
            void struct_swap(){
                rc_interface_mac.struct_swap();
                neighbor_interface_mac.struct_swap();
                link_metric_info.struct_swap();
            }
            void struct_init(){
                rc_interface_mac.struct_init();
                neighbor_interface_mac.struct_init();
                link_metric_info.struct_init();
            }
        } __attribute__((packed)) sInterfacePairInfo;
        
        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& reporter_al_mac();
        sMacAddr& neighbor_al_mac();
        //The following fields shall be repeated for each connected interface pair between the
        //receiving 1905.1 AL and the neighbor 1905.1 AL.
        size_t interface_pair_info_length() { return m_interface_pair_info_idx__ * sizeof(tlvReceiverLinkMetric::sInterfacePairInfo); }
        std::tuple<bool, sInterfacePairInfo&> interface_pair_info(size_t idx);
        bool alloc_interface_pair_info(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_reporter_al_mac = nullptr;
        sMacAddr* m_neighbor_al_mac = nullptr;
        sInterfacePairInfo* m_interface_pair_info = nullptr;
        size_t m_interface_pair_info_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVRECEIVERLINKMETRIC_H_
