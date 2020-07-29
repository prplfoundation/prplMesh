/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TOPOLOGY_TASK_H_
#define _TOPOLOGY_TASK_H_

#include "task.h"

#include <tlvf/CmduMessageTx.h>

namespace beerocks {

// Forward decleration for backhaul_manager context saving
class backhaul_manager;

class TopologyTask : public Task {
public:
    TopologyTask(backhaul_manager &btl_ctx, ieee1905_1::CmduMessageTx &cmdu_tx);
    ~TopologyTask() {}

private:
    /* Helper functions */
    void send_topology_discovery();
    void send_topology_notification();

    backhaul_manager &m_btl_ctx;
    ieee1905_1::CmduMessageTx &m_cmdu_tx;
};

} // namespace beerocks

#endif // _TOPOLOGY_TASK_H_
