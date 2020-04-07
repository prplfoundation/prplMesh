/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019-2020 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "dynamic_channel_selection_task.h"
#include "../son_actions.h"
#include <bcl/beerocks_defines.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#define SCAN_TRIGGERED_WAIT_TIME_MSEC 20000     //20 Sec
#define SCAN_RESULTS_DUMP_WAIT_TIME_MSEC 210000 //3.5 Min

std::string s_ar_states[] = {FOREACH_DCS_STATE(GENERATE_STRING)};
std::string s_ar_events[] = {FOREACH_DCS_EVENT(GENERATE_STRING)};

dynamic_channel_selection_task::dynamic_channel_selection_task(db &database_,
                                                               ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                               task_pool &tasks_,
                                                               sMacAddr radio_mac_)
    : database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_), m_radio_mac(radio_mac_)
{
    m_fsm_state = eState::INIT;
}
beerocks::eChannelScanErrCode dynamic_channel_selection_task::dcs_request_scan_dump()
{
    // When a dump is requested, do not set the scan_in_progress flag
    // simply send the request as is, and wait for the results ready event

    auto request = beerocks::message_com::create_vs_message<
        beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST>(cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed building message cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST";
        return beerocks::eChannelScanErrCode::CHANNEL_SCAN_INTERNAL_FAILURE;
    }

    // get the parent node to send the CMDU to the agent
    auto radio_mac_str = beerocks::net::network_utils::mac_to_string(m_radio_mac);
    auto ire           = database.get_node_parent_ire(radio_mac_str);
    son_actions::send_cmdu_to_agent(ire, cmdu_tx, database, radio_mac_str);

    return beerocks::eChannelScanErrCode::CHANNEL_SCAN_SUCCESS;
}
beerocks::eChannelScanErrCode dynamic_channel_selection_task::dcs_request_scan_trigger()
{
    // When a scan is requested send the scan parameters Channel pool & Dwell time

    auto request = beerocks::message_com::create_vs_message<
        beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST>(cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed building message cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST";
        return beerocks::eChannelScanErrCode::CHANNEL_SCAN_INTERNAL_FAILURE;
    }

    int32_t dwell_time_msec =
        database.get_channel_scan_dwell_time_msec(m_radio_mac, m_is_single_scan);
    if (dwell_time_msec <= 0) {
        LOG(ERROR) << "invalid dwell_time <= 0";
        return beerocks::eChannelScanErrCode::CHANNEL_SCAN_INVALID_PARAMS;
    }

    //get current channel pool from DB
    auto &curr_channel_pool = database.get_channel_scan_pool(m_radio_mac, m_is_single_scan);
    if (curr_channel_pool.empty()) {
        LOG(ERROR) << "empty channel pool is not supported. please set channel pool for mac="
                   << m_radio_mac;
        return beerocks::eChannelScanErrCode::CHANNEL_SCAN_INVALID_PARAMS;
    }

    if (curr_channel_pool.size() > beerocks::message::SUPPORTED_CHANNELS_LENGTH) {
        LOG(ERROR) << "channel_pool is too big";
        return beerocks::eChannelScanErrCode::CHANNEL_SCAN_POOL_TOO_BIG;
    }

    request->scan_params().radio_mac         = m_radio_mac;
    request->scan_params().dwell_time_ms     = dwell_time_msec;
    request->scan_params().channel_pool_size = curr_channel_pool.size();
    std::copy(curr_channel_pool.begin(), curr_channel_pool.end(),
              request->scan_params().channel_pool);

    // get the parent node to send the CMDU to the agent
    auto radio_mac_str = beerocks::net::network_utils::mac_to_string(m_radio_mac);
    auto ire           = database.get_node_parent_ire(radio_mac_str);
    son_actions::send_cmdu_to_agent(ire, cmdu_tx, database, radio_mac_str);

    return beerocks::eChannelScanErrCode::CHANNEL_SCAN_SUCCESS;
}
void dynamic_channel_selection_task::work()
{
    switch (m_fsm_state) {
    case eState::INIT: {
        database.assign_dynamic_channel_selection_task_id(m_radio_mac, id);
        // allow first scan
        m_next_scan_timestamp_interval = std::chrono::steady_clock::now();
        fsm_move_state(eState::IDLE);
        break;
    }
    case eState::IDLE: {
        if (m_is_single_scan_pending) {
            m_is_single_scan         = true;
            m_is_single_scan_pending = false;
            fsm_move_state(eState::TRIGGER_SCAN);
        } else if (database.get_channel_scan_is_enabled(m_radio_mac)) {
            auto now = std::chrono::steady_clock::now();

            if (now > m_next_scan_timestamp_interval) {
                m_is_single_scan = false;
                fsm_move_state(eState::TRIGGER_SCAN);
                m_last_scan_try_timestamp = now;
                LOG(DEBUG) << "interval condition is met, trigger scan";
            }
        }
        break;
    }
    case eState::TRIGGER_SCAN: {
        LOG(TRACE) << "TRIGGER_SCAN, mac=" << m_radio_mac << ", scan_type is "
                   << ((m_is_single_scan) ? "single-scan" : "continuous-scan");

        // Before sending any request, set the scan_in_progress flag to true
        // So another scan request would not launch on the same radio simultaneously
        database.set_channel_scan_in_progress(m_radio_mac, true, m_is_single_scan);

        // When a scan is requested, check the Dwell time parameter,
        // Normally send a "trigger scan" request.
        // but on a dwell time = 0 scenario, sent a "scan dump" request
        auto ret = beerocks::eChannelScanErrCode::CHANNEL_SCAN_SUCCESS;
        if (database.get_channel_scan_dwell_time_msec(m_radio_mac, m_is_single_scan) != 0) {
            // Send the "trigger scan" request and continue on to WAIT_FOR_SCAN_TRIGGERED
            if ((ret = dcs_request_scan_trigger()) !=
                beerocks::eChannelScanErrCode::CHANNEL_SCAN_SUCCESS) {
                m_last_scan_error_code = ret;
                fsm_move_state(eState::ABORT_SCAN);
            } else {
                set_events_timeout(SCAN_TRIGGERED_WAIT_TIME_MSEC);
                dcs_wait_for_event(eEvent::SCAN_TRIGGERED);

                fsm_move_state(eState::WAIT_FOR_SCAN_TRIGGERED);
            }
        } else {
            // Send the "scan dump" request but skip the WAIT_FOR_SCAN_TRIGGERED
            // and continue to WAIT_FOR_RESULTS_READY since no scan is triggered
            if ((ret = dcs_request_scan_dump()) !=
                beerocks::eChannelScanErrCode::CHANNEL_SCAN_SUCCESS) {
                m_last_scan_error_code = ret;
                fsm_move_state(eState::ABORT_SCAN);
            } else {
                set_events_timeout(SCAN_RESULTS_DUMP_WAIT_TIME_MSEC);
                dcs_wait_for_event(eEvent::SCAN_RESULTS_DUMP);

                fsm_move_state(eState::WAIT_FOR_RESULTS_READY);
            }
        }
        break;
    }
    case eState::WAIT_FOR_SCAN_TRIGGERED: {
        TASK_LOG(ERROR) << "This should not happen!";

        m_last_scan_error_code =
            beerocks::eChannelScanErrCode::CHANNEL_SCAN_TRIGGERED_EVENT_TIMEOUT;

        fsm_move_state(eState::ABORT_SCAN);
        break;
    }
    case eState::WAIT_FOR_RESULTS_READY: {
        TASK_LOG(ERROR) << "This should not happen!";

        m_last_scan_error_code =
            beerocks::eChannelScanErrCode::CHANNEL_SCAN_RESULTS_READY_EVENT_TIMEOUT;

        fsm_move_state(eState::ABORT_SCAN);
        break;
    }
    case eState::WAIT_FOR_RESULTS_DUMP: {
        TASK_LOG(ERROR) << "This should not happen!";

        m_last_scan_error_code =
            beerocks::eChannelScanErrCode::CHANNEL_SCAN_RESULTS_DUMP_EVENT_TIMEOUT;

        fsm_move_state(eState::ABORT_SCAN);
        break;
    }
    case eState::SCAN_DONE: {
        LOG(TRACE) << "SCAN_DONE";

        //update next continuous scan time
        if (!m_is_single_scan) {
            auto interval =
                std::chrono::seconds(database.get_channel_scan_interval_sec(m_radio_mac));
            m_next_scan_timestamp_interval = m_last_scan_try_timestamp + interval;
        }

        m_last_scan_error_code = beerocks::eChannelScanErrCode::CHANNEL_SCAN_SUCCESS;
        database.set_channel_scan_results_status(m_radio_mac, m_last_scan_error_code,
                                                 m_is_single_scan);

        database.set_channel_scan_in_progress(m_radio_mac, false, m_is_single_scan);
        m_is_single_scan = false;
        fsm_move_state(eState::IDLE);
        break;
    }
    case eState::ABORT_SCAN: {
        LOG(ERROR) << "aborting scan for mac=" << m_radio_mac << ", last_scan_timestamp is not set";

        database.set_channel_scan_results_status(m_radio_mac, m_last_scan_error_code,
                                                 m_is_single_scan);
        database.set_channel_scan_in_progress(m_radio_mac, false, m_is_single_scan);
        m_is_single_scan = false;

        fsm_move_state(eState::IDLE);
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

    auto event_radio_mac = *reinterpret_cast<sMacAddr *>(obj);
    if (m_radio_mac != event_radio_mac) {
        TASK_LOG(ERROR) << "event : " << s_ar_events[int(eEvent(event_type))]
                        << " in state: " << s_ar_states[int(m_fsm_state)]
                        << " for mac: " << event_radio_mac;
        if (m_dcs_waiting_for_event != eEvent::INVALID_EVENT) {
            TASK_LOG(DEBUG) << "calling wait_for_event for "
                            << s_ar_events[int(m_dcs_waiting_for_event)];
            wait_for_event(int(m_dcs_waiting_for_event));
        }
        return;
    }

    // This flag (event_handled) indicate if event has arived in the correct fsm state.
    bool event_handled = false;
    switch (eEvent(event_type)) {
    case eEvent::TRIGGER_SINGLE_SCAN: {
        TASK_LOG(DEBUG) << "TRIGGER_SINGLE_SCAN received";
        auto single_scan_event = reinterpret_cast<sScanEvent *>(obj);
        event_handled          = true;
        TASK_LOG(DEBUG) << "TRIGGER_SINGLE_SCAN handled on:" << single_scan_event->radio_mac.oct;
        m_is_single_scan_pending = true;
        break;
    }
    case eEvent::SCAN_TRIGGER_FAILED: {
        TASK_LOG(DEBUG) << "SCAN_TRIGGER_FAILED received";
        if (fsm_in_state(eState::WAIT_FOR_SCAN_TRIGGERED)) {
            auto scan_trigger_failed_event = reinterpret_cast<sScanEvent *>(obj);
            event_handled                  = true;
            TASK_LOG(DEBUG) << "SCAN_TRIGGER_FAILED handled on:"
                            << scan_trigger_failed_event->radio_mac.oct;
            m_last_scan_error_code = beerocks::eChannelScanErrCode::CHANNEL_SCAN_INTERNAL_FAILURE;
            clear_pending_events();
            fsm_move_state(eState::ABORT_SCAN);
        }
        break;
    }
    case eEvent::SCAN_TRIGGERED: {
        TASK_LOG(DEBUG) << "SCAN_TRIGGERED received";
        if (fsm_in_state(eState::WAIT_FOR_SCAN_TRIGGERED)) {
            auto scan_triggered_event = reinterpret_cast<sScanEvent *>(obj);
            event_handled             = true;
            TASK_LOG(DEBUG) << "SCAN_TRIGGERED handled on:" << scan_triggered_event->radio_mac.oct;

            set_events_timeout(SCAN_RESULTS_DUMP_WAIT_TIME_MSEC);
            dcs_wait_for_event(eEvent::SCAN_RESULTS_DUMP);

            fsm_move_state(eState::WAIT_FOR_RESULTS_READY);
        } else {
            TASK_LOG(DEBUG) << "ignoring SCAN_TRIGERRED event,"
                            << " not in state WAIT_FOR_SCAN_TRIGGERED";
        }
        break;
    }
    case eEvent::SCAN_RESULTS_READY: {
        TASK_LOG(DEBUG) << "SCAN_RESULTS_READY received";
        if (fsm_in_state(eState::WAIT_FOR_RESULTS_READY)) {
            auto scan_results_ready_event = reinterpret_cast<sScanEvent *>(obj);
            event_handled                 = true;
            TASK_LOG(DEBUG) << "SCAN_RESULTS_READY handled on:"
                            << scan_results_ready_event->radio_mac.oct;

            if (!database.clear_channel_scan_results(m_radio_mac, m_is_single_scan)) {
                TASK_LOG(ERROR) << "failed to clear scan results";
                m_last_scan_error_code =
                    beerocks::eChannelScanErrCode::CHANNEL_SCAN_INTERNAL_FAILURE;
                fsm_move_state(eState::ABORT_SCAN);
                break;
            }

            set_events_timeout(SCAN_RESULTS_DUMP_WAIT_TIME_MSEC);
            dcs_wait_for_event(eEvent::SCAN_RESULTS_DUMP);

            fsm_move_state(eState::WAIT_FOR_RESULTS_DUMP);
        } else {
            TASK_LOG(DEBUG) << "ignoring SCAN_RESULTS_READY event,"
                            << " not in state WAIT_FOR_RESULTS_READY";
        }
        break;
    }
    case eEvent::SCAN_RESULTS_DUMP: {
        TASK_LOG(DEBUG) << "SCAN_RESULTS_DUMP received";
        if (fsm_in_state(eState::WAIT_FOR_RESULTS_DUMP)) {
            auto scan_results_dump_event = reinterpret_cast<sScanEvent *>(obj);
            event_handled                = true;
            TASK_LOG(DEBUG) << "SCAN_RESULTS_DUMP handled on:"
                            << scan_results_dump_event->radio_mac.oct;
            auto channel = scan_results_dump_event->udata.scan_results.channel;

            if (!database.is_channel_in_pool(m_radio_mac, channel, m_is_single_scan)) {
                LOG(DEBUG) << "channel is not in channel pool - ignoring results";
            } else if (!database.add_channel_scan_results(
                           m_radio_mac, scan_results_dump_event->udata.scan_results,
                           m_is_single_scan)) {
                TASK_LOG(ERROR) << "failed to save scan results";
                m_last_scan_error_code =
                    beerocks::eChannelScanErrCode::CHANNEL_SCAN_INTERNAL_FAILURE;
                fsm_move_state(eState::ABORT_SCAN);
                break;
            } else {
                TASK_LOG(DEBUG) << "results for channel=" << int(channel)
                                << " saved successfully to DB";
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
        if (fsm_in_state(eState::WAIT_FOR_RESULTS_DUMP)) {
            auto scan_finished_event = reinterpret_cast<sScanEvent *>(obj);
            event_handled            = true;
            TASK_LOG(DEBUG) << "SCAN_FINISHED handled on:" << scan_finished_event->radio_mac.oct;

            //clear any pending events. for example SCAN_RESULTS_DUMP
            clear_pending_events();

            fsm_move_state(eState::SCAN_DONE);
        } else {
            TASK_LOG(DEBUG) << "ignoring SCAN_FINISHED event,"
                            << " not in state WAIT_FOR_RESULTS_DUMP";
        }
        break;
    }
    case eEvent::SCAN_ABORTED: {
        TASK_LOG(DEBUG) << "SCAN_ABORTED received";
        auto scan_abort_event = reinterpret_cast<sScanEvent *>(obj);
        event_handled         = true;
        TASK_LOG(DEBUG) << "SCAN_FINISHED handled on:" << scan_abort_event->radio_mac.oct;

        m_last_scan_error_code = beerocks::eChannelScanErrCode::CHANNEL_SCAN_ABORTED_BY_DRIVER;

        clear_pending_events();
        fsm_move_state(eState::ABORT_SCAN);
        break;
    }
    case eEvent::SCAN_ENABLE_CHANGE: {
        TASK_LOG(DEBUG) << "SCAN_ENABLE_CHANGE received";
        auto scan_enable_change_event = reinterpret_cast<sScanEvent *>(obj);
        event_handled                 = true;
        TASK_LOG(DEBUG) << "SCAN_FINISHED handled on:" << scan_enable_change_event->radio_mac.oct;

        if (fsm_in_state(eState::IDLE)) {
            TASK_LOG(DEBUG) << "current state:IDLE, resetting interval wait";
            m_next_scan_timestamp_interval = std::chrono::steady_clock::now();
        }
        break;
    }
    default: {
        break;
    }
    }

    if (!event_handled) {
        TASK_LOG(ERROR) << "unexpected event: " << s_ar_events[int(eEvent(event_type))]
                        << " in state: " << s_ar_states[int(m_fsm_state)];
    } else {
        m_dcs_waiting_for_event = eEvent::INVALID_EVENT;
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
            m_last_scan_error_code =
                beerocks::eChannelScanErrCode::CHANNEL_SCAN_TRIGGERED_EVENT_TIMEOUT;
            fsm_move_state(eState::ABORT_SCAN);
            break;
        }
        case eState::WAIT_FOR_RESULTS_READY: {
            m_last_scan_error_code =
                beerocks::eChannelScanErrCode::CHANNEL_SCAN_RESULTS_READY_EVENT_TIMEOUT;
            fsm_move_state(eState::ABORT_SCAN);
            break;
        }
        case eState::WAIT_FOR_RESULTS_DUMP: {
            m_last_scan_error_code =
                beerocks::eChannelScanErrCode::CHANNEL_SCAN_RESULTS_DUMP_EVENT_TIMEOUT;
            fsm_move_state(eState::ABORT_SCAN);
            break;
        }
        case eState::SCAN_DONE:
        case eState::IDLE:
        case eState::ABORT_SCAN: {
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

void dynamic_channel_selection_task::fsm_move_state(eState new_state)
{
    TASK_LOG(TRACE) << "FSM: " << s_ar_states[int(m_fsm_state)] << " --> "
                    << s_ar_states[int(new_state)];
    m_fsm_state = new_state;
}
bool dynamic_channel_selection_task::fsm_in_state(eState state) { return m_fsm_state == state; }