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

#ifndef _TLVF_IEEE_1905_1_ELINKMETRICSTYPE_H_
#define _TLVF_IEEE_1905_1_ELINKMETRICSTYPE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

enum eLinkMetricsType: uint8_t {
    TX_LINK_METRICS_ONLY = 0x0,
    RX_LINK_METRICS_ONLY = 0x1,
    BOTH_TX_AND_RX_LINK_METRICS = 0x2,
};


}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_ELINKMETRICSTYPE_H_
