///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
*/

#ifndef _TLVF_IEEE_1905_1_TLVLINKMETRICQUERY_H_
#define _TLVF_IEEE_1905_1_TLVLINKMETRICQUERY_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddress.h"

namespace ieee1905_1 {


class tlvLinkMetricQuery : public BaseClass
{
    public:
        tlvLinkMetricQuery(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvLinkMetricQuery(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvLinkMetricQuery();

        enum eNeighborType: uint8_t {
            ALL_NEIGHBORS = 0x0,
            SPECIFIC_NEIGHBOR = 0x1,
        };
        
        enum eLinkMetricsType: uint8_t {
            TX_LINK_METRICS_ONLY = 0x0,
            RX_LINK_METRICS_ONLY = 0x1,
            BOTH_TX_AND_RX_LINK_METRICS = 0x2,
        };
        
        const eTlvType& type();
        const uint16_t& length();
        eNeighborType& neighbor_type();
        sMacAddress& mac_al_1905_device();
        eLinkMetricsType& link_metrics();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eNeighborType* m_neighbor_type = nullptr;
        sMacAddress* m_mac_al_1905_device = nullptr;
        eLinkMetricsType* m_link_metrics = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVLINKMETRICQUERY_H_
