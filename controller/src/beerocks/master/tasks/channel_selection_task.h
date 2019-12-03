/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CHANNEL_SELECTION_TASK_H_
#define _CHANNEL_SELECTION_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

#include <beerocks/tlvf/beerocks_message.h>

#include <memory>

// Helper MACROs for Enum/String generation
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define CHANNEL_SELECTION_ALLOCATE_EVENT(s_event) ((s_event *)(new uint8_t[sizeof(s_event)]))
#define CHANNEL_20MHZ_STEP 4
#define CHANNEL_80MHZ_STEP 12

namespace son {

class channel_selection_task : public task, public std::enable_shared_from_this<task> {
public:
    channel_selection_task(db &database, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_);
    virtual ~channel_selection_task() {}

    typedef struct {
        sMacAddr hostap_mac;
    } sHostapChannelRequest_event;

    typedef struct {
        sMacAddr hostap_mac;
        uint8_t success;
    } sRestrictedChannelResponse_event;

    typedef struct {
        sMacAddr hostap_mac;
        beerocks_message::sApChannelSwitch cs_params;
    } sCsa_event;

    typedef struct {
        sMacAddr hostap_mac;
        beerocks_message::sDfsCacCompleted params;
    } sCacCompleted_event;

    typedef struct {
        sMacAddr hostap_mac;
        beerocks_message::sDfsChannelAvailable params;
    } sDfsChannelAvailable_event;
    typedef struct {
        sMacAddr hostap_mac;
        beerocks_message::sApChannelSwitch cs_params;

        beerocks_message::sWifiChannel
            supported_channels[beerocks::message::SUPPORTED_CHANNELS_LENGTH];
    } sAcsResponse_event;

    typedef struct {
        sMacAddr hostap_mac;
        bool low_pass_filter_on;
        bool backhaul_is_wireless;
        uint8_t backhaul_channel;
        uint8_t channel;
        beerocks_message::sApChannelSwitch cs_params;
        beerocks_message::sWifiChannel
            supported_channels[beerocks::message::SUPPORTED_CHANNELS_LENGTH];
        beerocks::net::sScanResult
            backhaul_scan_measurement_list[beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH];
    } sSlaveJoined_event;

    typedef struct {
        sMacAddr hostap_mac;
    } sApActivityIdle_event;

    typedef struct {
        sMacAddr hostap_mac;
        std::chrono::steady_clock::time_point timestamp;
        bool timeout_expired;
    } sDfsReEntrySampleSteeredClients_event;

    typedef struct {
        sMacAddr hostap_mac;
        std::chrono::steady_clock::time_point timestamp;
        bool timeout_expired;
    } sDfsCacPendinghostap_event;

    typedef struct {
        sMacAddr hostap_mac;
        uint8_t restricted_channels[beerocks::message::RESTRICTED_CHANNEL_LENGTH];
    } sConfiguredRestrictedChannels_event;

#define FOREACH_EVENT(EVENT)                                                                       \
    EVENT(INVALID_EVENT)                                                                           \
    EVENT(SLAVE_JOINED_EVENT)                                                                      \
    EVENT(RESTRICTED_CHANNEL_RESPONSE_EVENT)                                                       \
    EVENT(HOSTAP_CHANNEL_REQUEST_EVENT)                                                            \
    EVENT(ACS_RESPONSE_EVENT)                                                                      \
    EVENT(CSA_EVENT)                                                                               \
    EVENT(CAC_COMPLETED_EVENT)                                                                     \
    EVENT(DFS_CHANNEL_AVAILABLE_EVENT)                                                             \
    EVENT(AP_ACTIVITY_IDLE_EVENT)                                                                  \
    EVENT(DELETED_EVENT)                                                                           \
    EVENT(DFS_REENTRY_PENDING_STEERED_CLIENT)                                                      \
    EVENT(DFS_CAC_PENDING_HOSTAP_EVENT)                                                            \
    EVENT(CONFIGURED_RESTRICTED_CHANNELS_EVENT)

    // Event ENUM
    enum class eEvent { FOREACH_EVENT(GENERATE_ENUM) };

protected:
    virtual void work() override;
    // virtual void handle_responses_timeout(std::unordered_multimap<std::string, beerocks::message::eActionOp_CONTROL> timed_out_macs) override;
    // virtual void handle_response(std::string mac, beerocks::message::eActionOp_CONTROL action_op, uint8_t *buffer) override;
    virtual void handle_event(int event_type, void *obj) override;
    virtual void handle_events_timeout(std::multiset<int> pending_events) override;
    /*
        * State Machine
        */
private:
    void queue_push_event(eEvent event_type, uint8_t *event_obj);
    uint8_t *queue_get_event(eEvent &event_type);
    void queue_pop_event();
    int queue_pending_event_count();
    void queue_clear();
    void queue_clear_mac(std::string mac);

    bool is_2G_channel(int channel);
    void get_hostap_params();
    void ccl_fill_supported_channels();
    void ccl_fill_affected_supported_channels();
    void ccl_fill_active_channels();
    void ccl_remove_5G_subband(beerocks::eSubbandType band_type);
    bool ccl_has_free_channels_5G(beerocks::eWiFiBandwidth bw);
    bool ccl_has_free_dfs_channels(beerocks::eWiFiBandwidth bw);
    bool ccl_has_free_channels_2G();
    bool ccl_fill_channel_switch_request_with_least_used_channel();
    bool fill_restricted_channels_from_ccl_and_supported(uint8_t *channel_list);
    bool get_overlapping_channels_for_24G(uint8_t channel);
    void send_backhaul_reset();
    bool find_all_scan_hostap(const std::string &hostap_parent_mac);
    bool get_backhaul_manager_slave(std::string &backhaul_manager_slave_mac);
    bool acs_result_match();
    uint8_t get_gw_slave_5g_channel();
    void align_channel_to_80Mhz();
    void clear_events();
    void send_ap_disabled_notification();
    void cs_wait_for_event(eEvent cs_event);
    void wait_for_cac_completed(uint8_t channel, uint8_t bandwidth);

    bool fill_restricted_channels_from_ccl_busy_bands(uint8_t *channel_list);
    bool reentry_steered_client_check();
    bool cac_pending_hostap_check();
    void assign_config_global_restricted_channel_to_db();
    void run_optimal_path_for_connected_clients();

#define FOREACH_STATE(STATE)                                                                       \
    STATE(INIT)                                                                                    \
    STATE(GOTO_IDLE)                                                                               \
    STATE(IDLE)                                                                                    \
    STATE(ON_SLAVE_JOINED)                                                                         \
    STATE(ON_HOSTAP_CHANNEL_REQUEST)                                                               \
    STATE(COMPUTE_IRE_CANDIDATE_CHANNELS)                                                          \
    STATE(SEND_RESTRICTED_FAIL_SAFE_CHANNEL)                                                       \
    STATE(SEND_FAIL_SAFE_CHANNEL)                                                                  \
    STATE(SEND_CLEAR_RESTRICTED_CHANNEL)                                                           \
    STATE(SEND_RESTRICTED_CHANNEL_FOR_USED_BANDS)                                                  \
    STATE(WAIT_FOR_RESTRICTED_CHANNEL_RESPONSE)                                                    \
    STATE(WAIT_FOR_CLEAR_RESTRICTED_CHANNEL_RESPONSE)                                              \
    STATE(WAIT_FOR_FAIL_SAFE_CHANNEL_RESPONSE)                                                     \
    STATE(ON_RESTRICTED_FAIL_SAFE_CHANNEL_RESPONSE)                                                \
    STATE(SEND_ACS)                                                                                \
    STATE(SEND_CHANNEL_SWITCH)                                                                     \
    STATE(WAIT_FOR_ACS_RESPONSE)                                                                   \
    STATE(ON_ACS_RESPONSE)                                                                         \
    STATE(WAIT_FOR_CSA_NOTIFICATION)                                                               \
    STATE(ON_CSA_NOTIFICATION)                                                                     \
    STATE(ON_CSA_UNEXPECTED_NOTIFICATION)                                                          \
    STATE(ACTIVATE_SLAVE)                                                                          \
    STATE(ON_CAC_COMPLETED_NOTIFICATION)                                                           \
    STATE(ON_DFS_CHANNEL_AVAILABLE)                                                                \
    STATE(ON_FAIL_SAFE_CHANNEL)                                                                    \
    STATE(STEER_STA_BEFORE_DFS_REENTRY)                                                            \
    STATE(STEER_STA_BACK_AFTER_DFS_REENTRY)                                                        \
    STATE(ON_CONFIGURED_RESTRICTED_CHANNELS)

    // State ENUM
    enum class eState { FOREACH_STATE(GENERATE_ENUM) };

    // Strings Array
    static const char *s_ar_states[];
    static const char *s_ar_events[];

    eState fsm_state;

    const static int ACS_RESPONSE_WAIT_TIME                = 30000;
    const static int CSA_NOTIFICATION_RESPONSE_WAIT_TIME   = 20000;
    const static int RESTRICTED_CHANNEL_RESPONSE_WAIT_TIME = 20000;
    const static int CAC_COMPLETED_WAIT_TIME               = 660000; //11 minutes
    const static int REENTRY_STEERED_CLIENTS_WAIT          = 60000;
    const static int DISASSOC_STEER_TIMER_MS               = 15000;

    typedef struct {
        uint8_t channel = 0;
        bool is_2G      = true;
        std::string backhaul_mac;
        bool low_pass_filter_on                 = true;
        bool backhaul_is_wireless               = false;
        int backhaul_channel                    = 0;
        bool backhaul_is_2G                     = true;
        beerocks::eSubbandType backhaul_subband = beerocks::SUBBAND_UNKNOWN;
        uint8_t bandwidth                       = 0;
    } sHostapParams;

    typedef struct {
        uint8_t primary_channel      = 0;
        uint8_t secondary_channel    = 0;
        uint8_t overlap              = 0;
        bool disallow                = false;
        uint8_t ext_above_high_score = 255;
        uint8_t ext_above_low_score  = 255;
        uint8_t ch_score             = 255;
        bool radar_affected          = false;
    } sCandidateChannel;

    typedef struct {
        bool channel_ext_above_secondary = 0;
        uint8_t score                    = 0;
    } sLeastUsedCandidates;

    typedef struct {
        std::string hostap_mac;
        uint8_t channel = 0;
        uint8_t rssi    = 0;
    } sScanList;

    // events //
    bool event_handle_in_progress                                              = false;
    sSlaveJoined_event *slave_joined_event                                     = nullptr;
    sHostapChannelRequest_event *hostap_channel_request_event                  = nullptr;
    sRestrictedChannelResponse_event *restricted_channel_response_event        = nullptr;
    sAcsResponse_event *acs_response_event                                     = nullptr;
    sCsa_event *csa_event                                                      = nullptr;
    sCacCompleted_event *cac_completed_event                                   = nullptr;
    sDfsChannelAvailable_event *dfs_channel_available                          = nullptr;
    sApActivityIdle_event *ap_activity_idle                                    = nullptr;
    sDfsReEntrySampleSteeredClients_event *dfs_reentry_pending_steered_clients = nullptr;
    sDfsCacPendinghostap_event *dfs_cac_pending_hostap                         = nullptr;
    sConfiguredRestrictedChannels_event *configured_restricted_channels        = nullptr;
    //

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    std::string hostap_mac;
    sHostapParams hostap_params;
    std::unordered_map<uint8_t, sCandidateChannel> ccl;
    std::unordered_map<std::string, sScanList> backhaul_scan_measurement_list;

    beerocks_message::sApChannelSwitch channel_switch_request = {};

    std::unordered_map<std::string, std::chrono::steady_clock::time_point> hostaps_cac_pending;

    bool channel_switch_required = false;
    task_pool &tasks;
    eEvent cs_waiting_for_event = eEvent::INVALID_EVENT;
    //event queue
    std::deque<std::pair<eEvent, uint8_t *>> event_queue;
};
} //namespace son
#endif
