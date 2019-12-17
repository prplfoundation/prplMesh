/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_ERR_H_
#define _BPL_ERR_H_

namespace bpl {

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/* Beerocks Error Definitions */
#define BPL_ERROR_STRING_LEN 256 /* Maximal length of BPL error string */

// Helper MACROs for Enum/String generation
#define GENERATE_ERROR_ENUM(ENUM) ENUM,
#define GENERATE_ERROR_STRING(STRING) "BPL_ERR_" #STRING,

/* Beerocks Error Codes */

// clang-format off

#define FOREACH_ERROR_CODE(ERROR_CODE)                                                              \
/* 00 */ ERROR_CODE(NONE)                                                                   \
/* 01 */ ERROR_CODE(UNKNOWN)                                                                \
/* 02 */ ERROR_CODE(OPERATION_NOT_SUPPORTED)                                                \
/* 03 */ ERROR_CODE(BH_READING_DATA_FROM_THE_BRIDGE)                                        \
/* 04 */ ERROR_CODE(BH_TIMEOUT_ATTACHING_TO_WPA_SUPPLICANT)                                 \
/* 05 */ ERROR_CODE(BH_SCAN_FAILED_TO_INITIATE_SCAN)                                        \
/* 06 */ ERROR_CODE(BH_SCAN_TIMEOUT)                                                        \
/* 07 */ ERROR_CODE(BH_SCAN_EXCEEDED_MAXIMUM_FAILED_SCAN_ATTEMPTS)                          \
/* 08 */ ERROR_CODE(BH_CONNECTING_TO_MASTER)                                                \
/* 09 */ ERROR_CODE(BH_ASSOCIATE_4ADDR_TIMEOUT)                                             \
/* 10 */ ERROR_CODE(BH_ASSOCIATE_4ADDR_FAILURE)                                             \
/* 11 */ ERROR_CODE(BH_ROAMING)                                                             \
/* 12 */ ERROR_CODE(BH_DISCONNECTED)                                                        \
/* 13 */ ERROR_CODE(BH_WPA_SUPPLICANT_TERMINATED)                                           \
/* 14 */ ERROR_CODE(BH_SLAVE_SOCKET_DISCONNECTED)                                           \
/* 15 */ ERROR_CODE(BH_STOPPED)                                                             \
/* 16 */ ERROR_CODE(SLAVE_CONNECTING_TO_BACKHAUL_MANAGER)                                   \
/* 17 */ ERROR_CODE(SLAVE_MASTER_KEEP_ALIVE_TIMEOUT)                                        \
/* 18 */ ERROR_CODE(SLAVE_INVALID_MASTER_SOCKET)                                            \
/* 19 */ ERROR_CODE(SLAVE_FAILED_CONNECT_TO_PLATFORM_MANAGER)                               \
/* 20 */ ERROR_CODE(SLAVE_PLATFORM_MANAGER_REGISTER_TIMEOUT)                                \
/* 21 */ ERROR_CODE(SLAVE_SLAVE_BACKHAUL_MANAGER_DISCONNECTED)                              \
/* 22 */ ERROR_CODE(SLAVE_STOPPED)                                                          \
/* 23 */ ERROR_CODE(AP_MANAGER_START)                                                       \
/* 24 */ ERROR_CODE(AP_MANAGER_DISCONNECTED)                                                \
/* 25 */ ERROR_CODE(AP_MANAGER_HOSTAP_DISABLED)                                             \
/* 26 */ ERROR_CODE(AP_MANAGER_ATTACH_FAIL)                                                 \
/* 27 */ ERROR_CODE(AP_MANAGER_SUDDEN_DETACH)                                               \
/* 28 */ ERROR_CODE(AP_MANAGER_HAL_DISCONNECTED)                                            \
/* 29 */ ERROR_CODE(AP_MANAGER_CAC_TIMEOUT)                                                 \
/* 30 */ ERROR_CODE(MONITOR_DISCONNECTED)                                                   \
/* 31 */ ERROR_CODE(MONITOR_HOSTAP_DISABLED)                                                \
/* 32 */ ERROR_CODE(MONITOR_ATTACH_FAIL)                                                    \
/* 33 */ ERROR_CODE(MONITOR_SUDDEN_DETACH)                                                  \
/* 34 */ ERROR_CODE(MONITOR_HAL_DISCONNECTED)                                               \
/* 35 */ ERROR_CODE(MONITOR_REPORT_PROCESS_FAIL)                                            \
/* 36 */ ERROR_CODE(CONFIG_PLATFORM_REPORTED_INVALID_CONFIGURATION)                         \
/* 37 */ ERROR_CODE(CONFIG_BACKHAUL_WIRED_INTERFACE_IS_UNSUPPORTED)                         \
/* 38 */ ERROR_CODE(CONFIG_BACKHAUL_WIRELESS_INTERFACE_IS_UNSUPPORTED)                      \
/* 39 */ ERROR_CODE(CONFIG_NO_VALID_BACKHAUL_INTERFACE)                                     \
/* 40 */ ERROR_CODE(WATCHDOG_PROCESS_STUCK)                                                 \
/* 41 */ ERROR_CODE(WATCHDOG_PROCESS_ZOMBIE)                                                \
/* 42 */ ERROR_CODE(LAST)

// clang-format on

// Error codes ENUM
enum class eErrorCode { FOREACH_ERROR_CODE(GENERATE_ERROR_ENUM) };

} // namespace bpl

#endif /* _BPL_ERR_H_ */
