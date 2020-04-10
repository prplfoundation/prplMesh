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

#ifndef _TLVF_WSC_EWSCSTATE_H_
#define _TLVF_WSC_EWSCSTATE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum eWscState: uint8_t {
    WSC_STATE_NOT_CONFIGURED = 0x1,
    WSC_STATE_CONFIGURED = 0x2,
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCSTATE_H_
