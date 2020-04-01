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

#ifndef _TLVF_WFA_MAP_TLVASSOCIATEDSTALINKMETRICS_H_
#define _TLVF_WFA_MAP_TLVASSOCIATEDSTALINKMETRICS_H_

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

namespace wfa_map {


class tlvAssociatedStaLinkMetrics : public BaseClass
{
    public:
        tlvAssociatedStaLinkMetrics(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvAssociatedStaLinkMetrics(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvAssociatedStaLinkMetrics();

        typedef struct sBssidInfo {
            sMacAddr bssid;
            //The time delta in ms between the time at which the earliest
            //measurement that contributed to the data rate estimates were made,
            //and the time at which this report was sent
            uint32_t earliest_measurement_delta;
            uint32_t downlink_estimated_mac_data_rate_mbps;
            uint32_t uplink_estimated_mac_data_rate_mbps;
            uint8_t sta_measured_uplink_rssi_dbm_enc;
            void struct_swap(){
                bssid.struct_swap();
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&earliest_measurement_delta));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&downlink_estimated_mac_data_rate_mbps));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&uplink_estimated_mac_data_rate_mbps));
            }
            void struct_init(){
                bssid.struct_init();
            }
        } __attribute__((packed)) sBssidInfo;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& sta_mac();
        uint8_t& bssid_info_list_length();
        std::tuple<bool, sBssidInfo&> bssid_info_list(size_t idx);
        bool alloc_bssid_info_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        uint8_t* m_bssid_info_list_length = nullptr;
        sBssidInfo* m_bssid_info_list = nullptr;
        size_t m_bssid_info_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVASSOCIATEDSTALINKMETRICS_H_
