/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef _LOAD_BALANCER_TASK_H_
#define _LOAD_BALANCER_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

namespace son {
class load_balancer_task : public task {
public:
    load_balancer_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_,
                       std::string ire_mac_, std::string task_name_ = std::string("load_balancer"));
    virtual ~load_balancer_task() {}

protected:
    virtual void work() override;
    virtual void handle_responses_timeout(
        std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
        override;

private:
    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    std::string ire_mac;
    std::string sta_mac;
    std::string original_parent_mac;

    enum states {
        START = 0,
        REQUEST_LOAD_MEASUREMENTS,
        BALANCE_STATIONS,
    };

    int state = START;

    std::set<std::string> hostaps;
};

} // namespace son

#endif
