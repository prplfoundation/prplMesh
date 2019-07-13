///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////
/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SVENDOR_OUI_H_
#define _SVENDOR_OUI_H_

#include <stdint.h>
#include <algorithm>

typedef struct sVendorOUI {
    uint8_t upper;
    uint8_t middle;
    uint8_t lower;
    sVendorOUI(uint32_t val)
    {
        lower = val & 0xFF;
        middle = (val >> 8 ) & 0xFF;
        upper = (val >> 16 ) & 0xFF;
    }

    operator uint32_t() const
    {
        return (upper << 16) + (middle << 8) + lower;
    }

    void struct_swap()
    {
        std::swap(upper, lower);
    }

    void struct_init() { }
} __attribute__((packed)) sVendorOUI;


#endif //_SVENDOR_OUI_H_
