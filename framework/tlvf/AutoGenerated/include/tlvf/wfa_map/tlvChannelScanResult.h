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

#ifndef _TLVF_WFA_MAP_TLVCHANNELSCANRESULT_H_
#define _TLVF_WFA_MAP_TLVCHANNELSCANRESULT_H_

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


class tlvChannelScanResult : public BaseClass
{
    public:
        tlvChannelScanResult(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvChannelScanResult(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvChannelScanResult();

        enum eBssLoadElementPresent: uint8_t {
            FIELD_PRESENT = 0x80,
            FIELD_NOT_PRESENT = 0x0,
        };
        
        enum eScanStatus {
            SUCCESS = 0x0,
            SCAN_NOT_SUPPORTED_ON_THIS_OPERATING_CLASS_AND_CHANNEL_ON_THIS_RADIO = 0x1,
            REQUEST_TOO_SOON_AFTER_LAST_SCAN = 0x2,
            RADIO_TOO_BUSY_TO_PERFORM_SCAN = 0x3,
            SCAN_NOT_COMPLETED = 0x4,
            SCAN_ABORTED = 0x5,
            FRESH_SCAN_NOT_SUPPORTED_RADIO_ONLY_SUPPORTS_ON_BOOT_SCANS = 0x6,
        };
        
        enum eScanType: uint8_t {
            SCAN_WAS_ACTIVE_SCAN = 0x80,
            SCAN_WAS_PASSIVE_SCAN = 0x0,
        };
        
        typedef struct sNeighbors {
            sMacAddr bssid;
            uint8_t ssid_length;
            //The SSID indicated by the neighboring BSS
            uint8_t* ssid; //TLVF_TODO: not supported yet
            //An indicator of radio signal strength (RSSI) of the Beacon or Probe
            //Response frames of the neighboring BSS as received by the radio
            //measured in dBm
            uint8_t signal_strength;
            uint8_t channel_bw_length;
            //String indicating the maximum bandwidth at which the neighbor BSS is
            //operating, e.g., "20" or "40" or "80" or "80+80" or "160" MHz.
            uint8_t* channels_bw_list; //TLVF_TODO: not supported yet
            eBssLoadElementPresent bss_load_element_present;
            //If "BSS Load Element Present" bit is set to one, this field is present.
            //Otherwise it is omitted.
            //The value of the "Channel Utilization" field as reported by the neighboring
            //BSS in the BSS Load element
            uint8_t channel_utilization;
            //If "BSS Load Element Present" bit is set to one, this field is present.
            //Otherwise it is omitted.
            //The value of the "Channel Utilization" field as reported by the neighboring
            //BSS in the BSS Load element
            uint16_t station_count;
            void struct_swap(){
                bssid.struct_swap();
                tlvf_swap(8*sizeof(eBssLoadElementPresent), reinterpret_cast<uint8_t*>(&bss_load_element_present));
                tlvf_swap(16, reinterpret_cast<uint8_t*>(&station_count));
            }
            void struct_init(){
                bssid.struct_init();
            }
        } __attribute__((packed)) sNeighbors;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        uint8_t& operating_class();
        uint8_t& channel();
        eScanStatus& success();
        uint8_t& timestamp_length();
        //The timestamp shall be formatted as a string using the typedef dateandtime string
        //format as defined in section 3 of [1] and shall include timesecfrac and time-offset
        //as defined in section 5.6 of [1]
        uint8_t* timestamp(size_t idx = 0);
        bool set_timestamp(const void* buffer, size_t size);
        bool alloc_timestamp(size_t count = 1);
        //The current channel utilization measured by the radio on the scanned 20 MHz channel
        uint8_t& utilization();
        //An indicator of the average radio noise plus interference power measured
        //on the 20 MHz channel during a channel scan.
        uint8_t& noise();
        uint16_t& neighbors_list_length();
        std::tuple<bool, sNeighbors&> neighbors_list(size_t idx);
        bool alloc_neighbors_list(size_t count = 1);
        //Total time spent performing the scan of this channel in milliseconds
        uint32_t& aggregate_scan_duration();
        eScanType& scan_type();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        uint8_t* m_operating_class = nullptr;
        uint8_t* m_channel = nullptr;
        eScanStatus* m_success = nullptr;
        uint8_t* m_timestamp_length = nullptr;
        uint8_t* m_timestamp = nullptr;
        size_t m_timestamp_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_utilization = nullptr;
        uint8_t* m_noise = nullptr;
        uint16_t* m_neighbors_list_length = nullptr;
        sNeighbors* m_neighbors_list = nullptr;
        size_t m_neighbors_list_idx__ = 0;
        uint32_t* m_aggregate_scan_duration = nullptr;
        eScanType* m_scan_type = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCHANNELSCANRESULT_H_
