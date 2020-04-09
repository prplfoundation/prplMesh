/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_ACTIONS_H_
#define _SON_ACTIONS_H_

#include "son_master_thread.h"

#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_utils.h>

#define CLI_LOG(a) LOG(a)

#define LOG_CLI(LEVEL, msg)                                                                        \
    {                                                                                              \
        std::stringstream ss;                                                                      \
        ss << msg;                                                                                 \
        son_actions::send_cli_debug_message(database, cmdu_tx, ss);                                \
        CLI_LOG(LEVEL) << ss.rdbuf();                                                              \
    }

namespace son {
class son_actions {
public:
    static void handle_completed_connection(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                            task_pool &tasks, std::string client_mac);
    static bool add_node_to_default_location(db &database, std::string client_mac);
    static void unblock_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::string sta_mac);
    static int steer_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks,
                         std::string sta_mac, std::string chosen_hostap, bool disassoc_imminent,
                         int disassoc_timer_ms = beerocks::BSS_STEER_DISASSOC_TIMER_MS,
                         bool steer_restricted = false);
    static void disconnect_client(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                  const std::string &client_mac, const std::string &bssid,
                                  eDisconnectType type, uint32_t reason);
    static void activate_hostap(db &database, task_pool &tasks, std::string hostap_mac);
    static bool set_hostap_active(db &database, task_pool &tasks, std::string hostap_mac,
                                  bool active);
    static void send_cli_debug_message(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                       std::stringstream &ss);

    static void handle_dead_node(std::string mac, std::string hostap_mac, db &database,
                                 ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks);
    static bool validate_beacon_measurement_report(beerocks_message::sBeaconResponse11k report,
                                                   std::string sta_mac, std::string bssid);
    static bool has_matching_operating_class(wfa_map::tlvApRadioBasicCapabilities &radio_basic_caps,
                                             const wireless_utils::sBssInfoConf &bss_info_conf);
    static bool send_cmdu_to_agent(const std::string &dest_mac, ieee1905_1::CmduMessageTx &cmdu_tx,
                                   db &database, const std::string &radio_mac = std::string());

private:
    static bool
    check_hostap_activability(db &database,
                              std::string mac); // note: there isn't a word - "activability"
};

} // namespace son
#endif
