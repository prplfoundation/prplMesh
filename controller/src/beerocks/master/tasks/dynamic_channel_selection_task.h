/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _DYNAMIC_CHANNEL_SELECTION_TASK_H_
#define _DYNAMIC_CHANNEL_SELECTION_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

#include <beerocks/tlvf/beerocks_message.h>

#include <chrono>

// Helper MACROs for Enum/String generation
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

namespace son {

class dynamic_channel_selection_task : public task, public std::enable_shared_from_this<task> {
public:
    dynamic_channel_selection_task(db &database, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                   task_pool &tasks_, sMacAddr radio_mac_);
    virtual ~dynamic_channel_selection_task() {}

    struct sScanEvent {
        sMacAddr radio_mac;
        union {
            beerocks_message::sChannelScanResults scan_results;
        } udata;
    };

#define FOREACH_DCS_EVENT(EVENT)                                                                   \
    EVENT(INVALID_EVENT)                                                                           \
    EVENT(TRIGGER_SINGLE_SCAN)                                                                     \
    EVENT(SCAN_TRIGGERED)                                                                          \
    EVENT(SCAN_RESULTS_READY)                                                                      \
    EVENT(SCAN_RESULTS_DUMP)                                                                       \
    EVENT(SCAN_FINISHED)                                                                           \
    EVENT(SCAN_ABORTED)                                                                            \
    EVENT(SCAN_ENABLE_CHANGE)

    // Event ENUM
    enum class eEvent { FOREACH_DCS_EVENT(GENERATE_ENUM) };

#define FOREACH_DCS_STATE(STATE)                                                                   \
    STATE(INIT)                                                                                    \
    STATE(IDLE)                                                                                    \
    STATE(TRIGGER_SCAN)                                                                            \
    STATE(WAIT_FOR_SCAN_TRIGGERED)                                                                 \
    STATE(WAIT_FOR_RESULTS_READY)                                                                  \
    STATE(WAIT_FOR_RESULTS_DUMP)                                                                   \
    STATE(SCAN_DONE)                                                                               \
    STATE(ABORT_SCAN)

    // State ENUM
    enum class eState { FOREACH_DCS_STATE(GENERATE_ENUM) };

protected:
    virtual void work() override;
    virtual void handle_event(int event_type, void *obj) override;
    virtual void handle_events_timeout(std::multiset<int> pending_events) override;

private:
    void dcs_wait_for_event(eEvent cs_event);
    void fsm_move_state(eState new_state);
    bool fsm_in_state(eState state);

    beerocks::eChannelScanErrCode dcs_request_scan_trigger();
    beerocks::eChannelScanErrCode dcs_request_scan_dump();

    eState m_fsm_state;

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    sMacAddr m_radio_mac;
    std::chrono::steady_clock::time_point m_next_scan_timestamp_interval;
    std::chrono::steady_clock::time_point m_last_scan_try_timestamp;

    beerocks::eChannelScanErrCode m_last_scan_error_code =
        beerocks::eChannelScanErrCode::CHANNEL_SCAN_INVALID_PARAMS;
    eEvent m_dcs_waiting_for_event = eEvent::INVALID_EVENT;

    bool m_is_single_scan_pending = false;
    bool m_is_single_scan         = false;
};
} //namespace son
#endif
