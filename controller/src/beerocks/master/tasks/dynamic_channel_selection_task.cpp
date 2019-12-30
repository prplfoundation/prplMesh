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