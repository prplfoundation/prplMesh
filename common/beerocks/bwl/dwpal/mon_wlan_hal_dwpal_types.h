/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
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

struct sScanCfgParams {
    int passive;
    int active;
    int num_probe_reqs;
    int probe_reqs_interval;
    int passive_scan_valid_time;
    int active_scan_valid_time;
};

struct sScanCfgParamsBG {
    int passive;
    int active;
    int num_probe_reqs;
    int probe_reqs_interval;
    int num_chans_in_chunk;
    int break_time;
    int break_time_busy;
};

} // namespace dwpal
} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_DWPAL_TYPES_H_
