/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_TYPES_H_
#define _BWL_STA_WLAN_HAL_TYPES_H_

#include <stdint.h>

#include <bcl/network/net_struct.h>

namespace bwl {

// 16 Octets + 4 dashes + null terminator RFC 4122
#define UUID_STR_SIZE (16 * 2 + 4 + 4)

struct SScanResult {
    sMacAddr bssid;
    uint8_t channel;
    int8_t rssi;
};

} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_TYPES_H_
