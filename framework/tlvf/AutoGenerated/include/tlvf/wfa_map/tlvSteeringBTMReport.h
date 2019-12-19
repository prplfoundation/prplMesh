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

#ifndef _TLVF_WFA_MAP_TLVSTEERINGBTMREPORT_H_
#define _TLVF_WFA_MAP_TLVSTEERINGBTMREPORT_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"

namespace wfa_map {


class tlvSteeringBTMReport : public BaseClass
{
    public:
        tlvSteeringBTMReport(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvSteeringBTMReport(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvSteeringBTMReport();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& bssid();
        sMacAddr& sta_mac();
        uint8_t& btm_status_code();
        sMacAddr& target_bssid();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        uint8_t* m_btm_status_code = nullptr;
        sMacAddr* m_target_bssid = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVSTEERINGBTMREPORT_H_
