/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _WFA_CA_H_
#define _WFA_CA_H_

#include "db/db.h"
#include "tasks/task_pool.h"

using namespace son;

class wfa_ca {
public:
    static void
    handle_wfa_ca_message(Socket *sd,
                          std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
                          ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx,
                          db &database, task_pool &tasks);

private:
    enum class eWfaCaStatus : uint8_t { RUNNING, INVALID, ERROR, COMPLETE };
    static const std::string wfa_ca_status_to_string(eWfaCaStatus status);

    enum class eWfaCaCommand : uint8_t {
        CA_GET_VERSION,
        DEVICE_GET_INFO,
        DEV_GET_PARAMETER,
        DEV_RESET_DEFAULT,
        DEV_SEND_1905,
        DEV_SET_CONFIG,
        WFA_CA_COMMAND_MAX
    };
    static eWfaCaCommand wfa_ca_command_from_string(std::string command);

    static void reply_invalid_error(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx,
                                    eWfaCaStatus status, const std::string &description);

    static bool reply(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx,
                      const std::string &reply_string);
};

#endif
