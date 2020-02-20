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

#ifndef _TLVF_WSC_EWSCENCR_H_
#define _TLVF_WSC_EWSCENCR_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum class eWscEncr : uint16_t {
    WSC_ENCR_NONE = 0x1,
    WSC_ENCR_WEP = 0x2,
    WSC_ENCR_TKIP = 0x4,
    WSC_ENCR_AES = 0x8,
    WSC_ENCR_INVALID = 0xffff,
};
class eWscEncrValidate {
public:
    static bool check(uint16_t value) {
        bool ret = false;
        switch (value) {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
        case 0xffff:
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

#endif //_TLVF/WSC_EWSCENCR_H_
