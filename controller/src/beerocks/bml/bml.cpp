/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml.h"
#include "internal/bml_internal.h"

#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

using namespace beerocks::net;

int bml_connect(BML_CTX *ctx, const char *beerocks_path, void *user_data)
{
    if (!ctx) {
        LOG(ERROR) << "bml_connect - ctx is null!";
        return (-BML_RET_INVALID_ARGS);
    }

    // Clear context pointer
    *ctx = nullptr;

    // Create a new internal BML class instance
    bml_internal *pBML = new bml_internal();
    if (pBML == nullptr) {
        LOG(ERROR) << "bml_connect - bml_internal creation failed";
        return (-BML_RET_MEM_FAIL);
    }

    pBML->set_user_data(user_data);

    // Start the BML thread
    if (pBML->start("BML") == false) {
        LOG(ERROR) << "bml_connect - pBML->start failed";
        delete pBML;
        return (-BML_RET_INIT_FAIL);
    }

    // Connect to the platform
    int iRet;
    if ((iRet = pBML->connect(beerocks_path)) != BML_RET_OK) {

        LOG(ERROR) << "bml_connect - pBML->connect failed";

        // Stop the BML thread (and wait for it to stop...)
        pBML->stop(true);

        delete pBML;
        return (iRet);
    }

    // Store the context
    (*ctx) = pBML;

    return (BML_RET_OK);
}

int bml_disconnect(BML_CTX ctx)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    // Stop the BML thread (and wait for it to stop...)
    pBML->stop(true);

    // Delete the instance
    delete pBML;
    pBML = nullptr;

    return (BML_RET_OK);
}

int bml_onboard_status(BML_CTX ctx)
{
    if (!ctx)
        return (-1);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->is_onboarding() ? 1 : 0);
}

int bml_local_master_enabled(BML_CTX ctx)
{
    if (!ctx)
        return (-1);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->is_local_master() ? 1 : 0);
}

void *bml_get_user_data(BML_CTX ctx)
{
    if (!ctx)
        return (NULL);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->get_user_data());
}

int bml_set_vap_list_credentials(BML_CTX ctx, BML_VAP_INFO *vap_list, const uint8_t vaps_num)
{
    // Validate input parameters
    if (!ctx || !vap_list)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->bml_set_vap_list_credentials(vap_list, vaps_num));
}

int bml_get_vap_list_credentials(BML_CTX ctx, BML_VAP_INFO *vap_list, uint8_t *vaps_num)
{
    if (!ctx || !vap_list || !vaps_num)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->bml_get_vap_list_credentials(vap_list, *vaps_num));
}

int bml_ping(BML_CTX ctx)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->ping());
}

int bml_nw_map_register_query_cb(BML_CTX ctx, BML_NW_MAP_QUERY_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    pBML->register_nw_map_query_cb(cb);

    return (BML_RET_OK);
}

int bml_nw_map_register_update_cb(BML_CTX ctx, BML_NW_MAP_QUERY_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    pBML->register_nw_map_update_cb(cb);

    return (BML_RET_OK);
}

int bml_nw_map_query(BML_CTX ctx)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->nw_map_query());
}

int bml_stat_register_cb(BML_CTX ctx, BML_STATS_UPDATE_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->register_stats_cb(cb);
}

int bml_event_register_cb(BML_CTX ctx, BML_EVENT_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->register_event_cb(cb);
}

int bml_set_wifi_credentials(BML_CTX ctx, const char *ssid, const char *pass, int sec, int vap_id,
                             int force)
{
    // Validate input parameters
    if (!ctx || !ssid || !pass || (sec < BML_WLAN_SEC_NONE || sec > BML_WLAN_SEC_WPA_WPA2_PSK))
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->set_wifi_credentials(ssid, pass, sec, vap_id, force));
}

int bml_get_wifi_credentials(BML_CTX ctx, int vap_id, char *ssid, char *pass, int *sec)
{
    // Validate input parameters
    if (!ctx || !ssid || !sec)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->get_wifi_credentials(vap_id, ssid, pass, sec));
}

int bml_get_onboarding_state(BML_CTX ctx, int *enable)
{
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->get_onboarding_state(enable);
}

int bml_set_onboarding_state(BML_CTX ctx, int enable)
{
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->set_onboarding_state(enable);
}

int bml_wps_onboarding(BML_CTX ctx, const char *iface)
{
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->bml_wps_onboarding(iface);
}

int bml_set_wifi_configuration_start(BML_CTX ctx)
{
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->set_wifi_configuration_start();
}

int bml_set_wifi_configuration_end(BML_CTX ctx)
{
    bml_internal *pBML = (bml_internal *)ctx;

    return pBML->set_wifi_configuration_end();
}

int bml_get_administrator_credentials(BML_CTX ctx, char *user_password)
{
    // Validate input parameters
    if (!ctx || !user_password)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->get_administrator_credentials(user_password));
}

int bml_get_serial_number(BML_CTX ctx, char *serial_number)
{
    // Validate input parameters
    if (!ctx || !serial_number)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    BML_DEVICE_INFO device_info;
    auto ret = pBML->get_device_info(device_info);
    if (ret != 0) {
        return ret;
    }

    // Copy only the serial number
    beerocks::string_utils::copy_string(serial_number, device_info.serial_number, BML_DEV_INFO_LEN);

    return 0;
}

int bml_get_device_info(BML_CTX ctx, BML_DEVICE_INFO *device_info)
{
    // Validate input parameters
    if (!ctx || !device_info)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->get_device_info(*device_info));
}

int bml_set_client_roaming(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_client_roaming(enable));
}

int bml_get_client_roaming(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_client_roaming(*res));
}

int bml_set_legacy_client_roaming(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_legacy_client_roaming(enable));
}

int bml_get_legacy_client_roaming(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_legacy_client_roaming(*res));
}

int bml_set_client_roaming_prefer_signal_strength(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_client_roaming_prefer_signal_strength(enable));
}

int bml_get_client_roaming_prefer_signal_strength(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_client_roaming_prefer_signal_strength(*res));
}

int bml_set_client_band_steering(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->set_client_band_steering(enable));
}

int bml_get_client_band_steering(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_client_band_steering(*res));
}

int bml_set_ire_roaming(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->set_ire_roaming(enable));
}

int bml_get_ire_roaming(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_ire_roaming(*res));
}

int bml_set_load_balancer(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->set_load_balancer(enable));
}

int bml_get_load_balancer(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_load_balancer(*res));
}

int bml_set_service_fairness(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->set_service_fairness(enable));
}

int bml_get_service_fairness(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_service_fairness(*res));
}

int bml_set_dfs_reentry(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->set_dfs_reentry(enable));
}

int bml_get_dfs_reentry(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->get_dfs_reentry(*res));
}

int bml_set_log_level(BML_CTX ctx, const char *module_name, const char *log_level, uint8_t on,
                      const char *mac)
{
    // Validate input parameters
    if (!ctx || !module_name || !log_level || !mac)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    return (pBML->set_log_level(module_name, log_level, on, mac));
}

int bml_get_master_slave_versions(BML_CTX ctx, char *master_version, char *slave_version)
{
    // Validate input parameters
    if (!ctx || !master_version || !slave_version)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    if (pBML->is_local_master()) {
        beerocks::string_utils::copy_string(master_version, bml_get_bml_version(), BML_VERSION_LEN);
        beerocks::string_utils::copy_string(slave_version, bml_get_bml_version(), BML_VERSION_LEN);
        return BML_RET_OK;
    }

    return (pBML->get_master_slave_versions(master_version, slave_version));
}

int bml_set_local_log_context(void *log_ctx) { return (bml_internal::set_log_context(log_ctx)); }

const char *bml_get_bml_version() { return (BEEROCKS_VERSION); }

int bml_set_global_restricted_channels(BML_CTX ctx, const uint8_t *restricted_channels,
                                       uint8_t size)
{
    // Validate input parameters
    if (!ctx || !restricted_channels || !size)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    LOG(WARNING) << "bml_set_global_restricted_channels entry";

    return (pBML->set_restricted_channels(restricted_channels, network_utils::ZERO_MAC_STRING, 1,
                                          size));
}

int bml_get_global_restricted_channels(BML_CTX ctx, uint8_t *restricted_channels)
{
    // Validate input parameters
    if (!ctx || !restricted_channels)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->get_restricted_channels(restricted_channels, network_utils::ZERO_MAC_STRING, 1));
}

int bml_set_slave_restricted_channels(BML_CTX ctx, const uint8_t *restricted_channels,
                                      const char *mac, uint8_t size)
{
    // Validate input parameters
    if (!ctx || !restricted_channels || !mac || !size)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;
    LOG(WARNING) << "bml_set_slave_restricted_channels entry";

    std::string temp_mac(mac);
    LOG(WARNING) << "temp_mac = " << temp_mac;
    return (pBML->set_restricted_channels(restricted_channels, mac, 0, size));
}

int bml_get_slave_restricted_channels(BML_CTX ctx, uint8_t *restricted_channels, const char *mac)
{
    // Validate input parameters
    if (!ctx || !restricted_channels || !mac)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->get_restricted_channels(restricted_channels, mac, 0));
}

int bml_wfca_controller(BML_CTX ctx, const char *cmd, char *ret_buf, int ret_buf_size)
{
    // Validate input parameters
    if (!ctx || !cmd)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->wfca_controller(cmd, ret_buf, ret_buf_size));
}

int bml_wfca_agent(BML_CTX ctx, const char *cmd, char *ret_buf, int ret_buf_size)
{
    // Validate input parameters
    if (!ctx || !cmd)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = (bml_internal *)ctx;

    return (pBML->wfca_agent(cmd, ret_buf, ret_buf_size));
}
