/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "task.h"

#include <bcl/beerocks_utils.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace son;

int task::latest_id = 1; //can't be 0 since messages without a task id use 0 for that field

task::task(std::string task_name_, std::string node_mac)
    : task_name(task_name_), assigned_node(node_mac), id(latest_id++),
      start_timestamp(std::chrono::steady_clock::now())
{
    TASK_LOG(DEBUG) << std::endl << std::endl << std::endl << std::endl;
    TASK_LOG(DEBUG) << "start new task: " << task_name << ", id=" << id;
}

void task::response_received(std::string mac,beerocks_message::eActionOp_CONTROL action_op,
                             std::shared_ptr<message_com::beerocks_header> beerocks_header)
{
    // removing the responding mac with specific action_op from pending_macs
    auto range = pending_macs.equal_range(mac);
    for (auto it = range.first; it != range.second;) {
        if ((it->second == action_op) && (it->first == mac)) {
            it = pending_macs.erase(it);
            handle_response(mac, action_op, beerocks_header);
            return;
        } else {
            it++;
        }
    }

    // Handle the response even if we are not expecting it
    handle_response(mac, action_op, beerocks_header);
}

void task::event_received(int event_type, void *obj)
{
    auto range = pending_events.equal_range(event_type);
    for (auto it = range.first; it != range.second;) {
        if (*it == event_type) {
            it = pending_events.erase(it);
            break;
        } else {
            it++;
        }
    }

    handle_event(event_type, obj);
}

void task::pending_task_ended(int task_id)
{
    if (task_id == pending_task_id) {
        TASK_LOG(DEBUG) << "pending_task_id " << pending_task_id << " - ended";
        waiting_for_pending_task = false;
    }
}

void task::wait_for(int ms)
{
    next_action_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    waiting          = true;
}

void task::wait_for_event(int event)
{
    waiting_for_events = true;
    waiting            = true;
    pending_events.insert(event);
}

void task::wait_for_task_end(int task_id, int ms)
{
    if (waiting_for_pending_task) {
        TASK_LOG(WARNING) << "already waiting for a pending task! overwriting";
    }
    pending_task_timeout     = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    waiting_for_pending_task = true;
    waiting                  = true;
    pending_task_id          = task_id;
}

void task::finish()
{
    done = true;
    handle_task_end();
}

void task::set_task_timeout(int ms)
{
    task_timeout     = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    task_timeout_set = true;
}

void task::set_responses_timeout(int ms)
{
    responses_timeout     = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    responses_timeout_set = true;
}

void task::set_events_timeout(int ms)
{
    events_timeout     = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    events_timeout_set = true;
}

void task::execute()
{
    auto now = std::chrono::steady_clock::now();
    if (task_timeout_set && now >= task_timeout) {
        TASK_LOG(DEBUG) << "task timeout reached";
        finish();
    } else if (waiting) {
        if (now < next_action_time) {
            return;
        }
        if (waiting_for_pending_task && now >= pending_task_timeout) {
            TASK_LOG(DEBUG) << "pending task timed out";
            waiting_for_pending_task = false;
            handle_pending_task_timeout(pending_task_id);
        }
        if (events_timeout_set && waiting_for_events && now >= events_timeout) {
            TASK_LOG(DEBUG) << "pending events timed out";
            handle_events_timeout(pending_events);
            events_timeout_set = false;
            pending_events.clear();
        }
        if (responses_timeout_set && now >= responses_timeout) {
            // TASK_LOG(DEBUG) << "responses timed out";
            handle_responses_timeout(pending_macs);
            responses_timeout_set = false;
            pending_macs.clear();
        }
        if (waiting_for_events && pending_events.empty()) {
            TASK_LOG(DEBUG) << "done waiting for events";
            events_timeout_set = false;
            waiting_for_events = false;
        }
        if (waiting_for_responses && pending_macs.empty()) {
            // TASK_LOG(DEBUG) << "done waiting for responses";
            responses_timeout_set = false;
            waiting_for_responses = false;
        }
        if (!responses_timeout_set && !waiting_for_events && !waiting_for_responses &&
            !waiting_for_pending_task) {
            waiting = false;
        }
    }
    if (!waiting && !done) {
        work();
    }
}

bool task::is_done() { return done; }

void task::kill()
{
    TASK_LOG(DEBUG) << "killed!";
    finish();
}

void task::add_pending_macs(std::set<std::string> macs,
                            beerocks_message::eActionOp_CONTROL action_op)
{
    for (auto it = macs.begin(); macs.end() != it; ++it) {
        pending_macs.insert({*it, action_op});
    }

    //pending_macs.insert(macs.begin(), macs.end());
    waiting_for_responses = true;
    waiting               = true;
}

void task::add_pending_mac(std::string mac, beerocks_message::eActionOp_CONTROL action_op)
{
    pending_macs.insert({mac, action_op});
    waiting_for_responses = true;
    waiting               = true;
}

void task::clear_pending_macs() { pending_macs.clear(); }
