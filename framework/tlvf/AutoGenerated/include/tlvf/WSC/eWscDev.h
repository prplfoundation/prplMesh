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

#ifndef _TLVF_WSC_EWSCDEV_H_
#define _TLVF_WSC_EWSCDEV_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum eWscDev: uint16_t {
    WSC_DEV_NETWORK_INFRA_AP = 0x1,
    WSC_DEV_NETWORK_INFRA_ROUTER = 0x2,
    WSC_DEV_NETWORK_INFRA_SWITCH = 0x3,
    WSC_DEV_NETWORK_INFRA_GATEWAY = 0x4,
    WSC_DEV_NETWORK_INFRA_BRIDGE = 0x5,
    WSC_DEV_NETWORK_INFRA = 0x6,
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCDEV_H_
