/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BACKHAUL_MANAGER_THREAD_H
#define _BACKHAUL_MANAGER_THREAD_H

#include "wan_monitor.h"

#include <bcl/beerocks_backport.h>
#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_socket_thread.h>
#include <bcl/network/network_utils.h>
#include <btl/btl.h>
#include <bwl/sta_wlan_hal.h>

#include <beerocks/tlvf/beerocks_message_header.h>

#include <tlvf/ieee_1905_1/eMediaType.h>

#include "../agent_ucc_listener.h"
#include "../link_metrics/link_metrics.h"

#include <future>
#include <list>

namespace beerocks {

namespace bpl {
enum class eErrorCode;
}

class backhaul_manager : public btl::transport_socket_thread {

public:
    backhaul_manager(const config_file::sConfigSlave &config,
                     const std::set<std::string> &slave_ap_ifaces_,
                     const std::set<std::string> &slave_sta_ifaces_, int stop_on_failure_attempts_);
    ~backhaul_manager();

    virtual bool init() override;
    virtual bool work() override;

    // For agent_ucc_listener
    const std::string &get_controller_bridge_mac() { return controller_bridge_mac; }

private:
    // Forward declaration
    struct sRadioInfo;

    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void before_select() override;
    virtual void after_select(bool timeout) override;
    virtual void on_thread_stop() override;
    virtual void socket_connected(Socket *sd) override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

    bool backhaul_fsm_main(bool &skip_select);
    bool backhaul_fsm_wired(bool &skip_select);
    bool backhaul_fsm_wireless(bool &skip_select);
    bool is_front_radio(std::string mac);
    bool
    finalize_slaves_connect_state(bool fConnected,
                                  std::shared_ptr<sRadioInfo> pSocket = nullptr); // cmdu_duplicate

    bool send_autoconfig_search_message(std::shared_ptr<sRadioInfo> soc);
    bool send_1905_topology_discovery_message();

    // cmdu handlers
    bool handle_master_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                               std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header);
    bool handle_slave_backhaul_message(std::shared_ptr<sRadioInfo> soc,
                                       ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_1905_1_message(ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &src_mac);

    // 1905 messages handlers
    bool handle_1905_topology_discovery(const std::string &src_mac,
                                        ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_1905_autoconfiguration_response(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                const std::string &src_mac);
    bool handle_1905_topology_query(ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &src_mac);
    bool handle_1905_higher_layer_data_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                                               const std::string &src_mac);
    bool handle_1905_link_metric_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                       const std::string &src_mac);
    bool handle_1905_combined_infrastructure_metrics(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                     const std::string &src_mac);
    bool handle_ap_capability_query(ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &src_mac);
    bool handle_client_capability_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                        const std::string &src_mac);
    //bool sta_handle_event(const std::string &iface,const std::string& event_name, void* event_obj);
    bool hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr, std::string iface);

    bool is_eth_link_up();
    void get_scan_measurement();
    bool select_bssid();
    void platform_notify_error(bpl::eErrorCode code, const std::string &error_data);
    bool send_slaves_enable();

    void remove_client_from_all_radios(sMacAddr &client_mac);

    std::shared_ptr<bwl::sta_wlan_hal> get_wireless_hal(std::string iface = "");

private:
    const std::string &beerocks_temp_path;

    SocketClient *master_discovery_socket = nullptr;

    struct SBackhaulConfig {

        // Current connection type
        enum class EType {
            Invalid = 0, //!< Invalid connection
            Wired,       //!< Wired connection
            Wireless     //!< Wireless connection

        } eType;

        std::string ssid;
        std::string pass;
        std::string bridge_iface;
        std::string wire_iface;
        std::string wireless_iface;
        std::string preferred_bssid;
        std::string vendor;
        std::string model;
        int ucc_listener_port;
        beerocks::eIfaceType wire_iface_type;
        beerocks::eIfaceType wireless_iface_type;
        bwl::WiFiSec security_type;
        bool mem_only_psk;
        eFreqType backhaul_preferred_radio_band;

        // Slave handling the active wireless connection
        // std::shared_ptr<bwl::sta_wlan_hal> active_slave_hal;
        std::unordered_map<std::string, std::shared_ptr<sRadioInfo>> slave_iface_socket;

    } m_sConfig;

    Socket *unassociated_measurement_slave_soc  = nullptr;
    int unassociated_rssi_measurement_header_id = -1;

    //comes from config file
    const std::set<std::string> &slave_ap_ifaces;
    const std::set<std::string> &slave_sta_ifaces;

    //used for several states independently
    std::set<std::string> pending_slave_ifaces;
    std::set<std::string> pending_slave_sta_ifaces;

    std::list<std::shared_ptr<sRadioInfo>> slaves_sockets;
    std::shared_ptr<SocketClient> m_scPlatform;
    net::network_utils::iface_info bridge_info;

    int configuration_stop_on_failure_attempts;
    const std::string &config_const_bh_slave;

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
    const int DISCOVERY_NEIGHBOUR_REMOVAL_TIMEOUT =
        ieee1905_1_consts::DISCOVERY_NOTIFICATION_TIMEOUT_SEC + 3; // 3 seconds grace period

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
    bool m_eth_link_up  = false;
    bool pending_enable = false;

    std::string controller_bridge_mac;
    bool is_prplmesh_controller = false;
    std::string bssid_bridge_mac;

    std::unique_ptr<beerocks::agent_ucc_listener> m_agent_ucc_listener;

    struct sClientInfo {
        sMacAddr client_mac;
        sMacAddr bssid; // VAP mac
        std::chrono::steady_clock::time_point time_stamp;
        size_t asso_len;
        uint8_t assoc_req[ASSOCIATION_FRAME_SIZE];
    };

    /**
     * @brief Type definition for associated clients information.
     *
     * Associated client information consists of sClientInfo strucrt, which has the 
     * following fields:
     * - The MAC address of the 802.11 client that associates to a BSS.
     * - Timestamp of the 802.11 client's last association to this Multi-AP device.
     * - The length of the association frame.
     * - the association frame itself.
     *
     * Associated client information is gathered from
     * ACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION events received from slave threads.
     *
     * Associated client information is later used to fill in the Associated Clients TLV
     * in the Topology Response message and Client Capability Response message.
     */

    typedef std::unordered_map<sMacAddr, sClientInfo> associated_clients_t;

    /**
     * @brief Information gathered about a radio (= slave).
     *
     * Radio information is obtained from messages sent by slave threads and is used to build
     * the TLVs to include in notification messages or responses to CDMU query messages.
     */
    struct sRadioInfo {
        Socket *slave = nullptr;  /**< Socket connection to the slave */
        sMacAddr radio_mac;       /**< Radio ID (= radio MAC address) */
        std::string hostap_iface; /**< Name of the radio interface */
        std::string sta_iface;    /**< Name of the bSTA interface on the radio (if any) */
        eFreqType freq_type            = eFreqType::FREQ_UNKNOWN;
        bool sta_iface_filter_low      = false;
        bool slave_is_backhaul_manager = false;
        bool controller_discovered     = false;

        std::shared_ptr<bwl::sta_wlan_hal> sta_wlan_hal;
        Socket *sta_hal_ext_events = nullptr;
        Socket *sta_hal_int_events = nullptr;

        eWiFiBandwidth max_bandwidth =
            eWiFiBandwidth::BANDWIDTH_UNKNOWN; /**< Maximum supported bandwidth */
        bool ht_supported      = false;        /**< Is HT supported flag */
        uint16_t ht_capability = 0;            /**< HT capabilities */
        std::array<uint8_t, beerocks::message::HT_MCS_SET_SIZE>
            ht_mcs_set; /**< 16-byte attribute containing the MCS set as defined in 802.11n */
        bool vht_supported      = false; /**< Is VHT supported flag */
        uint32_t vht_capability = 0;     /**< VHT capabilities */
        std::array<uint8_t, beerocks::message::VHT_MCS_SET_SIZE>
            vht_mcs_set; /**< 32-byte attribute containing the MCS set as defined in 802.11ac */
        bool he_supported = false;             /**< Is HE supported flag */
        beerocks_message::sVapsList vaps_list; /**< List of VAPs in radio. */
        std::array<beerocks::message::sWifiChannel, beerocks::message::SUPPORTED_CHANNELS_LENGTH>
            supported_channels; /**< Array of supported channels in radio. */
        std::unordered_map<sMacAddr, associated_clients_t>
            associated_clients_map; /**< Associated clients grouped by BSSID. */
    };

    /**
     * @brief Interface in this device which connects to an interface in one or more neighbors.
     *
     * An interface is defined by its name, its MAC address and its MediaType as
     * defined in IEEE Std 1905.1, Table 6-12—Media type (intfType).
     */
    struct sLinkInterface {
        std::string iface_name; /**< The name of the interface. */
        sMacAddr iface_mac =
            beerocks::net::network_utils::ZERO_MAC; /**< The MAC address of the interface. */
        ieee1905_1::eMediaType media_type = ieee1905_1::eMediaType::
            UNKNONWN_MEDIA; /**< The underlying network technology of the connecting interface. */
        bool operator<(const sLinkInterface &rhs) const { return iface_name < rhs.iface_name; }
    };

    /**
     * @brief Neighbor 1905.1 device which connects to an interface in this device.
     *
     * A neighbor is defined by its 1905.1 AL MAC address and the MAC address of the interface in
     * the neighbor that connects to this device.
     */
    struct sLinkNeighbor {
        sMacAddr al_mac =
            beerocks::net::network_utils::ZERO_MAC; /**< The MAC address of the 1905.1 AL. */
        sMacAddr iface_mac =
            beerocks::net::network_utils::ZERO_MAC; /**< The MAC address of the interface. */
    };

    /**
     * @brief Gets media type group for given interface.
     *
     * @param[in] interface_name Name of the local interface.
     * @param[in, out] media_type_group The media type group of the connecting interface.
     *
     * @return True on success and false otherwise.
     */
    bool get_media_type_group(const std::string &interface_name,
                              ieee1905_1::eMediaTypeGroup &media_type_group);

    /**
     * @brief Gets media type for given interface.
     *
     * The mechanism to use to obtain media type depends on the media type group:
     * Ethernet, WiFi, MoCA, etc.
     *
     * @param[in] interface_name Name of the local interface.
     * @param[in] media_type_group The media type group of the connecting interface.
     * @param[in, out] media_type The underlying network technology of the connecting interface.
     *
     * @return True on success and false otherwise.
     */
    bool get_media_type(const std::string &interface_name,
                        ieee1905_1::eMediaTypeGroup media_type_group,
                        ieee1905_1::eMediaType &media_type);

    /**
     * @brief Creates a new link metrics collector for given media type.
     *
     * Creates a new link metrics collector suitable for the underlying network technology of the
     * connecting interface.
     * Collector choice depends on bits 15 to 8 of media type, that is, the media type group.
     *
     * @param[in] iface_mac MAC address of the connecting interface.
     * @param[in] media_type The underlying network technology of the connecting interface.
     *
     * @return Link metrics collector on success and nullptr otherwise.
     */
    std::unique_ptr<link_metrics_collector>
    create_link_metrics_collector(const sLinkInterface &link_interface) const;

    /**
     * @brief Gets the list of neighbor links from topology database.
     *
     * Neighbor links are pairs (interface, neighbor) where 'interface' is the name of the interface
     * that connects to the neighbor device and 'neighbor' is the MAC address of the neighbor device.
     *
     * @param[in] neighbor_mac_filter Optional MAC address to filter the neighbor links to be
     * returned. A value of network_utils::ZERO_MAC means no filter has to be applied. A specific
     * MAC address means that only links to that device must be included.
     * @param[in, out] neighbor_links_map Map containing lists of neighbors grouped by the interface
     * that connects to them.
     *
     * @return True on success and false otherwise.
     */
    bool get_neighbor_links(const sMacAddr &neighbor_mac_filter,
                            std::map<std::string, std::vector<sMacAddr>> &neighbor_links_map);
    /*
     * @brief List of known 1905 neighbor devices
     * 
     * key:     1905.1 device AL-MAC
     * value:   Last timestamp receiving discovery message from AL-MAC
     * Devices are being added to the list when receiving a 1905.1 Topology Discovery message from
     * an unknown 1905.1 device. Every 1905.1 device shall send this message every 60 seconds, and
     * we update the time stamp in which the message is received.
     */
    std::unordered_map<sMacAddr, std::chrono::steady_clock::time_point> m_1905_neighbor_devices;

    /**
     * @brief Adds an AP HT Capabilities TLV to AP Capability Report message.
     *
     * TLV is added to message only if given radio supports HT capabilities.
     * See section 17.2.8 of Multi-AP Specification for details.
     *
     * @param radio_info Radio structure containing the information required to fill in the TLV.
     *
     * @return True on success and false otherwise.
     */
    bool add_ap_ht_capabilities(const sRadioInfo &radio_info);

    /**
     * @brief Adds an AP VHT Capabilities TLV to AP Capability Report message.
     *
     * TLV is added to message only if given radio supports VHT capabilities.
     * See section 17.2.9 of Multi-AP Specification for details.
     *
     * @param radio_info Radio structure containing the information required to fill in the TLV.
     *
     * @return True on success and false otherwise.
     */
    bool add_ap_vht_capabilities(const sRadioInfo &radio_info);

    /**
     * @brief Adds an AP HE Capabilities TLV to AP Capability Report message.
     *
     * TLV is added to message only if given radio supports HE capabilities.
     * See section 17.2.10 of Multi-AP Specification for details.
     *
     * @param radio_info Radio structure containing the information required to fill in the TLV.
     *
     * @return True on success and false otherwise.
     */
    bool add_ap_he_capabilities(const sRadioInfo &radio_info);

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
    STATE(_WIRELESS_START_)                                                                        \
    STATE(INIT_HAL)                                                                                \
    STATE(WPA_ATTACH)                                                                              \
    STATE(INITIATE_SCAN)                                                                           \
    STATE(WAIT_FOR_SCAN_RESULTS)                                                                   \
    STATE(WIRELESS_CONFIG_4ADDR_MODE)                                                              \
    STATE(WIRELESS_ASSOCIATE_4ADDR)                                                                \
    STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)                                                           \
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

} // namespace beerocks

#endif // _BACKHAUL_MANAGER_THREAD_H
