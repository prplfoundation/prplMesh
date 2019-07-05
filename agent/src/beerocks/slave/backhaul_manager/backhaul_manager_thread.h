/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BACKHAUL_MANAGER_THREAD_H
#define _BACKHAUL_MANAGER_THREAD_H

#include "wan_monitor.h"

#include <beerocks/btl/btl.h>

#include <beerocks/bwl/sta_wlan_hal.h>

#include <beerocks/bcl/beerocks_socket_thread.h>
#include <beerocks/bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message_header.h>

#include <future>
#include <list>

namespace beerocks {
namespace backhaul_manager {

class main_thread : public btl::transport_socket_thread {

public:
    main_thread(std::string temp_path_, std::set<std::string> slave_ap_ifaces_,
                std::set<std::string> slave_sta_ifaces_, int stop_on_failure_attempts,
                std::string const_bh_slave_);
    ~main_thread();

    virtual bool init() override;
    virtual bool work() override;

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void after_select(bool timeout) override;
    virtual void on_thread_stop() override;
    virtual void socket_connected(Socket *sd) override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    // Forward declaration
    struct SSlaveSockets;

    bool backhaul_fsm_main(bool &skip_select);
    bool backhaul_fsm_wired(bool &skip_select);
    bool backhaul_fsm_wireless(bool &skip_select);
    bool is_front_radio(std::string mac);
    bool finalize_slaves_connect_state(
        bool fConnected, std::shared_ptr<SSlaveSockets> pSocket = nullptr); // cmdu_duplicate

    bool send_autoconfig_search_message(std::shared_ptr<SSlaveSockets> soc);

    // cmdu handlers
    bool handle_master_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                               std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header);
    bool handle_slave_backhaul_message(std::shared_ptr<SSlaveSockets> soc,
                                       ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_1905_1_message(ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &src_mac);

    // 1905 messages handlers
    bool handle_1905_autoconfiguration_response(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                const std::string &src_mac);

    //bool sta_handle_event(const std::string &iface,const std::string& event_name, void* event_obj);
    bool hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr, std::string iface);

    bool is_eth_link_up();
    void get_scan_measurement();
    bool select_bssid();
    void platform_notify_error(int code, const std::string &error_data);

    std::shared_ptr<bwl::sta_wlan_hal> get_wireless_hal(std::string iface = "");

private:
    std::string beerocks_temp_path;

    struct SSlaveSockets {
        Socket *slave = nullptr;
        std::string radio_mac;
        std::string hostap_iface;
        std::string sta_iface;
        beerocks::eFreqType freq_type  = beerocks::eFreqType::FREQ_UNKNOWN;
        bool sta_iface_filter_low      = false;
        bool slave_is_backhaul_manager = false;
        bool controller_discovered     = false;
        beerocks::eIfaceType slave_iface_type;

        std::shared_ptr<bwl::sta_wlan_hal> sta_wlan_hal;
        Socket *sta_hal_ext_events = nullptr;
        Socket *sta_hal_int_events = nullptr;
    };

    SocketClient *master_discovery_socket = nullptr;

    struct SBackhaulConfig {

        // Current connection type
        enum class EType {
            Invalid = 0, //!< Invalid connection
            Wired,       //!< Wired connection
            Wireless     //!< Wireless connection

        } eType;

        bool wired_backhaul;

        std::string ssid;
        std::string pass;
        std::string bridge_iface;
        std::string wire_iface;
        std::string wireless_iface;
        std::string preferred_bssid;
        beerocks::eIfaceType wire_iface_type;
        beerocks::eIfaceType wireless_iface_type;
        bwl::WiFiSec security_type;

        // Slave handling the active wireless connection
        // std::shared_ptr<bwl::sta_wlan_hal> active_slave_hal;
        std::unordered_map<std::string, std::shared_ptr<SSlaveSockets>> slave_iface_socket;

    } m_sConfig;

    Socket *unassociated_measurement_slave_soc  = nullptr;
    int unassociated_rssi_measurement_header_id = -1;

    //comes from config file
    const std::set<std::string> slave_ap_ifaces;
    const std::set<std::string> slave_sta_ifaces;

    //used for several states independently
    std::set<std::string> pending_slave_ifaces;
    std::set<std::string> pending_slave_sta_ifaces;

    std::list<std::shared_ptr<SSlaveSockets>> slaves_sockets;
    std::shared_ptr<SocketClient> m_scPlatform;
    net::network_utils::iface_info bridge_info;

    int configuration_stop_on_failure_attempts;
    std::string config_const_bh_slave;
    int stop_on_failure_attempts;
    bool local_master = false;
    bool local_gw     = false;
    bool onboarding   = true;

    //backlist bssid and timers (remove con(or wrong passphrase) ap from select bssid for limited time )
    struct ap_blacklist_entry {
        std::chrono::steady_clock::time_point timestamp;
        int attempts = 0;
    };
    std::unordered_map<std::string, ap_blacklist_entry> ap_blacklist;

    wan_monitor wan_mon;

    // Future to hold the DHCP client process exit code
    std::future<int> m_ftDHCPRetCode;

    // state switch mechanism
    const int SELECT_TIMEOUT_MSC                      = 500;
    const int DEVICE_QUERY_RESPONSE_TIMEOUT_SECONDS   = 3;
    const int WAIT_FOR_SCAN_RESULTS_TIMEOUT_SECONDS   = 20;
    const int WPA_ATTACH_TIMEOUT_SECONDS              = 5;
    const int CONNECTING_TO_MASTER_TIMEOUT_SECONDS    = 30;
    const int MAX_FAILED_SCAN_ATTEMPTS                = 3;
    const int MAX_FAILED_ROAM_SCAN_ATTEMPTS           = 4;
    const int MAX_FAILED_DHCP_ATTEMPTS                = 2;
    const int MAX_WIRELESS_ASSOCIATE_TIMEOUT_SECONDS  = 10;
    const int MAX_WIRELESS_ASSOCIATE_3ADDR_ATTEMPTS   = 2;
    const int POLL_TIMER_TIMEOUT_MS                   = 1000;
    const int WIRELESS_WAIT_FOR_RECONNECT_TIMEOUT     = 2;
    const int RSSI_POLL_INTERVAL_MS                   = 1000;
    const int STATE_WAIT_ENABLE_TIMEOUT_SECONDS       = 600;
    const int AP_BLACK_LIST_TIMEOUT_SECONDS           = 120;
    const int AP_BLACK_LIST_FAILED_ATTEMPTS_THRESHOLD = 2;
    const int INTERFACE_BRING_UP_TIMEOUT_SECONDS      = 600;
    const int DEAUTH_REASON_PASSPHRASE_MISMACH        = 2;
    const int AUTOCONFIG_DISCOVERY_TIMEOUT_SECONDS    = 1;
    const int MAX_FAILED_AUTOCONFIG_SEARCH_ATTEMPTS   = 20;

    std::chrono::steady_clock::time_point state_time_stamp_timeout;
    int state_attempts;

    bool hidden_ssid = false;

    std::string selected_bssid;
    int selected_bssid_channel;
    std::string roam_selected_bssid;
    int roam_selected_bssid_channel;
    bool roam_flag = false;
    std::unordered_map<std::string, net::sScanResult> scan_measurement_list;

    const int RSSI_THRESHOLD_5GHZ       = -80;
    const int RSSI_BAND_DELTA_THRESHOLD = 5;

    std::chrono::steady_clock::time_point rssi_poll_timer;
    std::chrono::steady_clock::time_point eth_link_poll_timer;
    bool m_eth_link_up       = false;
    bool pending_enable      = false;
    bool wifi_reconnect_flag = false;

    std::string controller_bridge_mac;
    std::string bssid_bridge_mac;

    /*
 * State Machines
 */
private:
// Helper MACROs for Enum/String generation
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_STATE(STATE)                                                                       \
    STATE(INIT)                                                                                    \
    STATE(WAIT_ENABLE)                                                                             \
    STATE(ENABLED)                                                                                 \
                                                                                                   \
    STATE(_WIRED_START_)                                                                           \
    STATE(WIRED_BRIDGE_DHCP)                                                                       \
    STATE(WIRED_BRIDGE_DHCP_WAIT)                                                                  \
    STATE(_WIRED_END_)                                                                             \
                                                                                                   \
    STATE(_WIRELESS_START_)                                                                        \
    STATE(INIT_HAL)                                                                                \
    STATE(WPA_ATTACH)                                                                              \
    STATE(INITIATE_SCAN)                                                                           \
    STATE(WAIT_FOR_SCAN_RESULTS)                                                                   \
    STATE(WIRELESS_CONFIG_4ADDR_MODE)                                                              \
    STATE(WIRELESS_ASSOCIATE_4ADDR)                                                                \
    STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)                                                           \
    STATE(WIRELESS_BRIDGE_DHCP)                                                                    \
    STATE(WIRELESS_BRIDGE_DHCP_WAIT)                                                               \
    STATE(WIRELESS_WAIT_FOR_RECONNECT)                                                             \
    STATE(_WIRELESS_END_)                                                                          \
                                                                                                   \
    STATE(MASTER_DISCOVERY)                                                                        \
    STATE(SEND_AUTOCONFIG_SEARCH_MESSAGE)                                                          \
    STATE(WAIT_FOR_AUTOCONFIG_RESPONSE_MESSAGE)                                                    \
    STATE(CONNECT_TO_MASTER)                                                                       \
    STATE(CONNECTED)                                                                               \
    STATE(OPERATIONAL)                                                                             \
    STATE(RESTART)                                                                                 \
    STATE(STOPPED)

    // States ENUM
    enum class EState { FOREACH_STATE(GENERATE_ENUM) };

    // States Strings Array
    static const char *s_arrStates[];

    EState m_eFSMState;
};

} // namespace backhaul_manager
} // namespace beerocks

#endif // _BACKHAUL_MANAGER_THREAD_H
