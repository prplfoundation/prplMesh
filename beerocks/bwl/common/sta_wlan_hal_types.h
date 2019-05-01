/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_TYPES_H_
#define _BWL_STA_WLAN_HAL_TYPES_H_

#include <stdint.h>

namespace bwl {

struct SScanResult {
    uint8_t bssid[6];
    uint8_t channel;
    int8_t rssi;
};

} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_TYPES_H_