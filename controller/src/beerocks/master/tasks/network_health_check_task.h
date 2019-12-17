/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _NETWORK_HEALTH_CHECK_TASK_H_
#define _NETWORK_HEALTH_CHECK_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

// #include <bcl/network/network_utils.h>

namespace son {
class network_health_check_task : public task {
public:
    network_health_check_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_,
                              int starting_delay_ms_,
                              std::string task_name_ = std::string("network_health_check_task"));
    virtual ~network_health_check_task() {}

protected:
    virtual void work() override;
    virtual void
    handle_response(std::string slave_mac,
                    std::shared_ptr<beerocks::beerocks_header> beerocks_header) override;
    virtual void handle_responses_timeout(
        std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
        override;

private:
    bool send_arp_query(std::string mac);

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;

    int starting_delay_ms = 0;

    enum states {
        START = 0,
        IRE_HEALTH_CHECK,
        CLIENT_HEALTH_CHECK,
        SEND_QUERY,
    };
    int state = START;

    const int task_timeout_ms             = 60000;
    const int ire_last_seen_timeout_ms    = 120000;
    const int client_last_seen_timeout_ms = 140000;
    std::set<std::string> suspected_dis_clients;
    std::string pending_node;
};

} // namespace son

#endif
