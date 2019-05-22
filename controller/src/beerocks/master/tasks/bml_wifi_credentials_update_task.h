/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_WIFI_CREDENTIALS_UPDATE_TASK_H_
#define _BML_WIFI_CREDENTIALS_UPDATE_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

#include <beerocks/bcl/network/socket.h>

#include <beerocks/tlvf/beerocks_message_common.h>

namespace son {
class bml_wifi_credentials_update_task : public task, public std::enable_shared_from_this<task> {
public:
    enum events {
        NEW_CREDENTIALS_REQUEST,
    };

    typedef struct {
        beerocks_message::sWifiCredentials cred;
        Socket *sd;
    } new_credential_event_t;

    bml_wifi_credentials_update_task(db &database, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                     task_pool &tasks_, Socket *sd_,
                                     const beerocks_message::sWifiCredentials &credentials_);

    virtual ~bml_wifi_credentials_update_task() {}

protected:
    virtual void work() override;
    virtual void handle_event(int event_type, void *obj) override;
    virtual void handle_responses_timeout(
        std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
        override;

private:
    enum class states {
        START = 0,
        PREPARE,
        PRE_COMMIT,
        COMMIT,
        WAIT_FOR_NETWORK_DOWN,
        WAIT_FOR_NETWORK_RECOVERY,
        ABORT,
        END
    };
    states state      = states::START;
    states state_prev = states::START;

    const int PREPARE_TIMEOUT_MSC    = 6000;
    const int PRE_COMMIT_TIMEOUT_MSC = 60000;

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    Socket *bml_client_socket;
    beerocks_message::sWifiCredentials credentials = {};

    std::chrono::steady_clock::time_point network_recovery_timeout;

    std::set<std::string> backhaul_manager_slaves;
    std::set<std::string> all_slaves;
    std::set<std::string> timedout_macs;
    std::unordered_map<std::string, bool> network_platforms;
    std::queue<new_credential_event_t> pending_credentials_requests;

private:
    void send_bml_response(uint8_t error_code);
};
} //namespace son
#endif
