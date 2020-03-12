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

#ifndef _TLVF_IEEE_1905_1_TLVASSOCIATEDSTALINKMETRICS_H_
#define _TLVF_IEEE_1905_1_TLVASSOCIATEDSTALINKMETRICS_H_

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


class tlvAssociatedStaLinkMetrics : public BaseClass
{
    public:
        tlvAssociatedStaLinkMetrics(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvAssociatedStaLinkMetrics(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvAssociatedStaLinkMetrics();

        typedef struct sPerBssStaLinkMetrics {
            sMacAddr bssid;
            uint32_t mac_data_rate_downlink;
            uint32_t mac_data_rate_uplink;
            uint8_t uplink_rcpi;
            void struct_swap(){
                bssid.struct_swap();
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&mac_data_rate_downlink));
                tlvf_swap(32, reinterpret_cast<uint8_t*>(&mac_data_rate_uplink));
            }
            void struct_init(){
                bssid.struct_init();
            }
        } __attribute__((packed)) sPerBssStaLinkMetrics;
        
        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& mac();
        uint8_t& bss_number();
        std::tuple<bool, sPerBssStaLinkMetrics&> per_bss_sta_link_metrics(size_t idx);
        bool alloc_per_bss_sta_link_metrics(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_mac = nullptr;
        uint8_t* m_bss_number = nullptr;
        sPerBssStaLinkMetrics* m_per_bss_sta_link_metrics = nullptr;
        size_t m_per_bss_sta_link_metrics_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVASSOCIATEDSTALINKMETRICS_H_
