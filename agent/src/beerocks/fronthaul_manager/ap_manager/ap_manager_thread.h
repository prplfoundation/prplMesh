/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _AP_MANAGER_THREAD_H
#define _AP_MANAGER_THREAD_H

#include <bcl/beerocks_socket_thread.h>

// AP HAL
#include <bwl/ap_wlan_hal.h>

#include <bcl/beerocks_logging.h>

#include <beerocks/tlvf/beerocks_message_apmanager.h>
#include <list>
#include <set>

namespace son {
class ap_manager_thread : public beerocks::socket_thread {

public:
    ap_manager_thread(const std::string &slave_uds_, const std::string &iface,
                      beerocks::logging &logger);
    virtual ~ap_manager_thread();

    struct ap_manager_conf_t {
        std::string hostap_iface;
        beerocks::eIfaceType hostap_iface_type;
        int channel;
        bool iface_filter_low;
    };

    void ap_manager_config(ap_manager_conf_t &conf);

    /**
     * disallowed client parameters
     * Used to save clients mac, bssid that the client is disallowed from and 
     * validity period of time (blocking) 
     * so we could unblock it when the period expires
     */
    struct disallowed_client_t {
        sMacAddr mac;
        sMacAddr bssid;
        std::chrono::steady_clock::time_point timeout;
    };

    enum eThreadErrors : uint32_t {
        APMANAGER_THREAD_ERROR_NO_ERROR            = 0,
        APMANAGER_THREAD_ERROR_HOSTAP_DISABLED     = 1,
        APMANAGER_THREAD_ERROR_ATTACH_FAIL         = 2,
        APMANAGER_THREAD_ERROR_SUDDEN_DETACH       = 3,
        APMANAGER_THREAD_ERROR_HAL_DISCONNECTED    = 4,
        APMANAGER_THREAD_ERROR_CAC_TIMEOUT         = 5,
        APMANAGER_THREAD_ERROR_REPORT_PROCESS_FAIL = 6,
    };

protected:
    virtual bool init() override;
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void after_select(bool timeout) override;
    virtual void on_thread_stop() override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    bool hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr);
    // bool hostap_handle_event(std::string& event, void* event_obj);
    void handle_hostapd_attached();
    bool handle_ap_enabled(int vap_id);
    void fill_cs_params(beerocks_message::sApChannelSwitch &params);
    void stop_ap_manager_thread();
    void send_heartbeat();
    void send_steering_return_status(beerocks_message::eActionOp_APMANAGER ActionOp,
                                     int32_t status);
    void remove_client_from_disallowed_list(const sMacAddr &mac, const sMacAddr &bssid);
    void allow_expired_clients();

    // Class constants
    static constexpr uint8_t BEACON_TRANSMIT_TIME_MS = 100;
    static constexpr uint8_t BSS_STEER_IMMINENT_VALID_INT_BTT =
        (beerocks::BSS_STEER_DISASSOC_TIMER_MS / BEACON_TRANSMIT_TIME_MS);
    static constexpr uint8_t BSS_STEER_VALID_INT_BTT = 2; // 200ms

    std::string slave_uds;
    std::string m_iface;
    beerocks::logging &m_logger;
    uint8_t wifi_channel;
    bool acs_enabled;
    bool low_filter;

    int bss_steer_valid_int          = BSS_STEER_VALID_INT_BTT;
    int bss_steer_imminent_valid_int = BSS_STEER_IMMINENT_VALID_INT_BTT;
    std::vector<disallowed_client_t> m_disallowed_clients;

    struct pending_disable_vap_t {
        int8_t vap_id;
        std::chrono::steady_clock::time_point timeout;
    };

    std::list<pending_disable_vap_t> pending_disable_vaps;

    Socket *slave_socket      = nullptr;
    Socket *ap_hal_ext_events = nullptr;
    Socket *ap_hal_int_events = nullptr;

    int sta_unassociated_rssi_measurement_header_id = -1;

    std::shared_ptr<bwl::ap_wlan_hal> ap_wlan_hal;

    //FIXME temp 4addr stuff
    Socket *four_addr_listener_socket = nullptr;
    struct pending_4addr_sta_t {
        std::string mac;
        std::chrono::steady_clock::time_point joined_timestamp;
        bool pending_reconnect = false;
    };
    std::unordered_map<std::string, pending_4addr_sta_t> pending_4addr_stas;
    //use to diff between ire and client
    std::set<std::string> connected_ires;

    std::chrono::steady_clock::time_point next_heartbeat_notification_timestamp;
    std::chrono::steady_clock::time_point next_tx_polling_timestamp;

    const uint8_t HEARTBEAT_NOTIFICATION_DELAY_SEC = 1;

    bool acs_completed_vap_update = false;
};

} // namespace son

#endif
