/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "task_pool.h"

using namespace beerocks;

bool TaskPool::add_task(const std::shared_ptr<Task> &new_task)
{
    if (!new_task) {
        LOG(ERROR) << "new task pointer is nullptr";
        return false;
    }
    m_task_pool[new_task->get_task_type()] = new_task;
    return true;
}

void TaskPool::run_tasks()
{
    for (auto &task_element : m_task_pool) {
        auto &task = task_element.second;
        task->work();
    }
}

void TaskPool::send_event(eTaskType task_type, uint8_t event, const void *event_obj)
{
    auto task_it = m_task_pool.find(task_type);
    if (task_it == m_task_pool.end()) {
        LOG(ERROR) << "task of type " << int(task_type) << " does not exist in the task_pool";
        return;
    }

    auto &task = task_it->second;
    task->handle_event(event, event_obj);
}

bool TaskPool::handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx, sMacAddr src_mac,
                           std::shared_ptr<beerocks_header> beerocks_header)
{
    for (auto &task_element : m_task_pool) {
        auto &task = task_element.second;
        if (task->handle_cmdu(cmdu_rx, src_mac, beerocks_header)) {
            return true;
        }
    }
    return false;
}
