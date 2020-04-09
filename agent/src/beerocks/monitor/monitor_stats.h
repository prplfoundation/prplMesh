/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MONITOR_LOAD_H
#define MONITOR_LOAD_H

#include "monitor_db.h"

#include <bcl/beerocks_message_structs.h>
#include <bcl/network/socket.h>

#include <tlvf/CmduMessageTx.h>

namespace son {
class monitor_stats {

public:
    monitor_stats(ieee1905_1::CmduMessageTx &cmdu_tx_);
    ~monitor_stats() {}
    bool start(monitor_db *mon_db_, Socket *slave_socket_);
    void stop();

    void add_request(uint16_t id, uint8_t sync);

    void process();

    int8_t conf_total_ch_load_notification_lo_th_percent    = 20;
    int8_t conf_total_ch_load_notification_hi_th_percent    = 90;
    int8_t conf_total_ch_load_notification_delta_th_percent = 30;
    int8_t conf_min_active_client_count                     = 2;
    int8_t conf_active_client_th                            = 3;
    int8_t conf_client_load_notification_delta_th_percent   = 20;
    uint32_t conf_ap_idle_threshold_B                       = 10000;
    uint32_t conf_ap_active_threshold_B                     = 20000;
    uint16_t conf_ap_idle_stable_time_sec                   = 600;

    //const uint32_t AP_IDLE_BYTES_THRESHOLD = 10000;
    const uint32_t AP_ACTIVE_BYTES_THRESHOLD = 20000;
    //const uint32_t AP_IDLE_TIME_SEC_THRESHOLD = 70; //600;

    uint32_t idle_timer = 0;
    std::chrono::steady_clock::time_point idle_prev_timestamp;
    beerocks::eApActiveMode eApActiveMode = beerocks::eApActiveMode::AP_ACTIVE_MODE;

private:
    void calculate_client_load(monitor_sta_node *sta_node, monitor_radio_node *radio_node,
                               int active_sta_th);

    std::string parent_thread_name;
    monitor_db *mon_db   = nullptr;
    Socket *slave_socket = nullptr;
    uint32_t next_poll_id;
    std::list<uint16_t> requests_list;

    ieee1905_1::CmduMessageTx &cmdu_tx;
};

} // namespace son

#endif
