/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_INTERNAL_H_
#define _BML_INTERNAL_H_

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_promise.h>
#include <bcl/beerocks_socket_thread.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

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
    int connect(const std::string &beerocks_conf_path);

    // Ping the master
    int ping();

    // Register a callback for the network map query results
    void register_nw_map_query_cb(BML_NW_MAP_QUERY_CB pCB);

    // Register a callback for the network map update results
    int register_nw_map_update_cb(BML_NW_MAP_QUERY_CB pCB);

    // Query the beerocks master for the network map
    int nw_map_query();

    // Query the beerocks master for the network map
    int device_oper_radios_query(BML_DEVICE_DATA *device_data);

    // Register topology discovery query so bml listener could receive the response event.
    int register_topology_discovery_response();

    // Unregister topology discovery response events.
    int unregister_topology_discovery_response();

    // Register a callback for the statistcs results
    int register_stats_cb(BML_STATS_UPDATE_CB pCB);

    // Register a callback for events
    int register_event_cb(BML_EVENT_CB pCB);

    /**
    * @brief Add the Wi-Fi credentials for the beerocks network.
    * 
    * @param [in] al_mac The agent mac adress
    * @param [in] wifi_credentials Structure with credentials (ssid, network_key, etc)
    * 
    * @return BML_RET_OK on success.
    */
    int set_wifi_credentials(const sMacAddr &al_mac,
                             const son::wireless_utils::sBssInfoConf &wifi_credentials);

    /**
    * @brief Clear wifi credentials for specific AL-MAC
    *
    * @param al_mac al_mac client AL-MAC address
    *
    * @return BML_RET_OK if success, error code otherwise
    */
    int clear_wifi_credentials(const sMacAddr &al_mac);

    /**
    * @brief Update wifi credentials for specific AL-MAC
    *
    * @param al_mac al_mac client AL-MAC address
    *
    * @return BML_RET_OK if success, error code otherwise
    */
    int update_wifi_credentials(const sMacAddr &al_mac);

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
    int set_log_level(const std::string &module_name, const std::string &log_level, uint8_t on,
                      const std::string &mac);

    // Return master & slave version
    int get_master_slave_versions(char *master_version, char *slave_version);

    // set global/slave restricted channel
    int set_restricted_channels(const uint8_t *restricted_channels, const std::string &mac,
                                uint8_t is_global, uint8_t size);

    // get global/slave restricted channel
    int get_restricted_channels(uint8_t *restricted_channels, const std::string &mac,
                                uint8_t is_global);

    // triggers topology discovery
    int trigger_topology_discovery_query(const char *al_mac);

    // triggers channel selection on specific Agent
    int channel_selection(const char *al_mac, const char *ruid);

    //set and get vaps list
    int bml_set_vap_list_credentials(const BML_VAP_INFO *vaps, const uint8_t vaps_num);
    int bml_get_vap_list_credentials(BML_VAP_INFO *vaps, uint8_t &vaps_num);

    /**
    * @brief Enables or disables beerocks DCS continuous scans.
    *
    * @param [in] radio_mac Radio MAC of selected radio
    * @param [in] enable    Value of 1 to enable or 0 to disable.
    *
    * @return BML_RET_OK on success.
    */
    int set_dcs_continuous_scan_enable(const sMacAddr &mac, int enable);
    /**
    * @brief Get DCS continuous scans param.
    *
    * @param [in] mac     Radio MAC of selected radio
    * @param [out] enable A reference for the result to be stored in.
    *
    * @return BML_RET_OK on success.
    */
    int get_dcs_continuous_scan_enable(const sMacAddr &mac, int &enable);

    /**
    * @brief Set DCS continuous scan params.
    *
    * @param [in] mac               Radio MAC of selected radio
    * @param [in] dwell_time        Set the dwell time in milliseconds.
    * @param [in] interval_time     Set the interval time in seconds.
    * @param [in] channel_pool      Set the channel pool for the DCS.
    * @param [in] channel_pool_size Set the DCS channel pool size.
    *
    * @return BML_RET_OK on success.
    */
    int set_dcs_continuous_scan_params(const sMacAddr &mac, int dwell_time, int interval_time,
                                       unsigned int *channel_pool, int channel_pool_size);

    /**
    * @brief Get DCS continuous scan params.
    *
    * @param [in] mac                Radio MAC of selected radio
    * @param [out] dwell_time        Get the dwell time in milliseconds.
    * @param [out] interval_time     Get the interval time in seconds.
    * @param [out] channel_pool      Get the channel pool for the DCS.
    * @param [out] channel_pool_size Get the DCS channel pool size.
    *
    * @return BML_RET_OK on success.
    */
    int get_dcs_continuous_scan_params(const sMacAddr &mac, int *dwell_time, int *interval_time,
                                       unsigned int *channel_pool, int *channel_pool_size);

    /**
    * @brief Get DCS channel scan results.
    *
    * @param [in] mac              Radio MAC of selected radio
    * @param [out] results         Returning results.
    * @param [out] results_size    Returning results size.
    * @param [in] max_results_size Max requested results
    * @param [out] result_status   Returning status of results
    * @param [in] is_single_scan   Flag, if the results should be from a single scan or continuous
    * 
    * @return BML_RET_OK on success.
    */
    int get_dcs_scan_results(const sMacAddr &mac, BML_NEIGHBOR_AP *results,
                             unsigned int &results_size, const unsigned int max_results_size,
                             uint8_t &result_status, bool is_single_scan);

    /**
    * Start a single DCS scan with parameters.
    *
    * @param [in] mac                  Radio MAC of selected radio
    * @param [in] dwell_time_ms        Set the dwell time in milliseconds.
    * @param [in] channel_pool         Set the channel pool for the DCS.
    * @param [in] channel_pool_size    Set the DCS channel pool size.
    *
    * @return BML_RET_OK on success.
    */
    int start_dcs_single_scan(const sMacAddr &mac, int dwell_time_ms, unsigned int *channel_pool,
                              int channel_pool_size);

    /**
     * Get client list.
     *
     * @param [in,out] client_list List of MAC addresses sepereted by a comma.
     * @param [in,out] client_list_size Size of client list.
     * @return BML_RET_OK on success.
     */
    int client_get_client_list(std::string &client_list, unsigned int *client_list_size);

    /**
     * Set client configuration.
     *
     * @param [in] sta_mac MAC address of a station.
     * @param [in] client_config Client configuration to be set.
     * @return BML_RET_OK on success.
     */
    int client_set_client(const sMacAddr &sta_mac, const BML_CLIENT_CONFIG &client_config);

    /**
     * Get client info.
     *
     * @param [in] sta_mac MAC address of a station.
     * @param [in,out] client Client information.
     * @return BML_RET_OK on success.
     */
    int client_get_client(const sMacAddr &sta_mac, BML_CLIENT *client);

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
    beerocks::promise<bool> *m_prmWiFiCredentialsSet    = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsUpdate = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsClear  = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsGet    = nullptr;
    beerocks::promise<bool> *m_prmAdminCredentialsGet   = nullptr;
    beerocks::promise<bool> *m_prmDeviceInfoGet         = nullptr;
    beerocks::promise<bool> *m_prmDeviceDataGet         = nullptr;
    beerocks::promise<bool> *m_prmMasterSlaveVersions   = nullptr;
    beerocks::promise<bool> *m_prmLocalMasterGet        = nullptr;
    beerocks::promise<bool> *m_prmRestrictedChannelsGet = nullptr;
    beerocks::promise<int> *m_prmRdkbWlan               = nullptr;
    //Promise used to indicate the GetParams response was received
    beerocks::promise<bool> *m_prmChannelScanParamsGet = nullptr;
    //Promise used to indicate the GetResults response was received
    beerocks::promise<int> *m_prmChannelScanResultsGet = nullptr;
    beerocks::promise<bool> *m_prmClientListGet        = nullptr;
    beerocks::promise<bool> *m_prmClientGet            = nullptr;

    std::map<uint8_t, beerocks::promise<int> *> m_prmCliResponses;

    // Callback functions
    BML_NW_MAP_QUERY_CB m_cbNetMapQuery  = nullptr;
    BML_NW_MAP_QUERY_CB m_cbNetMapUpdate = nullptr;
    BML_STATS_UPDATE_CB m_cbStatsUpdate  = nullptr;
    BML_EVENT_CB m_cbEvent               = nullptr;

    beerocks_message::sDeviceInfo *m_device_info                 = nullptr;
    beerocks_message::sDeviceData *m_device_data                 = nullptr;
    beerocks_message::sWifiCredentials *m_wifi_credentials       = nullptr;
    beerocks_message::sAdminCredentials *m_admin_credentials     = nullptr;
    beerocks_message::sVersions *m_master_slave_versions         = nullptr;
    beerocks_message::sRestrictedChannels *m_Restricted_channels = nullptr;
    //m_scan_params is used when receiving the channel scan parameters
    beerocks_message::sChannelScanRequestParams *m_scan_params = nullptr;
    //m_scan_results is used when receiving channel scan results
    std::list<beerocks_message::sChannelScanResults> *m_scan_results = nullptr;
    //m_scan_results_status is used to store the results' latest status
    uint8_t *m_scan_results_status = nullptr;
    //m_scan_results_maxsize is used to indicate the maximum capacity of the requested results
    uint32_t *m_scan_results_maxsize = nullptr;
    std::string *m_client_list       = nullptr;
    uint32_t *m_client_list_size     = nullptr;
    BML_CLIENT *m_client             = nullptr;
    BML_VAP_INFO *m_vaps             = nullptr;
    uint8_t *m_pvaps_list_size       = nullptr;
    uint16_t id                      = 0;
    static bool s_fExtLogContext;
};

#endif /* _BML_INTERNAL_H_ */
