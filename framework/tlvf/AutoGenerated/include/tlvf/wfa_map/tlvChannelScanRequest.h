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

#ifndef _TLVF_WFA_MAP_TLVCHANNELSCANREQUEST_H_
#define _TLVF_WFA_MAP_TLVCHANNELSCANREQUEST_H_

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

namespace wfa_map {

class cRadiosToScan;

class tlvChannelScanRequest : public BaseClass
{
    public:
        tlvChannelScanRequest(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvChannelScanRequest(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvChannelScanRequest();

        enum ePerformFreshScan: uint8_t {
            PERFORM_A_FRESH_SCAN_AND_RETURN_RESULTS = 0x80,
            RETURN_STORED_RESULTS_OF_LAST_SUCCESSFUL_SCAN = 0x0,
        };
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        ePerformFreshScan& perform_fresh_scan();
        uint8_t& radio_list_length();
        std::tuple<bool, cRadiosToScan&> radio_list(size_t idx);
        std::shared_ptr<cRadiosToScan> create_radio_list();
        bool add_radio_list(std::shared_ptr<cRadiosToScan> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        ePerformFreshScan* m_perform_fresh_scan = nullptr;
        uint8_t* m_radio_list_length = nullptr;
        cRadiosToScan* m_radio_list = nullptr;
        size_t m_radio_list_idx__ = 0;
        std::vector<std::shared_ptr<cRadiosToScan>> m_radio_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cRadiosToScan : public BaseClass
{
    public:
        cRadiosToScan(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cRadiosToScan(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cRadiosToScan();

        typedef struct sOperatingClasses {
            uint8_t operating_class;
            //Number of channels specified in the Channel List. channel_list_length=0
            //indicates that the Multi-AP Agent is requested to scan on all channels
            //in the Operating Class
            uint8_t channel_list_length;
            uint8_t* channels_list; //TLVF_TODO: not supported yet
            void struct_swap(){
            }
            void struct_init(){
            }
        } __attribute__((packed)) sOperatingClasses;
        
        sMacAddr& radio_uid();
        uint8_t& operating_classes_list_length();
        std::tuple<bool, sOperatingClasses&> operating_classes_list(size_t idx);
        bool alloc_operating_classes_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_radio_uid = nullptr;
        uint8_t* m_operating_classes_list_length = nullptr;
        sOperatingClasses* m_operating_classes_list = nullptr;
        size_t m_operating_classes_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCHANNELSCANREQUEST_H_
