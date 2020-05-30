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

#ifndef _TLVF_WFA_MAP_TLVMETRICREPORTINGPOLICY_H_
#define _TLVF_WFA_MAP_TLVMETRICREPORTINGPOLICY_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include <tuple>
#include <asm/byteorder.h>
#include "tlvf/common/sMacAddr.h"

namespace wfa_map {


class tlvMetricReportingPolicy : public BaseClass
{
    public:
        tlvMetricReportingPolicy(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvMetricReportingPolicy(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvMetricReportingPolicy();

        typedef struct sPolicy {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 6;
            uint8_t include_associated_sta_link_metrics_tlv_in_ap_metrics_response : 1;
            uint8_t include_associated_sta_traffic_stats_tlv_in_ap_metrics_response : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t include_associated_sta_traffic_stats_tlv_in_ap_metrics_response : 1;
            uint8_t include_associated_sta_link_metrics_tlv_in_ap_metrics_response : 1;
            uint8_t reserved : 6;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sPolicy;
        
        typedef struct sMetricsReportingConf {
            sMacAddr radio_uid;
            uint8_t sta_metrics_reporting_rcpi_threshold;
            uint8_t sta_metrics_reporting_rcpi_hysteresis_margin_override;
            uint8_t ap_channel_utilization_reporting_threshold;
            sPolicy policy;
            void struct_swap(){
                radio_uid.struct_swap();
                policy.struct_swap();
            }
            void struct_init(){
                radio_uid.struct_init();
                policy.struct_init();
            }
        } __attribute__((packed)) sMetricsReportingConf;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        //0 - Do not report AP Metric periodically
        //1-255 - AP Metrics reporting interval in seconds
        uint8_t& metrics_reporting_interval_sec();
        uint8_t& metrics_reporting_conf_list_length();
        std::tuple<bool, sMetricsReportingConf&> metrics_reporting_conf_list(size_t idx);
        bool alloc_metrics_reporting_conf_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_metrics_reporting_interval_sec = nullptr;
        uint8_t* m_metrics_reporting_conf_list_length = nullptr;
        sMetricsReportingConf* m_metrics_reporting_conf_list = nullptr;
        size_t m_metrics_reporting_conf_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVMETRICREPORTINGPOLICY_H_
