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

#ifndef _TLVF_WSC_EWSCRFBANDS_H_
#define _TLVF_WSC_EWSCRFBANDS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum eWscRfBands: uint8_t {
    WSC_RF_BAND_2GHZ = 0x1,
    WSC_RF_BAND_5GHZ = 0x2,
    WSC_RF_BAND_60GHZ = 0x4,
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCRFBANDS_H_
