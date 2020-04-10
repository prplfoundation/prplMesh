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

#ifndef _TLVF_WSC_EWSCVENDOREXT_H_
#define _TLVF_WSC_EWSCVENDOREXT_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum eWscVendorExtSubelementBssType {
    BACKHAUL_STA = 0x80,
    BACKHAUL_BSS = 0x40,
    FRONTHAUL_BSS = 0x20,
    TEARDOWN = 0x10,
};

enum eWscVendorExtVersionIE {
    WSC_VERSION2 = 0x20,
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCVENDOREXT_H_
