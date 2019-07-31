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
};

#endif
