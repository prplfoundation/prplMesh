/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

namespace beerocks {
namespace bpl {

// Notify the beerocks SL about a firmware version mismatch
bool sl_beerocks_notify_fw_mismatch();

// Notify the beerocks SL about a beerocks error
bool sl_beerocks_notify_error(int code, const char *data);

bool sl_beerocks_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif);

bool sl_beerocks_notify_onboarding_completed(const char *ssid, const char *pass, const char *sec,
                                             const char *iface_name, const int success);

} // namespace bpl
} // namespace beerocks
