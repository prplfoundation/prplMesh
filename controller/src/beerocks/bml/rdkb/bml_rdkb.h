/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_RDKB_H_
#define _BML_RDKB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bml_rdkb_defs.h"

/**
 * Initialize the BML library and connect to Beerocks.
 *
 * @param [out] ctx BML Context.
 * @param [in] beerocks_conf_path Path to the beerocks folder (e.g. /opt/beerocks)
 * @param [in] user_data Pointer to some arbitrary user data to be 
 * stored within the BML context.
 *
 * @return BML_RET_OK on success.
 */
int bml_rdkb_connect(BML_CTX *ctx, const char *beerocks_conf_path, void *user_data);

/**
 * Disconnect from Beerocks and purge the BML library context.
 * This operation will prevent any following calls to BML APIs
 * until a new call to bml_connect().
 *
 * @param [in] ctx BML Context.
 *
 * @return BML_RET_OK on success.
 */
int bml_rdkb_disconnect(BML_CTX ctx);

/*
* A steering group defines a group members of which can have steering done
* between them.
* To remove a group configuration call with NULL as cfg_2 and cfg_5.
* @param[in] ctx BML Context.
* @param[in] steeringGroupIndex  Wifi Steering Group index
* @param[in] cfg_2               2.4G apConfig
* @param[in] cfg_5               5G apConfig
*
* @return BML_RET_OK on success.
*
*/
int bml_rdkb_steering_set_group(BML_CTX ctx, uint32_t steeringGroupIndex,
                                struct BML_STEERING_AP_CONFIG *cfg_2,
                                struct BML_STEERING_AP_CONFIG *cfg_5);

/*
* Call this function to add/modify/remove per-client configuration config of client_mac.
* In order to remove a client configuration call with NULL as "config" parameter 
* or set config.snrProbeHWM to 0.
* @param[in] steeringGroupIndex   Wifi Steering Group index
* @param[in] bssid                AP bssid.
* @param[in] client_mac           The Client's MAC address.
* @param[in] config               The client configuration
* @param[in] ctx BML Context.
* @return BML_RET_OK on success.
*
*/
int bml_rdkb_steering_client_set(BML_CTX ctx, uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                 const BML_MAC_ADDR client_mac,
                                 struct BML_STEERING_CLIENT_CONFIG *config);

/*
* Callback registration function.
* Call this function to register/unregister the callback function.
* @param[in] ctx BML Context.
* @param[in] pCB pointer to callback function or NULL to unregister.
* @return BML_RET_OK on success.
*/
int bml_rdkb_steering_event_register(BML_CTX ctx, BML_EVENT_CB pCB);

/** 
* On demand measure.
*
* @param[in] ctx BML Context.
* @param[in] steeringGroupIndex  Wifi Steering Group index
* @param[in] bssid               AP bssid.
* @param[in] client_mac          The Client's MAC address.
*
* @return BML_RET_OK on success.
*
*/
int bml_rdkb_steering_client_measure(BML_CTX ctx, unsigned int steeringGroupIndex,
                                     const BML_MAC_ADDR bssid, const BML_MAC_ADDR client_mac);

/**Initiate a Client Disconnect.
 *
 * This is used to kick off a client, for steering purposes.
 * @param[in]  ctx BML Context.
 * @param[in]  steeringgroupIndex  Wifi Steering Group index
 * @param[in]  bssid               AP bssid. 
 * @param[in]  client_mac          The Client's MAC address
 * @param[in]  type                Disconnect Type
 * @param[in]  reason              Reason code to provide in deauth/disassoc frame.
 *
 * @return BML_RET_OK on success.
 */
int bml_rdkb_steering_client_disconnect(BML_CTX ctx, unsigned int steeringGroupIndex,
                                        const BML_MAC_ADDR bssid, const BML_MAC_ADDR client_mac,
                                        BML_DISCONNECT_TYPE type, unsigned int reason);
/**
 * Check whether the connection with the beerocks platform is alive and well.
 *
 * @return BML_RET_OK on success.
 */
int bml_rdkb_ping(BML_CTX ctx);

/**
 * Enables or disables beerocks client band steering.
 *
 * @param [in] ctx BML Context.
 * @param [in] enable 1 to enable or 0 to disable.
 *
 * @return BML_RET_OK on success.
 */
int bml_rdkb_set_client_band_steering(BML_CTX ctx, int enable);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BML_RDKB_H_ */
