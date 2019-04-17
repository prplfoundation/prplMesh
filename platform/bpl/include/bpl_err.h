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

 #ifndef _BPL_ERR_H_
 #define _BPL_ERR_H_
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /****************************************************************************/
 /******************************* Definitions ********************************/
 /****************************************************************************/
 
 /* Beerocks Error Definitions */
 #define BPL_ERROR_STRING_LEN    256 /* Maximal length of BPL error string */
 
 // Helper MACROs for Enum/String generation
 #define GENERATE_ERROR_ENUM(ENUM) ENUM,
 #define GENERATE_ERROR_STRING(STRING) #STRING,
 
 /* Beerocks Error Codes */
 
 #define FOREACH_ERROR_CODE(ERROR_CODE)    \
        /* 00 */ ERROR_CODE(BPL_ERR_NONE)  \
         \
        /* 01 */ ERROR_CODE(BPL_ERR_BH_READING_DATA_FROM_THE_BRIDGE)   \
        /* 02 */ ERROR_CODE(BPL_ERR_BH_TIMEOUT_ATTACHING_TO_WPA_SUPPLICANT)    \
        /* 03 */ ERROR_CODE(BPL_ERR_BH_SCAN_FAILED_TO_INITIATE_SCAN)   \
        /* 04 */ ERROR_CODE(BPL_ERR_BH_SCAN_TIMEOUT) \
        /* 05 */ ERROR_CODE(BPL_ERR_BH_FAILED_TO_RETRIEVE_SCAN_RESULTS) \
        /* 06 */ ERROR_CODE(BPL_ERR_BH_SCAN_EXCEEDED_MAXIMUM_FAILED_SCAN_ATTEMPTS) \
        /* 07 */ ERROR_CODE(BPL_ERR_BH_OBTAINING_DHCP_BRIDGE_INTERFACE) \
        /* 08 */ ERROR_CODE(BPL_ERR_BH_OBTAINING_DHCP_BRIDGE_INTERFACE_MAX_ATTEMPTS) \
        /* 09 */ ERROR_CODE(BPL_ERR_BH_CONNECTING_TO_MASTER) \
        /* 10 */ ERROR_CODE(BPL_ERR_BH_CONNECTING_TO_MASTER_USING_UDS) \
        /* 11 */ ERROR_CODE(BPL_ERR_BH_DEVICE_QUERY_TIMEOUT) \
        /* 12 */ ERROR_CODE(BPL_ERR_BH_ASSOCIATE_3ADDR) \
        /* 13 */ ERROR_CODE(BPL_ERR_BH_ASSOCIATE_3ADDR_TIMEOUT) \
        /* 14 */ ERROR_CODE(BPL_ERR_BH_ASSOCIATE_4ADDR_TIMEOUT) \
        /* 15 */ ERROR_CODE(BPL_ERR_BH_ASSOCIATE_4ADDR_FAILURE) \
        /* 16 */ ERROR_CODE(BPL_ERR_BH_TRANSITION_FROM_3ADDR_TO_4ADDR) \
        /* 17 */ ERROR_CODE(BPL_ERR_BH_ROAMING) \
        /* 18 */ ERROR_CODE(BPL_ERR_BH_DISCONNECTED) \
        /* 19 */ ERROR_CODE(BPL_ERR_BH_WPA_SUPPLICANT_TERMINATED) \
        /* 20 */ ERROR_CODE(BPL_ERR_BH_MASTER_SOCKET_DISCONNECTED) \
        /* 21 */ ERROR_CODE(BPL_ERR_BH_SLAVE_SOCKET_DISCONNECTED) \
        /* 22 */ ERROR_CODE(BPL_ERR_BH_OPEN_UDP_SOCKET_FAILURE) \
        /* 23 */ ERROR_CODE(BPL_ERR_BH_STOPPED) \
        /* 24 */ ERROR_CODE(BPL_ERR_SLAVE_CONNECTING_TO_BACKHAUL_MANAGER) \
        /* 25 */ ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_GET_WLAN_READY_STATUS_REQUEST) \
        /* 26 */ ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_WIFI_CREDENTIALS_SET_REQUEST) \
        /* 27 */ ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_ENABLE_REQUEST) \
        /* 28 */ ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_DISABLE_REQUEST)  \
        /* 29 */ ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_RESTORE_REQUEST)  \
        /* 30 */ ERROR_CODE(BPL_ERR_SLAVE_TIMEOUT_IFACE_RESTART_REQUEST)  \
        /* 31 */ ERROR_CODE(BPL_ERR_SLAVE_MASTER_KEEP_ALIVE_TIMEOUT)  \
        /* 32 */ ERROR_CODE(BPL_ERR_SLAVE_INVALID_MASTER_SOCKET)  \
        /* 33 */ ERROR_CODE(BPL_ERR_SLAVE_IFACE_CHANGE_STATE_FAILED) \
        /* 34 */ ERROR_CODE(BPL_ERR_SLAVE_IFACE_RESTORE_FAILED) \
        /* 35 */ ERROR_CODE(BPL_ERR_SLAVE_WIFI_CREDENTIALS_SET_FAILED)\
        /* 36 */ ERROR_CODE(BPL_ERR_SLAVE_POST_INIT_CONFIG_FAILED) \
        /* 37 */ ERROR_CODE(BPL_ERR_SLAVE_TX_CHANGE_STATE_FAILED) \
        /* 38 */ ERROR_CODE(BPL_ERR_SLAVE_UPDATE_CREDENTIALS_FAILED) \
        /* 39 */ ERROR_CODE(BPL_ERR_SLAVE_FAILED_CONNECT_TO_PLATFORM_MANAGER) \
        /* 40 */ ERROR_CODE(BPL_ERR_SLAVE_PLATFORM_MANAGER_REGISTER_TIMEOUT) \
        /* 41 */ ERROR_CODE(BPL_ERR_SLAVE_SLAVE_BACKHAUL_MANAGER_DISCONNECTED) \
        /* 42 */ ERROR_CODE(BPL_ERR_SLAVE_STOPPED) \
        /* 43 */ ERROR_CODE(BPL_ERR_AP_MANAGER_START) \
        /* 44 */ ERROR_CODE(BPL_ERR_AP_MANAGER_DISCONNECTED) \
        /* 45 */ ERROR_CODE(BPL_ERR_AP_MANAGER_HOSTAP_DISABLED) \
        /* 46 */ ERROR_CODE(BPL_ERR_AP_MANAGER_ATTACH_FAIL) \
        /* 47 */ ERROR_CODE(BPL_ERR_AP_MANAGER_SUDDEN_DETACH) \
        /* 48 */ ERROR_CODE(BPL_ERR_AP_MANAGER_HAL_DISCONNECTED) \
        /* 49 */ ERROR_CODE(BPL_ERR_AP_MANAGER_CAC_TIMEOUT) \
        /* 50 */ ERROR_CODE(BPL_ERR_MONITOR_DISCONNECTED) \
        /* 51 */ ERROR_CODE(BPL_ERR_MONITOR_HOSTAP_DISABLED) \
        /* 52 */ ERROR_CODE(BPL_ERR_MONITOR_ATTACH_FAIL) \
        /* 53 */ ERROR_CODE(BPL_ERR_MONITOR_SUDDEN_DETACH) \
        /* 54 */ ERROR_CODE(BPL_ERR_MONITOR_HAL_DISCONNECTED) \
        /* 55 */ ERROR_CODE(BPL_ERR_MONITOR_REPORT_PROCESS_FAIL) \
        /* 56 */ ERROR_CODE(BPL_ERR_CONFIG_PLATFORM_REPORTED_INVALID_CONFIGURATION) \
        /* 57 */ ERROR_CODE(BPL_ERR_CONFIG_BACKHAUL_WIRED_INTERFACE_IS_UNSUPPORTED) \
        /* 58 */ ERROR_CODE(BPL_ERR_CONFIG_BACKHAUL_WIRELESS_INTERFACE_IS_UNSUPPORTED) \
        /* 59 */ ERROR_CODE(BPL_ERR_CONFIG_NO_VALID_BACKHAUL_INTERFACE) \
        /* 60 */ ERROR_CODE(BPL_ERR_CONFIG_BRIDGE_INTERFACE_IS_EMPTY) \
        /* 61 */ ERROR_CODE(BPL_ERR_WIFI_CONFIGURATION_CHANGE_TIMEOUT) \
        /* 62 */ ERROR_CODE(BPL_ERR_WATCHDOG_PROCESS_STUCK) \
        /* 63 */ ERROR_CODE(BPL_ERR_WATCHDOG_PROCESS_ZOMBIE) \
         \
        /* 74 */ ERROR_CODE(BPL_ERR_LAST) \
 
 // States ENUM
 enum eErrorCode {
     FOREACH_ERROR_CODE(GENERATE_ERROR_ENUM)
 };
  
 #ifdef __cplusplus
 } /* extern "C" */
 #endif
 
 #endif /* _BPL_ERR_H_ */
