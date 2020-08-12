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

void ApAutoConfigurationTask::handle_event(uint8_t event_enum_value, const void *event_obj)
{
    switch (eEvent(event_enum_value)) {
    case START_AP_AUTOCONFIGURATION: {
        auto db = AgentDB::get();
        for (const auto &radio : db->get_radios_list()) {
            if (!radio) {
                continue;
            }

            if (event_obj) {
                auto specific_iface_ptr = reinterpret_cast<const std::string *>(event_obj);
                if (*specific_iface_ptr != radio->front.iface_name) {
                    continue;
                }
            }

            LOG(DEBUG) << "starting discovery sequence on radio_iface=" << radio->front.iface_name;
            FSM_MOVE_STATE(radio->front.iface_name, eState::CONTROLLER_DISCOVERY);

            // Reset the discovery statuses.
            m_discovery_status[radio->front.freq_type] = {};
        }
        // Call work() to not waste time, and send_ap_autoconfiguration_search_message immediately.
        work();
        break;
    }
    default: {
        LOG(DEBUG) << "Message handler doesn't exists for event type " << event_enum_value;
        break;
    }
    }
}

bool ApAutoConfigurationTask::handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx,
                                          const sMacAddr &src_mac,
                                          std::shared_ptr<beerocks_header> beerocks_header)
{
    return true;
}

bool ApAutoConfigurationTask::send_ap_autoconfiguration_search_message(
    const std::string &radio_iface)
{
    auto db = AgentDB::get();

    ieee1905_1::tlvAutoconfigFreqBand::eValue freq_band =
        ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    /*
     * TODO
     * this is a workaround, need to find a better way to know each slave's band
     */
    auto radio = db->radio(radio_iface);
    if (!radio) {
        LOG(DEBUG) << "Radio of iface " << radio_iface << " does not exist on the db";
        return false;
    }
    if (radio->front.freq_type == beerocks::eFreqType::FREQ_24G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    } else if (radio->front.freq_type == beerocks::eFreqType::FREQ_5G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_5_GHZ;
    } else {
        LOG(ERROR) << "unsupported freq_type=" << int(radio->front.freq_type)
                   << ", iface=" << radio_iface;
        return false;
    }

    auto cmdu_header =
        m_cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE);
    if (!cmdu_header) {
        LOG(ERROR) << "cmdu creation of type AP_AUTOCONFIGURATION_SEARCH_MESSAGE, has failed";
        return false;
    }

    auto tlvAlMacAddress = m_cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMacAddress) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAlMacAddress failed";
        return false;
    }
    tlvAlMacAddress->mac() = db->bridge.mac;

    auto tlvSearchedRole = m_cmdu_tx.addClass<ieee1905_1::tlvSearchedRole>();
    if (!tlvSearchedRole) {
        LOG(ERROR) << "addClass ieee1905_1::tlvSearchedRole failed";
        return false;
    }
    tlvSearchedRole->value() = ieee1905_1::tlvSearchedRole::REGISTRAR;

    auto tlvAutoconfigFreqBand = m_cmdu_tx.addClass<ieee1905_1::tlvAutoconfigFreqBand>();
    if (!tlvAutoconfigFreqBand) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAutoconfigFreqBand failed";
        return false;
    }
    tlvAutoconfigFreqBand->value() = freq_band;

    auto tlvSupportedService = m_cmdu_tx.addClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedService) {
        LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed";
        return false;
    }
    if (!tlvSupportedService->alloc_supported_service_list()) {
        LOG(ERROR) << "alloc_supported_service_list failed";
        return false;
    }
    auto supportedServiceTuple = tlvSupportedService->supported_service_list(0);
    if (!std::get<0>(supportedServiceTuple)) {
        LOG(ERROR) << "Failed accessing supported_service_list";
        return false;
    }
    std::get<1>(supportedServiceTuple) =
        wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT;

    auto tlvSearchedService = m_cmdu_tx.addClass<wfa_map::tlvSearchedService>();
    if (!tlvSearchedService) {
        LOG(ERROR) << "addClass wfa_map::tlvSearchedService failed";
        return false;
    }
    if (!tlvSearchedService->alloc_searched_service_list()) {
        LOG(ERROR) << "alloc_searched_service_list failed";
        return false;
    }
    auto searchedServiceTuple = tlvSearchedService->searched_service_list(0);
    if (!std::get<0>(searchedServiceTuple)) {
        LOG(ERROR) << "Failed accessing searched_service_list";
        return false;
    }
    std::get<1>(searchedServiceTuple) =
        wfa_map::tlvSearchedService::eSearchedService::MULTI_AP_CONTROLLER;

    // Add prplMesh handshake in a vendor specific TLV.
    // If the controller is prplMesh, it will reply to the autoconfig search with
    // handshake response.
    auto request =
        message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST>(
            m_cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed adding cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST";
        return false;
    }
    auto beerocks_header                      = message_com::get_beerocks_header(m_cmdu_tx);
    beerocks_header->actionhdr()->direction() = beerocks::BEEROCKS_DIRECTION_CONTROLLER;
    LOG(DEBUG) << "sending autoconfig search message, bridge_mac=" << db->bridge.mac;
    return m_btl_ctx.send_cmdu_to_broker(m_cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                         tlvf::mac_to_string(db->bridge.mac));
}
