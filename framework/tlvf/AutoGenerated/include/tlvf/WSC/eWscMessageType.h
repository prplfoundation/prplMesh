///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_EWSCMESSAGETYPE_H_
#define _TLVF_WSC_EWSCMESSAGETYPE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum eWscMessageType: uint8_t {
    WSC_MSG_TYPE_M1 = 0x4,
    WSC_MSG_TYPE_M2 = 0x5,
    WSC_MSG_TYPE_INVALID = 0xff,
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCMESSAGETYPE_H_
