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

#ifndef _TLVF_WFA_MAP_TLVTIMESTAMP_H_
#define _TLVF_WFA_MAP_TLVTIMESTAMP_H_

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


class tlvTimestamp : public BaseClass
{
    public:
        tlvTimestamp(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvTimestamp(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvTimestamp();

        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& timestamp_length();
        //The timestamp shall be formatted as a string using the typedef dateandtime string
        //format as defined in section 3 of [1] and shall include timesecfrac and time-offset
        //as defined in section 5.6 of [1]
        uint8_t* timestamp(size_t idx = 0);
        bool set_timestamp(const void* buffer, size_t size);
        bool alloc_timestamp(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_timestamp_length = nullptr;
        uint8_t* m_timestamp = nullptr;
        size_t m_timestamp_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVTIMESTAMP_H_
