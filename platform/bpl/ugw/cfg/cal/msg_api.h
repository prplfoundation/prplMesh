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
bool sl_beerocks_notify_error(int code, const char* data);

// Send Wi-Fi credentials update to the beerocks SL
bool sl_beerocks_set_wifi_credentials(const int radio_dir, const char* ssid, const char* pass, const char* sec);

// Send Wi-Fi advertise SSID flag update to the beerocks SL
bool sl_beerocks_set_wifi_advertise_ssid(const char* iface, int advertise_ssid);

// Send Wi-Fi set iface state
bool sl_beerocks_set_wifi_iface_state(const char* iface, int op);

// Send Wi-Fi set radio tx state
bool sl_beerocks_set_wifi_radio_tx_state(const char* iface, int enable);

bool sl_beerocks_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif);

bool sl_beerocks_notify_onboarding_completed(const char*ssid, const char* pass,const char* sec, const char* iface_name, const int success);

} // namespace bpl
} // namespace beerocks
