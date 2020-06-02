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

#ifndef _TLVF_WFA_MAP_TLVCHANNELSCANREPORTINGPOLICY_H_
#define _TLVF_WFA_MAP_TLVCHANNELSCANREPORTINGPOLICY_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"

namespace wfa_map {


class tlvChannelScanReportingPolicy : public BaseClass
{
    public:
        tlvChannelScanReportingPolicy(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvChannelScanReportingPolicy(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvChannelScanReportingPolicy();

        enum eReportIndependentChannelScan: uint8_t {
            REPORT_INDEPENDENT_CHANNEL_SCANS = 0x80,
            DO_NOT_REPORT_INDEPENDENT_CHANNEL_SCANS = 0x0,
        };
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        eReportIndependentChannelScan& report_independent_channel_scans();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eReportIndependentChannelScan* m_report_independent_channel_scans = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCHANNELSCANREPORTINGPOLICY_H_
