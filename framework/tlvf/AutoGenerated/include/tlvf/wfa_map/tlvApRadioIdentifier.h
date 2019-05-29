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

#ifndef _TLVF_WFA_MAP_TLVAPRADIOIDENTIFIER_H_
#define _TLVF_WFA_MAP_TLVAPRADIOIDENTIFIER_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddress.h"

namespace wfa_map {


class tlvApRadioIdentifier : public BaseClass
{
    public:
        tlvApRadioIdentifier(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvApRadioIdentifier(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvApRadioIdentifier();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddress& radio_uid();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddress* m_radio_uid = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPRADIOIDENTIFIER_H_
