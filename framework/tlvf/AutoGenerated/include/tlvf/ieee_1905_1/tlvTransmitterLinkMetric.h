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

#ifndef _TLVF_IEEE_1905_1_TLVTRANSMITTERLINKMETRIC_H_
#define _TLVF_IEEE_1905_1_TLVTRANSMITTERLINKMETRIC_H_

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


class tlvTransmitterLinkMetric : public BaseClass
{
    public:
        tlvTransmitterLinkMetric(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvTransmitterLinkMetric(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvTransmitterLinkMetric();

        enum eIEEE802_1BridgeFlag: uint8_t {
            LINK_DOES_NOT_INCLUDE_BRIDGE = 0x0,
            LINK_DOES_INCLUDE_ONE_OR_MORE_BRIDGE = 0x1,
        };
        
        typedef struct sLinkMetricInfo {
            //The underlying network technology
            eMediaType intfType;
            //Indicates whether or not the 1905.1 link includes one or more IEEE 802.1 bridges
            eIEEE802_1BridgeFlag IEEE802_1BridgeFlag;
            uint32_t packet_errors;
            //Estimated number of packets transmitted by the Transmitter of the link on the same measurement period used to estimate packetErrors.
            uint32_t transmitted_packets;
            //The maximum MAC throughput of the Link estimated at the transmitter and expressed in Mb/s.
            uint16_t mac_throughput_capacity;
            //The estimated average percentage of time that the link is available for data transmissions.
            uint16_t link_availability;
            //If the media type of the link is IEEE 802.3, then IEEE 1901 or MoCA 1.1 (8 MSB bits value of media type as defined in Table 6-12 is 0, 2, or 3).
            //This value is the PHY rate estimated at the transmitter of the link expressed in Mb/s; otherwise, it is set to 0xFFFF.
            uint16_t phy_rate;
            void struct_swap(){
                tlvf_swap(16, reinterpret_cast<uint8_t*>(&intfType));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&packet_errors));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&transmitted_packets));
                tlvf_swap(16, reinterpret_cast<uint8_t*>(&mac_throughput_capacity));
                tlvf_swap(16, reinterpret_cast<uint8_t*>(&link_availability));
                tlvf_swap(16, reinterpret_cast<uint8_t*>(&phy_rate));
            }
            void struct_init(){
                phy_rate = 0xffff;
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
        //The fields shall be repeated for each connected interface pair between the receiving
        //905.1 AL and the neighbor 1905.1 AL.
        size_t interface_pair_info_length() { return m_interface_pair_info_idx__ * sizeof(tlvTransmitterLinkMetric::sInterfacePairInfo); }
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

#endif //_TLVF/IEEE_1905_1_TLVTRANSMITTERLINKMETRIC_H_
