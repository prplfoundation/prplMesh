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

    void work() override;

    enum eEvent : uint8_t {
        AGENT_RADIO_STATE_CHANGED,
        AGENT_DEVICE_INITIALIZED,
    };

    void handle_event(uint8_t event_enum_value) override;

    bool handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac,
                     std::shared_ptr<beerocks_header> beerocks_header) override;

private:
    /* 1905.1 message handlers: */

    /**
    * @brief Handles 1905 Topology Discovery message.
    * 
    * @param[in] cmdu_rx Received CMDU.
    * @param[in] src_mac MAC address of the message sender.
    */
    void handle_topology_discovery(ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac);

    /**
    * @brief Handles 1905 Topology Query message.
    * 
    * @param[in] cmdu_rx Received CMDU.
    * @param[in] src_mac MAC address of the message sender.
    */
    void handle_topology_query(ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac);

    /* Helper functions */
    void send_topology_discovery();
    void send_topology_notification();

    backhaul_manager &m_btl_ctx;
    ieee1905_1::CmduMessageTx &m_cmdu_tx;
};

} // namespace beerocks

#endif // _TOPOLOGY_TASK_H_
