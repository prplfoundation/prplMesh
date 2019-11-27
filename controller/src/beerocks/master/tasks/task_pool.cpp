/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "task_pool.h"

#include <easylogging++.h>

using namespace beerocks;
using namespace son;

bool task_pool::add_task(std::shared_ptr<task> new_task)
{
    LOG(TRACE) << "inserting new task, id=" << int(new_task->id)
               << " task_name=" << new_task->task_name;
    return (scheduled_tasks.insert(std::make_pair(new_task->id, new_task))).second;
}

bool task_pool::is_task_running(int id)
{
    auto it = scheduled_tasks.find(id);
    if (it != scheduled_tasks.end() && it->second != nullptr && !it->second->is_done()) {
        return true;
    } else {
        return false;
    }
}

void task_pool::kill_task(int id)
{
    auto it = scheduled_tasks.find(id);
    if (it != scheduled_tasks.end() && it->second != nullptr) {
        LOG(DEBUG) << "killing task " << it->second->task_name << ", id " << it->first;
        it->second->kill();
    }
}

void task_pool::push_event(int task_id, int event_type, void *obj)
{
    auto it = scheduled_tasks.find(task_id);
    if (it != scheduled_tasks.end()) {
        if (it->second != nullptr) {
            it->second->event_received(event_type, obj);
        } else {
            LOG(ERROR) << "invalid task " << task_id;
        }
    } else {
        LOG(ERROR) << "can't find task " << task_id;
    }
}

void task_pool::pending_task_ended(int task_id)
{
    //TODO find a more efficient way for this
    for (auto t : scheduled_tasks) {
        t.second->pending_task_ended(task_id);
    }
}

void task_pool::response_received(int id, std::string mac,
                                  beerocks_message::eActionOp_CONTROL action_op,
                                  std::shared_ptr<beerocks::message_com::beerocks_header> beerocks_header)
{
    std::unordered_map<int, std::shared_ptr<task>>::const_iterator got = scheduled_tasks.find(id);
    if (got != scheduled_tasks.end()) {
        got->second->response_received(mac, action_op, beerocks_header);
    }
}

void task_pool::run_tasks()
{
    for (auto it = scheduled_tasks.begin(); it != scheduled_tasks.end();) {
        it->second->execute();
        if (it->second->is_done()) {
            pending_task_ended(it->first);
            LOG(DEBUG) << "erasing task " << it->second->task_name << ", id " << it->first;
            it = scheduled_tasks.erase(it);
        } else {
            ++it;
        }
    }
}
