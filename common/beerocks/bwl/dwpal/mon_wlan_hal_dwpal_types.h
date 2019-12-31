/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_MON_WLAN_HAL_DWPAL_TYPES_H_
#define _BWL_MON_WLAN_HAL_DWPAL_TYPES_H_

namespace bwl {
namespace dwpal {

#define NL_MAX_REPLY_BUFFSIZE 8192
#define NL_ATTR_HDR 4

/**
 * @brief channel scan driver configuration parameters.
 * 
 * @param passive_dwell_time time to wait on the channel during passive scans, in ms.
 * @param active_dwell_time time to wait on the channel during active scans, in ms.
 * @param num_probe_reqs number of probe requests to send for the same SSID.
 * @param probe_reqs_interval time in ms, after which to fire the next round of probe requests for the same SSIDs.
 * @param passive_scan_valid_time avoid new non-background passive scans of the channel for this time period, in seconds.
 * @param active_scan_valid_time avoid new non-background active scans of the channel for this time period, in seconds.
 */
struct sScanCfgParams {
    int passive_dwell_time;
    int active_dwell_time;
    int num_probe_reqs;
    int probe_reqs_interval;
    int passive_scan_valid_time;
    int active_scan_valid_time;
};

/**
 * @brief channel scan driver background configuration parameters.
 * 
 * @param passive_dwell_time time to wait on the channel during passive scans, in ms.
 * @param active_dwell_time time to wait on the channel during active scans, in ms.
 * @param num_probe_reqs number of probe requests to send for the same SSID.
 * @param probe_reqs_interval time in ms, after which to fire the next round of probe requests for the same SSIDs.
 * @param num_chans_in_chunk number of channels in single scan chunk.
 * @param break_time time in ms, background scan break time duration for dfs channels.
 * @param break_time_busy busy flag for backround scan break.
 */
struct sScanCfgParamsBG {
    int passive_dwell_time;
    int active_dwell_time;
    int num_probe_reqs;
    int probe_reqs_interval;
    int num_chans_in_chunk;
    int break_time;
    int break_time_busy;
};

} // namespace dwpal
} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_DWPAL_TYPES_H_
