/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _IRE_NETWORK_OPTIMIZATION_TASK_H_
#define _IRE_NETWORK_OPTIMIZATION_TASK_H_

#include "../db/db.h"
#include "optimal_path_task.h"
#include "task.h"
#include "task_pool.h"

#include <memory>

namespace son {
class ire_network_optimization_task : public task {
public:
    ire_network_optimization_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                  task_pool &tasks_,
                                  std::string task_name_ = std::string("ire_network_optimization"));
    virtual ~ire_network_optimization_task() {}

protected:
    virtual void work() override;
    virtual void handle_task_end() override;

private:
    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;

    enum eMobilityDirection { DIRECTION_UP, DIRECTION_DOWN, DIRECTION_UNDEFINED };

    enum states {
        INIT = 0,
        START,
        ITERATION,
        DELAY,
    };

    int state                         = INIT;
    eMobilityDirection direction      = eMobilityDirection::DIRECTION_UNDEFINED;
    bool original_client_roaming_flag = false;

    int current_hierarchy = 0;
    std::set<std::string> ires;
    std::set<std::string>::iterator current_ire_it = std::set<std::string>::iterator();
};

} // namespace son

#endif
