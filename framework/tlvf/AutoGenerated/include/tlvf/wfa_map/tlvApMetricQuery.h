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

#ifndef _TLVF_WFA_MAP_TLVAPMETRICQUERY_H_
#define _TLVF_WFA_MAP_TLVAPMETRICQUERY_H_

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


class tlvApMetricQuery : public BaseClass
{
    public:
        tlvApMetricQuery(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvApMetricQuery(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvApMetricQuery();

        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& bssid_list_length();
        std::tuple<bool, sMacAddr&> bssid_list(size_t idx);
        bool alloc_bssid_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_bssid_list_length = nullptr;
        sMacAddr* m_bssid_list = nullptr;
        size_t m_bssid_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPMETRICQUERY_H_
