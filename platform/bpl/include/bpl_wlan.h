/*
 * INTEL CONFIDENTIAL
 * Copyright 2018 Intel Corporation All Rights Reserved.
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
int bpl_wlan_ap_start(const char* iface);

/**
 * Stop hostapd on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or 1 if hostapd is not already running on specified interface or -1 on otherwise
 */
int bpl_wlan_ap_stop(const char* iface);

/**
 * Start supplicant on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_start(const char* iface);

/**
 * Stop supplicant on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_stop(const char* iface);

/**
 * Explicit hostapd start operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ap_enable(const char* iface);

/**
 * Explicit hostapd stop operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ap_disable(const char* iface);

/**
 * Explicit supplicant start operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_enable(const char* iface);

/**
 * Explicit supplicant stop operation on specified interface
 *
 * @param [in] iface WLAN interface name
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_sta_disable(const char* iface);

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
int bpl_wlan_sta_acs(const char* iface);

/**
 * Restore WLAN configuration and state to last state.
 *
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_restore(const char* iface);

/**
 * Configure and bringup associated VAPs as post init handler of specified AP
 *
 * @return 0 on success or -1 on failure
 */
int bpl_wlan_ap_postinit(const char* iface);

#ifdef __cplusplus
}
#endif

#endif // _BPL_WLAN_H_
