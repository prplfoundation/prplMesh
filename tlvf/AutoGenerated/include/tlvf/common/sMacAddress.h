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

#ifndef _TLVF_COMMON_SMACADDRESS_H_
#define _TLVF_COMMON_SMACADDRESS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
typedef struct sMacAddress {
    uint8_t mac[6];
    void struct_swap() {}
    void struct_init()
    {
        for (size_t i = 0; i < 6; i++) {
            mac[i] = 0x0;
        }
    }
} sMacAddress;

#endif //_TLVF/COMMON_SMACADDRESS_H_
