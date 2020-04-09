///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WFA_MAP_TLVSEARCHEDSERVICE_H_
#define _TLVF_WFA_MAP_TLVSEARCHEDSERVICE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include <tuple>

namespace wfa_map {


class tlvSearchedService : public BaseClass
{
    public:
        tlvSearchedService(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvSearchedService(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvSearchedService();

        enum eSearchedService: uint8_t {
            MULTI_AP_CONTROLLER = 0x0,
        };
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& searched_service_list_length();
        std::tuple<bool, eSearchedService&> searched_service_list(size_t idx);
        bool alloc_searched_service_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_searched_service_list_length = nullptr;
        eSearchedService* m_searched_service_list = nullptr;
        size_t m_searched_service_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVSEARCHEDSERVICE_H_
