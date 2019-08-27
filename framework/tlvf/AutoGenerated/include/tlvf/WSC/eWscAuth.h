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

#ifndef _TLVF_WSC_EWSCAUTH_H_
#define _TLVF_WSC_EWSCAUTH_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum class eWscAuth : uint16_t {
    WSC_AUTH_OPEN = 0x1,
    WSC_AUTH_WPAPSK = 0x2,
    WSC_AUTH_SHARED = 0x4,
    WSC_AUTH_WPA = 0x8,
    WSC_AUTH_WPA2 = 0x10,
    WSC_AUTH_WPA2PSK = 0x20,
};
class eWscAuthValidate {
public:
    static bool check(uint16_t value) {
        bool ret = false;
        switch (value) {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
        case 0x10:
        case 0x20:
                ret = true;
                break;
            default:
                ret = false;
                break;
        }
        return ret;
    }
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCAUTH_H_
