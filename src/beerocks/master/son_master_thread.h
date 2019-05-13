/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_MASTER_THREAD_H
#define _SON_MASTER_THREAD_H

#include "db/db.h"
#include "tasks/optimal_path_task.h"
#include "tasks/task_pool.h"

#include <beerocks/bcl/beerocks_defines.h>
#include <beerocks/bcl/beerocks_logging.h>
#include <beerocks/bcl/beerocks_message_structs.h>
#include <beerocks/bcl/beerocks_socket_thread.h>
#include <beerocks/bcl/network/network_utils.h>

#include <cstddef>
#include <ctime>
#include <stdint.h>

namespace son {
class master_thread : public beerocks::socket_thread {

public:
    master_thread(std::string master_uds_, db &database_);
    virtual ~master_thread();

    virtual bool init() override;
    virtual bool work() override;

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual void before_select() override;
    virtual void after_select(bool timeout) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    void disconnected_slave_cleanup();
    void handle_cmdu_1905_1_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool
    handle_cmdu_control_message(Socket *sd,
                                std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
                                ieee1905_1::CmduMessageRx &cmdu_rx);
    void handle_cmdu_control_ieee1905_1_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_slave_join(Socket *sd,
                           std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
                           ieee1905_1::CmduMessageRx &cmdu_rx);

    // 1905 messages handlers
    void handle_cmdu_1905_autoconfiguration_search(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx);

    db &database;
    task_pool tasks;
};

} // namespace son

#endif
