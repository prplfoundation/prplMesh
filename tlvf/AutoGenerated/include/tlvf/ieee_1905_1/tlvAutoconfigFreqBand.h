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

#ifndef _TLVF_IEEE_1905_1_TLVAUTOCONFIGFREQBAND_H_
#define _TLVF_IEEE_1905_1_TLVAUTOCONFIGFREQBAND_H_

#include "tlvf/ieee_1905_1/eTlvType.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string.h>
#include <tlvf/BaseClass.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

class tlvAutoconfigFreqBand : public BaseClass {
public:
    tlvAutoconfigFreqBand(uint8_t *buff, size_t buff_len, bool parse = false,
                          bool swap_needed = false);
    tlvAutoconfigFreqBand(std::shared_ptr<BaseClass> base, bool parse = false,
                          bool swap_needed = false);
    ~tlvAutoconfigFreqBand();

    enum eValue : uint8_t {
        IEEE_802_11_2_4_GHZ = 0x0,
        IEEE_802_11_5_GHZ   = 0x1,
        IEEE_802_11_60_GHZ  = 0x2,
    };

    const eTlvType &type();
    const uint16_t &length();
    eValue &value();
    void class_swap();
    static size_t get_initial_size();

private:
    bool init();
    eTlvType *m_type   = nullptr;
    uint16_t *m_length = nullptr;
    eValue *m_value    = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVAUTOCONFIGFREQBAND_H_
