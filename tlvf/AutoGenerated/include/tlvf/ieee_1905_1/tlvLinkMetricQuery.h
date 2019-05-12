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

#ifndef _TLVF_IEEE_1905_1_TLVLINKMETRICQUERY_H_
#define _TLVF_IEEE_1905_1_TLVLINKMETRICQUERY_H_

#include "tlvf/common/sMacAddress.h"
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string.h>
#include <tlvf/BaseClass.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

class tlvLinkMetricQuery : public BaseClass {
public:
    tlvLinkMetricQuery(uint8_t *buff, size_t buff_len, bool parse = false,
                       bool swap_needed = false);
    tlvLinkMetricQuery(std::shared_ptr<BaseClass> base, bool parse = false,
                       bool swap_needed = false);
    ~tlvLinkMetricQuery();

    enum eNeighborType : uint8_t {
        ALL_NEIGHBORS     = 0x0,
        SPECIFIC_NEIGHBOR = 0x1,
    };

    enum eLinkMetricsType : uint8_t {
        TX_LINK_METRICS_ONLY        = 0x0,
        RX_LINK_METRICS_ONLY        = 0x1,
        BOTH_TX_AND_RX_LINK_METRICS = 0x2,
    };

    const eTlvType &type();
    const uint16_t &length();
    eNeighborType &neighbor_type();
    sMacAddress &mac_al_1905_device();
    eLinkMetricsType &link_metrics();
    void class_swap();
    static size_t get_initial_size();

private:
    bool init();
    eTlvType *m_type                  = nullptr;
    uint16_t *m_length                = nullptr;
    eNeighborType *m_neighbor_type    = nullptr;
    sMacAddress *m_mac_al_1905_device = nullptr;
    eLinkMetricsType *m_link_metrics  = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVLINKMETRICQUERY_H_
