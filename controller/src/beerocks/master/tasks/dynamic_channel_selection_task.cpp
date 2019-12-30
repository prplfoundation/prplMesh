/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "dynamic_channel_selection_task.h"

#include "../son_actions.h"

#include <bcl/beerocks_defines.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

#define FSM_MOVE_STATE(new_state)                                                                  \
    ({                                                                                             \
        TASK_LOG(TRACE) << "FSM: " << s_ar_states[int(m_fsm_state)] << " --> "                     \
                        << s_ar_states[int(eState::new_state)];                                    \
        m_fsm_state = eState::new_state;                                                           \
    })

#define FSM_IS_IN_STATE(state) (m_fsm_state == eState::state)
#define FSM_CURR_STATE_STR s_ar_states[int(m_fsm_state)]

#define EVENT_STR(event) s_ar_events[int(event)]

const char *dynamic_channel_selection_task::s_ar_states[] = {FOREACH_DCS_STATE(GENERATE_STRING)};

const char *dynamic_channel_selection_task::s_ar_events[] = {FOREACH_DCS_EVENT(GENERATE_STRING)};

dynamic_channel_selection_task::dynamic_channel_selection_task(db &database_,
                                                               ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                               task_pool &tasks_,
                                                               std::string radio_mac_)
    : database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_), m_radio_mac(radio_mac_)
{
    m_fsm_state = eState::INIT;
}

void dynamic_channel_selection_task::work()
{
    switch (m_fsm_state) {
    case eState::INIT: {
        database.assign_dynamic_channel_selection_task_id(m_radio_mac, id);
        // allow first scan
        m_next_scan_timestamp_interval = std::chrono::steady_clock::now();
        FSM_MOVE_STATE(IDLE);
        break;
    }
    case eState::IDLE: {
        if (m_is_single_scan_pending) {
            m_is_single_scan         = true;
            m_is_single_scan_pending = false;
            FSM_MOVE_STATE(TRIGGER_SCAN);
        } else if (database.get_dcs_is_enabled(m_radio_mac)) {
            auto now = std::chrono::steady_clock::now();

            if (now > m_next_scan_timestamp_interval) {
                m_is_single_scan = false;
                FSM_MOVE_STATE(TRIGGER_SCAN);
                m_last_scan_try_timestamp = now;
                LOG(DEBUG) << "interval condition is met, trigger scan";
            }
        }
        break;
    }
    case eState::TRIGGER_SCAN: {
        LOG(TRACE) << "TRIGGER_SCAN, mac=" << m_radio_mac << "scan_type is "
                   << ((m_is_single_scan) ? "single-scan" : "continuous-scan");

        // When a scan is requested send the scan parameters Channel pool & Dwell time
        // Before sending the request set the scan_in_progress flag to true
        // So another scan would not trigger on the same radio simultaneously

        database.set_dcs_scan_in_progress(m_radio_mac, true, m_is_single_scan);

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_DCS_TRIGGER_SCAN_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message cACTION_CONTROL_DCS_TRIGGER_SCAN_REQUEST !";
            m_last_scan_error_code = beerocks::DCS_SCAN_INTERNAL_FAILURE;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }

        int32_t dwell_time_msec = database.get_dcs_dwell_time_msec(m_radio_mac, m_is_single_scan);
        if (dwell_time_msec < 0) {
            LOG(ERROR) << "invalid dwell_time < 0";
            m_last_scan_error_code = beerocks::DCS_SCAN_INVALID_SCAN_PARAMS;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }

        //get current channel pool from DB
        auto &curr_channel_pool = database.get_dcs_channel_pool(m_radio_mac, m_is_single_scan);
        if (curr_channel_pool.empty()) {
            LOG(ERROR) << "empty channel pool is not supported. please set channel pool for mac="
                       << m_radio_mac;
            m_last_scan_error_code = beerocks::DCS_SCAN_INVALID_SCAN_PARAMS;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }

        if (curr_channel_pool.size() > beerocks::message::SUPPORTED_CHANNELS_LENGTH) {
            LOG(ERROR) << "channel_pool is too big";
            m_last_scan_error_code = beerocks::DCS_SCAN_CHANNEL_POOL_TOO_BIG;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }

        request->scan_params().radio_mac         = net::network_utils::mac_from_string(m_radio_mac);
        request->scan_params().dwell_time_ms     = dwell_time_msec;
        request->scan_params().channel_pool_size = curr_channel_pool.size();
        std::copy(curr_channel_pool.begin(), curr_channel_pool.end(),
                  request->scan_params().channel_pool);

        // get the parent node to send the CMDU to the agent
        auto ire = database.get_node_parent_ire(m_radio_mac);
        son_actions::send_cmdu_to_agent(ire, cmdu_tx, database, m_radio_mac);

        set_events_timeout(SCAN_TRIGGERED_WAIT_TIME_MSEC);
        dcs_wait_for_event(eEvent::SCAN_TRIGGERED);

        FSM_MOVE_STATE(WAIT_FOR_SCAN_TRIGGERED);
        break;
    }
    case eState::WAIT_FOR_SCAN_TRIGGERED: {
        TASK_LOG(ERROR) << "This should not happen!";

        m_last_scan_error_code = beerocks::DCS_SCAN_SCAN_TRIGGERED_EVENT_TIMEOUT;

        FSM_MOVE_STATE(ABORT_SCAN);
        break;
    }
    case eState::WAIT_FOR_RESULTS_READY: {
        TASK_LOG(ERROR) << "This should not happen!";

        m_last_scan_error_code = beerocks::DCS_SCAN_RESULTS_READY_EVENT_TIMEOUT;

        FSM_MOVE_STATE(ABORT_SCAN);
        break;
    }
    case eState::WAIT_FOR_RESULTS_DUMP: {
        TASK_LOG(ERROR) << "This should not happen!";

        m_last_scan_error_code = beerocks::DCS_SCAN_RESULTS_DUMP_EVENT_TIMEOUT;

        FSM_MOVE_STATE(ABORT_SCAN);
        break;
    }
    case eState::SCAN_DONE: {
        LOG(TRACE) << "SCAN_DONE";

        //update next continuous scan time
        if (!m_is_single_scan) {
            auto interval = std::chrono::seconds(database.get_dcs_interval_sec(m_radio_mac));
            m_next_scan_timestamp_interval = m_last_scan_try_timestamp + interval;
        }

        m_last_scan_error_code = beerocks::DCS_SCAN_NO_ERROR;
        database.set_dcs_last_scan_success(m_radio_mac, m_last_scan_error_code, m_is_single_scan);

        database.set_dcs_scan_in_progress(m_radio_mac, false, m_is_single_scan);
        m_is_single_scan = false;
        FSM_MOVE_STATE(IDLE);
        break;
    }
    case eState::ABORT_SCAN: {
        LOG(ERROR) << "aborting scan for mac=" << m_radio_mac << ", last_scan_timestamp is not set";

        database.set_dcs_last_scan_success(m_radio_mac, m_last_scan_error_code, m_is_single_scan);
        database.set_dcs_scan_in_progress(m_radio_mac, false, m_is_single_scan);
        m_is_single_scan = false;

        FSM_MOVE_STATE(IDLE);
        work(); // Skip pause, move to Idle immediately
        break;
    }
    default: {
        break;
    }
    }
}

void dynamic_channel_selection_task::handle_event(int event_type, void *obj)
{
    if (obj == nullptr) {
        TASK_LOG(ERROR) << "obj == nullptr";
        return;
    }
    auto event_radio_mac = network_utils::mac_to_string(((sMacAddr *)obj)->oct);
    if (m_radio_mac == event_radio_mac) {
        bool handle_event = false;
        switch (eEvent(event_type)) {
        case eEvent::TRIGGER_SINGLE_SCAN: {
            TASK_LOG(DEBUG) << "TRIGGER_SINGLE_SCAN received";
            auto trigger_single_scan_event = reinterpret_cast<sTriggerSingleScan_event *>(obj);
            handle_event                   = true;
            TASK_LOG(DEBUG) << "TRIGGER_SINGLE_SCAN handled on:"
                            << network_utils::mac_to_string(
                                   trigger_single_scan_event->radio_mac.oct);
            m_is_single_scan_pending = true;
            break;
        }
        case eEvent::SCAN_TRIGGERED: {
            TASK_LOG(DEBUG) << "SCAN_TRIGGERED received";
            if (FSM_IS_IN_STATE(WAIT_FOR_SCAN_TRIGGERED)) {
                auto scan_triggered_event = reinterpret_cast<sScanTriggered_event *>(obj);
                handle_event              = true;
                TASK_LOG(DEBUG) << "SCAN_TRIGGERED handled on:"
                                << network_utils::mac_to_string(
                                       scan_triggered_event->radio_mac.oct);

                set_events_timeout(SCAN_RESULTS_DUMP_WAIT_TIME_MSEC);
                dcs_wait_for_event(eEvent::SCAN_RESULTS_DUMP);

                FSM_MOVE_STATE(WAIT_FOR_RESULTS_READY);
            } else {
                TASK_LOG(DEBUG) << "ignoring SCAN_TRIGERRED event,"
                                << " not in state WAIT_FOR_SCAN_TRIGGERED";
            }
            break;
        }
        case eEvent::SCAN_RESULTS_READY: {
            TASK_LOG(DEBUG) << "SCAN_RESULTS_READY received";
            if (FSM_IS_IN_STATE(WAIT_FOR_RESULTS_READY)) {
                auto scan_results_ready_event = reinterpret_cast<sScanResultsReady_event *>(obj);
                handle_event                  = true;
                TASK_LOG(DEBUG) << "SCAN_RESULTS_READY handled on:"
                                << network_utils::mac_to_string(
                                       scan_results_ready_event->radio_mac.oct);

                if (!database.clear_dcs_scan_results(m_radio_mac, m_is_single_scan)) {
                    TASK_LOG(ERROR) << "failed to clear scan results";
                    m_last_scan_error_code = beerocks::DCS_SCAN_INTERNAL_FAILURE;
                    FSM_MOVE_STATE(ABORT_SCAN);
                    break;
                }

                set_events_timeout(SCAN_RESULTS_DUMP_WAIT_TIME_MSEC);
                dcs_wait_for_event(eEvent::SCAN_RESULTS_DUMP);

                FSM_MOVE_STATE(WAIT_FOR_RESULTS_DUMP);
            } else {
                TASK_LOG(DEBUG) << "ignoring SCAN_RESULTS_READY event,"
                                << " not in state WAIT_FOR_RESULTS_READY";
            }
        }
        case eEvent::SCAN_RESULTS_DUMP: {
            TASK_LOG(DEBUG) << "SCAN_RESULTS_DUMP received";
            if (FSM_IS_IN_STATE(WAIT_FOR_RESULTS_DUMP)) {
                auto scan_results_dump_event = reinterpret_cast<sScanResultsDump_event *>(obj);
                handle_event                 = true;
                TASK_LOG(DEBUG) << "SCAN_RESULTS_DUMP handled on:"
                                << network_utils::mac_to_string(
                                       scan_results_dump_event->radio_mac.oct);
                auto channel = scan_results_dump_event->scan_results.channel;

                if (!database.is_channel_in_pool(m_radio_mac, channel, m_is_single_scan)) {
                    LOG(DEBUG) << "channel is not in channel pool - ignoring results";
                } else if (!database.add_dcs_scan_results(m_radio_mac,
                                                          scan_results_dump_event->scan_results,
                                                          m_is_single_scan)) {
                    TASK_LOG(ERROR) << "failed to save scan results";
                    m_last_scan_error_code = beerocks::DCS_SCAN_INTERNAL_FAILURE;
                    FSM_MOVE_STATE(ABORT_SCAN);
                    break;
                } else {
                    TASK_LOG(DEBUG)
                        << "results for channel=" << int(channel) << " saved successfully to DB";
                }

                set_events_timeout(SCAN_RESULTS_DUMP_WAIT_TIME_MSEC);
                dcs_wait_for_event(eEvent::SCAN_RESULTS_DUMP);
            } else {
                TASK_LOG(DEBUG) << "ignoring SCAN_RESULTS_DUMP event,"
                                << " not in state WAIT_FOR_RESULTS_DUMP";
            }
            break;
        }
        case eEvent::SCAN_FINISHED: {
            TASK_LOG(DEBUG) << "SCAN_FINISHED received";
            if (FSM_IS_IN_STATE(WAIT_FOR_RESULTS_DUMP)) {
                auto scan_finished_event = reinterpret_cast<sScanFinished_event *>(obj);
                handle_event             = true;
                TASK_LOG(DEBUG) << "SCAN_FINISHED handled on:"
                                << network_utils::mac_to_string(scan_finished_event->radio_mac.oct);

                //clear any pending events. for example SCAN_RESULTS_DUMP
                clear_pending_events();

                FSM_MOVE_STATE(SCAN_DONE);
            } else {
                TASK_LOG(DEBUG) << "ignoring SCAN_FINISHED event,"
                                << " not in state WAIT_FOR_RESULTS_DUMP";
            }
            break;
        }
        case eEvent::SCAN_ABORTED: {
            TASK_LOG(DEBUG) << "SCAN_ABORTED received";
            auto scan_abort_event = reinterpret_cast<sScanAbort_event *>(obj);
            handle_event          = true;
            TASK_LOG(DEBUG) << "SCAN_FINISHED handled on:"
                            << network_utils::mac_to_string(scan_abort_event->radio_mac.oct);

            m_last_scan_error_code = beerocks::DCS_SCAN_ABORTED_BY_DRIVER;

            clear_pending_events();
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }
        case eEvent::SCAN_ENABLE_CHANGE: {
            TASK_LOG(DEBUG) << "SCAN_ENABLE_CHANGE received";
            auto scan_enable_change_event = reinterpret_cast<sScanEnableChange_event *>(obj);
            handle_event                  = true;
            TASK_LOG(DEBUG) << "SCAN_FINISHED handled on:"
                            << network_utils::mac_to_string(
                                   scan_enable_change_event->radio_mac.oct);

            if (FSM_IS_IN_STATE(IDLE)) {
                TASK_LOG(DEBUG) << "current state:IDLE, resetting interval wait";
                m_next_scan_timestamp_interval = std::chrono::steady_clock::now();
            }
            break;
        }
        default: {
            break;
        }
        }

        if (!handle_event) {
            TASK_LOG(ERROR) << "unexpected event: " << EVENT_STR(eEvent(event_type))
                            << " in state: " << FSM_CURR_STATE_STR;
        } else {
            m_dcs_waiting_for_event = eEvent::INVALID_EVENT;
        }
    } else {
        TASK_LOG(ERROR) << "event : " << EVENT_STR(eEvent(event_type))
                        << " in state: " << FSM_CURR_STATE_STR << " for mac: " << event_radio_mac;
        if (m_dcs_waiting_for_event != eEvent::INVALID_EVENT) {
            TASK_LOG(DEBUG) << "calling wait_for_event for " << EVENT_STR(m_dcs_waiting_for_event);
            wait_for_event(int(m_dcs_waiting_for_event));
        }
    }
}

void dynamic_channel_selection_task::handle_events_timeout(std::multiset<int> pending_events)
{
    // only one event is expected
    for (std::multiset<int>::iterator it = pending_events.begin(); it != pending_events.end();
         ++it) {
        TASK_LOG(ERROR) << "event " << *it << " timed out on " << m_radio_mac
                        << ", going to abort-scan state -> handle_nl_dead_node";

        switch (m_fsm_state) {
        case eState::WAIT_FOR_SCAN_TRIGGERED: {
            m_last_scan_error_code = DCS_SCAN_SCAN_TRIGGERED_EVENT_TIMEOUT;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }
        case eState::WAIT_FOR_RESULTS_READY: {
            m_last_scan_error_code = DCS_SCAN_RESULTS_READY_EVENT_TIMEOUT;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }
        case eState::WAIT_FOR_RESULTS_DUMP: {
            m_last_scan_error_code = DCS_SCAN_RESULTS_DUMP_EVENT_TIMEOUT;
            FSM_MOVE_STATE(ABORT_SCAN);
            break;
        }
        case eState::SCAN_DONE:
        case eState::IDLE:
        case eState::ABORT_SCAN: {
            TASK_LOG(ERROR) << "Unexpected event timeout - last scan flow already completed";
            TASK_LOG(DEBUG) << "Unexpected event timeout - not changing state, not updating error";
            break;
        }
        default: {
            TASK_LOG(ERROR) << "Unexpected event timeout!";
            break;
        }
        }
    }
    m_dcs_waiting_for_event = eEvent::INVALID_EVENT;
}

void dynamic_channel_selection_task::dcs_wait_for_event(eEvent dcs_event)
{
    m_dcs_waiting_for_event = dcs_event;
    wait_for_event((int)dcs_event);
}