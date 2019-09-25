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

#ifndef _TLVF_WFA_MAP_TLVCLIENTASSOCIATIONCONTROLREQUEST_H_
#define _TLVF_WFA_MAP_TLVCLIENTASSOCIATIONCONTROLREQUEST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>

namespace wfa_map {


class tlvClientAssociationControlRequest : public BaseClass
{
    public:
        tlvClientAssociationControlRequest(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvClientAssociationControlRequest(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvClientAssociationControlRequest();

        enum eAssociationControl: uint8_t {
            BLOCK = 0x0,
            UNBLOCK = 0x1,
        };
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& bssid_to_block_client();
        eAssociationControl& association_control();
        uint16_t& validity_period_sec();
        uint8_t& sta_list_length();
        std::tuple<bool, sMacAddr&> sta_list(size_t idx);
        bool alloc_sta_list(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_bssid_to_block_client = nullptr;
        eAssociationControl* m_association_control = nullptr;
        uint16_t* m_validity_period_sec = nullptr;
        uint8_t* m_sta_list_length = nullptr;
        sMacAddr* m_sta_list = nullptr;
        size_t m_sta_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCLIENTASSOCIATIONCONTROLREQUEST_H_
