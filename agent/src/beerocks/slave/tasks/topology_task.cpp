/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "topology_task.h"
#include "../agent_db.h"
#include "../backhaul_manager/backhaul_manager_thread.h"
#include "../helpers/media_type.h"

#include <bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message_backhaul.h>

#include <tlvf/ieee_1905_1/s802_11SpecificInformation.h>
#include <tlvf/ieee_1905_1/tlv1905NeighborDevice.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/ieee_1905_1/tlvMacAddress.h>

#include <tlvf/wfa_map/tlvApOperationalBSS.h>
#include <tlvf/wfa_map/tlvAssociatedClients.h>
#include <tlvf/wfa_map/tlvSupportedService.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

TopologyTask::TopologyTask(backhaul_manager &btl_ctx, ieee1905_1::CmduMessageTx &cmdu_tx)
    : Task(eTaskType::TOPOLOGY), m_btl_ctx(btl_ctx), m_cmdu_tx(cmdu_tx)
{
}

void TopologyTask::send_topology_notification()
{
    auto cmdu_header = m_cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
    if (!cmdu_header) {
        LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
        return;
    }
    auto db = AgentDB::get();
    m_btl_ctx.send_cmdu_to_broker(m_cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                  tlvf::mac_to_string(db->bridge.mac));
}
