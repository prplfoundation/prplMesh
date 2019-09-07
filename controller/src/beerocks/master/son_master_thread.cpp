/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "son_master_thread.h"
#include "son_actions.h"
#include "son_management.h"
#include "tasks/bml_task.h"
#include "tasks/channel_selection_task.h"
#include "tasks/client_steering_task.h"
#include "tasks/load_balancer_task.h"
#include "tasks/optimal_path_task.h"
#include "tasks/statistics_polling_task.h"
#ifdef BEEROCKS_RDKB
#include "tasks/rdkb/rdkb_wlan_task.h"
#endif
#include "db/db_algo.h"
#include "db/network_map.h"
#include "tasks/bml_wifi_credentials_update_task.h"
#include "tasks/client_locating_task.h"
#include "tasks/ire_network_optimization_task.h"
#include "tasks/network_health_check_task.h"

#include <beerocks/bcl/beerocks_backport.h>
#include <beerocks/bcl/beerocks_version.h>
#include <beerocks/bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_1905_vs.h>
#include <beerocks/tlvf/beerocks_message_control.h>

#include <tlvf/ieee_1905_1/eMessageType.h>
#include <tlvf/ieee_1905_1/eTlvType.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddressType.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/ieee_1905_1/tlvMacAddress.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/wfa_map/tlvClientAssociationEvent.h>
#include <tlvf/wfa_map/tlvHigherLayerData.h>
#include <tlvf/wfa_map/tlvOperatingChannelReport.h>
#include <tlvf/wfa_map/tlvRadioOperationRestriction.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvSteeringBTMReport.h>
#include <tlvf/wfa_map/tlvSupportedService.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>

#define SOCKET_MAX_CONNECTIONS 20
#define SOCKETS_SELECT_TIMEOUT_MSEC 50
#define CLIENT_RECONNECT_TIME_WINDOW_MSEC 2000

using namespace beerocks;
using namespace net;
using namespace son;

master_thread::master_thread(std::string master_uds_, db &database_)
    : beerocks::socket_thread(master_uds_), database(database_)
{
    thread_name = "master";
}

master_thread::~master_thread() { LOG(DEBUG) << "closing"; }

bool master_thread::init()
{
    set_server_max_connections(SOCKET_MAX_CONNECTIONS);
    set_select_timeout(SOCKETS_SELECT_TIMEOUT_MSEC);

    auto new_statistics_polling_task =
        std::make_shared<statistics_polling_task>(database, cmdu_tx, tasks);
    tasks.add_task(new_statistics_polling_task);

    auto new_bml_task = std::make_shared<bml_task>(database, cmdu_tx, tasks);
    tasks.add_task(new_bml_task);

    auto new_channel_selection_task =
        std::make_shared<channel_selection_task>(database, cmdu_tx, tasks);
    tasks.add_task(new_channel_selection_task);

    if (database.settings_health_check()) {
        auto new_network_health_check_task = std::make_shared<network_health_check_task>(
            database, cmdu_tx, tasks, 0, "network_health_check_task");
        tasks.add_task(new_network_health_check_task);
    } else {
        LOG(DEBUG) << "Health check is DISABLED!";
    }

    if (database.setting_certification_mode()) {
        if (!database.allocate_certification_tx_buffer()) {
            LOG(ERROR) << "failed to allocate certification_tx_buffer";
            return false;
        }
    }

    if (database.config.ucc_listener_port != 0) {
        m_controller_ucc_listener = std::make_unique<controller_ucc_listener>(database);
        if (m_controller_ucc_listener && !m_controller_ucc_listener->start("ucc_listener")) {
            LOG(ERROR) << "failed start controller_ucc_listener";
            return false;
        }
    }

    return socket_thread::init();
}

bool master_thread::work()
{
    if (!socket_thread::work()) {
        return false;
    }

    tasks.run_tasks();
    return true;
}

void master_thread::before_select() { database.unlock(); }

void master_thread::after_select(bool timeout) { database.lock(); }

std::string master_thread::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

bool master_thread::socket_disconnected(Socket *sd)
{
    std::string mac = sd->getPeerMac();

    if (database.get_node_socket(mac) == sd) {
        LOG(DEBUG) << "socket disconnected, mac=" << mac << ", sd=" << intptr_t(sd);

        auto backhaul = database.get_node_parent_backhaul(mac);
        if (database.get_node_socket(backhaul) == sd) {
            son_actions::handle_dead_node(backhaul, database.get_node_parent(backhaul), database,
                                          cmdu_tx, tasks);
        } else {
            son_actions::handle_dead_node(mac, database.get_node_parent(mac), database, cmdu_tx,
                                          tasks);
        }

        disconnected_slave_cleanup();
        return false;
    } else {
        if (mac.empty()) {
            LOG(DEBUG) << "socket with no mac disconnect sd=" << intptr_t(sd);
            database.remove_cli_socket(sd);
            database.remove_bml_socket(sd);
#ifdef BEEROCKS_RDKB
            if (database.settings_rdkb_extensions()) {
                //TODO - use rdkb_wlan_hal_db instead of task event
                rdkb_wlan_task::listener_general_register_unregister_event new_event;
                new_event.sd = sd;
                tasks.push_event(database.get_rdkb_wlan_task_id(),
                                 rdkb_wlan_task::events::STEERING_REMOVE_SOCKET, &new_event);
            }
#endif
        } else {
            LOG(DEBUG) << "old socket disconnected of mac " << mac
                       << ", ignoring. sd=" << intptr_t(sd);
        }
    }
    return true;
}

void master_thread::disconnected_slave_cleanup()
{
    while (!database.disconnected_slave_mac_queue_empty()) {
        std::string slave_mac = database.disconnected_slave_mac_queue_pop();
        auto sd               = database.get_node_socket(slave_mac);
        if (sd) {
            LOG(DEBUG) << "closing socket sd=" << intptr_t(sd) << " of node " << slave_mac;
            remove_socket(sd);
            delete sd;
        }
    }
}

bool master_thread::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool vendor_specific = false;

    if (cmdu_rx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
        vendor_specific = true;
    }

    if (vendor_specific) {
        auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
        if (!beerocks_header) {
            LOG(ERROR) << "Not a vendor specific message";
            return false;
        }
        switch (beerocks_header->action()) {
        case beerocks_message::ACTION_CLI: {
            son_management::handle_cli_message(sd, beerocks_header, cmdu_rx, cmdu_tx, database,
                                               tasks);
        } break;
        case beerocks_message::ACTION_BML: {
            son_management::handle_bml_message(sd, beerocks_header, cmdu_rx, cmdu_tx, database,
                                               tasks);
        } break;
        case beerocks_message::ACTION_CONTROL: {
            handle_cmdu_control_message(sd, beerocks_header, cmdu_rx);
        } break;
        default: {
            LOG(ERROR) << "Unknown message, action: " << int(beerocks_header->action());
        }
        }
    } else {
        LOG(DEBUG) << "received 1905.1 cmdu message";
        handle_cmdu_1905_1_message(sd, cmdu_rx);
    }

    disconnected_slave_cleanup();

    return true;
}

bool master_thread::handle_cmdu_1905_1_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE:
        return handle_cmdu_1905_autoconfiguration_search(sd, cmdu_rx);
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE:
        return handle_cmdu_1905_autoconfiguration_WSC(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CHANNEL_PREFERENCE_REPORT_MESSAGE:
        return handle_cmdu_1905_channel_preference_report(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CHANNEL_SELECTION_RESPONSE_MESSAGE:
        return handle_cmdu_1905_channel_selection_response(sd, cmdu_rx);
    case ieee1905_1::eMessageType::STEERING_COMPLETED_MESSAGE:
        return handle_cmdu_1905_steering_completed_message(sd, cmdu_rx);
    case ieee1905_1::eMessageType::ACK_MESSAGE:
        return handle_cmdu_1905_ack_message(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CLIENT_STEERING_BTM_REPORT_MESSAGE:
        return handle_cmdu_1905_client_steering_btm_report_message(sd, cmdu_rx);
    case ieee1905_1::eMessageType::OPERATING_CHANNEL_REPORT_MESSAGE:
        return handle_cmdu_1905_operating_channel_report(sd, cmdu_rx);
    case ieee1905_1::eMessageType::TOPOLOGY_DISCOVERY_MESSAGE:
        return handle_cmdu_1905_topology_discovery(sd, cmdu_rx);
    case ieee1905_1::eMessageType::HIGHER_LAYER_DATA_MESSAGE:
        return handle_cmdu_1905_higher_layer_data_message(sd, cmdu_rx);
    case ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE:
        return handle_cmdu_1905_topology_notification(sd, cmdu_rx);
    case ieee1905_1::eMessageType::LINK_METRIC_RESPONSE_MESSAGE:
        return handle_cmdu_1905_link_metric_response(sd, cmdu_rx);
    default:
        break;
    }

    LOG(WARNING) << "Unknown 1905 message received. Ignoring";
    return true;
}

bool master_thread::handle_cmdu_1905_autoconfiguration_search(Socket *sd,
                                                              ieee1905_1::CmduMessageRx &cmdu_rx)
{
    std::string al_mac;

    LOG(DEBUG) << "Received AP_AUTOCONFIGURATION_SEARCH_MESSAGE";

    std::shared_ptr<ieee1905_1::tlvAlMacAddressType> tlvAlMacAddressType     = nullptr;
    std::shared_ptr<ieee1905_1::tlvSearchedRole> tlvSearchedRole             = nullptr;
    std::shared_ptr<ieee1905_1::tlvAutoconfigFreqBand> tlvAutoconfigFreqBand = nullptr;
    std::shared_ptr<wfa_map::tlvSupportedService> tlvSupportedServiceIn      = nullptr;
    std::shared_ptr<wfa_map::tlvSearchedService> tlvSearchedService          = nullptr;
    int type = cmdu_rx.getNextTlvType();

    while ((type = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        switch (type) {
        case int(wfa_map::eTlvTypeMap::TLV_SUPPORTED_SERVICE):
            LOG(DEBUG) << "Found TLV_SUPPORTED_SERVICE TLV";
            tlvSupportedServiceIn = cmdu_rx.addClass<wfa_map::tlvSupportedService>();
            break;
        case int(ieee1905_1::eTlvType::TLV_AL_MAC_ADDRESS_TYPE):
            LOG(DEBUG) << "Found TLV_AL_MAC_ADDRESS_TYPE TLV";
            tlvAlMacAddressType = cmdu_rx.addClass<ieee1905_1::tlvAlMacAddressType>();
            break;
        case int(ieee1905_1::eTlvType::TLV_SEARCHED_ROLE):
            LOG(DEBUG) << "Found TLV_SEARCHED_ROLE TLV";
            tlvSearchedRole = cmdu_rx.addClass<ieee1905_1::tlvSearchedRole>();
            break;
        case int(ieee1905_1::eTlvType::TLV_AUTOCONFIG_FREQ_BAND):
            LOG(DEBUG) << "Found TLV_AUTOCONFIG_FREQ_BAND TLV";
            tlvAutoconfigFreqBand = cmdu_rx.addClass<ieee1905_1::tlvAutoconfigFreqBand>();
            break;
        case int(wfa_map::eTlvTypeMap::TLV_SEARCHED_SERVICE):
            LOG(DEBUG) << "Found TLV_SEARCHED_SERVICE TLV";
            tlvSearchedService = cmdu_rx.addClass<wfa_map::tlvSearchedService>();
            break;
        default:
            LOG(ERROR) << "Unknown TLV, type " << std::hex << type << ", dropping...";
            return false;
            break;
        }
    }

    if (tlvAlMacAddressType) {
        al_mac =
            network_utils::mac_to_string((const unsigned char *)tlvAlMacAddressType->mac().oct);
        LOG(DEBUG) << "mac=" << al_mac;
    } else {
        LOG(ERROR) << "tlvAlMacAddressType missing - ignoring autconfig search message";
        return false;
    }

    if (tlvSearchedRole) {
        LOG(DEBUG) << "searched_role=" << int(tlvSearchedRole->value());
        if (tlvSearchedRole->value() != ieee1905_1::tlvSearchedRole::REGISTRAR) {
            LOG(ERROR) << "invalid tlvSearchedRole value";
            return false;
        }
    } else {
        LOG(ERROR) << "tlvSearchedRole missing - ignoring autconfig search message";
        return false;
    }

    if (!tlvAutoconfigFreqBand) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAutoconfigFreqBand failed";
    }

    auto &auto_config_freq_band = tlvAutoconfigFreqBand->value();
    if (tlvAutoconfigFreqBand) {
        LOG(DEBUG) << "band=" << int(auto_config_freq_band);
    } else {
        LOG(ERROR) << "tlvAutoconfigFreqBand missing - ignoring autconfig search message";
        return false;
    }

    if (tlvSupportedServiceIn) {
        for (int i = 0; i < tlvSupportedServiceIn->supported_service_list_length(); i++) {
            auto supportedServiceTuple = tlvSupportedServiceIn->supported_service_list(i);
            if (!std::get<0>(supportedServiceTuple)) {
                LOG(ERROR) << "Invalid tlvSupportedService";
                return false;
            }
            auto supportedService = std::get<1>(supportedServiceTuple);
            if (supportedService !=
                wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT) {
                LOG(WARNING) << "Invalid tlvSupportedService - supported service is not "
                                "MULTI_AP_AGENT. Received value: "
                             << std::hex << int(supportedService);
                return false;
            }
        }
    } else {
        LOG(ERROR) << "tlvSupportedService missing - ignoring autconfig search message";
        return false;
    }

    if (tlvSearchedService) {
        for (int i = 0; i < tlvSearchedService->searched_service_list_length(); i++) {
            auto searchedServiceTuple = tlvSearchedService->searched_service_list(i);
            if (!std::get<0>(searchedServiceTuple)) {
                LOG(ERROR) << "Invalid tlvSearchedService";
                return false;
            }
            if (std::get<1>(searchedServiceTuple) !=
                wfa_map::tlvSearchedService::eSearchedService::MULTI_AP_CONTROLLER) {
                LOG(WARNING)
                    << "Invalid tlvSearchedService - searched service is not MULTI_AP_CONTROLLER";
                return false;
            }
        }
    } else {
        LOG(ERROR) << "tlvSearchedService missing - ignoring autconfig search message";
        return false;
    }

    auto cmdu_header = cmdu_tx.create(
        cmdu_rx.getMessageId(), ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE);

    auto tlvSupportedRole = cmdu_tx.addClass<ieee1905_1::tlvSupportedRole>();
    if (!tlvSupportedRole) {
        LOG(ERROR) << "addClass ieee1905_1::tlvSupportedRole failed";
        return false;
    }
    tlvSupportedRole->value() = ieee1905_1::tlvSupportedRole::REGISTRAR;

    auto tlvSupportedFreqBand = cmdu_tx.addClass<ieee1905_1::tlvSupportedFreqBand>();
    if (!tlvSupportedFreqBand) {
        LOG(ERROR) << "addClass ieee1905_1::tlvSupportedFreqBand failed";
        return false;
    }

    switch (auto_config_freq_band) {
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_2_4G;
        break;
    }
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_5_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_5G;
        break;
    }
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_60_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_60G;
        break;
    }
    default: {
        LOG(ERROR) << "unknown autoconfig freq band, value=" << int(auto_config_freq_band);
        return false;
    }
    }

    auto tlvSupportedServiceOut = cmdu_tx.addClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedServiceOut) {
        LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed";
        return false;
    }
    if (!tlvSupportedServiceOut->alloc_supported_service_list()) {
        LOG(ERROR) << "alloc_supported_service_list failed";
        return false;
    }
    auto supportedServiceTuple = tlvSupportedServiceOut->supported_service_list(0);
    if (!std::get<0>(supportedServiceTuple)) {
        LOG(ERROR) << "Failed accessing supported_service_list";
        return false;
    }
    std::get<1>(supportedServiceTuple) =
        wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER;

    LOG(DEBUG) << "sending autoconfig response message";
    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

/**
 * @brief Encrypt the config data using AES and add to the WSC M2 TLV
 *        The encrypted data length is the config data length padded to 16 bytes boundary.
 *
 * @param[in] m2 WSC M2 TLV
 * @param[in] config_data config data in network byte order (swapped)
 * @param[in] authkey 32 bytes calculated authentication key
 * @param[in] keywrapkey 16 bytes calculated key wrap key
 * @return true on success
 * @return false on failure
 */
bool master_thread::autoconfig_wsc_add_m2_encrypted_settings(
    std::shared_ptr<ieee1905_1::tlvWscM2> m2, WSC::cConfigData &config_data, uint8_t authkey[32],
    uint8_t keywrapkey[16])
{
    // Step 1 - prepare the plaintext: [config_data | keywrapauth]:
    // We use the config_data buffer as the plaintext buffer for encryption.
    // The config_data buffer has room for 12 bytes for the keywrapauth (2 bytes
    // attribute type, 2 bytes attribute length, 8 bytes data), but check it anyway
    // to be on the safe side. Then, we add keywrapauth at its end.
    uint8_t *plaintext = config_data.getStartBuffPtr();
    int plaintextlen   = config_data.getLen() + sizeof(WSC::sWscAttrKeyWrapAuthenticator);
    if (config_data.getBuffRemainingBytes() < sizeof(WSC::sWscAttrKeyWrapAuthenticator)) {
        LOG(ERROR) << "No room for keywrap auth in config_data";
        return false;
    }
    WSC::sWscAttrKeyWrapAuthenticator *keywrapauth =
        reinterpret_cast<WSC::sWscAttrKeyWrapAuthenticator *>(&plaintext[config_data.getLen()]);
    keywrapauth->struct_init();
    uint8_t *kwa = reinterpret_cast<uint8_t *>(keywrapauth->data);
    // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
    if (!mapf::encryption::kwa_compute(authkey, plaintext, config_data.getLen(), kwa))
        return false;
    keywrapauth->struct_swap();

    // Step 2 - AES encryption using temporary buffer. This is needed since we only
    // know the encrypted length after encryption.
    // Calculate initialization vector (IV), and encrypt the plaintext using aes128 cbc.
    // leave room for up to 16 bytes internal padding length - see aes_encrypt()
    auto encrypted_settings = m2->create_encrypted_settings();
    if (!encrypted_settings)
        return false;
    int cipherlen = plaintextlen + 16;
    uint8_t ciphertext[cipherlen];
    uint8_t *iv = reinterpret_cast<uint8_t *>(encrypted_settings->iv());
    if (!mapf::encryption::create_iv(iv, WSC::WSC_ENCRYPTED_SETTINGS_IV_LENGTH)) {
        LOG(ERROR) << "create iv failure";
        return false;
    }
    if (!mapf::encryption::aes_encrypt(keywrapkey, iv, plaintext, plaintextlen, ciphertext,
                                       cipherlen)) {
        LOG(ERROR) << "aes encrypt failure";
        return false;
    }
    LOG(DEBUG) << "ciphertext: " << std::endl << utils::dump_buffer(ciphertext, cipherlen);

    // Step 3 - Allocate encrypted settings with the size of the output ciphertext,
    // and copy the ciphertext to it.
    if (!encrypted_settings->alloc_encrypted_settings(cipherlen))
        return false;
    std::copy_n(ciphertext, cipherlen, encrypted_settings->encrypted_settings());
    if (!m2->add_encrypted_settings(encrypted_settings))
        return false;

    return true;
}

/**
 * @brief Calculate keys and update M2 attributes.
 *
 * @param[in] m1 WSC M1 TLV received from the radio agent
 * @param[in] m2 WSC M2 TLV to be sent to the radio agent
 * @param[in] dh diffie helman key exchange class containing the keypair
 * @param[out] authkey 32 bytes calculated authentication key
 * @param[out] keywrapkey 16 bytes calculated key wrap key
 * @return true on success
 * @return false on failure
 */
bool master_thread::autoconfig_wsc_calculate_keys(std::shared_ptr<ieee1905_1::tlvWscM1> m1,
                                                  std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                                  const mapf::encryption::diffie_hellman &dh,
                                                  uint8_t authkey[32], uint8_t keywrapkey[16])
{
    std::copy_n(m1->enrolee_nonce_attr().data, m1->enrolee_nonce_attr().data_length,
                m2->enrolee_nonce_attr().data);
    std::copy_n(dh.nonce(), dh.nonce_length(), m2->registrar_nonce_attr().data);
    if (!mapf::encryption::wps_calculate_keys(
            dh, m1->public_key_attr().data, m1->public_key_attr().data_length,
            m1->enrolee_nonce_attr().data, m1->mac_attr().data.oct, m2->registrar_nonce_attr().data,
            authkey, keywrapkey)) {
        LOG(ERROR) << "Failed to calculate WPS keys";
        return false;
    }
    std::copy(dh.pubkey(), dh.pubkey() + dh.pubkey_length(), m2->public_key_attr().data);

    return true;
}

/**
 * @brief autoconfig global authenticator attribute calculation
 * 
 * Calculate authentication on the Full M1 || M2* whereas M2* = M2 without the authenticator
 * attribute.
 * 
 * @param m1 WSC M1 TLV
 * @param m2 WSC M2 TLV
 * @param authkey authentication key
 * @return true on success
 * @return false on failure
 */
bool master_thread::autoconfig_wsc_authentication(std::shared_ptr<ieee1905_1::tlvWscM1> m1,
                                                  std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                                  uint8_t authkey[32])
{
    // Authentication on Full M1 || M2* (without the authenticator attribute)
    // This is the content of M1 and M2, without the type and length.
    // Authentication is done on swapped data, so first swap the m1 and m2, calculate authenticator,
    // then swap back since finalize will do the swapping.
    m1->class_swap();
    m2->class_swap();
    uint8_t buf[m1->getLen() - 3 + m2->getLen() - 3 - sizeof(WSC::sWscAttrAuthenticator)];
    auto next = std::copy_n(m1->getStartBuffPtr() + 3, m1->getLen() - 3, buf);
    std::copy_n(m2->getStartBuffPtr() + 3, m2->getLen() - 3 - sizeof(WSC::sWscAttrAuthenticator),
                next);
    LOG(DEBUG) << "m1 buf:" << std::endl
               << utils::dump_buffer(m1->getStartBuffPtr() + 3, m1->getLen() - 3);
    LOG(DEBUG) << "m2 buf:" << std::endl
               << utils::dump_buffer(m2->getStartBuffPtr() + 3, m2->getLen() - 3);
    // swap back
    m1->class_swap();
    m2->class_swap();
    uint8_t *kwa = reinterpret_cast<uint8_t *>(m2->authenticator().data);
    // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
    if (!mapf::encryption::kwa_compute(authkey, buf, sizeof(buf), kwa)) {
        LOG(ERROR) << "kwa_compute failure";
        return false;
    }

    return true;
}

/**
 * @brief add WSC M2 TLV to the current CMDU
 *
 *        the config_data contains the secret ssid, authentication and encryption types,
 *        the network key, bssid and the key_wrap_auth attribute.
 *        It does encryption using the keywrapkey and HMAC with the authkey generated
 *        in the WSC keys calculation from the M1 and M2 nonce values, the radio agent's
 *        mac, and a random initialization vector.
 *        The encrypted config_data blob is copied to the encrypted_data attribute
 *        in the M2 TLV, which marks the WSC M2 TLV ready to be sent to the agent.
 *
 * @param tlvWscM1 WSC M1 TLV received from the radio agent as part of the WSC autoconfiguration
 *        CMDU
 * @return true on success
 * @return false on failure
 */
bool master_thread::autoconfig_wsc_add_m2(std::shared_ptr<ieee1905_1::tlvWscM1> tlvWscM1,
                                          const db::bss_info_conf_t *bss_info_conf)
{
    if (!tlvWscM1) {
        LOG(ERROR) << "Invalid M1";
        return false;
    }

    auto tlvWscM2 = cmdu_tx.addClass<ieee1905_1::tlvWscM2>();
    if (!tlvWscM2) {
        LOG(ERROR) << "Failed creating tlvWscM2";
        return false;
    }

    tlvWscM2->message_type_attr().data = WSC::WSC_MSG_TYPE_M2;
    // enrolee_nonce and registrar_nonce are set in autoconfig_wsc_calculate_keys()
    // TODO the following should be taken from the database
    std::memset(tlvWscM2->uuid_r_attr().data, 0xee, tlvWscM2->uuid_r_attr().data_length);
    // public_key is set in autoconfig_wsc_calculate_keys()
    tlvWscM2->authentication_type_flags_attr().data =
        uint16_t(WSC::eWscAuth::WSC_AUTH_OPEN) | uint16_t(WSC::eWscAuth::WSC_AUTH_WPA2PSK);
    tlvWscM2->encryption_type_flags_attr().data =
        uint16_t(WSC::eWscEncr::WSC_ENCR_NONE) | uint16_t(WSC::eWscEncr::WSC_ENCR_AES);
    // connection_type and configuration_methods have default values
    // TODO the following should be taken from the database
    if (!tlvWscM2->set_manufacturer("Intel"))
        return false;
    if (!tlvWscM2->set_model_name("Ubuntu"))
        return false;
    if (!tlvWscM2->set_model_number("18.04"))
        return false;
    if (!tlvWscM2->set_serial_number("prpl12345"))
        return false;
    tlvWscM2->primary_device_type_attr().sub_category_id = WSC::WSC_DEV_NETWORK_INFRA_GATEWAY;

    // TODO Maybe the band should be taken from bss_info_conf.operating_class instead?
    tlvWscM2->rf_bands_attr().data = (tlvWscM1->rf_bands_attr().data & WSC::WSC_RF_BAND_5GHZ)
                                         ? WSC::WSC_RF_BAND_5GHZ
                                         : WSC::WSC_RF_BAND_2GHZ;
    // association_state, configuration_error, device_password_id, os_version and vendor_extension
    // have default values

    ///////////////////////////////
    // @brief encryption support //
    ///////////////////////////////
    mapf::encryption::diffie_hellman dh;
    uint8_t authkey[32];
    uint8_t keywrapkey[16];
    if (!autoconfig_wsc_calculate_keys(tlvWscM1, tlvWscM2, dh, authkey, keywrapkey))
        return false;

    // Encrypted settings
    // Encrypted settings are the ConfigData + IV. First create the ConfigData,
    // Then copy it to the encrypted data, add an IV and encrypt.
    // Finally, add HMAC

    // Create ConfigData
    uint8_t buf[1024];
    WSC::cConfigData config_data(buf, sizeof(buf), false, true);
    if (bss_info_conf) {
        config_data.set_ssid(bss_info_conf->ssid);
        config_data.authentication_type_attr().data = bss_info_conf->authentication_type;
        config_data.encryption_type_attr().data     = bss_info_conf->encryption_type;
        config_data.set_network_key(bss_info_conf->network_key);
        config_data.multiap_attr().subelement_value = bss_info_conf->bss_type;

        LOG(DEBUG) << "WSC config_data:" << std::hex << std::endl
                   << "     ssid: " << config_data.ssid_str() << std::endl
                   << "     authentication_type: "
                   << int(config_data.authentication_type_attr().data) << std::endl
                   << "     encryption_type: " << int(config_data.encryption_type_attr().data)
                   << std::dec << std::endl;
    } else {
        // Tear down. No need to set any parameter except the teardown bit and the MAC address.
        config_data.multiap_attr().subelement_value = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
        LOG(DEBUG) << "WSC config_data: tear down";
    }

    // The MAC address in the config data is tricky... According to "Wi-Fi Simple Configuration
    // Technical Specification v2.0.6", section 7.2.2 "Validation of Configuration Data" the MAC
    // address should be validated to match the Enrollee's own MAC address. "IEEE Std 1905.1-2013"
    // section 10.1.2, Table 10-1 "IEEE 802.11 settings (ConfigData) in M2 frame" says that it
    // should be "AP’s MAC address (BSSID)". The Multi-AP doesn't say anything about the MAC
    // addresses in M2, but it does say that the Enrollee MAC address in the M1 message must be the
    // AL-MAC address.
    //
    // Clearly, we can't use the real BSSID for the MAC address, since it's the responsibility of
    // the agent to use one of its assigned unique addresses as BSSID, and we don't have that
    // information in the controller. So we could use the AL-MAC addresses or the Radio UID. It
    // seems the most logical to make sure it matches the MAC address in the M1, since that stays
    // the closest to the WSC-specified behaviour.
    //
    // Note that the BBF 1905.1 implementation (meshComms) simply ignores the MAC address in M2.
    config_data.bssid_attr().data = tlvWscM1->mac_attr().data;

    config_data.class_swap();

    if (!autoconfig_wsc_add_m2_encrypted_settings(tlvWscM2, config_data, authkey, keywrapkey))
        return false;

    if (!autoconfig_wsc_authentication(tlvWscM1, tlvWscM2, authkey))
        return false;

    return true;
}

/**
 * @brief Parse AP-Autoconfiguration WSC which should include one AP Radio Basic Capabilities
 *        TLV and one WSC TLV containing M1. If this is Intel agent, it will also have vendor specific tlv.
 * 
 * @param sd socket descriptor
 * @param cmdu_rx received CMDU which contains M1
 * @return true on success
 * @return false on failure
 */
bool master_thread::handle_cmdu_1905_autoconfiguration_WSC(Socket *sd,
                                                           ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(DEBUG) << "Received AP_AUTOCONFIGURATION_WSC_MESSAGE";

    std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_basic_caps = nullptr;
    std::shared_ptr<ieee1905_1::tlvWscM1> tlvwscM1                         = nullptr;
    bool intel_agent                                                       = false;
    int type;

    while ((type = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        if (type == int(wfa_map::eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES)) {
            radio_basic_caps = cmdu_rx.addClass<wfa_map::tlvApRadioBasicCapabilities>();
            if (!radio_basic_caps)
                return false;
            LOG(DEBUG) << "Found TLV_AP_RADIO_BASIC_CAPABILITIES TLV";
        } else if (type == int(ieee1905_1::eTlvType::TLV_WSC)) {
            tlvwscM1 = cmdu_rx.addClass<ieee1905_1::tlvWscM1>();
            if (!tlvwscM1)
                return false;
            LOG(DEBUG) << "Found TLV_WSC TLV (assuming M1)";
        } else if (type == int(ieee1905_1::eTlvType::TLV_VENDOR_SPECIFIC)) {
            // If this is an Intel Agent, it will have VS TLV as the last TLV.
            // Currently, we don't support skipping TLVs, so if we see a VS TLV, we assume
            // It is an Intel agent, and will add the class in the Intel agent handling below.
            intel_agent = true;
            break;
        } else if (type == int(wfa_map::eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER)) {
            // Check if this is a M2 message that we sent to the agent, which was just looped back.
            // The M1 and M2 messages are both of CMDU type AP_Autoconfiguration_WSC. Thus,
            // when we send the M2 to the local agent, it will be published back on the local bus because
            // the destination is our AL-MAC, and the controller does listen to this CMDU.
            // Ideally, we should use the message type attribute from the WSC payload to distinguish.
            // Unfortunately, that is a bit complicated with the current tlv parser. However, there is another
            // way to distinguish them: the M1 message has the AP_Radio_Basic_Capabilities TLV,
            // while the M2 has the AP_Radio_Identifier TLV.
            // If this is a looped back M2 CMDU, we can treat is as handled successfully.
            LOG(DEBUG) << "Loopbed back M2 CMDU";
            return true;
        } else {
            LOG(ERROR) << "Unknown TLV, type " << std::hex << type;
            return false;
        }
        type = cmdu_rx.getNextTlvType();
    }

    if (!radio_basic_caps) {
        LOG(ERROR) << "Failed to get APRadioBasicCapabilities TLV";
        return false;
    }

    if (!tlvwscM1) {
        LOG(ERROR) << "Failed to get TLV_WSC M1 TLV";
        return false;
    }

    auto al_mac = network_utils::mac_to_string(tlvwscM1->mac_attr().data.oct);
    auto ruid   = network_utils::mac_to_string(radio_basic_caps->radio_uid());
    LOG(INFO) << "AP_AUTOCONFIGURATION_WSC M1 al_mac=" << al_mac << " ruid=" << ruid;
    LOG(DEBUG) << "   device " << tlvwscM1->manufacturer_str() << " " << tlvwscM1->model_name_str()
               << " " << tlvwscM1->device_name_str() << " " << tlvwscM1->serial_number_str();

    //TODO autoconfig process the rest of the class
    //TODO autoconfig Keep intel agent support only as intel enhancements
    /**
     * @brief Reply with AP-Autoconfiguration WSC with a single AP Radio Identifier TLV
     * and one (TODO do we need more?) WSC TLV containing M2.
     */
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
        LOG(ERROR) << "Create AP_AUTOCONFIGURATION_WSC_MESSAGE response";
        return false;
    }
    // All attributes which are not explicitely set below are set to
    // default by the TLV factory, see WSC_Attributes.yml
    auto tlvRuid = cmdu_tx.addClass<wfa_map::tlvApRadioIdentifier>();
    if (!tlvRuid) {
        LOG(ERROR) << "error creating tlvApRadioIdentifier TLV";
        return false;
    }

    tlvRuid->radio_uid() = network_utils::mac_from_string(ruid);

    const auto &bss_info_confs = database.get_bss_info_configuration(tlvwscM1->mac_attr().data);
    uint8_t num_bsss           = 0;

    for (const auto &bss_info_conf : bss_info_confs) {
        // Check if the radio supports it
        if (!son_actions::has_matching_operating_class(*radio_basic_caps, bss_info_conf)) {
            LOG(INFO) << "Skipping " << bss_info_conf.ssid << " due to operclass mismatch";
            continue;
        }
        if (!(tlvwscM1->authentication_type_flags_attr().data &
              uint16_t(bss_info_conf.authentication_type))) {
            LOG(INFO) << std::hex << "Auth mismatch for " << bss_info_conf.ssid << ": get 0x"
                      << tlvwscM1->authentication_type_flags_attr().data << " need 0x"
                      << uint16_t(bss_info_conf.authentication_type);
        }
        if (!(tlvwscM1->encryption_type_flags_attr().data &
              uint16_t(bss_info_conf.encryption_type))) {
            LOG(INFO) << std::hex << "Encr mismatch for " << bss_info_conf.ssid << ": get 0x"
                      << tlvwscM1->encryption_type_flags_attr().data << " need 0x"
                      << uint16_t(bss_info_conf.encryption_type);
        }
        if (num_bsss >= radio_basic_caps->maximum_number_of_bsss_supported()) {
            LOG(INFO) << "Configured #BSS exceeds maximum for " << al_mac << " radio " << ruid;
            break;
        }
        if (!autoconfig_wsc_add_m2(tlvwscM1, &bss_info_conf)) {
            LOG(ERROR) << "Failed setting M2 attributes";
            return false;
        }
        num_bsss++;
    }

    // If no BSS (either because none are configured, or because they don't match), tear down.
    if (num_bsss == 0) {
        if (!autoconfig_wsc_add_m2(tlvwscM1, nullptr)) {
            LOG(ERROR) << "Failed setting M2 attributes";
            return false;
        }
    }

    if (intel_agent) {
        LOG(INFO) << "Intel radio agent join (al_mac=" << al_mac << " ruid=" << ruid;
        if (!handle_intel_slave_join(sd, radio_basic_caps, cmdu_rx, cmdu_tx)) {
            LOG(ERROR) << "Intel radio agent join failed (al_mac=" << al_mac << " ruid=" << ruid
                       << ")";
            return false;
        }
    } else {
        LOG(INFO) << "Non-Intel radio agent join (al_mac=" << al_mac << " ruid=" << ruid << ")";
        // Multi-AP Agent doesn't say anything about the bridge, so we have to rely on Intel Slave Join for that.
        // We'll use AL-MAC as the bridge
        // TODO convert source address into AL-MAC address
        if (!handle_non_intel_slave_join(sd, radio_basic_caps, tlvwscM1, al_mac, ruid, cmdu_tx)) {
            LOG(ERROR) << "Non-Intel radio agent join failed (al_mac=" << al_mac << " ruid=" << ruid
                       << ")";
            return false;
        }
    }

    if (!database.setting_certification_mode()) {
        // trigger channel selection
        if (!cmdu_tx.create(0, ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE)) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        son_actions::send_cmdu_to_agent(sd, cmdu_tx);
    }

    return true;
}

bool master_thread::handle_cmdu_1905_channel_preference_report(Socket *sd,
                                                               ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received CHANNEL_PREFERENCE_REPORT_MESSAGE, mid=" << std::dec << int(mid);

    // TODO: in actual channel selection task, it is important to validate that rx mid is identical
    // to the mid sent in channel preference request message

    // build channel request message
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::CHANNEL_SELECTION_REQUEST_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type CHANNEL_SELECTION_REQUEST_MESSAGE, has failed";
        return false;
    }

    // Define ruid list in order to create only one reply of tlvChannelPreference per ruid
    std::list<sMacAddr> ruid_list;
    // parse all tlvs in cmdu
    int tlvType;
    while ((tlvType = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {

        if (tlvType < 0) {
            LOG(ERROR) << "getNextTlvType has failed";
            return false;
        }

        if (tlvType == int(wfa_map::eTlvTypeMap::TLV_CHANNEL_PREFERENCE)) {

            // parse channel preference report message
            auto channel_preference_tlv_rx = cmdu_rx.addClass<wfa_map::tlvChannelPreference>();
            if (!channel_preference_tlv_rx) {
                LOG(ERROR) << "addClass wfa_map::tlvChannelPreference has failed";
                return false;
            }

            // read all operating class list
            auto operating_classes_list_length =
                channel_preference_tlv_rx->operating_classes_list_length();

            for (int oc_idx = 0; oc_idx < operating_classes_list_length; oc_idx++) {
                std::stringstream ss;
                auto operating_class_tuple =
                    channel_preference_tlv_rx->operating_classes_list(oc_idx);
                if (!std::get<0>(operating_class_tuple)) {
                    LOG(ERROR) << "getting operating class entry has failed!";
                    return false;
                }
                auto &op_class_channels_rx = std::get<1>(operating_class_tuple);
                auto operating_class       = op_class_channels_rx.operating_class();
                ss << "operating class=" << int(operating_class);

                const auto &flags = op_class_channels_rx.flags();
                auto preference   = flags.preference;
                auto reason_code  = flags.reason_code;
                ss << ", preference=" << int(preference) << ", reason=" << int(reason_code);
                ss << ", channel_list={";

                auto channel_list_length = op_class_channels_rx.channel_list_length();
                for (int ch_idx = 0; ch_idx < channel_list_length; ch_idx++) {
                    auto channel_rx = op_class_channels_rx.channel_list(ch_idx);
                    if (!channel_rx) {
                        LOG(ERROR) << "getting channel entry has failed!";
                        return false;
                    }

                    ss << int(*channel_rx);

                    // add comma if not last channel in the list, else close list by add curl brackets
                    ss << (((ch_idx + 1) != channel_list_length) ? "," : "}");

                    // If reply tlvChannelPreference was not created for this ruid, then:
                    // mark first supported non-dfs channel as selected channel
                    // TODO: need to check that selected channel does not violate radio restriction
                    const auto &ruid = channel_preference_tlv_rx->radio_uid();

                    if (std::find(ruid_list.begin(), ruid_list.end(), ruid) != ruid_list.end() ||
                        preference == 0 || wireless_utils::is_dfs_channel(*channel_rx)) {
                        continue;
                    }

                    LOG(INFO) << "ruid=" << network_utils::mac_to_string(ruid);
                    LOG(INFO) << "selected_operating_class=" << std::dec << int(operating_class);
                    LOG(INFO) << "selected_channel=" << int(*channel_rx);

                    ruid_list.push_back(ruid);

                    // send one channel preference report for each ruid
                    auto channel_preference_tlv_tx =
                        cmdu_tx.addClass<wfa_map::tlvChannelPreference>();
                    if (!channel_preference_tlv_tx) {
                        LOG(ERROR) << "addClass ieee1905_1::tlvChannelPreference has failed";
                        return false;
                    }

                    channel_preference_tlv_tx->radio_uid() = ruid;

                    // Create operating class object
                    auto op_class_channels_tx =
                        channel_preference_tlv_tx->create_operating_classes_list();
                    if (!op_class_channels_tx) {
                        LOG(ERROR) << "create_operating_classes_list() has failed!";
                        return false;
                    }

                    // TODO: check that the data is parsed properly after fixing the following bug:
                    // Since sFlags is defined after dynamic list cPreferenceOperatingClasses it cause data override
                    // on the first channel on the list and sFlags itself.
                    // See: https://github.com/prplfoundation/prplMesh/issues/8

                    // Fill operating class object
                    op_class_channels_tx->operating_class() = operating_class;

                    // allocate 1 channel
                    if (!op_class_channels_tx->alloc_channel_list()) {
                        LOG(ERROR) << "alloc_channel_list() has failed!";
                        return false;
                    }
                    auto channel_idx = op_class_channels_tx->channel_list_length();
                    auto channel_tx  = op_class_channels_tx->channel_list(channel_idx - 1);
                    if (!channel_tx) {
                        LOG(ERROR) << "getting channel entry has failed!";
                        return false;
                    }
                    *channel_tx = *channel_rx;

                    op_class_channels_tx->flags() = op_class_channels_rx.flags();

                    // Push operating class object to the list of operating class objects
                    if (!channel_preference_tlv_tx->add_operating_classes_list(
                            op_class_channels_tx)) {
                        LOG(ERROR) << "add_operating_classes_list() has failed!";
                        return false;
                    }
                }
                LOG(DEBUG) << ss.str();
            }
        } else if (tlvType == int(wfa_map::eTlvTypeMap::TLV_RADIO_OPERATION_RESTRICTION)) {
            // parse radio operation restriction tlv
            auto radio_operation_restriction_tlv_rx =
                cmdu_rx.addClass<wfa_map::tlvRadioOperationRestriction>();
            if (!radio_operation_restriction_tlv_rx) {
                LOG(ERROR) << "addClass wfa_map::tlvRadioOperationRestriction has failed";
                return false;
            }

            // TODO: continute to parse this tlv
            // This TLV contains radio restriction in channel selection that must be considered
            // in channel selection request message. Since this is a dummy message, this TLV is
            // ignored. Full implemtation will be as part of channel selection task.

        } else {
            LOG(ERROR) << "Unexpected tlv type in CHANNEL_PREFERENCE_QUERY_MESSAGE, type="
                       << int(tlvType);
            // TODO: replace return statement with function that skip s unexpected tlv
            // see: https://github.com/prplfoundation/prplMesh/issues/107

            return false;
        } // close if (tlvType == some_tlv_type)

    } //close while (cmdu_rx.getNextTlvType(tlvType))

    if (database.setting_certification_mode()) {
        auto certification_tx_buffer = database.get_certification_tx_buffer();
        if (!certification_tx_buffer) {
            LOG(ERROR) << "certification_tx_buffer is not allocated!";
            return false;
        }
        database.fill_certification_tx_buffer(cmdu_tx);
        return true;
    }

    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

bool master_thread::handle_cmdu_1905_ack_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    //TODO: the ACK should be sent to the correct task and will be done as part of agent certification
    LOG(DEBUG) << "Received ACK_MESSAGE, mid=" << std::hex << int(mid);
    return true;
}

bool master_thread::handle_cmdu_1905_steering_completed_message(Socket *sd,
                                                                ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received STEERING_COMPLETED_MESSAGE, mid=" << std::hex << int(mid);
    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);

    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message back to agent, mid=" << std::hex << int(mid);
    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

bool master_thread::handle_cmdu_1905_client_steering_btm_report_message(
    Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received CLIENT_STEERING_BTM_REPORT_MESSAGE, mid=" << std::hex << int(mid);

    auto steering_btm_report = cmdu_rx.addClass<wfa_map::tlvSteeringBTMReport>();
    if (!steering_btm_report) {
        LOG(ERROR) << "addClass wfa_map::tlvSteeringBTMReportfailed";
        return false;
    }

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);

    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message back to agent";
    son_actions::send_cmdu_to_agent(sd, cmdu_tx);

    std::string client_mac = network_utils::mac_to_string(steering_btm_report->sta_mac());
    uint8_t status_code    = steering_btm_report->btm_status_code();

    LOG(DEBUG) << "BSS_TM_RESP from client_mac=" << client_mac
               << " status_code=" << (int)status_code;

    int steering_task_id = database.get_steering_task_id(client_mac);
    tasks.push_event(steering_task_id, client_steering_task::BSS_TM_RESPONSE_RECEIVED);
    database.update_node_11v_responsiveness(client_mac, true);

    if (status_code != 0) {
        LOG(DEBUG) << "sta " << client_mac << " rejected BSS steer request";
        LOG(DEBUG) << "killing roaming task";

        int prev_roaming_task = database.get_roaming_task_id(client_mac);
        tasks.kill_task(prev_roaming_task);

        tasks.push_event(steering_task_id, client_steering_task::BSS_TM_REQUEST_REJECTED);
    }

    return true;
}

bool master_thread::handle_cmdu_1905_channel_selection_response(Socket *sd,
                                                                ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received CHANNEL_SELECTION_RESPONSE_MESSAGE, mid=" << std::dec << int(mid);
    do {
        auto channel_selection_response_tlv =
            cmdu_rx.addClass<wfa_map::tlvChannelSelectionResponse>();
        if (!channel_selection_response_tlv) {
            LOG(ERROR) << "addClass ieee1905_1::tlvChannelSelectionResponse has failed";
            return false;
        }

        auto &ruid         = channel_selection_response_tlv->radio_uid();
        auto response_code = channel_selection_response_tlv->response_code();

        LOG(DEBUG)
            << "channel selection response from ruid=" << network_utils::mac_to_string(ruid)
            << ", response_code="
            << ([](const wfa_map::tlvChannelSelectionResponse::eResponseCode &response_code) {
                   std::string ret_str;
                   switch (response_code) {
                   case wfa_map::tlvChannelSelectionResponse::eResponseCode::ACCEPT:
                       ret_str.assign("ACCEPT");
                       break;
                   case wfa_map::tlvChannelSelectionResponse::eResponseCode::
                       DECLINE_VIOLATES_CURRENT_PREFERENCES:
                       ret_str.assign("DECLINE_VIOLATES_CURRENT_PREFERENCES");
                       break;
                   case wfa_map::tlvChannelSelectionResponse::eResponseCode::
                       DECLINE_VIOLATES_MOST_RECENTLY_REPORTED_PREFERENCES:
                       ret_str.assign("DECLINE_VIOLATES_MOST_RECENTLY_REPORTED_PREFERENCES");
                       break;
                   case wfa_map::tlvChannelSelectionResponse::eResponseCode::
                       DECLINE_PREVENT_OPERATION_OF_BACKHAUL_LINK:
                       ret_str.assign("DECLINE_PREVENT_OPERATION_OF_BACKHAUL_LINK");
                       break;
                   default:
                       ret_str.assign("ERROR:UNFAMILIAR_RESPONSE_CODE");
                       break;
                   }
                   return ret_str;
               })(response_code);

    } while (cmdu_rx.getNextTlvType() != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE));

    return true;
}

static void print_link_metric_map(
    std::unordered_map<sMacAddr, std::unordered_map<sMacAddr, son::node::link_metrics_data>>
        &link_metric_data)
{
    LOG(DEBUG) << "Printing Link Metrics data map";
    for (auto const &pair_agent : link_metric_data) {
        LOG(DEBUG) << "sent from al_mac= " << network_utils::mac_to_string(pair_agent.first)
                   << std::endl;

        for (auto const &pair_neighbor : link_metric_data[pair_agent.first]) {
            LOG(DEBUG) << "reporting neighbor al_mac= "
                       << network_utils::mac_to_string(pair_neighbor.first) << std::endl;

            auto &vrx = pair_neighbor.second.receiverLinkMetrics;
            for (unsigned int i = 0; i < vrx.size(); ++i) {
                LOG(DEBUG) << "rx interface metric data # " << i << "  neighbor al_mac:"
                           << network_utils::mac_to_string(vrx[i].neighbor_interface_mac)
                           << "  receiving al_mac:"
                           << network_utils::mac_to_string(vrx[i].rc_interface_mac)
                           << "  rssi= " << std::hex << int(vrx[i].link_metric_info.rssi_db)
                           << std::endl;
            }

            auto &vtx = pair_neighbor.second.transmitterLinkMetrics;
            for (unsigned int i = 0; i < vtx.size(); i++) {
                LOG(DEBUG) << "tx interface metric data # " << i << "  neighbor al_mac:"
                           << network_utils::mac_to_string(vtx[i].neighbor_interface_mac)
                           << "  receiving al_mac:"
                           << network_utils::mac_to_string(vtx[i].rc_interface_mac)
                           << "  phy_rate= " << std::hex << int(vtx[i].link_metric_info.phy_rate)
                           << std::endl;
            }
        }
    }
}

bool master_thread::handle_cmdu_1905_link_metric_response(Socket *sd,
                                                          ieee1905_1::CmduMessageRx &cmdu_rx)
{

    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received LINK_METRIC_RESPONSE_MESSAGE, mid=" << std::dec << int(mid);

    //getting reference for link metric data storage from db
    auto &link_metric_data = database.get_link_metric_data_map();

    //will hold new link metric data from Reporting Agent
    son::node::link_metrics_data new_link_metric_data;

    int tlvType;
    // holds mac address of the reporting agent, used as link_metric_data key
    sMacAddr reporting_agent_al_mac, neighbor_al_mac;

    // parse all tlvs in cmdu and save ap metric data to database
    while ((tlvType = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {

        if (tlvType < 0) {
            LOG(ERROR) << "getNextTlvType has failed";
            return false;
        }

        if (tlvType == int(ieee1905_1::eTlvType::TLV_TRANSMITTER_LINK_METRIC)) {
            // parse tx_Link_metric_data
            auto TxLinkMetricData = cmdu_rx.addClass<ieee1905_1::tlvTransmitterLinkMetric>();
            if (!TxLinkMetricData) {
                LOG(ERROR) << "addClass ieee1905_1::tx_Link_metric_data has failed";
                return false;
            }

            reporting_agent_al_mac = TxLinkMetricData->reporter_al_mac();
            neighbor_al_mac        = TxLinkMetricData->neighbor_al_mac();

            LOG(DEBUG) << "recieved  tlvTransmitterLinkMetric from al_mac ="
                       << network_utils::mac_to_string(reporting_agent_al_mac) << std::endl
                       << "reported  al_mac =" << network_utils::mac_to_string(neighbor_al_mac)
                       << std::endl;

            //fill tx data from TLV
            if (!new_link_metric_data.add_transmitter_link_metric(TxLinkMetricData)) {
                LOG(ERROR) << "adding TxLinkMetricData has failed";
                return false;
            }
        } else if (tlvType == int(ieee1905_1::eTlvType::TLV_RECEIVER_LINK_METRIC)) {
            // parse tlvReceiverLinkMetric
            auto RxLinkMetricData = cmdu_rx.addClass<ieee1905_1::tlvReceiverLinkMetric>();
            if (!RxLinkMetricData) {
                LOG(ERROR) << "addClass ieee1905_1::tlvReceiverLinkMetric has failed";
                return false;
            }

            if (reporting_agent_al_mac != RxLinkMetricData->reporter_al_mac()) {
                LOG(ERROR) << "TLV_RECEIVER_LINK_METRIC reporter al_mac ="
                           << network_utils::mac_to_string(reporting_agent_al_mac) << std::endl
                           << " and TLV_TRANSMITTER_LINK_METRIC reporter al_mac ="
                           << network_utils::mac_to_string(RxLinkMetricData->reporter_al_mac())
                           << std::endl
                           << " not the same";
                return false;
            }

            if (neighbor_al_mac != RxLinkMetricData->neighbor_al_mac()) {
                LOG(ERROR) << "TLV_RECEIVER_LINK_METRIC reported al_mac ="
                           << network_utils::mac_to_string(neighbor_al_mac) << std::endl
                           << " and TLV_TRANSMITTER_LINK_METRIC reported al_mac ="
                           << network_utils::mac_to_string(RxLinkMetricData->neighbor_al_mac())
                           << std::endl
                           << " not the same";
                return false;
            }

            LOG(DEBUG) << "recieved  tlvReceiverLinkMetric from al_mac="
                       << network_utils::mac_to_string(reporting_agent_al_mac) << std::endl
                       << "reported  al_mac =" << network_utils::mac_to_string(neighbor_al_mac)
                       << std::endl;

            //fill rx data from TLV
            if (!new_link_metric_data.add_receiver_link_metric(RxLinkMetricData)) {
                LOG(ERROR) << "adding RxLinkMetricData has failed";
                return false;
            }
        } else {
            LOG(ERROR) << " unexpected tlv is recieved ";
            return false;
        }
    }

    //save agent link metric data to database or modify already existing.
    std::string reporter_status_s = "new";
    std::string neighbor_status_s = "new";
    auto reporting_map_it         = link_metric_data.find(reporting_agent_al_mac);

    if (reporting_map_it != link_metric_data.end()) {
        auto neighbor_map_it = reporting_map_it->second.find(neighbor_al_mac);
        if (neighbor_map_it != reporting_map_it->second.end()) {
            reporter_status_s = "existing";
        } else {
            neighbor_status_s = "existing";
        }
    }

    link_metric_data[reporting_agent_al_mac][reporting_agent_al_mac] = new_link_metric_data;

    LOG(DEBUG) << " Added metric data from " << reporter_status_s
               << " al_mac = " << network_utils::mac_to_string(reporting_agent_al_mac) << std::endl
               << "reported data for " << neighbor_status_s
               << " neighbor al_mac =" << network_utils::mac_to_string(neighbor_al_mac)
               << std::endl;

    print_link_metric_map(link_metric_data);

    return true;
}

bool master_thread::handle_cmdu_1905_operating_channel_report(Socket *sd,
                                                              ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received OPERATING_CHANNEL_REPORT_MESSAGE, mid=" << std::dec << int(mid);

    do {
        auto operating_channel_report_tlv = cmdu_rx.addClass<wfa_map::tlvOperatingChannelReport>();
        if (!operating_channel_report_tlv) {
            LOG(ERROR) << "addClass ieee1905_1::operating_channel_report_tlv has failed";
            return false;
        }

        auto &ruid    = operating_channel_report_tlv->radio_uid();
        auto tx_power = operating_channel_report_tlv->current_transmit_power();

        LOG(INFO) << "operating channel report, ruid=" << network_utils::mac_to_string(ruid)
                  << ", tx_power=" << std::dec << int(tx_power);

        auto operating_classes_list_length =
            operating_channel_report_tlv->operating_classes_list_length();

        for (uint8_t oc = 0; oc < operating_classes_list_length; oc++) {
            auto operating_class_tuple = operating_channel_report_tlv->operating_classes_list(oc);
            if (!std::get<0>(operating_class_tuple)) {
                LOG(ERROR) << "getting operating class entry has failed!";
                return false;
            }

            auto &operating_class_struct = std::get<1>(operating_class_tuple);
            auto operating_class         = operating_class_struct.operating_class;
            auto channel                 = operating_class_struct.channel_number;
            LOG(INFO) << "operating_class=" << int(operating_class)
                      << ", operating_channel=" << int(channel);
        }

    } while (cmdu_rx.getNextTlvType() != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE));

    // send ACK_MESSAGE back to the Agent
    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

bool master_thread::handle_cmdu_1905_topology_discovery(Socket *sd,
                                                        ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received TOPOLOGY_DISCOVERY_MESSAGE, mid=" << std::dec << int(mid);
    auto tlvAlMac = cmdu_rx.addClass<ieee1905_1::tlvAlMacAddressType>();
    if (!tlvAlMac) {
        LOG(ERROR) << "addClass tlvAlMacAddressType failed";
        return false;
    }
    auto al_mac = network_utils::mac_to_string(tlvAlMac->mac());
    if (database.has_node(tlvAlMac->mac())) {
        LOG(DEBUG) << "Skip setting AL MAC " << al_mac << " node";
        return true;
    }
    LOG(DEBUG) << "adding node " << al_mac << " under " << network_utils::ZERO_MAC_STRING
               << ", and mark as type " << beerocks::TYPE_IRE;
    database.add_node(network_utils::mac_from_string(al_mac), network_utils::ZERO_MAC,
                      beerocks::TYPE_IRE);
    database.set_node_state(al_mac, beerocks::STATE_CONNECTED);
    database.set_node_socket(al_mac, sd);

    return true;
}

bool master_thread::handle_cmdu_1905_higher_layer_data_message(Socket *sd,
                                                               ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received HIGHER_LAYER_DATA_MESSAGE , mid=" << std::hex << int(mid);

    auto tlvHigherLayerData = cmdu_rx.addClass<wfa_map::tlvHigherLayerData>();
    if (!tlvHigherLayerData) {
        LOG(ERROR) << "addClass wfa_map::tlvHigherLayerData failed";
        return false;
    }

    const auto protocol       = tlvHigherLayerData->protocol();
    const auto payload_length = tlvHigherLayerData->payload_length();
    LOG(DEBUG) << "protocol: " << std::hex << protocol;
    LOG(DEBUG) << "payload length: " << int(payload_length);

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message to the agent, mid=" << std::hex << int(mid);
    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

bool master_thread::handle_cmdu_1905_topology_notification(Socket *sd,
                                                           ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received TOPOLOGY_NOTIFICATION_MESSAGE, mid=" << std::hex << int(mid);

    std::shared_ptr<wfa_map::tlvClientAssociationEvent> client_association_event_tlv = nullptr;
    std::shared_ptr<beerocks_message::tlvVsClientAssociationEvent> vs_tlv            = nullptr;

    int tlvType;
    while ((tlvType = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        switch (tlvType) {
        case int(wfa_map::eTlvTypeMap::TLV_CLIENT_ASSOCIATION_EVENT):
            LOG(DEBUG) << "Found TLV_CLIENT_ASSOCIATION_EVENT TLV";
            client_association_event_tlv = cmdu_rx.addClass<wfa_map::tlvClientAssociationEvent>();
            if (!client_association_event_tlv) {
                LOG(ERROR) << "addClass wfa_map::tlvClientAssociationEvent failed";
                return false;
            }
            break;
        case int(ieee1905_1::eTlvType::TLV_VENDOR_SPECIFIC):
            LOG(DEBUG) << "Found TLV_VENDOR_SPECIFIC TLV";

            if (!message_com::parse_intel_vs_message(cmdu_rx)) {
                LOG(DEBUG) << "Failed to parse intel vs message (not Intel?)";
                continue;
            }

            vs_tlv = cmdu_rx.addClass<beerocks_message::tlvVsClientAssociationEvent>();
            if (!vs_tlv) {
                LOG(ERROR) << "addClass wfa_map::tlvVsClientAssociationEvent failed";
                return false;
            }
            break;
        default:
            LOG(ERROR) << "Unexpected tlv type in TOPOLOGY_NOTIFICATION_MESSAGE, type="
                       << int(tlvType);
            // TODO: replace return statement with function that skip s unexpected tlv
            // see: https://github.com/prplfoundation/prplMesh/issues/107
            return false;
        }
    }

    if (!client_association_event_tlv) {
        // The standard allows having zero TLV_CLIENT_ASSOCIATION_EVENT, in that case, there is no
        // point to continue the handling of this message.
        LOG(INFO) << "wfa_map::tlvClientAssociationEvent not found";
        return true;
    }

    auto &client_mac    = client_association_event_tlv->client_mac();
    auto client_mac_str = network_utils::mac_to_string(client_mac);

    auto &bssid    = client_association_event_tlv->bssid();
    auto bssid_str = network_utils::mac_to_string(bssid);

    auto association_event = client_association_event_tlv->association_event();
    bool client_connected =
        (association_event == wfa_map::tlvClientAssociationEvent::CLIENT_HAS_JOINED_THE_BSS);

    LOG(INFO) << "client " << (client_connected ? "connected" : "disconnected")
              << ", client_mac=" << client_mac_str << ", bssid=" << bssid_str;

    if (client_connected) {
        //add or update node parent
        database.add_node(client_mac, bssid);

        LOG(INFO) << "client connected, mac=" << client_mac_str << ", bssid=" << bssid_str;

        database.set_node_channel_bw(client_mac_str, database.get_node_channel(bssid_str),
                                     database.get_node_bw(bssid_str),
                                     database.get_node_channel_ext_above_secondary(bssid_str), 0,
                                     database.get_hostap_vht_center_frequency(bssid_str));

        database.clear_node_cross_rssi(client_mac_str);
        database.clear_node_stats_info(client_mac_str);

        if (!(database.get_node_type(client_mac_str) == beerocks::TYPE_IRE_BACKHAUL &&
              database.get_node_handoff_flag(client_mac_str))) {
            // The node is not an IRE in handoff
            database.set_node_type(client_mac_str, beerocks::TYPE_CLIENT);
        }

        database.set_node_backhaul_iface_type(client_mac_str,
                                              beerocks::IFACE_TYPE_WIFI_UNSPECIFIED);

        if (vs_tlv) {
            database.set_node_vap_id(client_mac_str, vs_tlv->vap_id());
            database.set_station_capabilities(client_mac_str, vs_tlv->capabilities());
        }

        // Notify existing steering task of completed connection
        int prev_steering_task = database.get_steering_task_id(client_mac_str);
        tasks.push_event(prev_steering_task, client_steering_task::STA_CONNECTED);

#ifdef BEEROCKS_RDKB
        //push event to rdkb_wlan_hal task
        if (vs_tlv && database.settings_rdkb_extensions()) {
            beerocks_message::sClientAssociationParams new_event = {};

            new_event.mac          = client_mac;
            new_event.bssid        = bssid;
            new_event.vap_id       = vs_tlv->vap_id();
            new_event.capabilities = vs_tlv->capabilities();

            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_CLIENT_CONNECT_AVAILABLE,
                             &new_event);
        }
#endif
        if (database.get_node_ipv4(client_mac_str).empty()) {
            database.set_node_state(client_mac_str, beerocks::STATE_CONNECTED_IP_UNKNOWN);
            LOG(INFO) << "STATE_CONNECTED_IP_UNKNOWN for node mac " << client_mac_str;
            return true;
        }

        son_actions::handle_completed_connection(database, cmdu_tx, tasks, client_mac_str);

    } else {
        // client disconnected

#ifdef BEEROCKS_RDKB
        //push event to rdkb_wlan_hal task
        if (vs_tlv && database.settings_rdkb_extensions()) {
            beerocks_message::sSteeringEvDisconnect new_event = {};
            new_event.client_mac                              = client_mac_str;
            new_event.bssid                                   = bssid_str;
            new_event.reason                                  = vs_tlv->params().reason;
            new_event.source = beerocks_message::eDisconnectSource(vs_tlv->params().source);
            new_event.type   = beerocks_message::eDisconnectType(vs_tlv->params().type);

            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_CLIENT_DISCONNECT_AVAILABLE,
                             &new_event);
        }
#endif

        son_actions::handle_dead_node(client_mac_str, bssid_str, database, cmdu_tx, tasks);
    }

    return true;
}

bool master_thread::handle_intel_slave_join(
    Socket *sd, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
    ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx)
{
    // Prepare outcoming response vs tlv
    auto join_response =
        message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_RESPONSE>(cmdu_tx);
    if (!join_response) {
        LOG(ERROR) << "Failed adding intel vendor specific TLV";
        return false;
    }

    // Parse incomeing vs_tlv
    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(ERROR) << "Failed to parse intel vs message (not Intel?)";
        return false;
    }

    if (beerocks_header->action_op() !=
        beerocks_message::ACTION_CONTROL_SLAVE_JOINED_NOTIFICATION) {
        LOG(ERROR) << "Unexpected Intel action op " << beerocks_header->action_op();
        return false;
    }

    auto notification =
        cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION>();
    if (!notification) {
        LOG(ERROR) << "addClass cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION failed";
        return false;
    }

    std::string slave_version = std::string(notification->slave_version(message::VERSION_LENGTH));
    std::string radio_mac     = network_utils::mac_to_string(notification->hostap().iface_mac);
    std::string gw_ipv4 = network_utils::ipv4_to_string(notification->backhaul_params().gw_ipv4);
    std::string gw_bridge_mac =
        network_utils::mac_to_string(notification->backhaul_params().gw_bridge_mac);
    std::string parent_bssid_mac =
        network_utils::mac_to_string(notification->backhaul_params().backhaul_bssid);
    std::string backhaul_mac =
        network_utils::mac_to_string(notification->backhaul_params().backhaul_mac);
    std::string backhaul_ipv4 =
        network_utils::ipv4_to_string(notification->backhaul_params().backhaul_ipv4);
    beerocks::eIfaceType backhaul_iface_type =
        (beerocks::eIfaceType)notification->backhaul_params().backhaul_iface_type;
    bool is_gw_slave         = (backhaul_iface_type == beerocks::IFACE_TYPE_GW_BRIDGE);
    beerocks::eType ire_type = is_gw_slave ? beerocks::TYPE_GW : beerocks::TYPE_IRE;
    int backhaul_channel     = notification->backhaul_params().backhaul_channel;
    std::string bridge_mac =
        network_utils::mac_to_string(notification->backhaul_params().bridge_mac);
    std::string bridge_ipv4 =
        network_utils::ipv4_to_string(notification->backhaul_params().bridge_ipv4);
    bool backhaul_manager            = (bool)notification->backhaul_params().is_backhaul_manager;
    beerocks::ePlatform ire_platform = (beerocks::ePlatform)notification->platform();
    std::string radio_identifier = network_utils::mac_to_string(notification->radio_identifier());
    bool acs_enabled             = (notification->wlan_settings().channel == 0);

    std::string gw_name;
    if (is_gw_slave) {
        gw_name =
            "GW" +
            std::string(notification->platform_settings().local_master ? "_MASTER" : "_SLAVE_ONLY");
    }
    std::string slave_name =
        is_gw_slave
            ? gw_name
            : ("IRE_" +
               (notification->platform_settings().local_master ? "MASTER_" : std::string()) +
               bridge_mac.substr(bridge_mac.size() - 5, bridge_mac.size() - 1));

    LOG(INFO) << "IRE Slave joined, sd=" << intptr_t(sd) << std::endl
              << "    slave_version=" << slave_version << std::endl
              << "    gw_ipv4=" << gw_ipv4 << std::endl
              << "    gw_bridge_mac=" << gw_bridge_mac << std::endl
              << "    slave_name=" << slave_name << std::endl
              << "    parent_bssid_mac=" << parent_bssid_mac << std::endl
              << "    backhaul_mac=" << backhaul_mac << std::endl
              << "    backhaul_ipv4=" << backhaul_ipv4 << std::endl
              << "    bridge_mac=" << bridge_mac << std::endl
              << "    bridge_ipv4=" << bridge_ipv4 << std::endl
              << "    backhaul_manager=" << int(backhaul_manager) << std::endl
              << "    backhaul_type=" << utils::get_iface_type_string(backhaul_iface_type)
              << std::endl
              << "    platform=" << utils::get_platform_string(ire_platform) << std::endl
              << "    low_pass_filter_on = " << int(notification->low_pass_filter_on()) << std::endl
              << "    radio_identifier = " << radio_identifier << std::endl
              << "    radio_mac = " << radio_mac << std::endl
              << "    channel = " << int(notification->wlan_settings().channel) << std::endl
              << "    is_gw_slave = " << int(is_gw_slave) << std::endl;

    if (!is_gw_slave) {

        // rejecting join if gw haven't joined yet
        if ((parent_bssid_mac != network_utils::ZERO_MAC_STRING) &&
            (!database.has_node(network_utils::mac_from_string(parent_bssid_mac)) ||
             (database.get_node_state(parent_bssid_mac) != beerocks::STATE_CONNECTED))) {
            LOG(DEBUG) << "sending back join reject!";
            LOG(DEBUG) << "reject_debug: parent_bssid_has_node="
                       << (int)(database.has_node(
                              network_utils::mac_from_string(parent_bssid_mac)));

            join_response->err_code() = beerocks::JOIN_RESP_REJECT;
            return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
        }

        // sending to BML listeners, client disconnect notification on ire backhaul before changing it type from TYPE_CLIENT to TYPE_IRE_BACKHAUL
        if (database.get_node_type(backhaul_mac) == beerocks::TYPE_CLIENT &&
            database.get_node_state(backhaul_mac) == beerocks::STATE_CONNECTED) {
            LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << backhaul_mac
                       << ", FORCING DISCONNECT NOTIFICATION!";
            bml_task::connection_change_event new_event;
            new_event.mac                     = backhaul_mac;
            new_event.force_client_disconnect = true;
            tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        }

        //TODO might need to handle bssids of VAP nodes as well in this case
        if (parent_bssid_mac != network_utils::ZERO_MAC_STRING) {
            //add a placeholder
            LOG(DEBUG) << "add a placeholder backhaul_mac = " << backhaul_mac
                       << ", parent_bssid_mac = " << parent_bssid_mac;
            database.add_node(network_utils::mac_from_string(backhaul_mac),
                              network_utils::mac_from_string(parent_bssid_mac),
                              beerocks::TYPE_IRE_BACKHAUL);
        } else if (database.get_node_state(backhaul_mac) != beerocks::STATE_CONNECTED) {
            /* if the backhaul node doesn't exist, or is not already marked as connected,
            * we assume it is connected to the GW's LAN switch
            */
            LOG(DEBUG) << "connected to the GW's LAN switch ";
            auto gw_container = database.get_nodes_from_hierarchy(0, beerocks::TYPE_GW);
            if (gw_container.empty()) {
                LOG(ERROR) << "can't get GW node!";
                return false;
            }

            auto gw_mac          = *gw_container.begin();
            auto gw_lan_switches = database.get_node_children(gw_mac, beerocks::TYPE_ETH_SWITCH);

            if (gw_lan_switches.empty()) {
                LOG(ERROR) << "GW has no LAN SWITCH node!";
                return false;
            }

            auto gw_lan_switch = *gw_lan_switches.begin();

            LOG(DEBUG) << "add a placeholder backhaul_mac = " << backhaul_mac
                       << " gw_lan_switch = " << gw_lan_switch
                       << " TYPE_IRE_BACKHAUL , STATE_CONNECTED";
            database.add_node(network_utils::mac_from_string(backhaul_mac),
                              network_utils::mac_from_string(gw_lan_switch),
                              beerocks::TYPE_IRE_BACKHAUL);
            database.set_node_state(backhaul_mac, beerocks::STATE_CONNECTED);
        }
    } else {
        backhaul_mac.clear();
    }

    //if the IRE connects via a different backhaul, mark previous backhaul as disconnected
    std::string previous_backhaul = database.get_node_parent(bridge_mac);
    if (!previous_backhaul.empty() && previous_backhaul != backhaul_mac &&
        database.get_node_type(previous_backhaul) == beerocks::TYPE_IRE_BACKHAUL) {
        LOG(DEBUG) << "marking previous backhaul " << previous_backhaul << " for IRE " << bridge_mac
                   << " as disconnected";
        database.set_node_state(previous_backhaul, beerocks::STATE_DISCONNECTED);
    }

    // bridge_mac node may have been created from DHCP/ARP event, if so delete it
    // this may only occur once
    if (database.has_node(network_utils::mac_from_string(bridge_mac)) &&
        (database.get_node_type(bridge_mac) != ire_type)) {
        database.remove_node(network_utils::mac_from_string(bridge_mac));
    }
    // add new GW/IRE bridge_mac
    LOG(DEBUG) << "adding node " << bridge_mac << " under " << backhaul_mac << ", and mark as type "
               << ire_type;
    database.add_node(network_utils::mac_from_string(bridge_mac),
                      network_utils::mac_from_string(backhaul_mac), ire_type);
    database.set_node_state(bridge_mac, beerocks::STATE_CONNECTED);
    database.set_node_socket(bridge_mac, sd);

    /*
    * Set IRE backhaul manager slave
    * keep in mind that the socket's peer mac will be the hostap mac
    */
    if (backhaul_manager) {
        /*
        * handle the IRE node itself, representing the backhaul
        */
        database.set_node_platform(backhaul_mac, ire_platform);
        database.set_node_platform(bridge_mac, ire_platform);

        database.set_node_backhaul_iface_type(backhaul_mac, backhaul_iface_type);
        database.set_node_backhaul_iface_type(bridge_mac, beerocks::IFACE_TYPE_BRIDGE);

        database.set_node_ipv4(backhaul_mac, bridge_ipv4);
        database.set_node_ipv4(bridge_mac, bridge_ipv4);
        database.set_node_manufacturer(backhaul_mac, "Intel");
        database.set_node_manufacturer(bridge_mac, "Intel");

        database.set_node_type(backhaul_mac, beerocks::TYPE_IRE_BACKHAUL);

        database.set_node_name(backhaul_mac, slave_name + "_BH");
        database.set_node_name(bridge_mac, slave_name);

        //TODO slave should include eth switch mac in the message
        auto eth_sw_mac_binary = notification->backhaul_params().bridge_mac;
        ++eth_sw_mac_binary.oct[5];

        std::string eth_switch_mac = network_utils::mac_to_string(eth_sw_mac_binary);
        database.add_node(network_utils::mac_from_string(eth_switch_mac),
                          network_utils::mac_from_string(bridge_mac), beerocks::TYPE_ETH_SWITCH);
        database.set_node_state(eth_switch_mac, beerocks::STATE_CONNECTED);
        database.set_node_name(eth_switch_mac, slave_name + "_ETH");
        database.set_node_ipv4(eth_switch_mac, bridge_ipv4);
        database.set_node_manufacturer(eth_switch_mac, "Intel");

        //run locating task on ire
        if (!database.is_node_wireless(backhaul_mac)) {
            LOG(DEBUG) << "run_client_locating_task client_mac = " << bridge_mac;
            auto new_task = std::make_shared<client_locating_task>(database, cmdu_tx, tasks,
                                                                   bridge_mac, true, 2000);
            tasks.add_task(new_task);
        }

        //Run the client locating tasks for the previously located wired IRE. If cascaded IREs are connected with wire
        //the slave_join notification for the 2nd level IRE can come before 1st level IRE, causing the 2nd
        //level IRE to be placed at the same level as the 1st IRE in the DB
        auto ires = database.get_all_connected_ires();
        for (auto ire : ires) {
            if (ire == bridge_mac || database.get_node_type(ire) == beerocks::TYPE_GW) {
                LOG(INFO) << "client_locating_task is not run again for this ire: " << ire;
                continue;
            }
            auto ire_backhaul_mac = database.get_node_parent_backhaul(ire);
            if (!database.is_node_wireless(ire_backhaul_mac) &&
                nullptr != database.get_node_socket(ire)) {
                LOG(DEBUG) << "run_client_locating_task client_mac = " << ire;
                auto new_task = std::make_shared<client_locating_task>(database, cmdu_tx, tasks,
                                                                       ire, true, 2000);
                tasks.add_task(new_task);
            }
        }
    }

    // Check Slave BeeRocks version //
    auto slave_version_s  = version::version_from_string(slave_version);
    auto master_version_s = version::version_from_string(BEEROCKS_VERSION);

    string_utils::copy_string(join_response->master_version(), BEEROCKS_VERSION,
                              message::VERSION_LENGTH);

    // check if fatal mismatch
    if (slave_version_s.major != master_version_s.major ||
        slave_version_s.minor != master_version_s.minor) {
        LOG(INFO) << "IRE Slave joined, Mismatch version! slave_version="
                  << std::string(slave_version)
                  << " master_version=" << std::string(BEEROCKS_VERSION);
        LOG(INFO) << " bridge_mac=" << bridge_mac << " bridge_ipv4=" << bridge_ipv4;

        join_response->err_code() = beerocks::JOIN_RESP_VERSION_MISMATCH;
        string_utils::copy_string(join_response->master_version(message::VERSION_LENGTH),
                                  BEEROCKS_VERSION, message::VERSION_LENGTH);
        return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
    }

    beerocks::eIfaceType hostap_iface_type =
        (beerocks::eIfaceType)notification->hostap().iface_type;

    bool advertise_ssid = (bool)notification->wlan_settings().advertise_ssid;
    LOG(DEBUG) << "joined slave advertise_ssid=" << (advertise_ssid ? "true" : "false")
               << " for vap " << radio_mac;
    /*
        * TODO temporarily disabled for 1.1
    auto local_slave_advertise_ssid = database.get_hostap_advertise_ssid_flag(local_slave_mac);
    if (!is_gw_slave && local_slave_advertise_ssid != advertise_ssid) {
        LOG(INFO) << "advertise SSID flag mismatch! local_slave_advertise_ssid=" << (local_slave_advertise_ssid?"true":"false");
        message::sACTION_CONTROL_SLAVE_JOINED_RESPONSE joined_response = {};
        joined_response.err_code = beerocks::JOIN_RESP_ADVERTISE_SSID_FLAG_MISMATCH;
        std::ptrdiff_t size = message_com::build_message(tx_buffer, message::ACTION_CONTROL_SLAVE_JOINED_RESPONSE, joined_response);
        message_com::send_message(sd, tx_buffer, size);
        break;
    }
    */

    LOG(INFO) << std::endl
              << "    hostap_iface_name=" << notification->hostap().iface_name << std::endl
              << "    hostap_iface_type=" << utils::get_iface_type_string(hostap_iface_type)
              << std::endl
              << "    ant_num=" << int(notification->hostap().ant_num)
              << " ant_gain=" << int(notification->hostap().ant_gain)
              << " channel=" << int(notification->cs_params().channel)
              << " conducted=" << int(notification->hostap().conducted_power) << std::endl
              << "    radio_mac=" << radio_mac << std::endl;

    bool local_master = (bool)notification->platform_settings().local_master;
    if (local_master) {
        database.set_local_slave_mac(radio_mac);
        LOG(DEBUG) << "local_slave_mac = " << radio_mac;
#ifdef BEEROCKS_RDKB
        database.settings_rdkb_extensions(
            notification->platform_settings().rdkb_extensions_enabled);
        if (database.settings_rdkb_extensions()) {
            int prev_task_id = database.get_rdkb_wlan_task_id();
            if (!tasks.is_task_running(prev_task_id)) {
                LOG(DEBUG) << "starting RDKB task";
                auto new_rdkb_wlan_task =
                    std::make_shared<rdkb_wlan_task>(database, cmdu_tx, tasks);
                tasks.add_task(new_rdkb_wlan_task);
            }
        }
#endif
        database.settings_client_band_steering(
            notification->platform_settings().client_band_steering_enabled);
        database.settings_client_optimal_path_roaming(
            notification->platform_settings().client_optimal_path_roaming_enabled);
        database.settings_client_optimal_path_roaming_prefer_signal_strength(
            notification->platform_settings()
                .client_optimal_path_roaming_prefer_signal_strength_enabled);
        database.settings_client_11k_roaming(
            notification->platform_settings().client_11k_roaming_enabled);
        database.settings_load_balancing(notification->platform_settings().load_balancing_enabled);
        database.settings_service_fairness(
            notification->platform_settings().service_fairness_enabled);
        database.settings_dfs_reentry(notification->platform_settings().dfs_reentry_enabled);
    }

    /*
    * handle the HOSTAP node
    */
    if (database.has_node(network_utils::mac_from_string(radio_mac))) {
        if (database.get_node_type(radio_mac) != beerocks::TYPE_SLAVE) {
            database.set_node_type(radio_mac, beerocks::TYPE_SLAVE);
            LOG(ERROR) << "Existing mac node is not TYPE_SLAVE";
        }
        database.clear_hostap_stats_info(radio_mac);
    } else {
        database.add_node(network_utils::mac_from_string(radio_mac),
                          network_utils::mac_from_string(bridge_mac), beerocks::TYPE_SLAVE,
                          network_utils::mac_from_string(radio_identifier));
    }
    database.set_hostap_is_acs_enabled(radio_mac, acs_enabled);
    database.set_hostap_passive_mode_enabled(
        radio_mac, notification->platform_settings().passive_mode_enabled);

    if (!notification->is_slave_reconf()) {
        son_actions::set_hostap_active(database, tasks, radio_mac,
                                       false); // make sure AP is marked as not active
    }

    if (backhaul_manager) {
        // clear backhaul manager flag for all slaves except for this backhaul_manager slave
        auto ire_hostaps = database.get_node_children(bridge_mac, beerocks::TYPE_SLAVE);
        for (auto tmp_slave_mac : ire_hostaps) {
            if (tmp_slave_mac != radio_mac) {
                database.set_hostap_backhaul_manager(tmp_slave_mac, false);
            }
        }
    }
    database.set_hostap_repeater_mode_flag(radio_mac, notification->enable_repeater_mode());
    database.set_hostap_backhaul_manager(radio_mac, backhaul_manager);

    database.set_node_state(radio_mac, beerocks::STATE_CONNECTED);
    database.set_node_backhaul_iface_type(radio_mac, is_gw_slave ? beerocks::IFACE_TYPE_GW_BRIDGE
                                                                 : beerocks::IFACE_TYPE_BRIDGE);
    database.set_hostap_iface_name(radio_mac, notification->hostap().iface_name);
    database.set_hostap_iface_type(radio_mac, hostap_iface_type);
    database.set_hostap_driver_version(radio_mac, notification->hostap().driver_version);

    database.set_hostap_ant_num(radio_mac, (beerocks::eWiFiAntNum)notification->hostap().ant_num);
    database.set_hostap_ant_gain(radio_mac, notification->hostap().ant_gain);
    database.set_hostap_conducted_power(radio_mac, notification->hostap().conducted_power);

    database.set_node_name(radio_mac, slave_name + "_AP");
    database.set_node_ipv4(radio_mac, bridge_ipv4);
    database.set_node_manufacturer(radio_mac, "Intel");

    // sd is assigned to src bridge mac
    sd->setPeerMac(bridge_mac);

    database.set_hostap_supported_channels(radio_mac, notification->hostap().supported_channels,
                                           message::SUPPORTED_CHANNELS_LENGTH);

    database.set_hostap_advertise_ssid_flag(radio_mac, advertise_ssid);

    if (database.get_node_5ghz_support(radio_mac)) {
        if (notification->low_pass_filter_on()) {
            database.set_hostap_band_capability(radio_mac, beerocks::LOW_SUBBAND_ONLY);
        } else {
            database.set_hostap_band_capability(radio_mac, beerocks::BOTH_SUBBAND);
        }
    } else {
        database.set_hostap_band_capability(radio_mac, beerocks::SUBBAND_CAPABILITY_UNKNOWN);
    }
    autoconfig_wsc_parse_radio_caps(radio_mac, radio_caps);

    // send JOINED_RESPONSE with son config
    {

        string_utils::copy_string(join_response->master_version(message::VERSION_LENGTH),
                                  BEEROCKS_VERSION, message::VERSION_LENGTH);
        join_response->config().monitor_total_ch_load_notification_hi_th_percent =
            database.config.monitor_total_ch_load_notification_hi_th_percent;
        join_response->config().monitor_total_ch_load_notification_lo_th_percent =
            database.config.monitor_total_ch_load_notification_lo_th_percent;
        join_response->config().monitor_total_ch_load_notification_delta_th_percent =
            database.config.monitor_total_ch_load_notification_delta_th_percent;
        join_response->config().monitor_min_active_clients =
            database.config.monitor_min_active_clients;
        join_response->config().monitor_active_client_th = database.config.monitor_active_client_th;
        join_response->config().monitor_client_load_notification_delta_th_percent =
            database.config.monitor_client_load_notification_delta_th_percent;
        join_response->config().monitor_rx_rssi_notification_threshold_dbm =
            database.config.monitor_rx_rssi_notification_threshold_dbm;
        join_response->config().monitor_rx_rssi_notification_delta_db =
            database.config.monitor_rx_rssi_notification_delta_db;
        join_response->config().monitor_ap_idle_threshold_B =
            database.config.monitor_ap_idle_threshold_B;
        join_response->config().monitor_ap_active_threshold_B =
            database.config.monitor_ap_active_threshold_B;
        join_response->config().monitor_ap_idle_stable_time_sec =
            database.config.monitor_ap_idle_stable_time_sec;
        join_response->config().monitor_disable_initiative_arp =
            database.config.monitor_disable_initiative_arp;
        join_response->config().slave_keep_alive_retries = database.config.slave_keep_alive_retries;
        join_response->config().ire_rssi_report_rate_sec = database.config.ire_rssi_report_rate_sec;

        LOG(DEBUG) << "send SLAVE_JOINED_RESPONSE";
        son_actions::send_cmdu_to_agent(sd, cmdu_tx);
    }

    // calling this function to update arp monitor with new ip addr (bridge ip), which is diffrent from the ip received from, dhcp on the backhaul
    if (backhaul_manager && (!is_gw_slave) && database.is_node_wireless(backhaul_mac)) {
        son_actions::handle_completed_connection(database, cmdu_tx, tasks, backhaul_mac);
    }

    // update bml listeners
    bml_task::connection_change_event bml_new_event;
    bml_new_event.mac = bridge_mac;
    tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &bml_new_event);
    LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << bml_new_event.mac;

    if (!notification->is_slave_reconf()) {
        //sending event to CS task
        LOG(DEBUG) << "CS_task,sending SLAVE_JOINED_EVENT for mac " << radio_mac;
        auto cs_new_event =
            CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sSlaveJoined_event);
        cs_new_event->backhaul_is_wireless = utils::is_node_wireless(backhaul_iface_type);
        cs_new_event->backhaul_channel     = backhaul_channel;
        cs_new_event->channel              = notification->cs_params().channel;
        cs_new_event->low_pass_filter_on   = notification->low_pass_filter_on();
        LOG(DEBUG) << "cs_new_event->low_pass_filter_on = " << int(cs_new_event->low_pass_filter_on)
                   << " cs_new_event = " << intptr_t(cs_new_event);
        cs_new_event->hostap_mac = network_utils::mac_from_string(radio_mac);
        cs_new_event->cs_params  = notification->cs_params();
        for (auto supported_channel : notification->hostap().supported_channels) {
            if (supported_channel.channel > 0) {
                LOG(DEBUG) << "supported_channel = " << int(supported_channel.channel);
            }
        }

        std::copy_n(notification->backhaul_params().backhaul_scan_measurement_list,
                    beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH,
                    cs_new_event->backhaul_scan_measurement_list);

        for (unsigned int i = 0; i < message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH; i++) {
            if (cs_new_event->backhaul_scan_measurement_list[i].channel > 0) {
                LOG(DEBUG) << "mac = "
                           << network_utils::mac_to_string(
                                  cs_new_event->backhaul_scan_measurement_list[i].mac)
                           << " channel = "
                           << int(cs_new_event->backhaul_scan_measurement_list[i].channel)
                           << " rssi = "
                           << int(cs_new_event->backhaul_scan_measurement_list[i].rssi);
            }
        }
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::SLAVE_JOINED_EVENT,
                         (void *)cs_new_event);
#ifdef BEEROCKS_RDKB
        //sending event to rdkb_wlan_task
        if (database.settings_rdkb_extensions()) {
            LOG(DEBUG) << "rdkb_wlan_task,sending STEERING_SLAVE_JOIN for mac " << radio_mac;
            rdkb_wlan_task::steering_slave_join_event new_event{};
            new_event.radio_mac = radio_mac;
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_SLAVE_JOIN, &new_event);
        }
#endif
    }

    //Update all (Slaves) last seen timestamp
    if (database.get_node_type(radio_mac) == beerocks::TYPE_SLAVE) {
        database.update_node_last_seen(radio_mac);
    }

    return true;
}

/**
 * @brief Parse the radio basic capabilities TLV and store the operating class
 * in the database as supported channels.
 * 
 * @param radio_mac radio mac address (RUID in non-Intel agent case)
 * @param radio_caps radio basic capabilities TLV received from the remote agent
 * @return true on success
 * @return false on failure
 */
bool master_thread::autoconfig_wsc_parse_radio_caps(
    std::string radio_mac, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps)
{
    // read all operating class list
    auto operating_classes_list_length = radio_caps->operating_classes_info_list_length();
    if (operating_classes_list_length > beerocks::message::SUPPORTED_CHANNELS_LENGTH) {
        LOG(WARNING) << "operating class info list larger then maximum supported channels";
        operating_classes_list_length = beerocks::message::SUPPORTED_CHANNELS_LENGTH;
    }
    for (int oc_idx = 0; oc_idx < operating_classes_list_length; oc_idx++) {
        std::stringstream ss;
        auto operating_class_tuple = radio_caps->operating_classes_info_list(oc_idx);
        if (!std::get<0>(operating_class_tuple)) {
            LOG(ERROR) << "getting operating class entry has failed!";
            return false;
        }
        auto &op_class                  = std::get<1>(operating_class_tuple);
        auto operating_class            = op_class.operating_class();
        auto maximum_transmit_power_dbm = op_class.maximum_transmit_power_dbm();
        ss << "operating_class=" << int(operating_class) << std::endl;
        ss << "maximum_transmit_power_dbm=" << int(maximum_transmit_power_dbm) << std::endl;
        ss << "channel list={ ";
        auto channel_list = son::wireless_utils::operating_class_to_channel_set(operating_class);
        for (auto channel : channel_list) {
            ss << int(channel) << " ";
        }
        ss << "}" << std::endl;
        ss << "statically_non_operable_channel_list={ ";

        auto non_oper_channels_list_length =
            op_class.statically_non_operable_channels_list_length();
        std::vector<uint8_t> non_operable_channels;
        for (int ch_idx = 0; ch_idx < non_oper_channels_list_length; ch_idx++) {
            auto channel = op_class.statically_non_operable_channels_list(ch_idx);
            ss << int(*channel) << " ";
            non_operable_channels.push_back(*channel);
        }
        ss << " }" << std::endl;
        LOG(DEBUG) << ss.str();
        // store operating class in the DB for this hostap
        database.add_hostap_supported_operating_class(
            radio_mac, operating_class, maximum_transmit_power_dbm, non_operable_channels);
    }

    return true;
}

bool master_thread::handle_non_intel_slave_join(
    Socket *sd, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
    std::shared_ptr<ieee1905_1::tlvWscM1> tlvwscM1, std::string bridge_mac, std::string radio_mac,
    ieee1905_1::CmduMessageTx &cmdu_tx)
{

    // Multi-AP Agent doesn't say anything about the backhaul, so simulate ethernet backhaul to satisfy
    // network map. MAC address is the bridge MAC with the last octet incremented by 1.
    // The mac address for the backhaul is the same since it is ethernet backhaul.
    sMacAddr mac = network_utils::mac_from_string(bridge_mac);
    mac.oct[5]++;
    std::string backhaul_mac = network_utils::mac_to_string(mac);
    mac.oct[5]++;
    std::string eth_switch_mac   = network_utils::mac_to_string(mac);
    std::string parent_bssid_mac = network_utils::ZERO_MAC_STRING;
    auto manufacturer            = tlvwscM1->manufacturer_str();
    LOG(INFO) << "IRE generic Slave joined, sd=" << intptr_t(sd) << std::endl
              << "    manufacturer=" << manufacturer << std::endl
              << "    parent_bssid_mac=" << parent_bssid_mac << std::endl
              << "    al_mac=" << bridge_mac << std::endl
              << "    eth_switch_mac=" << eth_switch_mac << std::endl
              << "    backhaul_mac=" << backhaul_mac << std::endl
              << "    radio_identifier = " << radio_mac << std::endl;

    LOG(DEBUG) << "simulate backhaul connected to the GW's LAN switch ";
    auto gw_container = database.get_nodes_from_hierarchy(0, beerocks::TYPE_GW);
    if (gw_container.empty()) {
        LOG(ERROR) << "can't get GW node!";
        return false;
    }

    auto gw_mac          = *gw_container.begin();
    auto gw_lan_switches = database.get_node_children(gw_mac, beerocks::TYPE_ETH_SWITCH);

    if (gw_lan_switches.empty()) {
        LOG(ERROR) << "GW has no LAN SWITCH node!";
        return false;
    }

    auto gw_lan_switch = *gw_lan_switches.begin();

    LOG(DEBUG) << "add a placeholder backhaul_mac = " << backhaul_mac
               << " gw_lan_switch = " << gw_lan_switch << " TYPE_IRE_BACKHAUL , STATE_CONNECTED";
    database.add_node(network_utils::mac_from_string(backhaul_mac),
                      network_utils::mac_from_string(gw_lan_switch), beerocks::TYPE_IRE_BACKHAUL);
    database.set_node_state(backhaul_mac, beerocks::STATE_CONNECTED);

    // TODO bridge handling.
    // Assume repeater
    beerocks::eType ire_type = beerocks::TYPE_IRE;

    // bridge_mac node may have been created from DHCP/ARP event, if so delete it
    // this may only occur once
    if (database.has_node(network_utils::mac_from_string(bridge_mac)) &&
        (database.get_node_type(bridge_mac) != ire_type)) {
        database.remove_node(network_utils::mac_from_string(bridge_mac));
    }
    // add new GW/IRE bridge_mac
    LOG(DEBUG) << "adding node " << bridge_mac << " under " << backhaul_mac << ", and mark as type "
               << ire_type;
    database.add_node(network_utils::mac_from_string(bridge_mac),
                      network_utils::mac_from_string(backhaul_mac), ire_type);
    database.set_node_state(bridge_mac, beerocks::STATE_CONNECTED);
    database.set_node_socket(bridge_mac, sd);
    database.set_node_platform(backhaul_mac, beerocks::ePlatform::PLATFORM_LINUX);
    database.set_node_platform(bridge_mac, beerocks::ePlatform::PLATFORM_LINUX);
    database.set_node_backhaul_iface_type(backhaul_mac, beerocks::eIfaceType::IFACE_TYPE_ETHERNET);
    database.set_node_backhaul_iface_type(bridge_mac, beerocks::IFACE_TYPE_BRIDGE);
    database.set_node_manufacturer(backhaul_mac, manufacturer);
    database.set_node_manufacturer(bridge_mac, manufacturer);
    database.set_node_type(backhaul_mac, beerocks::TYPE_IRE_BACKHAUL);
    database.set_node_name(backhaul_mac, manufacturer + "_BH");
    database.set_node_name(bridge_mac, manufacturer);
    database.add_node(network_utils::mac_from_string(eth_switch_mac),
                      network_utils::mac_from_string(bridge_mac), beerocks::TYPE_ETH_SWITCH);
    database.set_node_state(eth_switch_mac, beerocks::STATE_CONNECTED);
    database.set_node_name(eth_switch_mac, manufacturer + "_ETH");
    database.set_node_manufacturer(eth_switch_mac, eth_switch_mac);

    // Update existing node, or add a new one
    if (database.has_node(network_utils::mac_from_string(radio_mac))) {
        if (database.get_node_type(radio_mac) != beerocks::TYPE_SLAVE) {
            database.set_node_type(radio_mac, beerocks::TYPE_SLAVE);
            LOG(ERROR) << "Existing mac node is not TYPE_SLAVE";
        }
        database.clear_hostap_stats_info(radio_mac);
    } else {
        // TODO Intel Slave Join has separate radio MAC and UID; we use radio_mac for both.
        database.add_node(network_utils::mac_from_string(radio_mac),
                          network_utils::mac_from_string(bridge_mac), beerocks::TYPE_SLAVE,
                          network_utils::mac_from_string(radio_mac));
    }
    database.set_hostap_is_acs_enabled(radio_mac, false);

    // TODO Assume repeater mode
    database.set_hostap_repeater_mode_flag(radio_mac, true);
    // TODO Assume no backhaul manager
    database.set_hostap_backhaul_manager(radio_mac, false);

    database.set_node_state(radio_mac, beerocks::STATE_CONNECTED);
    database.set_node_backhaul_iface_type(radio_mac, beerocks::IFACE_TYPE_BRIDGE);
    // TODO driver_version will not be set
    database.set_hostap_iface_name(radio_mac, "N/A");
    database.set_hostap_iface_type(radio_mac, IFACE_TYPE_WIFI_UNSPECIFIED);

    // TODO number of antennas comes from HT/VHT capabilities (implicit from NxM)
    // TODO ant_gain and conducted_power will not be set
    database.set_hostap_ant_num(radio_mac, beerocks::eWiFiAntNum::ANT_NONE);
    database.set_hostap_ant_gain(radio_mac, 0);
    database.set_hostap_conducted_power(radio_mac, 0);
    database.set_hostap_active(radio_mac, true);
    database.set_node_name(radio_mac, manufacturer + "_AP");
    database.set_node_manufacturer(radio_mac, manufacturer);
    // TODO ipv4 will not be set

    // sd is assigned to src bridge mac
    sd->setPeerMac(bridge_mac);

    autoconfig_wsc_parse_radio_caps(radio_mac, radio_caps);
    // TODO assume SSIDs are not hidden
    database.set_hostap_advertise_ssid_flag(radio_mac, true);

    // TODO
    //        if (database.get_node_5ghz_support(radio_mac)) {
    //            if (notification->low_pass_filter_on()) {
    //                database.set_hostap_band_capability(radio_mac, beerocks::LOW_SUBBAND_ONLY);
    //            } else {
    //                database.set_hostap_band_capability(radio_mac, beerocks::BOTH_SUBBAND);
    //            }
    //        } else {
    database.set_hostap_band_capability(radio_mac, beerocks::SUBBAND_CAPABILITY_UNKNOWN);
    //        }

    // update bml listeners
    bml_task::connection_change_event bml_new_event;
    bml_new_event.mac = bridge_mac;
    tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &bml_new_event);
    LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << bml_new_event.mac;

    LOG(DEBUG) << "send AP_AUTOCONFIG_WSC M2";
    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

bool master_thread::handle_cmdu_control_message(
    Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    std::string hostap_mac = network_utils::mac_to_string(beerocks_header->radio_mac());

    // Sanity tests
    if (hostap_mac.empty()) {
        LOG(ERROR) << "CMDU received with id=" << int(beerocks_header->id())
                   << " op=" << int(beerocks_header->action_op()) << " with empty mac!";
        return false;
    }

    if (beerocks_header->direction() == beerocks::BEEROCKS_DIRECTION_AGENT) {
        return true;
    }

    //Update all (Slaves) last seen timestamp
    if (database.get_node_type(hostap_mac) == beerocks::TYPE_SLAVE) {
        database.update_node_last_seen(hostap_mac);
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CONTROL_HOSTAP_TX_ON_RESPONSE: {
        LOG(DEBUG) << "received ACTION_CONTROL_HOSTAP_TX_ON_RESPONSE hostap_mac=" << hostap_mac;

        auto new_event =
            CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sTxOnResponse_event);
        new_event->hostap_mac = network_utils::mac_from_string(hostap_mac);
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::TX_ON_RESPONSE_EVENT,
                         (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE: {
        LOG(DEBUG)
            << "received ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE from "
            << hostap_mac;
        auto response = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE>();

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        auto new_event = CHANNEL_SELECTION_ALLOCATE_EVENT(
            channel_selection_task::sRestrictedChannelResponse_event);
        new_event->hostap_mac = beerocks_header->radio_mac();
        new_event->success    = response->success();
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT,
                         (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION>();

        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        int vap_id = notification->vap_id();
        LOG(INFO) << "received ACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION from " << hostap_mac
                  << " vap_id=" << vap_id;

        const auto disabled_bssid = database.get_hostap_vap_mac(hostap_mac, vap_id);
        if (disabled_bssid.empty()) {
            LOG(INFO) << "AP Disabled on unknown vap, vap_id=" << vap_id;
            break;
        }
        auto client_list = database.get_node_children(disabled_bssid, beerocks::TYPE_CLIENT);

        for (auto &client : client_list) {
            son_actions::handle_dead_node(client, disabled_bssid, database, cmdu_tx, tasks);
        }

        database.remove_vap(hostap_mac, vap_id);

        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION>();

        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        int vap_id = notification->vap_id();
        LOG(INFO) << "received ACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION from " << hostap_mac
                  << " vap_id=" << vap_id;

        std::string radio_mac = hostap_mac;
        auto bssid            = net::network_utils::mac_to_string(notification->vap_info().mac);
        auto ssid             = std::string((char *)notification->vap_info().ssid);

        database.add_vap(radio_mac, vap_id, bssid, ssid, notification->vap_info().backhaul_vap);

        // update bml listeners
        bml_task::connection_change_event new_event;
        new_event.mac = database.get_node_parent_ire(radio_mac);
        tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << new_event.mac;

        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION: {
        std::string backhaul_mac = database.get_node_parent(hostap_mac);

        LOG(ERROR) << "Hostap CSA ERROR for IRE " << backhaul_mac << " hostap mac=" << hostap_mac;

        // TODO handle CSA error
        son_actions::set_hostap_active(database, tasks, hostap_mac, false);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_CSA_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_CONTROL_HOSTAP_CSA_NOTIFICATION from " << hostap_mac;

        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION failed";
            return false;
        }

        LOG(DEBUG) << "CS_task,sending CSA_EVENT for mac " << hostap_mac;
        auto new_event = CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sCsa_event);
        new_event->hostap_mac = beerocks_header->radio_mac();
        new_event->cs_params  = notification->cs_params();
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::CSA_EVENT, (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_ACS_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_CONTROL_HOSTAP_ACS_NOTIFICATION from " << hostap_mac;

        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION failed";
            return false;
        }
        LOG(DEBUG) << "CS_task,sending ACS_RESPONSE_EVENT for mac " << hostap_mac;

        auto new_event =
            CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sAcsResponse_event);
        new_event->hostap_mac         = network_utils::mac_from_string(hostap_mac);
        new_event->cs_params          = notification->cs_params();
        auto tuple_supported_channels = notification->supported_channels(0);
        std::copy_n(&std::get<1>(tuple_supported_channels), message::SUPPORTED_CHANNELS_LENGTH,
                    new_event->supported_channels);
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::ACS_RESPONSE_EVENT,
                         (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION: {
        auto notification =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION failed";
            return false;
        }
        std::unordered_map<int8_t, sVapElement> vaps_info;
        std::string vaps_list;
        for (int8_t vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id < IFACE_VAP_ID_MAX; vap_id++) {
            auto vap_mac = network_utils::mac_to_string(notification->params().vaps[vap_id].mac);
            if (vap_mac != network_utils::ZERO_MAC_STRING) {
                vaps_info[vap_id].mac = vap_mac;
                vaps_info[vap_id].ssid =
                    std::string((char *)notification->params().vaps[vap_id].ssid);
                vaps_info[vap_id].backhaul_vap = notification->params().vaps[vap_id].backhaul_vap;
                vaps_list += ("    vap_id=" + std::to_string(vap_id) +
                              ", vap_mac=" + (vaps_info[vap_id]).mac +
                              " , ssid=" + (vaps_info[vap_id]).ssid + std::string("\n"));
            }
        }

        LOG(INFO) << "sACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION from slave "
                  << hostap_mac << std::endl
                  << "vaps_list:" << std::endl
                  << vaps_list;

        std::string radio_mac = hostap_mac;

        for (auto vap : vaps_info) {
            if (!database.has_node(network_utils::mac_from_string(vap.second.mac))) {
                database.add_virtual_node(network_utils::mac_from_string(vap.second.mac),
                                          network_utils::mac_from_string(radio_mac));
            }
        }

        database.set_hostap_vap_list(radio_mac, vaps_info);

        // update bml listeners
        bml_task::connection_change_event new_event;
        new_event.mac = database.get_node_parent_ire(radio_mac);
        tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << new_event.mac;

        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION: {

        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION failed";
            return false;
        }

        std::string client_mac  = network_utils::mac_to_string(notification->params().mac);
        std::string client_ipv4 = network_utils::ipv4_to_string(notification->params().ipv4);
        LOG(DEBUG) << "received arp monitor notification from slave mac " << hostap_mac << ":"
                   << std::endl
                   << "   client_mac=" << client_mac << std::endl
                   << "   client_ipv4=" << client_ipv4 << std::endl
                   << "   state=" << int(notification->params().state)
                   << "   source=" << int(notification->params().source)
                   << "   type=" << int(notification->params().type);

        // IMPORTANT: Ignore RTM_DELNEIGH messages on the GRX350/IRE220 platforms.
        // Since the transport layer is accelerated, the OS may incorrectly decide
        // that a connected client has disconnected.
        //  if(notification->params.type == ARP_TYPE_DELNEIGH && !database.is_node_wireless(client_mac)) {
        //     auto eth_switch = database.get_node_parent(client_mac);
        //     LOG(INFO) << "ARP type RTM_DELNEIGH received!! handle dead client mac = " << client_mac;
        //     son_actions::handle_dead_node(client_mac, eth_switch, database, tasks);
        //     break;
        //  }

        if (client_ipv4 == network_utils::ZERO_IP_STRING) {
            LOG(DEBUG) << "arp ipv4 is 0.0.0.0, ignoring";
            break;
        }

        bool new_node = !database.has_node(network_utils::mac_from_string(client_mac));

        beerocks::eType new_node_type = database.get_node_type(client_mac);

        if ((new_node == false) && (new_node_type != beerocks::TYPE_CLIENT) &&
            (new_node_type != beerocks::TYPE_UNDEFINED)) {
            LOG(DEBUG) << "node " << client_mac << " type: " << (int)new_node_type
                       << " is (not a client/backhaul node) and (not stale), ignoring";
            break;
        }
        bool run_locating_task = false;
        // Since wireless clients are added to the DB on association, an ARP on non-existing node
        // can only be received for Ethernet clients
        if (new_node || !database.is_node_wireless(client_mac)) {

            // Assume node is connected to the GW's LAN switch
            // client_locating_task will find the correct position
            if (new_node) {
                LOG(DEBUG) << "handle_control_message - calling add_node_to_gw_default_location "
                              "client_mac = "
                           << client_mac;
                if (!son_actions::add_node_to_default_location(database, client_mac)) {
                    LOG(ERROR) << "handle_control_message - add_node_to_default_location failed!";
                    break;
                }
                new_node_type = database.get_node_type(client_mac);
            }

            // New IP
            if (new_node || database.get_node_ipv4(client_mac) != client_ipv4) {
                LOG(DEBUG) << "Update node IP - mac: " << client_mac << " ipv4: " << client_ipv4;
                database.set_node_ipv4(client_mac, client_ipv4);
                son_actions::handle_completed_connection(database, cmdu_tx, tasks, client_mac);
            }

            // Run locating task only on CLIENTs or IREs
            if ((new_node_type == beerocks::TYPE_CLIENT) || (new_node_type == beerocks::TYPE_IRE)) {
                run_locating_task = true;
            }

            // Wireless Node
        } else {

            // Client NOT connected
            if (database.get_node_state(client_mac) == beerocks::STATE_DISCONNECTED) {
                LOG(DEBUG) << "node_state = DISCONNECTED client_mac = " << client_mac
                           << " client_ipv4 =" << client_ipv4;

                // The IP has changed
            } else if (database.get_node_ipv4(client_mac) != client_ipv4) {

                LOG(DEBUG) << "Update node IP - mac: " << client_mac << " ipv4: " << client_ipv4;
                database.set_node_ipv4(client_mac, client_ipv4);
                son_actions::handle_completed_connection(database, cmdu_tx, tasks, client_mac);
            }
        }

        // Update the last-seen timestamp
        // Handled at this point to make sure the client was added to the DB
        database.update_node_last_seen(client_mac);

        // Run client locating task for reachable or stale client/IRE nodes only if on ETH_FRONT port
        // or WIRELESS_FRONT (in case of eth devices connected to IREs and arp notf was send from GW)
        if (run_locating_task && ((notification->params().source == ARP_SRC_ETH_FRONT) ||
                                  (notification->params().source == ARP_SRC_WIRELESS_FRONT))) {
            LOG(DEBUG) << "run_client_locating_task client_mac = " << client_mac;

            auto eth_switches = database.get_node_siblings(hostap_mac, beerocks::TYPE_ETH_SWITCH);
            if (eth_switches.size() != 1) {
                LOG(ERROR) << "SLAVE " << hostap_mac
                           << " does not have an Ethernet switch sibling!";
                break;
            }

            std::string eth_switch = *(eth_switches.begin());
            int prev_task_id = database.get_client_locating_task_id(client_mac, true /*reachable*/);

            if (tasks.is_task_running(prev_task_id)) {
                LOG(DEBUG) << "client locating task already running for " << client_mac;
            } else {
                LOG(DEBUG) << "running client_locating_task on client = " << client_mac;
                auto new_task = std::make_shared<client_locating_task>(
                    database, cmdu_tx, tasks, client_mac, true /*reachable*/, 2000, eth_switch);
                tasks.add_task(new_task);
            }
        } else {
            LOG(INFO) << "Not running client_locating_task for client_mac " << client_mac
                      << " notification->params.source: " << (int)notification->params().source;
        }

        break;
    }
    case beerocks_message::ACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION failed";
            return false;
        }
        auto bridge_mac = network_utils::mac_to_string(notification->bridge_mac());

        LOG(TRACE) << "ACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION: " << bridge_mac
                   << ", new_operational_state=" << int(notification->operational());
        database.set_node_operational_state(bridge_mac, notification->operational());
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR)
                << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION failed";
            return false;
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        auto notification =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE failed";
            return false;
        }

        std::string client_mac = network_utils::mac_to_string(notification->params().result.mac);
        std::string ap_mac     = hostap_mac;
        bool is_parent         = (database.get_node_parent(client_mac) ==
                          database.get_hostap_vap_mac(ap_mac, notification->params().vap_id));

        LOG_CLI(DEBUG,
                "rssi measurement response: "
                    << client_mac << " (sta) <-> (ap) " << ap_mac
                    << " rx_packets=" << int(notification->params().rx_packets)
                    << " rx_rssi=" << int(notification->params().rx_rssi)
                    << " phy_rate_100kb (RX|TX)=" << int(notification->params().rx_phy_rate_100kb)
                    << " | " << int(notification->params().tx_phy_rate_100kb)
                    << " is_parent=" << (is_parent ? "1" : "0")
                    << " src_module=" << int(notification->params().src_module)
                    << " id=" << int(beerocks_header->id()));
        //response return from slave backhaul manager , updating the matching same band sibling.
        if (database.is_hostap_backhaul_manager(ap_mac) &&
            database.is_node_wireless(database.get_node_parent_backhaul(ap_mac)) &&
            database.is_node_5ghz(client_mac)) {
            auto priv_ap_mac = ap_mac;
            ap_mac           = database.get_5ghz_sibling_hostap(ap_mac);
            LOG(DEBUG) << "update rssi measurement BH manager from ap_mac = " << priv_ap_mac
                       << " to = " << ap_mac;
        }
        if (ap_mac.empty() ||
            !database.set_node_cross_rx_rssi(client_mac, ap_mac, notification->params().rx_rssi,
                                             notification->params().rx_packets)) {
            LOG(ERROR) << "update rssi measurement failed";
        }
        if (is_parent) {
            database.set_node_cross_tx_phy_rate_100kb(client_mac,
                                                      notification->params().tx_phy_rate_100kb);
            database.set_node_cross_rx_phy_rate_100kb(client_mac,
                                                      notification->params().rx_phy_rate_100kb);
        }
#ifdef BEEROCKS_RDKB
        if (database.settings_rdkb_extensions() &&
            (beerocks_header->id() == database.get_rdkb_wlan_task_id())) {
            beerocks_message::sSteeringEvSnr new_event;
            new_event.snr = notification->params().rx_snr;
            std::copy_n(notification->params().result.mac.oct, sizeof(new_event.client_mac.oct),
                        new_event.client_mac.oct);
            new_event.bssid = network_utils::mac_from_string(
                database.get_hostap_vap_mac(ap_mac, notification->params().vap_id));
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_SNR_AVAILABLE, &new_event);
        }
#endif
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION failed";
            return false;
        }
        std::string client_mac    = network_utils::mac_to_string(notification->params().result.mac);
        std::string client_parent = database.get_node_parent(client_mac);
        std::string ap_mac = database.get_hostap_vap_mac(hostap_mac, notification->params().vap_id);
        bool is_parent =
            (client_parent == database.get_hostap_vap_mac(ap_mac, notification->params().vap_id));

        int rx_rssi = int(notification->params().rx_rssi);

        LOG_CLI(DEBUG,
                "measurement change notification: "
                    << client_mac << " (sta) <-> (ap) " << ap_mac << " rx_rssi=" << rx_rssi
                    << " phy_rate_100kb (RX|TX)=" << int(notification->params().rx_phy_rate_100kb)
                    << " | " << int(notification->params().tx_phy_rate_100kb));

        if ((database.get_node_type(client_mac) == beerocks::TYPE_CLIENT) &&
            (database.get_node_state(client_mac) == beerocks::STATE_CONNECTED) &&
            (!database.get_node_handoff_flag(client_mac)) && is_parent) {

            database.set_node_cross_rx_rssi(client_mac, ap_mac, notification->params().rx_rssi,
                                            notification->params().rx_packets);
            database.set_node_cross_tx_phy_rate_100kb(client_mac,
                                                      notification->params().tx_phy_rate_100kb);
            database.set_node_cross_rx_phy_rate_100kb(client_mac,
                                                      notification->params().rx_phy_rate_100kb);

            /*
                * when a notification arrives, it means a large change in rx_rssi occurred (above the defined thershold)
                * therefore, we need to create an optimal path task to relocate the node if needed
                */
            int prev_task_id = database.get_roaming_task_id(client_mac);
            if (tasks.is_task_running(prev_task_id)) {
                LOG(DEBUG) << "roaming task already running for " << client_mac;
            } else {
                auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, tasks,
                                                                    client_mac, 0, "");
                tasks.add_task(new_task);
            }
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_AGENT_PING_REQUEST: {
        if (hostap_mac.empty()) {
            LOG(WARNING) << "PING_MSG_REQUEST unknown peer mac!";
        } else if (!database.update_node_last_seen(hostap_mac)) {
            LOG(DEBUG) << "PING_MSG_REQUEST received from ire " << hostap_mac
                       << " , can't update last seen time for ";
        }

        auto request = cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_AGENT_PING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_AGENT_PING_REQUEST failed";
            return false;
        }

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_CONTROL_AGENT_PING_RESPONSE>(
                cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        response->total() = request->total();
        response->seq()   = request->seq();
        response->size()  = request->size();

        if (response->size()) {
            if (!request->alloc_data(response->size())) {
                LOG(ERROR) << "Failed buffer allocation to size=" << int(response->size());
                break;
            }
            memset(request->data(), 0, request->data_length());
        }

        son_actions::send_cmdu_to_agent(sd, cmdu_tx, hostap_mac);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CONTROLLER_PING_RESPONSE: {
        if (hostap_mac.empty()) {
            LOG(ERROR) << "PING_MSG_RESPONSE unknown peer mac!";
        } else {
            auto response =
                cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CONTROLLER_PING_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_CONTROL_CONTROLLER_PING_RESPONSE failed";
                return false;
            }
            if (!database.update_node_last_ping_received(hostap_mac, response->seq())) {
                LOG(DEBUG) << "PING_MSG_RESPONSE received from slave " << hostap_mac
                           << " , can't update last seen time for ";
            } else {
                LOG_CLI(DEBUG,
                        "PING_MSG_RESPONSE received from slave = "
                            << hostap_mac << " , seq = " << (int)response->seq()
                            << " , size = " << (int)response->size() << " , RTT = "
                            << float((std::chrono::duration_cast<std::chrono::duration<double>>(
                                          database.get_node_last_ping_received(hostap_mac) -
                                          database.get_node_last_ping_sent(hostap_mac)))
                                         .count())
                            << "[sec]" << std::endl);
            }
            if (response->seq() < (response->total() - 1)) { //send next ping request
                auto request = message_com::create_vs_message<
                    beerocks_message::cACTION_CONTROL_CONTROLLER_PING_REQUEST>(cmdu_tx);
                if (request == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return false;
                }
                request->total() = response->total();
                request->seq()   = response->seq() + 1;
                request->size()  = response->size();
                if (!request->alloc_data(response->size())) {
                    LOG(ERROR) << "Failed buffer allocation to size=" << int(response->size());
                    break;
                }
                memset(request->data(), 0, request->data_length());
                if (!database.update_node_last_ping_sent(hostap_mac)) {
                    LOG(DEBUG) << "sending PING_MSG_REQUEST for slave " << hostap_mac
                               << " , can't update last ping sent time for ";
                }
                son_actions::send_cmdu_to_agent(sd, cmdu_tx, hostap_mac);
            } else if (response->seq() == (response->total() - 1)) {
                if (!database.update_node_last_ping_received_avg(hostap_mac, response->total())) {
                    LOG(DEBUG) << "last PING_MSG_RESPONSE received from slave " << hostap_mac
                               << " , can't update last ping received avg ";
                } else {
                    LOG_CLI(DEBUG, "last PING_MSG_RESPONSE received from slave = "
                                       << hostap_mac << " RTT summary: " << std::endl
                                       << "min = " << database.get_node_last_ping_min_ms(hostap_mac)
                                       << " [ms], "
                                       << "max = " << database.get_node_last_ping_max_ms(hostap_mac)
                                       << " [ms], "
                                       << "avg = " << database.get_node_last_ping_avg_ms(hostap_mac)
                                       << " [ms]" << std::endl);
                }
            }
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION failed";
            return false;
        }
        std::string client_mac = network_utils::mac_to_string(notification->mac());

        LOG(INFO) << "ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION, client_mac=" << client_mac
                  << " hostap mac=" << hostap_mac;

        if (database.get_node_type(client_mac) == beerocks::TYPE_IRE_BACKHAUL) {
            LOG(INFO) << "IRE CLIENT_NO_RESPONSE_NOTIFICATION, client_mac=" << client_mac
                      << " hostap mac=" << hostap_mac
                      << " closing socket and marking as disconnected";
            son_actions::handle_dead_node(client_mac, hostap_mac, database, cmdu_tx, tasks);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION failed";
            return false;
        }

        std::string client_mac = network_utils::mac_to_string(notification->mac());
        std::string ipv4       = network_utils::ipv4_to_string(notification->ipv4());
        LOG(DEBUG) << "dhcp complete for client " << client_mac << " new ip=" << ipv4
                   << " previous ip=" << database.get_node_ipv4(client_mac);

        if (!database.has_node(network_utils::mac_from_string(client_mac))) {
            LOG(DEBUG) << "client mac not in DB, add temp node " << client_mac;
            database.add_node(network_utils::mac_from_string(client_mac));
            database.update_node_last_seen(client_mac);
        }

        if (database.get_node_type(client_mac) == beerocks::TYPE_CLIENT) {
            auto db_ipv4 = database.get_node_ipv4(client_mac);
            if (!database.set_node_ipv4(client_mac, ipv4)) {
                LOG(ERROR) << "set node ipv4 failed";
            }

            if (!database.set_node_name(
                    client_mac, std::string(notification->name(message::NODE_NAME_LENGTH)))) {
                LOG(ERROR) << "set node name failed";
            }

            if (database.get_node_ipv4(client_mac) != ipv4) {
                LOG(DEBUG) << "handle_completed_connection client_mac = " << client_mac;
                son_actions::handle_completed_connection(database, cmdu_tx, tasks, client_mac);
            }

            if (!database.is_node_wireless(client_mac)) {
                LOG(DEBUG) << "run_client_locating_task client_mac = " << client_mac;
                int prev_task_id = database.get_client_locating_task_id(client_mac, true);
                if (tasks.is_task_running(prev_task_id)) {
                    LOG(DEBUG) << "client locating task already running for " << client_mac;
                } else {
                    auto new_task = std::make_shared<client_locating_task>(database, cmdu_tx, tasks,
                                                                           client_mac, true, 2000);
                    tasks.add_task(new_task);
                }
            }
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION: {
        /* TODO decide what this code should do now that tx_rssi is no longer used
            auto report = (message::sACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION*)rx_buffer;
            std::string backhaul_mac = database.get_node_parent_backhaul(sd->hostap_mac());
            std::string parent_mac = database.get_node_parent(backhaul_mac);

            int prev_rssi = database.get_node_cross_tx_rssi(backhaul_mac, parent_mac);
            int rssi = report->params.rssi;

            if (prev_rssi == beerocks::RSSI_INVALID) {
                prev_rssi = rssi;
            }

            //LOG(DEBUG) << "rssi report from ire " << ire_mac << " rssi=" << rssi << " prev_rssi=" << prev_rssi;
            database.set_node_cross_tx_rssi(backhaul_mac, parent_mac, rssi);
            if ((abs(prev_rssi - rssi) >= int(database.config.monitor_rx_rssi_notification_delta_db)) && database.settings_ire_roaming()) {
                auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, tasks, backhaul_mac, 0, "ire_rssi_report");
                tasks.add_task(new_task);
            }
            */
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION failed";
            return false;
        }
        LOG(DEBUG) << "received ACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION hostap_mac="
                   << hostap_mac;

        auto new_event =
            CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sCacCompleted_event);
        new_event->hostap_mac = network_utils::mac_from_string(hostap_mac);
        new_event->params     = notification->params();
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::CAC_COMPLETED_EVENT,
                         (void *)new_event);

        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION failed";
            return false;
        }
        LOG(DEBUG)
            << "received ACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION hostap_mac="
            << hostap_mac;

        auto new_event =
            CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sDfsChannelAvailable_event);
        new_event->hostap_mac = network_utils::mac_from_string(hostap_mac);
        new_event->params     = notification->params();
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::DFS_CHANNEL_AVAILABLE_EVENT,
                         (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE: {
        auto response =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE failed";
            return false;
        }

        for (auto i = 0; i < response->sta_stats_size(); i++) {
            auto sta_stats_tuple = response->sta_stats(i);
            if (!std::get<0>(sta_stats_tuple)) {
                LOG(ERROR) << "Couldn't access sta in location " << i;
                continue;
            }
            auto &sta_stats = std::get<1>(sta_stats_tuple);
            auto client_mac = network_utils::mac_to_string(sta_stats.mac);

            if (!database.has_node(network_utils::mac_from_string(client_mac))) {
                LOG(ERROR) << "sta " << client_mac << " is not in DB!";
                continue;
            } else if (database.get_node_state(client_mac) != beerocks::STATE_CONNECTED) {
                LOG(DEBUG) << "sta " << client_mac << " is not connected to hostap " << hostap_mac
                           << ", update is invalid!";
                continue;
            }
            database.set_node_stats_info(client_mac, &sta_stats);
        }

        database.set_hostap_stats_info(hostap_mac, &response->ap_stats());
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE: {
        auto response =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE failed";
            return false;
        }
        LOG_CLI(
            DEBUG,
            "beacon response , ID: "
                << beerocks_header->id() << std::endl
                << "sta_mac: " << network_utils::mac_to_string(response->params().sta_mac)
                << std::endl
                << "measurement_rep_mode: " << (int)response->params().rep_mode << std::endl
                << "op_class: " << (int)response->params().op_class << std::endl
                << "channel: "
                << (int)response->params().channel
                //<< std::endl << "start_time: "           << (int)response->params.start_time
                << std::endl
                << "duration: "
                << (int)response->params().duration
                //<< std::endl << "phy_type: "             << (int)response->params.phy_type
                //<< std::endl << "frame_type: "           << (int)response->params.frame_type
                << std::endl
                << "rcpi: " << (int)response->params().rcpi << std::endl
                << "rsni: " << (int)response->params().rsni << std::endl
                << "bssid: " << network_utils::mac_to_string(response->params().bssid)
            //<< std::endl << "ant_id: "               << (int)response->params.ant_id
            //<< std::endl << "tsf: "                  << (int)response->params.parent_tsf
            //<< std::endl << "new_ch_width: "                         << (int)response->params.new_ch_width
            //<< std::endl << "new_ch_center_freq_seg_0: "             << (int)response->params.new_ch_center_freq_seg_0
            //<< std::endl << "new_ch_center_freq_seg_1: "             << (int)response->params.new_ch_center_freq_seg_1
        );
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE: {
        auto response =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE failed";
            return false;
        }
        LOG_CLI(DEBUG,
                "sta channel load response:"
                    << std::endl
                    << "sta_mac: " << network_utils::mac_to_string(response->params().sta_mac)
                    << std::endl
                    << "measurement_rep_mode: " << (int)response->params().rep_mode << std::endl
                    << "op_class: " << (int)response->params().op_class << std::endl
                    << "channel: " << (int)response->params().channel << std::endl
                    << "start_time: " << (int)response->params().start_time << std::endl
                    << "duration: " << (int)response->params().duration << std::endl
                    << "channel_load: " << (int)response->params().channel_load

                    << std::endl
                    << "new_ch_width: " << (int)response->params().new_ch_width << std::endl
                    << "new_ch_center_freq_seg_0: "
                    << (int)response->params().new_ch_center_freq_seg_0 << std::endl
                    << "new_ch_center_freq_seg_1: "
                    << (int)response->params().new_ch_center_freq_seg_1);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE: {
        auto response =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE failed";
            return false;
        }
        std::string statistics_group_data;
        for (uint8_t i = 0; i < response->params().statistics_group_data_size; i++) {
            statistics_group_data +=
                std::to_string(response->params().statistics_group_data[i]) + ",";
        }
        statistics_group_data.pop_back(); // deletes last comma
        LOG_CLI(DEBUG,
                "statistics response: "
                    << std::endl
                    << "sta_mac: " << network_utils::mac_to_string(response->params().sta_mac)
                    << std::endl
                    << "measurement_rep_mode: " << (int)response->params().rep_mode << std::endl
                    << "duration: " << (int)response->params().duration << std::endl
                    << "group_identity: " << (int)response->params().group_identity << std::endl
                    << "statistics_group_data: " << statistics_group_data

                    << std::endl
                    << "average_trigger: " << (int)response->params().average_trigger << std::endl
                    << "consecutive_trigger: " << (int)response->params().consecutive_trigger
                    << std::endl
                    << "delay_trigger: " << (int)response->params().delay_trigger);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE: {
        auto response = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE failed";
            return false;
        }
        LOG_CLI(DEBUG,
                "link measurements response: "
                    << std::endl
                    << "sta_mac: " << network_utils::mac_to_string(response->params().sta_mac)
                    << std::endl
                    << "transmit_power: " << (int)response->params().transmit_power << std::endl
                    << "link_margin: " << (int)response->params().link_margin << std::endl
                    << "rx_ant_id: " << (int)response->params().rx_ant_id << std::endl
                    << "tx_ant_id: " << (int)response->params().tx_ant_id << std::endl
                    << "rcpi: " << (int)response->params().rcpi << std::endl
                    << "rsni: " << (int)response->params().rsni

                    << std::endl
                    << "dmg_link_margin_activity: "
                    << (int)response->params().dmg_link_margin_activity << std::endl
                    << "dmg_link_margin_mcs: " << (int)response->params().dmg_link_margin_mcs
                    << std::endl
                    << "dmg_link_margin_link_margin: "
                    << (int)response->params().dmg_link_margin_link_margin << std::endl
                    << "dmg_link_margin_snr: " << (int)response->params().dmg_link_margin_snr
                    << std::endl
                    << "dmg_link_margin_reference_timestamp: "
                    << (int)response->params().dmg_link_margin_reference_timestamp << std::endl
                    << "dmg_link_adapt_ack_activity: "
                    << (int)response->params().dmg_link_adapt_ack_activity << std::endl
                    << "dmg_link_adapt_ack_reference_timestamp: "
                    << (int)response->params().dmg_link_adapt_ack_reference_timestamp);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: {
        auto response = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE failed";
            return false;
        }
        std::string client_mac = network_utils::mac_to_string(response->mac());
        int channel            = database.get_node_channel(client_mac);
        LOG(DEBUG) << "ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE client_mac="
                   << client_mac << " received from hostap " << hostap_mac
                   << " channel=" << int(channel) << " ïd = " << int(beerocks_header->id());
        //calculating response delay for associate client ap and cross ap's
        database.set_measurement_recv_delta(hostap_mac);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION failed";
            return false;
        }
        std::string client_mac = network_utils::mac_to_string(notification->mac());
        LOG(INFO) << "CLIENT NO ACTIVITY MSG RX'ed for client" << client_mac;
        int prev_task_id = database.get_roaming_task_id(client_mac);
        if (tasks.is_task_running(prev_task_id)) {
            LOG(DEBUG) << "roaming task already running for " << client_mac;
        } else {
            LOG(INFO) << "Starting optimal path for client" << client_mac;
            auto new_task =
                std::make_shared<optimal_path_task>(database, cmdu_tx, tasks, client_mac, 0, "");
            tasks.add_task(new_task);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION failed";
            return false;
        }

        database.set_hostap_activity_mode(
            hostap_mac, beerocks::eApActiveMode(notification->params().ap_activity_mode));
        if (notification->params().ap_activity_mode == beerocks::AP_IDLE_MODE) {
            LOG(DEBUG) << "CS_task,sending AP_ACTIVITY_IDLE_EVENT for mac " << hostap_mac;
            auto new_event =
                CHANNEL_SELECTION_ALLOCATE_EVENT(channel_selection_task::sApActivityIdle_event);
            new_event->hostap_mac = network_utils::mac_from_string(hostap_mac);
            tasks.push_event(database.get_channel_selection_task_id(),
                             (int)channel_selection_task::eEvent::AP_ACTIVITY_IDLE_EVENT,
                             (void *)new_event);
        }

        break;
    }
    case beerocks_message::ACTION_CONTROL_ARP_QUERY_RESPONSE: {
        LOG(DEBUG) << "ACTION_CONTROL_ARP_QUERY_RESPONSE from "
                   << " id=" << beerocks_header->id();
        auto response = cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_ARP_QUERY_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_ARP_QUERY_RESPONSE failed";
            return false;
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE: {
        LOG(TRACE) << "ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE: "
                   << network_utils::mac_to_string(beerocks_header->radio_mac())
                   << ", ID: " << beerocks_header->id();
        break;
    }
    case beerocks_message::ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE: {
        LOG(TRACE) << "ACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE: "
                   << network_utils::mac_to_string(beerocks_header->radio_mac())
                   << ", ID: " << beerocks_header->id();
        break;
    }
#ifdef BEEROCKS_RDKB
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION failed";
            return false;
        }
        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            beerocks_message::sSteeringEvActivity new_event;
            new_event = notification->params();
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_CLIENT_ACTIVITY_AVAILABLE,
                             &new_event);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION: {
        auto notification =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION failed";
            return false;
        }
        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            beerocks_message::sSteeringEvSnrXing new_event;
            new_event = notification->params();
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_SNR_XING_AVAILABLE, &new_event);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION failed";
            return false;
        }
        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            beerocks_message::sSteeringEvProbeReq new_event;
            new_event = notification->params();
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_PROBE_REQ_AVAILABLE,
                             &new_event);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION: {
        auto notification = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION failed";
            return false;
        }
        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            beerocks_message::sSteeringEvAuthFail new_event;
            new_event = notification->params();
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_EVENT_AUTH_FAIL_AVAILABLE,
                             &new_event);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE: {
        auto notification =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST failed";
            return false;
        }
        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            rdkb_wlan_task::steering_set_group_response_event new_event;
            new_event.ret_code = notification->params().error_code;
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_SET_GROUP_RESPONSE, &new_event);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE failed";
            return false;
        }

        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            rdkb_wlan_task::steering_client_set_response_event new_event;
            new_event.ret_code = notification->params().error_code;
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_CLIENT_SET_RESPONSE, &new_event);
        }
        break;
    }
#endif // BEEROCKS_RDKB
    case beerocks_message::ACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE: {
        auto notification =
            cmdu_rx.addClass<beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE failed";
            return false;
        }
#ifdef BEEROCKS_RDKB
        //push event to rdkb_wlan_hal task
        if (database.settings_rdkb_extensions()) {
            rdkb_wlan_task::steering_client_disconnect_response_event new_event;
            new_event.ret_code = notification->params().error_code;
            tasks.push_event(database.get_rdkb_wlan_task_id(),
                             rdkb_wlan_task::events::STEERING_CLIENT_DISCONNECT_RESPONSE,
                             &new_event);
        }
#endif
        break;
    }
    default: {
        LOG_CLI(ERROR, "Unsupported CONTROL action_op: " << int(beerocks_header->action_op()));
        return false;
    }
    }

    // If this is a response message to a task (header->id() == task id), send it to it directly - cmdu_rx is owned by the task
    // e.g. only the task may call addClass
    if (beerocks_header->id()) {
        tasks.response_received(beerocks_header->id(), hostap_mac,
                                (beerocks_message::eActionOp_CONTROL)beerocks_header->action_op(),
                                cmdu_rx);
        return true;
    }

    return true;
}
