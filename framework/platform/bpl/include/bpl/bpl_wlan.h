/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_WLAN_H_
#define _BPL_WLAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/******************************** Functions *********************************/
/****************************************************************************/

/**
 * Start hostapd on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or 1 if hostapd is already running on specified interface or -1 on otherwise
 */
int bpl_wlan_ap_start(const char *iface);

/**
 * Stop hostapd on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or 1 if hostapd is not already running on specified interface or -1 on otherwise
 */
int bpl_wlan_ap_stop(const char *iface);

/**
 * Start supplicant on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_start(const char *iface);

/**
 * Stop supplicant on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_stop(const char *iface);

/**
 * Explicit hostapd start operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ap_enable(const char *iface);

/**
 * Explicit hostapd stop operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ap_disable(const char *iface);

/**
 * Explicit supplicant start operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_enable(const char *iface);

/**
 * Explicit supplicant stop operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_disable(const char *iface);

/**
 * Identify if WLAN is initialized and ready for use
 *
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ready();

/**
 * TODO
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_acs(const char *iface);

/**
 * Restore WLAN configuration and state to last state.
 *
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_restore(const char *iface);

/**
 * Configure and bringup associated VAPs as post init handler of specified AP
 *
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ap_postinit(const char *iface);

#ifdef __cplusplus
}
#endif

#endif // _BPL_WLAN_H_
