/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_rdkb.h"
#include "../bml.h"
#include "internal/bml_rdkb_internal.h"

#include <bcl/network/network_utils.h>
#include <easylogging++.h>

using namespace beerocks::net;

int bml_rdkb_connect(BML_CTX *ctx, const char *beerocks_conf_path, void *user_data)
{
    LOG(DEBUG) << "bml_rdkb_connect entry";
    if (!ctx) {
        LOG(ERROR) << "bml_rdkb_connect - ctx is null!";
        return (-BML_RET_INVALID_ARGS);
    }

    // Clear context pointer
    *ctx = nullptr;

    // Create a new internal BML class instance
    bml_rdkb_internal *pBML = new bml_rdkb_internal();

    if (pBML == nullptr) {
        LOG(ERROR) << "bml_rdkb_connect - bml_rdkb_internal creation failed";
        return (-BML_RET_MEM_FAIL);
    }

    pBML->set_user_data(user_data);

    // Start the BML thread
    if (pBML->start("BML") == false) {
        LOG(ERROR) << "bml_rdkb_connect - pBML->start failed";
        delete pBML;
        return (-BML_RET_INIT_FAIL);
    }

    // Connect to the platform
    int iRet;
    if ((iRet = pBML->connect(beerocks_conf_path)) != BML_RET_OK) {

        LOG(ERROR) << "bml_rdkb_connect - pBML->connect failed";

        // Stop the BML thread (and wait for it to stop...)
        pBML->stop(true);

        delete pBML;
        return (iRet);
    }

    // Store the context
    (*ctx) = pBML;

    return (BML_RET_OK);
}

int bml_rdkb_disconnect(BML_CTX ctx)
{
    LOG(DEBUG) << "bml_rdkb_disconnect entry";
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_rdkb_internal *pBML = (bml_rdkb_internal *)ctx;

    // Stop the BML thread (and wait for it to stop...)
    pBML->stop(true);

    // Delete the instance
    delete pBML;
    pBML = nullptr;

    return (BML_RET_OK);
}

int bml_rdkb_steering_set_group(BML_CTX ctx, uint32_t steeringGroupIndex,
                                BML_STEERING_AP_CONFIG *cfg_2, BML_STEERING_AP_CONFIG *cfg_5)
{
    LOG(DEBUG) << "bml_rdkb_set_ap_set_config entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_rdkb_internal *pBML = (bml_rdkb_internal *)ctx;

    return (pBML->steering_set_group(steeringGroupIndex, cfg_2, cfg_5));
}

int bml_rdkb_steering_client_set(BML_CTX ctx, uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                 const BML_MAC_ADDR client_mac, BML_STEERING_CLIENT_CONFIG *config)
{
    LOG(DEBUG) << "bml_rdkb_steering_client_set entry";
    // Validate input parameters
    if (!ctx || !client_mac)
        return (-BML_RET_INVALID_ARGS);

    bml_rdkb_internal *pBML = (bml_rdkb_internal *)ctx;

    return (pBML->steering_client_set(steeringGroupIndex, bssid, client_mac, config));
}

int bml_rdkb_steering_event_register(BML_CTX ctx, BML_EVENT_CB pCB)
{
    LOG(DEBUG) << "bml_rdkb_steering_event_register entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_rdkb_internal *pBML = (bml_rdkb_internal *)ctx;

    return (pBML->steering_event_register(pCB));
}

int bml_rdkb_steering_client_measure(BML_CTX ctx, unsigned int steeringGroupIndex,
                                     const BML_MAC_ADDR bssid, const BML_MAC_ADDR client_mac)
{
    LOG(DEBUG) << "bml_rdkb_steering_client_measure entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_rdkb_internal *pBML = (bml_rdkb_internal *)ctx;

    return (pBML->steering_client_measure(steeringGroupIndex, bssid, client_mac));
}

int bml_rdkb_steering_client_disconnect(BML_CTX ctx, unsigned int steeringGroupIndex,
                                        const BML_MAC_ADDR bssid, const BML_MAC_ADDR client_mac,
                                        BML_DISCONNECT_TYPE type, unsigned int reason)
{
    LOG(DEBUG) << "bml_rdkb_steering_client_disconnect entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_rdkb_internal *pBML = (bml_rdkb_internal *)ctx;

    return (pBML->steering_client_disconnect(steeringGroupIndex, bssid, client_mac, type, reason));
}

int bml_rdkb_ping(BML_CTX ctx) { return bml_ping(ctx); }

int bml_rdkb_set_client_band_steering(BML_CTX ctx, int enable)
{
    return bml_set_client_band_steering(ctx, enable);
}