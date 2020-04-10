/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "statistics_polling_task.h"

#include "../db/db_algo.h"
#include "../son_actions.h"
#include "bml_task.h"

#include <beerocks/tlvf/beerocks_message.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace son;

statistics_polling_task::statistics_polling_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                 task_pool &tasks_)
    : task("statistics polling task"), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_)
{
}

void statistics_polling_task::work()
{
    switch (state) {
    case SEND_REQUESTS: {
        if (!database.settings_diagnostics_measurements()) {
            return;
        }

        auto hostaps = database.get_active_hostaps();
        for (auto &hostap : hostaps) {
            auto stats_request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST>(cmdu_tx, id);
            if (stats_request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return;
            }

            auto agent_mac = database.get_node_parent_ire(hostap);
            son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, hostap);
            add_pending_mac(hostap,
                            beerocks_message::ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE);
        }

        //TASK_LOG(DEBUG) << "sent requests";
        state = SEND_UPDATES;
        set_responses_timeout(
            1500 *
            database.config
                .diagnostics_measurements_polling_rate_sec); // 1500 msec to prevent missing statistics time gaps
        wait_for(
            1000 *
            database.config
                .diagnostics_measurements_polling_rate_sec); // 1000 msec to prevent the task getting to next step before 1000 msec
        break;
    }
    case SEND_UPDATES: {
        //TASK_LOG(DEBUG) << "updating bml task";
        if (!valid_hostaps.empty()) {
            int bml_task_id = database.get_bml_task_id();

            bml_task::stats_info_available_event new_event;
            new_event.valid_hostaps = valid_hostaps;

            tasks.push_event(bml_task_id, bml_task::STATS_INFO_AVAILABLE, &new_event);

            valid_hostaps.clear();
        } else {
            // TASK_LOG(DEBUG) << "valid_hostaps is empty";
        }
        state = SEND_REQUESTS;
        break;
    }
    }
}

void statistics_polling_task::handle_response(
    std::string mac, std::shared_ptr<beerocks::beerocks_header> beerocks_header)
{
    valid_hostaps.insert(mac);
}

void statistics_polling_task::handle_responses_timeout(
    std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
{
    for (auto macs : timed_out_macs) {
        TASK_LOG(WARNING) << "timeout slave " << macs.first;
    }
    return;
}
