/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TASK_POOL_H_
#define _TASK_POOL_H_

#include "task.h"

#include <beerocks/tlvf/beerocks_message_action.h>

namespace son {

class task_pool {

public:
    task_pool() {}
    ~task_pool() {}

    bool add_task(std::shared_ptr<task> new_task);
    bool is_task_running(int id);
    void kill_task(int id);
    void push_event(int task_id, int event_type, void *obj = nullptr);
    void response_received(std::string mac,
                           std::shared_ptr<beerocks::beerocks_header> beerocks_header);
    void pending_task_ended(int task_id);
    void run_tasks();

private:
    std::unordered_map<int, std::shared_ptr<task>> scheduled_tasks;
};

} // namespace son

#endif
