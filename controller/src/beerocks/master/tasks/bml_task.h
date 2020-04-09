/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_TASK_H_
#define _BML_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

namespace son {
class bml_task : public task {
public:
    struct listener_general_register_unregister_event {
        Socket *sd;
    };

    struct connection_change_event {
        std::string mac;
        bool force_client_disconnect = false;
    };

    struct stats_info_available_event {
        std::set<std::string> valid_hostaps;
    };

    struct bss_tm_req_available_event {
        std::string target_bssid;
        uint8_t disassoc_imminent;
    };

    struct bh_roam_req_available_event {
        std::string bssid;
        uint8_t channel;
    };

    struct client_allow_req_available_event {
        std::string sta_mac;
        std::string hostap_mac;
        std::string ip;
    };

    struct client_disallow_req_available_event {
        std::string sta_mac;
        std::string hostap_mac;
    };

    struct acs_start_event {
        std::string hostap_mac;
    };

    struct csa_notification_event {
        std::string hostap_mac;
        uint8_t bandwidth;
        uint8_t channel;
        uint8_t channel_ext_above_primary;
        uint16_t vht_center_frequency;
    };

    struct cac_status_changed_notification_event {
        std::string hostap_mac;
        uint8_t cac_completed;
    };

    enum events {
        REGISTER_TO_NW_MAP_UPDATES,
        UNREGISTER_TO_NW_MAP_UPDATES,
        REGISTER_TO_STATS_UPDATES,
        UNREGISTER_TO_STATS_UPDATES,
        REGISTER_TO_EVENTS_UPDATES,
        UNREGISTER_TO_EVENTS_UPDATES,
        CONNECTION_CHANGE,
        STATS_INFO_AVAILABLE,
        BSS_TM_REQ_EVENT_AVAILABLE,
        BH_ROAM_REQ_EVENT_AVAILABLE,
        CLIENT_ALLOW_REQ_EVENT_AVAILABLE,
        CLIENT_DISALLOW_REQ_EVENT_AVAILABLE,
        ACS_START_EVENT_AVAILABLE,
        CSA_NOTIFICATION_EVENT_AVAILABLE,
        CAC_STATUS_CHANGED_NOTIFICATION_EVENT_AVAILABLE,
    };

public:
    bml_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_);
    virtual ~bml_task() {}

protected:
    virtual void work() override;
    virtual void handle_event(int event_type, void *obj) override;

private:
    enum states {
        START = 0,
        IDLE,
        LISTENING,
    };
    int state = START;

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;

    void update_bml_nw_map(std::string mac, bool force_client_disconnect = false);
};

} // namespace son

#endif
