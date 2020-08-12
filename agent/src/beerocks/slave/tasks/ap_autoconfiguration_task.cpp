/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_autoconfiguration_task.h"

#include "../agent_db.h"
#include "../backhaul_manager/backhaul_manager_thread.h"

#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvSupportedService.h>

#include <beerocks/tlvf/beerocks_message_control.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

static constexpr uint8_t MAX_FAILED_AUTOCONFIG_SEARCH_ATTEMPTS = 20;
static constexpr uint8_t AUTOCONFIG_DISCOVERY_TIMEOUT_SECONDS  = 1;

#define FSM_MOVE_STATE(radio_iface, new_state)                                                     \
    ({                                                                                             \
        LOG(TRACE) << "AP_AUTOCONFIGURATION " << radio_iface                                       \
                   << " FSM: " << fsm_state_to_string(m_state[radio_iface].state) << " --> "       \
                   << fsm_state_to_string(new_state);                                              \
        m_state[radio_iface].state = new_state;                                                    \
    })

const std::string ApAutoConfigurationTask::fsm_state_to_string(eState status)
{
    switch (status) {
    case eState::UNCONFIGURED:
        return "UNCONFIGURED";
    case eState::CONTROLLER_DISCOVERY:
        return "CONTROLLER_DISCOVERY";
    case eState::WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE:
        return "WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE";
    case eState::AP_CONFIGURATION:
        return "AP_CONFIGURATION";
    case eState::WAIT_FOR_AP_CONFIGURATION_COMPLETE:
        return "WAIT_FOR_AP_CONFIGURATION_COMPLETE";
    case eState::CONFIGIRED:
        return "CONFIGIRED";
    default:
        LOG(ERROR) << "state argument doesn't have an enum";
        break;
    }
    return std::string();
}

ApAutoConfigurationTask::ApAutoConfigurationTask(backhaul_manager &btl_ctx,
                                                 ieee1905_1::CmduMessageTx &cmdu_tx)
    : Task(eTaskType::AP_AUTOCONFIGURATION), m_btl_ctx(btl_ctx), m_cmdu_tx(cmdu_tx)
{
}

void ApAutoConfigurationTask::work()
{
    uint8_t configured_aps_count = 0;
    for (auto &state_kv : m_state) {
        // Currently commented out because it makes the compilation to fail on unused variable.
        // Will be commented out on the next commits.
        // const auto &radio_iface = state_kv.first;
        auto &state_status = state_kv.second;
        switch (state_status.state) {
        case eState::UNCONFIGURED: {
            break;
        }
        case eState::CONTROLLER_DISCOVERY: {
            break;
        }
        case eState::WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE: {
            break;
        }
        case eState::AP_CONFIGURATION: {
            break;
        }
        case eState::WAIT_FOR_AP_CONFIGURATION_COMPLETE: {
            break;
        }
        case eState::CONFIGIRED: {
            configured_aps_count++;
            break;
        }
        default:
            break;
        }
    }

    // Update status on the database.
    auto db                                     = AgentDB::get();
    db->statuses.ap_autoconfiguration_completed = configured_aps_count == m_state.size();
}

void ApAutoConfigurationTask::handle_event(uint8_t event_enum_value, void *event_obj) {}

bool ApAutoConfigurationTask::handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx,
                                          const sMacAddr &src_mac,
                                          std::shared_ptr<beerocks_header> beerocks_header)
{
    return true;
}
