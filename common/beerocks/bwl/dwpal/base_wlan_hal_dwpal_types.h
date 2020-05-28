/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_DWPAL_TYPES_H_
#define _BWL_BASE_WLAN_HAL_DWPAL_TYPES_H_

#include <stdint.h>

namespace bwl {
namespace dwpal {

#define NL_MAX_REPLY_BUFFSIZE 8192
#define NL_ATTR_HDR 4

/**
 * @brief Phy channel status.
 *
 * Data structure returned by driver via sub-command LTQ_NL80211_VENDOR_SUBCMD_GET_PHY_CHAN_STATUS
 * of NL80211_CMD_VENDOR command.
 */
struct sPhyChanStatus {
    int8_t noise;                /**< noise level of channel (dBm) */
    uint8_t ch_load;             /**< channel load [0..100%] */
    uint8_t ch_util;             /**< total channel utilization [0..100%] */
    uint8_t airtime;             /**< air time [0..100%] */
    uint32_t airtime_efficiency; /**< air time efficiency (bytes/s) */
};

} // namespace dwpal
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_DWPAL_TYPES_H_
