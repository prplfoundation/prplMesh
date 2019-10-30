/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_SLAVE_THREAD_H
#define _SON_SLAVE_THREAD_H

#include "ap_manager_thread.h"

#include <beerocks/bcl/beerocks_backport.h>
#include <beerocks/bcl/beerocks_logging.h>
#include <beerocks/bcl/beerocks_socket_thread.h>

#include <beerocks/tlvf/beerocks_message_header.h>

#include <mapf/common/encryption.h>
#include <tlvf/ieee_1905_1/tlvWscM1.h>
#include <tlvf/ieee_1905_1/tlvWscM2.h>

namespace son {
class slave_thread : public beerocks::socket_thread {

public:
    typedef struct {
        // from slave config file //
        std::string temp_path;
        beerocks::ePlatform platform;
        std::string bridge_iface;
        int stop_on_failure_attempts;
        bool enable_keep_alive;
        bool enable_credentials_automatic_unify;
        bool debug_disable_arp;
        bool enable_bpl_iface_status_notifications;
        bool enable_repeater_mode;
        std::string backhaul_wire_iface;
        beerocks::eIfaceType backhaul_wire_iface_type;
        std::string backhaul_wireless_iface;
        beerocks::eIfaceType backhaul_wireless_iface_type;
        bool backhaul_wireless_iface_filter_low;
        std::string backhaul_preferred_bssid;
        std::string hostap_iface;
        beerocks::eIfaceType hostap_iface_type;
        int hostap_ant_gain;
        std::string radio_identifier; //mAP RUID
    } sSlaveConfig;

    typedef struct {
        std::string gw_ipv4;
        std::string gw_bridge_mac;
        std::string controller_bridge_mac;
        std::string bridge_iface;
        std::string bridge_mac;
        std::string bridge_ipv4;
        std::string backhaul_iface;
        std::string backhaul_mac;
        std::string backhaul_ipv4;
        std::string backhaul_bssid;
        uint32_t backhaul_freq;
        uint8_t backhaul_channel;
        uint8_t backhaul_is_wireless;
        uint8_t backhaul_iface_type;
        beerocks::net::sScanResult
            backhaul_scan_measurement_list[beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH];
    } sSlaveBackhaulParams;

    enum eSlaveState {
        STATE_WAIT_BERFORE_INIT = 0,
        STATE_INIT,
        STATE_CONNECT_TO_PLATFORM_MANAGER,
        STATE_WAIT_FOR_PLATFORM_MANAGER_ONBOARD_QUERY_RESPONSE,
        STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE,
        STATE_WAIT_FOR_PLATFORM_MANAGER_CREDENTIALS_UPDATE_RESPONSE,
        STATE_CONNECT_TO_BACKHAUL_MANAGER,
        STATE_WAIT_RETRY_CONNECT_TO_BACKHAUL_MANAGER,
        STATE_WAIT_FOR_BACKHAUL_MANAGER_REGISTER_RESPONSE,
        STATE_JOIN_INIT,
        STATE_GET_WLAN_READY_STATUS,
        STATE_WAIT_FOR_WLAN_READY_STATUS_RESPONSE,
        STATE_JOIN_INIT_BRING_UP_INTERFACES,
        STATE_JOIN_INIT_WAIT_FOR_IFACE_CHANGE_DONE,
        STATE_START_AP_MANAGER,
        STATE_WAIT_FOR_AP_MANAGER_INIT_DONE_NOTIFICATION,
        STATE_WAIT_FOR_AP_MANAGER_JOINED,
        STATE_AP_MANAGER_JOINED,
        STATE_UNIFY_WIFI_CREDENTIALS,
        STATE_START_MONITOR,
        STATE_WAIT_FOR_UNIFY_WIFI_CREDENTIALS_RESPONSE,
        STATE_WAIT_FOR_MONITOR_JOINED,
        STATE_MONITOR_JOINED,
        STATE_WAIT_BEFORE_JOIN_MASTER,
        STATE_BACKHAUL_ENABLE,
        STATE_SEND_BACKHAUL_MANAGER_ENABLE,
        STATE_WAIT_FOR_BACKHAUL_MANAGER_CONNECTED_NOTIFICATION,
        STATE_BACKHAUL_MANAGER_CONNECTED,
        STATE_WAIT_BACKHAUL_MANAGER_BUSY,
        STATE_JOIN_MASTER,
        STATE_WAIT_FOR_JOINED_RESPONSE,
        STATE_UPDATE_MONITOR_SON_CONFIG,
        STATE_OPERATIONAL,
        STATE_ONBOARDING,
        STATE_WAIT_FOR_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE,
        STATE_WAIT_FOR_WIFI_CONFIGURATION_UPDATE_COMPLETE,
        STATE_WAIT_FOR_ANOTHER_WIFI_CONFIGURATION_UPDATE,
        STATE_VERSION_MISMATCH,
        STATE_SSID_MISMATCH,
        STATE_STOPPED,
    };

    slave_thread(sSlaveConfig conf, beerocks::logging &logger_);
    virtual ~slave_thread();

    virtual bool init() override;
    virtual bool work() override;

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void on_thread_stop() override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    bool
    handle_cmdu_control_message(Socket *sd,
                                std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
                                ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_backhaul_manager_message(
        Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
        ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_platform_manager_message(
        Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
        ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_ap_manager_message(
        Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
        ieee1905_1::CmduMessageRx &cmdu_rx);
    bool
    handle_cmdu_monitor_message(Socket *sd,
                                std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
                                ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_control_ieee1905_1_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);

    void process_keep_alive();

    bool slave_fsm(bool &call_slave_select);
    void slave_reset();
    void stop_slave_thread();
    void backhaul_manager_stop();
    void platform_manager_stop();
    void hostap_services_off();
    bool hostap_services_on();
    bool ap_manager_start();
    void ap_manager_stop();
    void monitor_start();
    void monitor_stop();
    void log_son_config();
    bool set_wifi_iface_state(const std::string iface,
                              beerocks::eWifiIfaceOperation iface_operation);
    bool set_radio_tx_enable(const std::string iface, bool enable);
    void platform_notify_error(int code, const std::string &error_data);
    void update_iface_status(bool is_ap, int8_t iface_status);
    void send_iface_status();
    void send_platform_iface_status_notif(beerocks::eRadioStatus radio_status,
                                          bool status_operational);
    bool monitor_heartbeat_check();
    bool ap_manager_heartbeat_check();
    bool send_cmdu_to_controller(ieee1905_1::CmduMessageTx &cmdu_tx);

private:
    const int SELECT_TIMEOUT_MSEC                                                      = 200;
    const int SLAVE_INIT_DELAY_SEC                                                     = 4;
    const int WAIT_FOR_JOINED_RESPONSE_TIMEOUT_SEC                                     = 30;
    const int WAIT_BEFORE_SEND_SLAVE_JOINED_NOTIFICATION_SEC                           = 1;
    const int WAIT_BEFORE_SEND_BH_ENABLE_NOTIFICATION_SEC                              = 1;
    const int IFACE_ACTION_TIMEOUT_SEC                                                 = 600;
    const int WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE_TIMEOUT_SEC                  = 600;
    const int STATE_WAIT_FOR_WIFI_CONFIGURATION_UPDATE_COMPLETE_TIMEOUT_SEC            = 120;
    const int STATE_WAIT_FOR_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE_TIMEOUT_SEC = 602;
    const int STATE_WAIT_FOR_RETRY_CONNECT_TO_BACKHAUL_MANAGER_SEC                     = 2;
    const int MONITOR_HEARTBEAT_TIMEOUT_SEC                                            = 10;
    const int MONITOR_HEARTBEAT_RETRIES                                                = 10;
    const int AP_MANAGER_HEARTBEAT_TIMEOUT_SEC                                         = 10;
    const int AP_MANAGER_HEARTBEAT_RETRIES                                             = 10;
    const int STATE_WAIT_FOR_WLAN_READY_STATUS_RESPONSE_TIMEOUT_SEC                    = 4;
    const int STATE_WAIT_FOR_UNIFY_WIFI_CREDENTIALS_RESPONSE_TIMEOUT_SEC               = 60;

    const int CONNECT_PLATFORM_RETRY_SLEEP     = 1000;
    const int CONNECT_PLATFORM_RETRY_COUNT_MAX = 5;

    int connect_platform_retry_counter = 0;

    std::string slave_uds;
    std::string backhaul_manager_uds;
    std::string platform_manager_uds;
    sSlaveConfig config;
    beerocks_message::sPlatformSettings platform_settings; // from platform manager //
    beerocks_message::sWlanSettings wlan_settings;         // from platform manager //
    beerocks_message::sSonConfig son_config;
    beerocks::logging &logger;
    std::string master_version;
    bool configuration_stop_on_failure_attempts;
    int stop_on_failure_attempts;
    bool stopped                                       = false;
    bool is_backhaul_disconnected                      = false;
    bool is_credentials_changed_on_db                  = false;
    bool is_wlan_credentials_unified                   = false;
    bool is_slave_reset                                = false;
    bool is_backhual_reconf                            = false;
    bool detach_on_conf_change                         = false;
    beerocks_message::sWifiCredentials new_credentials = {};

    bool is_backhaul_manager = false;

    //slave FSM //
    eSlaveState slave_state;
    bool connected_4addr_mode;
    std::chrono::steady_clock::time_point slave_state_timer;
    int keep_alive_retries = 0;
    bool hostap_params_available;
    bool wifi_configuration_in_progress;
    int slave_resets_counter = 0;

    typedef struct {
        std::string iface;
        int operation;
        std::chrono::steady_clock::time_point timestamp;
    } iface_action;
    std::unordered_map<std::string, iface_action> pending_iface_actions;

    sSlaveBackhaulParams backhaul_params;
    beerocks_message::sNodeHostap hostap_params;
    beerocks_message::sApChannelSwitch hostap_cs_params;

    std::map<uint8_t, std::set<uint8_t>> get_supported_channels_map();

    SocketClient *platform_manager_socket = nullptr;
    SocketClient *backhaul_manager_socket = nullptr;
    SocketClient *master_socket           = nullptr;

    Socket *monitor_socket    = nullptr;
    Socket *ap_manager_socket = nullptr;

    std::chrono::steady_clock::time_point master_last_seen;
    std::chrono::steady_clock::time_point monitor_last_seen;
    std::chrono::steady_clock::time_point ap_manager_last_seen;
    int monitor_retries_counter    = 0;
    int ap_manager_retries_counter = 0;

    int last_reported_backhaul_rssi = beerocks::RSSI_INVALID;

    ap_manager_thread *ap_manager = nullptr;

    beerocks::eRadioStatus iface_status_ap            = beerocks::eRadioStatus::INVALID;
    beerocks::eRadioStatus iface_status_ap_prev       = beerocks::eRadioStatus::INVALID;
    beerocks::eRadioStatus iface_status_bh            = beerocks::eRadioStatus::INVALID;
    beerocks::eRadioStatus iface_status_bh_prev       = beerocks::eRadioStatus::INVALID;
    beerocks::eRadioStatus iface_status_bh_wired      = beerocks::eRadioStatus::INVALID;
    beerocks::eRadioStatus iface_status_bh_wired_prev = beerocks::eRadioStatus::INVALID;
    bool iface_status_operational_state               = false;
    bool iface_status_operational_state_prev          = false;

    // Encryption support - move to common library
    bool autoconfig_wsc_calculate_keys(std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                       uint8_t authkey[32], uint8_t keywrapkey[16]);
    bool autoconfig_wsc_parse_m2_encrypted_settings(std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                                    uint8_t authkey[32], uint8_t keywrapkey[16],
                                                    std::string &ssid, sMacAddr &bssid,
                                                    WSC::eWscAuth &auth_type,
                                                    WSC::eWscEncr &encr_type, bool &backhaul,
                                                    bool &fronthaul, bool &teardown);
    bool autoconfig_wsc_authenticate(std::shared_ptr<ieee1905_1::tlvWscM2> m2, uint8_t authkey[32]);

    std::unique_ptr<mapf::encryption::diffie_hellman> dh = nullptr;
    //copy of M1 message used for authentication
    uint8_t *m1_auth_buf   = nullptr;
    size_t m1_auth_buf_len = 0;

    bool parse_intel_join_response(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool parse_non_intel_join_response(Socket *sd);
    bool handle_autoconfiguration_wsc(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_autoconfiguration_renew(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool autoconfig_wsc_add_m1();
    bool add_radio_basic_capabilities();
    bool handle_ap_metrics_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_link_metrics_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_channel_preference_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_channel_selection_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_multi_ap_policy_config_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_client_capability_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_ap_capability_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_client_association_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_client_steering_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_ack_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
};

} // namespace son

#endif
