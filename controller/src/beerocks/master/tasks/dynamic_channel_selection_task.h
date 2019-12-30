/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
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
                                   task_pool &tasks_, std::string radio_mac_);
    virtual ~dynamic_channel_selection_task() {}

    typedef struct {
        sMacAddr radio_mac;
    } sTriggerSingleScan_event;

    typedef struct {
        sMacAddr radio_mac;
    } sScanTriggered_event;

    typedef struct {
        sMacAddr radio_mac;
    } sScanResultsReady_event;

    typedef struct {
        sMacAddr radio_mac;
        sDcsScanResultsElement scan_results;
    } sScanResultsDump_event;

    typedef struct {
        sMacAddr radio_mac;
    } sScanFinished_event;

    typedef struct {
        sMacAddr radio_mac;
    } sScanAbort_event;

    typedef struct {
        sMacAddr radio_mac;
    } sScanEnableChange_event;

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

protected:
    virtual void work() override;
    virtual void handle_event(int event_type, void *obj) override;
    virtual void handle_events_timeout(std::multiset<int> pending_events) override;

private:
    void dcs_wait_for_event(eEvent cs_event);

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

    // Strings Array
    static const char *s_ar_states[];
    static const char *s_ar_events[];

    eState m_fsm_state;

    const static int SCAN_TRIGGERED_WAIT_TIME_MSEC    = 20000;
    const static int SCAN_RESULTS_DUMP_WAIT_TIME_MSEC = 40000;

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    std::string m_radio_mac;
    std::chrono::steady_clock::time_point m_next_scan_timestamp_interval;
    std::chrono::steady_clock::time_point m_last_scan_try_timestamp;

    beerocks::eDcsScanErrCode m_last_scan_error_code = beerocks::eDcsScanErrCode::DCS_SCAN_NO_ERROR;
    eEvent m_dcs_waiting_for_event                   = eEvent::INVALID_EVENT;

    bool m_is_single_scan_pending = false;
    bool m_is_single_scan         = false;
};
} //namespace son
#endif