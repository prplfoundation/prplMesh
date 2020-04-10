/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CLIENT_LOCATING_TASK_H_
#define _CLIENT_LOCATING_TASK_H_

#include <bcl/network/network_utils.h>

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

namespace son {
class client_locating_task : public task {
public:
    client_locating_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_,
                         std::string client_mac_, bool new_connection_, int starting_delay_ms_ = 0,
                         std::string eth_switch_ = beerocks::net::network_utils::ZERO_MAC_STRING,
                         std::string task_name_  = std::string("client_locating"));

    virtual ~client_locating_task() {}

protected:
    virtual void work() override;
    virtual void
    handle_response(std::string slave_mac,
                    std::shared_ptr<beerocks::beerocks_header> beerocks_header) override;

private:
    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;

    std::string client_mac;
    std::string client_ipv4;
    bool new_connection   = false;
    int starting_delay_ms = 0;
    std::string eth_switch;

    enum states {
        START = 0,
        SEND_ARP_QUERIES,
        FINISH,
    };
    int state = START;

    std::string deepest_slave;
    int deepest_slave_hierarchy    = -1;
    const int task_timeout_seconds = 30;
    uint8_t pending_ires_num       = 0;
};

} // namespace son

#endif
