/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_ERR_H_
#define _BPL_ERR_H_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/* Beerocks Error Definitions */
#define BPL_ERROR_STRING_LEN 256 /* Maximal length of BPL error string */

// Helper MACROs for Enum/String generation
#define GENERATE_ERROR_ENUM(ENUM) ENUM,
#define GENERATE_ERROR_STRING(STRING) #STRING,

/* Beerocks Error Codes */

#define FOREACH_ERROR_CODE(ERROR_CODE)                                                             \
    ERROR_CODE(BPL_ERR_NONE)                                                                       \
    ERROR_CODE(BPL_ERR_UNKNOWN)                                                                    \
    ERROR_CODE(BPL_ERR_OPERATION_NOT_SUPPORTED)                                                    \
    ERROR_CODE(BPL_ERR_BH_READING_DATA_FROM_THE_BRIDGE)                                            \
    ERROR_CODE(BPL_ERR_BH_TIMEOUT_ATTACHING_TO_WPA_SUPPLICANT)                                     \
    ERROR_CODE(BPL_ERR_BH_SCAN_FAILED_TO_INITIATE_SCAN)                                            \
    ERROR_CODE(BPL_ERR_BH_SCAN_TIMEOUT)                                                            \
    ERROR_CODE(BPL_ERR_BH_FAILED_TO_RETRIEVE_SCAN_RESULTS)                                         \
    ERROR_CODE(BPL_ERR_BH_SCAN_EXCEEDED_MAXIMUM_FAILED_SCAN_ATTEMPTS)                              \
    ERROR_CODE(BPL_ERR_BH_OBTAINING_DHCP_BRIDGE_INTERFACE)                                         \
    ERROR_CODE(BPL_ERR_BH_OBTAINING_DHCP_BRIDGE_INTERFACE_MAX_ATTEMPTS)                            \
    ERROR_CODE(BPL_ERR_BH_CONNECTING_TO_MASTER)                                                    \
    ERROR_CODE(BPL_ERR_BH_CONNECTING_TO_MASTER_USING_UDS)                                          \
    ERROR_CODE(BPL_ERR_BH_DEVICE_QUERY_TIMEOUT)                                                    \
    ERROR_CODE(BPL_ERR_BH_ASSOCIATE_3ADDR)                                                         \
    ERROR_CODE(BPL_ERR_BH_ASSOCIATE_3ADDR_TIMEOUT)                                                 \
    ERROR_CODE(BPL_ERR_BH_ASSOCIATE_4ADDR_TIMEOUT)                                                 \
    ERROR_CODE(BPL_ERR_BH_ASSOCIATE_4ADDR_FAILURE)                                                 \
    ERROR_CODE(BPL_ERR_BH_TRANSITION_FROM_3ADDR_TO_4ADDR)                                          \
    ERROR_CODE(BPL_ERR_BH_ROAMING)                                                                 \
    ERROR_CODE(BPL_ERR_BH_DISCONNECTED)                                                            \
    ERROR_CODE(BPL_ERR_BH_WPA_SUPPLICANT_TERMINATED)                                               \
    ERROR_CODE(BPL_ERR_BH_MASTER_SOCKET_DISCONNECTED)                                              \
    ERROR_CODE(BPL_ERR_BH_SLAVE_SOCKET_DISCONNECTED)                                               \
    ERROR_CODE(BPL_ERR_BH_OPEN_UDP_SOCKET_FAILURE)                                                 \
    ERROR_CODE(BPL_ERR_BH_STOPPED)                                                                 \
    ERROR_CODE(BPL_ERR_SLAVE_CONNECTING_TO_BACKHAUL_MANAGER)                                       \
    ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_GET_WLAN_READY_STATUS_REQUEST)                                \
    ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_WIFI_CREDENTIALS_SET_REQUEST)                                 \
    ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_ENABLE_REQUEST)                                         \
    ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_DISABLE_REQUEST)                                        \
    ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_RESTORE_REQUEST)                                        \
    ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_RESTART_REQUEST)                                        \
    ERROR_CODE(BPL_ERR_SLAVE_MASTER_KEEP_ALIVE_TIMEOUT)                                            \
    ERROR_CODE(BPL_ERR_SLAVE_INVALID_MASTER_SOCKET)                                                \
    ERROR_CODE(BPL_ERR_SLAVE_IFACE_CHANGE_STATE_FAILED)                                            \
    ERROR_CODE(BPL_ERR_SLAVE_IFACE_RESTORE_FAILED)                                                 \
    ERROR_CODE(BPL_ERR_SLAVE_WIFI_CREDENTIALS_SET_FAILED)                                          \
    ERROR_CODE(BPL_ERR_SLAVE_POST_INIT_CONFIG_FAILED)                                              \
    ERROR_CODE(BPL_ERR_SLAVE_TX_CHANGE_STATE_FAILED)                                               \
    ERROR_CODE(BPL_ERR_SLAVE_UPDATE_CREDENTIALS_FAILED)                                            \
    ERROR_CODE(BPL_ERR_SLAVE_FAILED_CONNECT_TO_PLATFORM_MANAGER)                                   \
    ERROR_CODE(BPL_ERR_SLAVE_PLATFORM_MANAGER_REGISTER_TIMEOUT)                                    \
    ERROR_CODE(BPL_ERR_SLAVE_SLAVE_BACKHAUL_MANAGER_DISCONNECTED)                                  \
    ERROR_CODE(BPL_ERR_SLAVE_STOPPED)                                                              \
    ERROR_CODE(BPL_ERR_AP_MANAGER_START)                                                           \
    ERROR_CODE(BPL_ERR_AP_MANAGER_DISCONNECTED)                                                    \
    ERROR_CODE(BPL_ERR_AP_MANAGER_HOSTAP_DISABLED)                                                 \
    ERROR_CODE(BPL_ERR_AP_MANAGER_ATTACH_FAIL)                                                     \
    ERROR_CODE(BPL_ERR_AP_MANAGER_SUDDEN_DETACH)                                                   \
    ERROR_CODE(BPL_ERR_AP_MANAGER_HAL_DISCONNECTED)                                                \
    ERROR_CODE(BPL_ERR_AP_MANAGER_CAC_TIMEOUT)                                                     \
    ERROR_CODE(BPL_ERR_MONITOR_DISCONNECTED)                                                       \
    ERROR_CODE(BPL_ERR_MONITOR_HOSTAP_DISABLED)                                                    \
    ERROR_CODE(BPL_ERR_MONITOR_ATTACH_FAIL)                                                        \
    ERROR_CODE(BPL_ERR_MONITOR_SUDDEN_DETACH)                                                      \
    ERROR_CODE(BPL_ERR_MONITOR_HAL_DISCONNECTED)                                                   \
    ERROR_CODE(BPL_ERR_MONITOR_REPORT_PROCESS_FAIL)                                                \
    ERROR_CODE(BPL_ERR_CONFIG_PLATFORM_REPORTED_INVALID_CONFIGURATION)                             \
    ERROR_CODE(BPL_ERR_CONFIG_BACKHAUL_WIRED_INTERFACE_IS_UNSUPPORTED)                             \
    ERROR_CODE(BPL_ERR_CONFIG_BACKHAUL_WIRELESS_INTERFACE_IS_UNSUPPORTED)                          \
    ERROR_CODE(BPL_ERR_CONFIG_NO_VALID_BACKHAUL_INTERFACE)                                         \
    ERROR_CODE(BPL_ERR_CONFIG_BRIDGE_INTERFACE_IS_EMPTY)                                           \
    ERROR_CODE(BPL_ERR_WIFI_CONFIGURATION_CHANGE_TIMEOUT)                                          \
    ERROR_CODE(BPL_ERR_WATCHDOG_PROCESS_STUCK)                                                     \
    ERROR_CODE(BPL_ERR_WATCHDOG_PROCESS_ZOMBIE)                                                    \
    ERROR_CODE(BPL_ERR_LAST)

// States ENUM
enum eErrorCode { FOREACH_ERROR_CODE(GENERATE_ERROR_ENUM) };

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BPL_ERR_H_ */
