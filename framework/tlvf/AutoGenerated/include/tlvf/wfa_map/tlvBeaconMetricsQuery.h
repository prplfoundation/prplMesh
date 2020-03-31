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

#ifndef _TLVF_WFA_MAP_TLVBEACONMETRICSQUERY_H_
#define _TLVF_WFA_MAP_TLVBEACONMETRICSQUERY_H_

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
#include <tlvf/tlvfutils.h>

namespace wfa_map {


class tlvBeaconMetricsQuery : public BaseClass
{
    public:
        tlvBeaconMetricsQuery(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvBeaconMetricsQuery(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvBeaconMetricsQuery();

        typedef struct sApChannelReports {
            uint8_t ap_channel_report_list_length;
            uint8_t operating_class_in_ap_channel_report;
            uint8_t* ap_channel_report_list; //TLVF_TODO: not supported yet
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sApChannelReports;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& associated_sta_mac();
        uint8_t& operating_class();
        uint8_t& channel_number();
        sMacAddr& bssid();
        uint8_t& reporting_detail_value();
        uint8_t& ssid_length();
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        uint8_t& ap_channel_reports_list_length();
        std::tuple<bool, sApChannelReports&> ap_channel_reports_list(size_t idx);
        bool alloc_ap_channel_reports_list(size_t count = 1);
        uint8_t& elemnt_id_list_length();
        uint8_t* elemnt_id_list(size_t idx = 0);
        bool set_elemnt_id_list(const void* buffer, size_t size);
        bool alloc_elemnt_id_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_associated_sta_mac = nullptr;
        uint8_t* m_operating_class = nullptr;
        uint8_t* m_channel_number = nullptr;
        sMacAddr* m_bssid = nullptr;
        uint8_t* m_reporting_detail_value = nullptr;
        uint8_t* m_ssid_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_ap_channel_reports_list_length = nullptr;
        sApChannelReports* m_ap_channel_reports_list = nullptr;
        size_t m_ap_channel_reports_list_idx__ = 0;
        uint8_t* m_elemnt_id_list_length = nullptr;
        uint8_t* m_elemnt_id_list = nullptr;
        size_t m_elemnt_id_list_idx__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVBEACONMETRICSQUERY_H_
