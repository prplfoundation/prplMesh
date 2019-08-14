/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_H_
#define _BML_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bml_defs.h"

/**
 * Initialize the BML library and connect to Beerocks.
 *
 * @param [out] ctx BML Context.
 * @param [in] beerocks_conf_path Path to the beerocks configuration filesfolder (e.g. /opt/beerocks)
 * @param [in] user_data Pointer to some arbitrary user data to be 
 * stored within the BML context.
 *
 * @return BML_RET_OK on success.
 */
int bml_connect(BML_CTX *ctx, const char *beerocks_conf_path, void *user_data);

/**
 * Disconnect from Beerocks and purge the BML library context.
 * This operation will prevent any following calls to BML APIs
 * until a new call to bml_connect().
 *
 * @param [in] ctx BML Context.
 *
 * @return BML_RET_OK on success.
 */
int bml_disconnect(BML_CTX ctx);

/* Returns the onboard state of the platform
 * @return 1 Platform is in onboarding state.
 * @return 0 Platform is in operational mode.
 * @return -1 Error.
 */
int bml_onboard_status(BML_CTX ctx);

/* Check whether master is enabled local platform
 * @return 1 Master enabled on local platform.
 * @return 0 Master is not enabled on local platform.
 * @return -1 Error.
 */
int bml_local_master_enabled(BML_CTX ctx);

/**
 * Returns the user data stored within the BML context.
 *
 * @param [in] ctx BML Context.
 *
 * @return Pointer to the user data passed to the bml_connect() function.
 */
void *bml_get_user_data(BML_CTX ctx);

/**
 * Check whether the connection with the beerocks platform is alive and well.
 *
 * @return BML_RET_OK on success.
 */
int bml_ping(BML_CTX ctx);

/**
 * Registers a callback functions for the network map query operation.
 * When calling the bml_nw_map_query() function, a request to send the entire
 * network map will be sent to the beerocks platform. On arrival of the response,
 * the callback function will be called for processing.
 *
 * The function can be called with NULL value to unregister the callback.
 *
 * @param [in] ctx BML Context.
 * @param [in] cb Pointer to the network map query callback.
 *
 * @return BML_RET_OK on success.
 */
int bml_nw_map_register_query_cb(BML_CTX ctx, BML_NW_MAP_QUERY_CB cb);

/**
 * Registers a callback functions for the network map update operation.
 * A network map update occur asynchronously when an update message is received
 * from the beerocks platform. If a callback function is registered, it'll be called.
 *
 * The function can be called with NULL value to unregister the callback.
 *
 * @param [in] ctx BML Context.
 * @param [in] cb Pointer to the network map update callback.
 *
 * @return BML_RET_OK on success.
 */
int bml_nw_map_register_update_cb(BML_CTX ctx, BML_NW_MAP_QUERY_CB cb);

/**
 * Query the beerocks for the latest network map.
 * This function is asynchronous and returns immediatly.
 * When the result is available, and a callback function was registered using the
 * bml_nw_map_register() function, it will be called.
 * 
 * @param [in] ctx BML Context.
 *
 * @return BML_RET_OK on success.
 */
int bml_nw_map_query(BML_CTX ctx);

/**
 * Registers a callback function to periodic statistics update from 
 * the beerocks platform.
 *
 * The function can be called with NULL value to unregister the callback.
 *
 * @param [in] ctx BML Context.
 * @param [in] cb Pointer to the statistics update callback.
 *
 * @return BML_RET_OK on success.
 */
int bml_stat_register_cb(BML_CTX ctx, BML_STATS_UPDATE_CB cb);

/**
 * Registers a callback function to events from 
 * the beerocks platform.
 *
 * The function can be called with NULL value to unregister the callback.
 *
 * @param [in] ctx BML Context.
 * @param [in] cb Pointer to the event callback.
 *
 * @return BML_RET_OK on success.
 */
int bml_event_register_cb(BML_CTX ctx, BML_EVENT_CB cb);

/**
 * Updates the Wi-Fi credentials (for WPA2-Personal policy) for the 
 * beerocks network.
 *
 * @param [in] ctx BML Context.
 * @param [in] ssid[BML_NODE_SSID_LEN] The SSID for the network. ssid array length must be equal to BML_NODE_SSID_LEN
 * @param [in] pass[BML_NODE_PASS_LEN] The WPA2 passphrase for the network. pass array length must be equal to BML_NODE_PASS_LEN
 * @param [in] sec Wi-Fi security/encryption (BML_WLAN_SEC_*).
 * @param [in] vap_id ID of the requested VAP (0 for main VAP).
 * @param [in] force Force update even if not all registered IREs are online.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_wifi_credentials(BML_CTX ctx, const char *ssid, const char *pass, int sec, int vap_id,
                             int force);

/**
 * Get Wi-Fi credentials (SSID and security) for the beerocks network.
 *
 * NOTE: If the system is in "onboarding" state, the function will return 
 *       BML_RET_OP_FAILED.
 *
 * @param [in] ctx BML Context.
 * @param [in] vap_id ID of the requested VAP (0 for main VAP).
 * @param [out] ssid[BML_NODE_SSID_LEN] The SSID for the network. ssid array length must be equal to BML_NODE_SSID_LEN
 * @param [out] pass[BML_NODE_PASS_LEN] The WPA2 passphrase for the network.  pass array length must be equal to BML_NODE_PASS_LEN
 * @param [out] sec Wi-Fi security/encryption (BML_WLAN_SEC_*).
 *
 * @return BML_RET_OK on success.
 */
int bml_get_wifi_credentials(BML_CTX ctx, int vap_id, char *ssid, char *pass, int *sec);

/* Get the onboard state of the platform
 * @param [in] ctx BML Context.
 * @param [out] enable = 1 (onboarding enabled), disabled = 0 (onboarding disabled).
 * @return BML_RET_OK on success.
 */
int bml_get_onboarding_state(BML_CTX ctx, int *enable);

/* Set the onboard state of the platform
 * @param [in] ctx BML Context.
 * @param [in] enable, set to 1 for onboarding enabled, set to 0 for onboarding disabled.
 * @return BML_RET_OK on success.
 */
int bml_set_onboarding_state(BML_CTX ctx, int enable);

/* Do WPS onboarding of a specific iface
 * @param [in] ctx BML Context.
 * @param [in] iface, interface name, if empty, all interfaces.
 * @return BML_RET_OK on success.
 */
int bml_wps_onboarding(BML_CTX ctx, const char *iface);

/**
* Notification to BeeRocks on Wi-Fi configuration start. It is expected that Hostapd will be down during configuration.
*
* @param [in] ctx BML Context.
*
* @return BML_RET_OK on success.
*/
int bml_set_wifi_configuration_start(BML_CTX ctx);

/**
* Notification to BeeRocks on Wi-Fi configuration end. It is expected that Hostapd is down and ready for re-activation.
*
* @param [in] ctx BML Context.
*
* @return BML_RET_OK on success.
*/
int bml_set_wifi_configuration_end(BML_CTX ctx);

/**
 * Get administrator credentials (admin user password).
 *
 * @param [in] ctx BML Context.
 * @param [out] user_password[BML_NODE_USER_PASS_LEN] The password of the administrator user
 *
 * @return BML_RET_OK on success.
 */

int bml_get_administrator_credentials(BML_CTX ctx, char *user_password);

/**
 * Get serial number of the device
 * NOTE: DEPRECATED - Use bml_get_device_info() instead.
 *
 * @param [in] ctx BML Context.
 * @param [out] serial_number[BML_NODE_SERIAL_NUMBER_LEN] The serial number of the device
 *
 * @return BML_RET_OK on success.
 */
int __attribute__((deprecated)) bml_get_serial_number(BML_CTX ctx, char *serial_number);

/**
 * Get generic device information.
 *
 * @param [in] ctx BML Context.
 * @param [out] device_info Device information structure.
 *
 * @return BML_RET_OK on success.
 */

int bml_get_device_info(BML_CTX ctx, struct BML_DEVICE_INFO *device_info);

/**
 * Enables or disables beerocks roaming.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_client_roaming(BML_CTX ctx, int enable);

/**
 * Returns beerocks roaming mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_client_roaming(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks legacy (forced) client roaming.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_legacy_client_roaming(BML_CTX ctx, int enable);

/**
 * Returns beerocks legacy client roaming mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_legacy_client_roaming(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks client roaming based on maximum signal strength.
 * When disabled, client roaming will be done based on maximum client throuput.
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_client_roaming_prefer_signal_strength(BML_CTX ctx, int enable);

/**
 * Returns beerocks client roaming prefer signal strength configuration.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result: 0 - disabled, 1 - enable.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_client_roaming_prefer_signal_strength(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks client band steering.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_client_band_steering(BML_CTX ctx, int enable);

/**
 * Returns beerocks client band steering mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_client_band_steering(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks ire roaming.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_ire_roaming(BML_CTX ctx, int enable);

/**
 * Returns beerocks ire roaming mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_ire_roaming(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks load_balancer.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_load_balancer(BML_CTX ctx, int enable);

/**
 * Returns beerocks load_balancer mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_load_balancer(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks service_fairness.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_service_fairness(BML_CTX ctx, int enable);

/**
 * Returns beerocks service_fairness mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_service_fairness(BML_CTX ctx, int *res);

/**
 * Enables or disables beerocks DFS reentry feature.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_dfs_reentry(BML_CTX ctx, int enable);

/**
 * Returns beerocks DFS reentry feature.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_dfs_reentry(BML_CTX ctx, int *res);

/**
 * Enables or disables certification mode.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_certification_mode(BML_CTX ctx, int enable);

/**
 * Returns certification mode enable.
 *
 * @param [in] ctx BML Context.
 * @param [in] res A reference for the result to be stored in.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_certification_mode(BML_CTX ctx, int *res);

/**
 * Set beerocks modules log level.
 *
 * @param [in] ctx BML Context.
 * @param [in] module_name A module name - master/slave/monitor/platform/all.
 * @param [in] log_level A log level - 'i'-info, 'd'-debug, 'e'-error, 'f'-fatal, 't'-trace, 'w'-warning, 'a'-all.
 * @param [in] on On/Off state.
 * @param [in] mac The required mac.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_log_level(BML_CTX ctx, const char *module_name, const char *log_level, uint8_t on,
                      const char *mac);

/**
 * Returns strings representation of the beerocks master & slave versions.
 * The version is of type X.Y.Z (or NULL in case of an error).
 *
 * @param [in] ctx BML Context.
 * @param [out] version The BEEROCKS MASTER VERSION.
 * @param [out] version The BEEROCKS SLAVE VERSION.
 * 
 * @return BML_RET_OK on success.
 */
int bml_get_master_slave_versions(BML_CTX ctx, char *master_version, char *slave_version);

/**
 * Use provided easylogging context.
 *
 * @param [in] log_ctx Pointer to an easylogging context.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_local_log_context(void *log_ctx);

/**
 * Returns a string representation of the bml version.
 * The version is of type X.Y.Z.
 */
const char *bml_get_bml_version();

/**
 * Updates global resrected channels for the 
 * beerocks network.
 *
 * @param [in] ctx BML Context.
 * @param [in] restricted_channels[BML_NODE_RESTRICTED_CHANNELS_LEN] The restricted_channels for the system. restricted_channels array length must be equal to BML_NODE_RESTRICTED_CHANNELS_LEN
 *
 * @return BML_RET_OK on success.
 */
int bml_set_global_restricted_channels(BML_CTX ctx, const uint8_t *restricted_channels,
                                       uint8_t size);

/**
 * Updates global resrected channels for the 
 * beerocks network.
 *
 * @param [in] ctx BML Context.
 * @param [in/out] restricted_channels[BML_NODE_RESTRICTED_CHANNELS_LEN] The restricted_channels for the system. restricted_channels array length must be equal to BML_NODE_RESTRICTED_CHANNELS_LEN
 *
 * @return BML_RET_OK on success.
 */
int bml_get_global_restricted_channels(BML_CTX ctx, uint8_t *restricted_channels);

/**
 * Updates Slave resrected channels for the 
 * beerocks network.
 *
 * @param [in] ctx BML Context.
 * @param [in] restricted_channels[BML_NODE_RESTRICTED_CHANNELS_LEN] The restricted_channels for a slave. restricted_channels array length must be equal to BML_NODE_SSID_LEN
 * @param [in] mac The required mac.
 *
 * @return BML_RET_OK on success.
 */
int bml_set_slave_restricted_channels(BML_CTX ctx, const uint8_t *restricted_channels,
                                      const char *mac, uint8_t size);

/**
 * Updates Slave resrected channels for the 
 * beerocks network.
 *
 * @param [in] ctx BML Context.
 * @param [in/out] restricted_channels[BML_NODE_RESTRICTED_CHANNELS_LEN] The restricted_channels for a slave. restricted_channels array length must be equal to BML_NODE_SSID_LEN
 * @param [in] mac The required mac.
 *
 * @return BML_RET_OK on success.
 */
int bml_get_slave_restricted_channels(BML_CTX ctx, uint8_t *restricted_channels, const char *mac);

/**
 * Sends wfa-ca command to controller.
 * 
 * @param [in] ctx BML Context.
 * @param [in] cmd, command buffer
 * @param [in] command, command buffer
 * @param [in] command_len, command buffer length
 * @param [in] reply_cb, callback function which prints a reply to a wfa-ca command
 *
 *
 */
int bml_wfa_ca_controller(BML_CTX ctx, const char *command, int command_len,
                          BML_WFA_CA_CB reply_cb);

/*
 * Sends wfa-ca command to agent
 *
 * @param [in] ctx BML Context.
 * @param [in] cmd, command buffer
 * @param [out] ret_buf, return buffer
 * @param [in] ret_buf_size, return buffer size
 *
 * @return BML_RET_OK on success.
 */
int bml_wfa_ca_agent(BML_CTX ctx, const char *cmd, char *ret_buf, int ret_buf_size);

/**
 * Trigger Topology Discovery Task.
 *
 * @param [in] ctx BML Context.
 * @param [in] al_mac al_mac of the platform. 
 *
 * @return BML_RET_OK on success.
 */
int bml_trigger_topology_discovery(BML_CTX ctx, const char *al_mac);

/**
 * Trigger AP Capability Query task.
 *
 * @param [in] ctx BML Context.
 * @param [in] al_mac al_mac of the platform. 
 *
 * @return BML_RET_OK on success.
 */
int bml_trigger_ap_capability(BML_CTX ctx, const char *al_mac);

/**
 * Trigger Channel Selection Task on a specific Agent.
 *
 * @param [in] ctx BML Context.
 * @param [in] al_mac al_mac of the platform. 
 * @param [in] ruid Agent radio identifier. 
 * 
 * @return BML_RET_OK on success.
 */
int bml_channel_selection(BML_CTX ctx, const char *al_mac, const char *ruid);

/**
 * Set a VAP information list.
 *
 * @param [in] ctx BML Context.
 * @param [in] vap_list Array of BML_VAP_INFO elements. 
 * @param [in] vaps_num Number of elements in vap_list.
 * 
 * @return BML_RET_OK on success.
 */
int bml_set_vap_list_credentials(BML_CTX ctx, struct BML_VAP_INFO *vap_list,
                                 const uint8_t vaps_num);

/**
 * Get a VAP information list.
 *
 * @param [in] ctx BML Context.
 * @param [out] vap_list User allocated array of BML_VAP_INFO elements.
 * @param [in/out] vaps_num Number of elements in vap_list.
 * 
 * @return BML_RET_OK on success.
 */
int bml_get_vap_list_credentials(BML_CTX ctx, struct BML_VAP_INFO *vap_list, uint8_t *vaps_num);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BML_H_ */
