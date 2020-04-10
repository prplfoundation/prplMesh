///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_IEEE_1905_1_TLVLINKMETRICQUERY_H_
#define _TLVF_IEEE_1905_1_TLVLINKMETRICQUERY_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/ieee_1905_1/eLinkMetricNeighborType.h"
#include "tlvf/ieee_1905_1/eLinkMetricsType.h"
#include "tlvf/common/sMacAddr.h"

namespace ieee1905_1 {

class tlvLinkMetricQuery;

class tlvLinkMetricQueryAllNeighbors : public BaseClass
{
    public:
        tlvLinkMetricQueryAllNeighbors(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvLinkMetricQueryAllNeighbors(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvLinkMetricQueryAllNeighbors();

        const eTlvType& type();
        const uint16_t& length();
        const eLinkMetricNeighborType& neighbor_type();
        eLinkMetricsType& link_metrics_type();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eLinkMetricNeighborType* m_neighbor_type = nullptr;
        eLinkMetricsType* m_link_metrics_type = nullptr;
};

class tlvLinkMetricQuery : public BaseClass
{
    public:
        tlvLinkMetricQuery(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvLinkMetricQuery(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvLinkMetricQuery();

        const eTlvType& type();
        const uint16_t& length();
        eLinkMetricNeighborType& neighbor_type();
        sMacAddr& mac_al_1905_device();
        eLinkMetricsType& link_metrics_type();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eLinkMetricNeighborType* m_neighbor_type = nullptr;
        sMacAddr* m_mac_al_1905_device = nullptr;
        eLinkMetricsType* m_link_metrics_type = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVLINKMETRICQUERY_H_
