/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MONITOR_THREAD_H
#define MONITOR_THREAD_H

#include "monitor_rssi.h"
#ifdef BEEROCKS_RDKB
#include "rdkb/monitor_rdkb_hal.h"
#endif
#include "monitor_db.h"
#include "monitor_stats.h"

#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_socket_thread.h>

#include <beerocks/tlvf/beerocks_message_monitor.h>

#include <tlvf/wfa_map/tlvMetricReportingPolicy.h>

// Monitor HAL
#include <bwl/mon_wlan_hal.h>

namespace son {
class monitor_thread : public beerocks::socket_thread {
public:
    monitor_thread(const std::string &slave_uds_, const std::string &monitor_iface_,
                   beerocks::config_file::sConfigSlave &beerocks_slave_conf_,
                   beerocks::logging &logger_);
    virtual ~monitor_thread();

    virtual bool init() override;

    enum eThreadErrors : uint32_t {
        MONITOR_THREAD_ERROR_NO_ERROR                      = 0,
        MONITOR_THREAD_ERROR_HOSTAP_DISABLED               = 1,
        MONITOR_THREAD_ERROR_ATTACH_FAIL                   = 2,
        MONITOR_THREAD_ERROR_SUDDEN_DETACH                 = 3,
        MONITOR_THREAD_ERROR_HAL_DISCONNECTED              = 4,
        MONITOR_THREAD_ERROR_REPORT_PROCESS_FAIL           = 5,
        MONITOR_THREAD_ERROR_NL_ATTACH_FAIL                = 6,
        MONITOR_THREAD_ERROR_NL_REPORT_PROCESS_FAIL        = 7,
        MONITOR_THREAD_ERROR_NL_EVENTS_SOCKET_DISCONNECTED = 8,
    };

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void before_select() override;
    virtual void after_select(bool timeout) override;
    virtual void on_thread_stop() override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    void stop_monitor_thread();
    bool hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr);

    /**
     * @brief Executes when channel utilization measurement period has elapsed.
     *
     * This method is periodically invoked if AP Metrics Channel Utilization Reporting Threshold
     * has been set to a non-zero value in last received Metric Reporting Policy TLV.
     * Measurement period is set to an implementation-specific value.
     * On invocation, method shall measure current channel utilization on the radio. If difference
     * with respect to the previous measurement has crossed the reporting threshold, it shall send
     * an AP Metrics Response message to the controller.
     */
    void on_channel_utilization_measurement_period_elapsed();

    /**
     * @brief Creates AP Metrics Response message
     *
     * @param mid Message ID.
     * @param bssid_list list of BSSID of BSS operated by the Multi-AP Agent to include in the AP
     * Metrics Response message.
     * @return True on success and false otherwise.
     */
    bool create_ap_metrics_response(uint16_t mid, const std::vector<sMacAddr> &bssid_list);

    bool update_ap_stats();
    bool update_sta_stats();

    void debug_channel_load_11k_request(beerocks_message::sStaChannelLoadRequest11k &request);
    void debug_channel_load_11k_response(beerocks_message::sStaChannelLoadResponse11k &event);
    void debug_beacon_11k_request(beerocks_message::sBeaconRequest11k &request);
    void debug_beacon_11k_response(beerocks_message::sBeaconResponse11k &event);
    void debug_statistics_11k_request(beerocks_message::sStatisticsRequest11k &request);
    void debug_statistics_11k_response(beerocks_message::sStatisticsResponse11k &event);
    void
    debug_link_measurements_11k_response(beerocks_message::sLinkMeasurementsResponse11k &event);

    void send_heartbeat();
    void update_vaps_in_db();
#ifdef BEEROCKS_RDKB
    void send_steering_return_status(beerocks_message::eActionOp_MONITOR ActionOp, int32_t status);
#endif
    beerocks_message::sSonConfig son_config;

    /**
     * MAC address of the radio interface that this monitor instance operates on.
     * It is obtained by getting the MAC address of the monitor interface provided as parameter in
     * constructor.
     */
    sMacAddr m_radio_mac;

    std::string monitor_iface;
    beerocks::config_file::sConfigSlave &beerocks_slave_conf;
    std::string bridge_iface;
    std::string slave_uds;
    Socket *slave_socket       = nullptr;
    Socket *mon_hal_ext_events = nullptr;
    Socket *mon_hal_int_events = nullptr;
    Socket *mon_hal_nl_events  = nullptr;
    beerocks::logging &logger;
    bool m_logger_configured = false;

    typedef struct {
        std::string sta_mac;
        int dialog_token;
        std::chrono::steady_clock::time_point timestamp;
        int id;
    } sEvent11k;
    std::unordered_multimap<std::string, sEvent11k> pending_11k_events;

    monitor_db mon_db;
    monitor_rssi mon_rssi;
#ifdef BEEROCKS_RDKB
    monitor_rdkb_hal mon_rdkb_hal;
#endif
    monitor_stats mon_stats;

    int8_t received_error_notification_ack_retry = -1;

    int hal_command_failures_count = 0;

    std::shared_ptr<bwl::mon_wlan_hal> mon_wlan_hal;
    bool mon_hal_attached           = false;
    bwl::HALState last_attach_state = bwl::HALState::Uninitialized;

    std::chrono::steady_clock::time_point m_sta_stats_polling_start_timestamp;
    bool m_sta_stats_polling_completed = true;

    bool handle_cmdu_vs_message(Socket &sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_ieee1905_1_message(Socket &sd, ieee1905_1::CmduMessageRx &cmdu_rx);

    bool handle_multi_ap_policy_config_request(Socket &sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_ap_metrics_query(Socket &sd, ieee1905_1::CmduMessageRx &cmdu_rx);
};
} // namespace son

#endif
