/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TASK_POOL_H_
#define _TASK_POOL_H_

#include "task.h"
#include <unordered_map>

namespace beerocks {

/**
 * @brief The TaskPool is single instance tasks container which responsible to run each
 * task and allow passing messages and events from the Agent to the task.
 * 
 */
class TaskPool {
public:
    /**
     * @brief Add new task to the task pool.
     * 
     * @param new_task Shared pointer to the task.
     * @return true on success, otherwise false.
     */
    bool add_task(const std::shared_ptr<Task> &new_task);

    /**
     * @brief Run all tasks on the pool, by calling each task work() function.
     */
    void run_tasks();

    /**
     * @brief Send an 'event' defined on a specific task 'task_type'. 
     * 
     * @param task_type Task type, defined on Task base class.
     * @param event Event type, defined on the task itself.
     */
    void send_event(eTaskType task_type, uint8_t event);

    /**
     * @brief Iterate over all tasks on the pool and pass them the message on 'cmdu_rx'.
     * 
     * @param cmdu_rx CMDU object containing the message.
     * @param src_mac MAC address of the message sender.
     * @param beerocks_header Beerocks header (Only on VS message).
     * @return true if the message has been handled, otherwise false.
     */
    bool handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx, sMacAddr src_mac,
                     std::shared_ptr<beerocks_header> beerocks_header = nullptr);

private:
    // Decalaring unordered_map with key which is an enum, does not compiles on older gcc version.
    // It was considered a defect in the standard, and was fixed in C++14, and also fixed in the
    // version of libstdc++ shipping with gcc as of 6.1.
    // To make unordered_map work with an enum as key, std::hash<int> function was added as third
    // template argument.
    std::unordered_map<eTaskType, std::shared_ptr<Task>, std::hash<int>> m_task_pool;
};

} // namespace beerocks
#endif // _TASK_POOL_H_
