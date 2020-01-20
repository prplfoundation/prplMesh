/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_INTERNAL_H_
#define _BML_INTERNAL_H_

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_promise.h>
#include <bcl/beerocks_socket_thread.h>

#include <beerocks/tlvf/beerocks_message_common.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

#include "bml_defs.h"

#include <list>
#include <map>
#include <mutex>

class bml_internal : public beerocks::socket_thread {

public:
    bml_internal();
    ~bml_internal();

    // Initialize and connect to platform/master
    int connect(const std::string beerocks_conf_path);

    // Ping the master
    int ping();

    // Register a callback for the network map query results
    void register_nw_map_query_cb(BML_NW_MAP_QUERY_CB pCB);

    // Register a callback for the network map update results
    int register_nw_map_update_cb(BML_NW_MAP_QUERY_CB pCB);

    // Query the beerocks master for the network map
    int nw_map_query();

    // Register a callback for the statistcs results
    int register_stats_cb(BML_STATS_UPDATE_CB pCB);

    // Register a callback for events
    int register_event_cb(BML_EVENT_CB pCB);

    // Set the wireless lan SSID and password
    int set_wifi_credentials(const std::string ssid, const std::string pass, int sec, int vap_id,
                             int force);

    // Get wireless SSID and security
    int get_wifi_credentials(int vap_id, char *ssid, char *pass, int *sec);

    // get the platform onboarding state
    int get_onboarding_state(int *enable);

    // set the platform onboarding state
    int set_onboarding_state(int enable);

    // wps onboarding
    int bml_wps_onboarding(const char *iface);

    // Get administrator user credentials
    int get_administrator_credentials(char *user_password);

    // Get device information
    int get_device_info(BML_DEVICE_INFO &device_info);

    // Enable/Disable client roaming
    int set_client_roaming(bool enable);

    // Return client roaming status (in res)
    int get_client_roaming(int &res);
    //
    // Enable/Disable legacy client roaming
    int set_legacy_client_roaming(bool enable);

    // Return legacy client roaming status (in res)
    int get_legacy_client_roaming(int &res);

    // Enable/Disable client roaming prefer by signal_strength
    int set_client_roaming_prefer_signal_strength(bool enable);

    // Return client roaming prefer signal strength status (in res)
    int get_client_roaming_prefer_signal_strength(int &res);

    // Enable/Disable band steering
    int set_client_band_steering(bool enable);

    // Return band steering status (in res)
    int get_client_band_steering(int &res);

    // Enable/Disable ire roaming
    int set_ire_roaming(bool enable);

    // Return ire roaming status (in res)
    int get_ire_roaming(int &res);

    // Enable/Disable load_balancer
    int set_load_balancer(bool enable);

    // Return load_balancer status (in res)
    int get_load_balancer(int &res);

    // Enable/Disable service_fairness
    int set_service_fairness(bool enable);

    // Return service_fairness status (in res)
    int get_service_fairness(int &res);

    // Enable/Disable service_fairness
    int set_dfs_reentry(bool enable);

    // Return service_fairness status (in res)
    int get_dfs_reentry(int &res);

    // Enable/Disable certification mode
    int set_certification_mode(bool enable);

    // Return certification mode enable (in res)
    int get_certification_mode(int &res);

    // Set log level
    int set_log_level(const std::string module_name, const std::string log_level, uint8_t on,
                      const std::string mac);

    // Return master & slave version
    int get_master_slave_versions(char *master_version, char *slave_version);

    // set global/slave restricted channel
    int set_restricted_channels(const uint8_t *restricted_channels, const std::string mac,
                                uint8_t is_global, uint8_t size);

    // get global/slave restricted channel
    int get_restricted_channels(uint8_t *restricted_channels, const std::string mac,
                                uint8_t is_global);

    // triggers topology discovery
    int topology_discovery(const char *al_mac);

    // triggers channel selection on specific Agent
    int channel_selection(const char *al_mac, const char *ruid);

    //set and get vaps list
    int bml_set_vap_list_credentials(const BML_VAP_INFO *vaps, const uint8_t vaps_num);
    int bml_get_vap_list_credentials(BML_VAP_INFO *vaps, uint8_t &vaps_num);

    //set and get channel scan enable flag
    int set_dcs_continuous_scan_enable(const std::string &mac, int enable);
    int get_dcs_continuous_scan_enable(const std::string &mac, int *output_enable);

    //set and get channel scan params
    int set_dcs_continuous_scan_params(const std::string &mac, int dwell_time, int interval_time,
                                       unsigned int *channel_pool, int channel_pool_size);
    int get_dcs_continuous_scan_params(const std::string &mac, int *output_dwell_time,
                                       int *output_interval_time, unsigned int *output_channel_pool,
                                       int *output_channel_pool_size);

    //get channel scan results
    int get_dcs_scan_results(const std::string &mac, BML_NEIGHBOR_AP **output_results,
                             unsigned int *output_results_size, const unsigned int max_results_size,
                             uint8_t *output_result_status, bool is_single_scan);

    //trigger single channel scan
    int start_dcs_single_scan(const std::string &mac, int dwell_time_ms, int channel_pool_size,
                              unsigned int *channel_pool);
    /*
 * Public static methods:
 */
public:
    // Set easylogging context
    static int set_log_context(void *log_ctx);

    /*
 * Public getter/setter methods:
 */
public:
    void set_user_data(void *pUserData) { m_pUserData = pUserData; }

    void *get_user_data() const { return (m_pUserData); }

    bool is_onboarding() const { return (m_fOnboarding); }

    bool is_local_master() const { return (m_fLocal_Master); }

protected:
    virtual bool init() override;
    virtual void on_thread_stop() override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;
    bool wake_up(uint8_t action_opcode, int value);
    bool connect_to_master();
    virtual int process_cmdu_header(std::shared_ptr<beerocks::beerocks_header> beerocks_header);

    SocketClient *m_sockMaster = nullptr;

private:
    bool initialize(const std::string &beerocks_conf_path);
    bool connect_to_platform();

    bool handle_nw_map_query_update(int elements_num, int last_node, void *data_buffer,
                                    bool is_query);
    bool handle_stats_update(int elements_num, void *data_buffer);
    bool handle_event_update(uint8_t *data_buffer);
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    // Send message contained in cmdu to m_sockMaster,
    int send_bml_cmdu(int &result, uint8_t action_op);

private:
    std::string m_strBeerocksConfPath;
    beerocks::config_file::sConfigSlave m_sConfig;
    bool m_fOnboarding   = false;
    bool m_fLocal_Master = false;

    void *m_pUserData            = nullptr;
    SocketClient *m_sockPlatform = nullptr;

    std::mutex m_mtxLock;
    beerocks::promise<bool> *m_prmPing                  = nullptr;
    beerocks::promise<bool> *m_prmGetVapListCreds       = nullptr;
    beerocks::promise<bool> *m_prmSetVapListCreds       = nullptr;
    beerocks::promise<bool> *m_prmOnboard               = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsUpdate = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsGet    = nullptr;
    beerocks::promise<bool> *m_prmAdminCredentialsGet   = nullptr;
    beerocks::promise<bool> *m_prmDeviceInfoGet         = nullptr;
    beerocks::promise<bool> *m_prmMasterSlaveVersions   = nullptr;
    beerocks::promise<bool> *m_prmLocalMasterGet        = nullptr;
    beerocks::promise<bool> *m_prmRestrictedChannelsGet = nullptr;
    beerocks::promise<int> *m_prmRdkbWlan               = nullptr;
    beerocks::promise<bool> *m_prmChannelScanParamsGet  = nullptr;
    beerocks::promise<int> *m_prmChannelScanResultsGet  = nullptr;

    std::map<uint8_t, beerocks::promise<int> *> m_prmCliResponses;

    // Callback functions
    BML_NW_MAP_QUERY_CB m_cbNetMapQuery  = nullptr;
    BML_NW_MAP_QUERY_CB m_cbNetMapUpdate = nullptr;
    BML_STATS_UPDATE_CB m_cbStatsUpdate  = nullptr;
    BML_EVENT_CB m_cbEvent               = nullptr;

    beerocks_message::sDeviceInfo *m_device_info                     = nullptr;
    beerocks_message::sWifiCredentials *m_wifi_credentials           = nullptr;
    beerocks_message::sAdminCredentials *m_admin_credentials         = nullptr;
    beerocks_message::sVersions *m_master_slave_versions             = nullptr;
    beerocks_message::sRestrictedChannels *m_Restricted_channels     = nullptr;
    beerocks_message::sChannelScanRequestParams *m_scan_params       = nullptr;
    std::list<beerocks_message::sChannelScanResults> *m_scan_results = nullptr;
    uint8_t *m_scan_results_status                                   = nullptr;
    uint32_t *m_scan_results_size                                    = nullptr;
    uint32_t *m_scan_results_maxsize                                 = nullptr;
    BML_VAP_INFO *m_vaps                                             = nullptr;
    uint8_t *m_pvaps_list_size                                       = nullptr;
    uint16_t id                                                      = 0;
    static bool s_fExtLogContext;
};

#endif /* _BML_INTERNAL_H_ */
