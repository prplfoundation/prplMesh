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

#define SCAN_TRIGGERED_WAIT_TIME_MSEC 20000
#define SCAN_RESULTS_DUMP_WAIT_TIME_MSEC 40000

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

void dynamic_channel_selection_task::work()
{
    TASK_LOG(DEBUG) << "FSM current state: " << s_ar_states[int(m_fsm_state)];

    switch (m_fsm_state) {
    case eState::INIT:
    case eState::IDLE:
    case eState::TRIGGER_SCAN:
    case eState::WAIT_FOR_SCAN_TRIGGERED:
    case eState::WAIT_FOR_RESULTS_READY:
    case eState::WAIT_FOR_RESULTS_DUMP:
    case eState::SCAN_DONE:
    case eState::ABORT_SCAN:
    default: {
        break;
    }
    }
    TASK_LOG(DEBUG) << "FSM next state: " << s_ar_states[int(m_fsm_state)];
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
    case eEvent::TRIGGER_SINGLE_SCAN:
    case eEvent::SCAN_TRIGGERED:
    case eEvent::SCAN_RESULTS_READY:
    case eEvent::SCAN_RESULTS_DUMP:
    case eEvent::SCAN_FINISHED:
    case eEvent::SCAN_ABORTED:
    case eEvent::SCAN_ENABLE_CHANGE:
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
        case eState::WAIT_FOR_SCAN_TRIGGERED:
        case eState::WAIT_FOR_RESULTS_READY:
        case eState::WAIT_FOR_RESULTS_DUMP:
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
