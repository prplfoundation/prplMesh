/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
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
