/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _STATISTICS_POLLING_TASK_H_
#define _STATISTICS_POLLING_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

namespace son {
class statistics_polling_task : public task {
public:
    statistics_polling_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_);
    virtual ~statistics_polling_task() {}

protected:
    virtual void work() override;
    virtual void handle_response(std::string slave_mac,
                                 beerocks_message::eActionOp_CONTROL action_op,
                                 ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void handle_responses_timeout(
        std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
        override;

private:
    enum states {
        SEND_REQUESTS = 0,
        SEND_UPDATES,
    };
    int state = SEND_REQUESTS;

    std::set<std::string> valid_hostaps;

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
};

} // namespace son

#endif
