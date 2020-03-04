/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CLIENT_STEERING_TASK_H_
#define _CLIENT_STEERING_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

namespace son {
class client_steering_task : public task {
public:
    enum events {
        STA_CONNECTED,
        STA_DISCONNECTED,
        BTM_REPORT_RECEIVED,
        BSS_TM_REQUEST_REJECTED,
    };

public:
    client_steering_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_,
                         std::string sta_mac_, std::string hostap_mac, bool disassoc_imminent_,
                         int disassoc_timer_ms_ = beerocks::BSS_STEER_DISASSOC_TIMER_MS,
                         bool steer_restricted_ = false,
                         std::string task_name_ = std::string("client_steering_task"));
    virtual ~client_steering_task() {}

protected:
    virtual void work() override;
    virtual void handle_event(int event_type, void *obj) override;
    virtual void handle_task_end() override;

private:
    void steer_sta();

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    const std::string sta_mac;
    const std::string hostap_mac;
    std::string original_hostap_mac;
    bool steering_success  = false;
    bool disassoc_imminent = true;
    const int disassoc_timer_ms;
    bool btm_report_received = false;
    bool steer_restricted    = false;

    const static int steering_wait_time_ms = 25000;

    enum states {
        STEER = 0,
        FINALIZE,
    };

    int state = 0;
};
} // namespace son

#endif
