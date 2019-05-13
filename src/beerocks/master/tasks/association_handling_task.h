/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _ASSOCIATION_HANDLING_TASK_H_
#define _ASSOCIATION_HANDLING_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

namespace son {
class association_handling_task : public task {
public:
    association_handling_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_,
                              std::string sta_mac_,
                              std::string task_name_ = std::string("association_handling"));
    virtual ~association_handling_task() {}

protected:
    virtual void work() override;
    virtual void handle_response(std::string radio_mac,
                                 beerocks_message::eActionOp_CONTROL action_op,
                                 ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void handle_responses_timeout(
        std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
        override;

private:
    void finalize_new_connection();

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    const std::string sta_mac;
    std::string original_parent_mac;

    enum states {
        START = 0,
        START_RSSI_MONITORING,
        CHECK_11K_BEACON_MEASURE_CAP,
        REQUEST_RSSI_MEASUREMENT_WAIT,
        REQUEST_RSSI_MEASUREMENT,
        FINISH,
    };

    int state = START;

    int attempts     = 0;
    int max_attempts = 0;

    std::chrono::steady_clock::time_point task_started_timestamp;
};
} // namespace son
#endif
