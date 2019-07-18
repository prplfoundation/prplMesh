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

#ifndef _TLVF_IEEE_1905_1_TLVMACADDRESS_H_
#define _TLVF_IEEE_1905_1_TLVMACADDRESS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <tuple>

namespace ieee1905_1 {


class tlvMacAddress : public BaseClass
{
    public:
        tlvMacAddress(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvMacAddress(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvMacAddress();

        const eTlvType& type();
        const uint16_t& length();
        std::tuple<bool, uint8_t&> mac(size_t idx);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_mac = nullptr;
        size_t m_mac_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVMACADDRESS_H_
