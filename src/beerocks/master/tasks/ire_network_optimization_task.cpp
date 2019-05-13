/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ire_network_optimization_task.h"
#include "../db/db_algo.h"
#include "../son_actions.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/network/socket.h>
#include <easylogging++.h>

#include <climits>
#include <cstdlib>
#include <set>

using namespace son;

ire_network_optimization_task::ire_network_optimization_task(db &database_,
                                                             ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                             task_pool &tasks_,
                                                             std::string task_name_)
    : task(task_name_), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_)
{
}

void ire_network_optimization_task::work()
{
    switch (state) {

    case INIT: {
        int prev_task_id = database.get_network_optimization_task_id();
        tasks.kill_task(prev_task_id);

        database.assign_network_optimization_task_id(id);
        state = START;

        //TODO disable optimal path for all clients when task is active
        break;
    }

    case START: {
        //database.network_changed_flag = false;
        direction = DIRECTION_UP;

        current_hierarchy = 1;
        ires = database.get_nodes_from_hierarchy(current_hierarchy, beerocks::TYPE_IRE_BACKHAUL);
        current_ire_it = ires.begin();

        state = ITERATION;
        break;
    }

    case ITERATION: {
        while (current_ire_it != ires.end() &&
               database.get_node_state(*current_ire_it) != beerocks::STATE_CONNECTED) {
            ++current_ire_it;
        }

        if (current_ire_it == ires.end()) {
            ++current_hierarchy;
            if (current_hierarchy == beerocks::HIERARCHY_MAX) {
                TASK_LOG(DEBUG) << "done optimizing network";
                finish();
                break;
            }

            ires =
                database.get_nodes_from_hierarchy(current_hierarchy, beerocks::TYPE_IRE_BACKHAUL);
            current_ire_it = ires.begin();
        }

        if (current_ire_it != ires.end()) {
            std::string ire = *current_ire_it;
            if (!database.is_node_wireless(ire)) {
                ++current_ire_it;
                break;
            }
            auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, tasks, ire, 0,
                                                                "ire_network_optimization_task");
            tasks.add_task(new_task);
            wait_for_task_end(new_task->id, 30000);
            state = DELAY;
            ++current_ire_it;
        }
        break;
    }

    case DELAY: {
        wait_for(2000);
        state = ITERATION;
        break;
    }

    default:
        finish();
        break;
    }
}

void ire_network_optimization_task::handle_task_end() {}
