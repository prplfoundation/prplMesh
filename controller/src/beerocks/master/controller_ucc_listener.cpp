/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "controller_ucc_listener.h"
#include "son_actions.h"

using namespace beerocks;

controller_ucc_listener::controller_ucc_listener(db &database)
    : beerocks_ucc_listener(database.config.ucc_listener_port), m_database(database)
{
    m_ucc_listener_run_on = eUccListenerRunOn::CONTROLLER;
}

/**
 * @brief Returns string filled with reply to "DEVICE_GET_INFO" command.
 * 
 * @return const std::string Device info in UCC reply format.
 */
std::string controller_ucc_listener::fill_version_reply_string()
{
    return std::string("vendor,") + m_database.settings_vendor() + std::string(",model,") +
           m_database.settings_model() + std::string(",version,") + BEEROCKS_VERSION;
}

/**
 * @brief Clear configuration on Controller database.
 * 
 * @return None.
 */
void controller_ucc_listener::clear_configuration() { m_database.clear_bss_info_configuration(); }

/**
 * @brief Return socket to Agent with bridge 'dest_alid` MAC address.
 * 
 * @param[in] dest_alid Agent bridge MAC address.
 * @return Socket* Socket to the Agent.
 */
Socket *controller_ucc_listener::get_dev_send_1905_destination_socket(const std::string &dest_alid)
{
    return m_database.get_node_socket(dest_alid);
}

/**
 * @brief Get preprepared buffer with CMDU in it.
 * 
 * @return std::shared_ptr<uint8_t> Buffer pointer.
 */
std::shared_ptr<uint8_t> controller_ucc_listener::get_buffer_filled_with_cmdu()
{
    return m_database.get_certification_tx_buffer();
}

/**
 * @brief Send CMDU to destined Agent.
 * 
 * @param[in] sd Agent socket.
 * @param[in] cmdu_tx CMDU object.
 * @return true if successful, false if not.
 */
bool controller_ucc_listener::send_cmdu_to_destination(Socket *sd,
                                                       ieee1905_1::CmduMessageTx &cmdu_tx)
{
    return son_actions::send_cmdu_to_agent(sd, cmdu_tx);
}

/**
 * @brief Handle DEV_SET_CONFIG command. Parse the command and save the parameters on the controller
 * database.
 * 
 * @param[in] params Command parameters.
 * @param[out] err_string Contains an error description if the function fails.
 * @return true if successful, false if not.
 */
bool controller_ucc_listener::handle_dev_set_config(
    std::unordered_map<std::string, std::string> &params, std::string &err_string)
{
    if (params.find("backhaul") != params.end()) {
        err_string = "parameter 'backhaul' is not relevant to the controller";
        return false;
    }

    for (uint8_t bss_info_idx = 1; bss_info_idx < UINT8_MAX; bss_info_idx++) {
        std::string key("bss_info");
        key += std::to_string(bss_info_idx);
        if (params.find(key) == params.end()) {
            if (bss_info_idx == 1) {
                err_string = "command has no bss_info configuration";
                return false;
            }
            break;
        }

        db::bss_info_conf_t bss_info_conf;
        auto al_mac = parse_bss_info(params[key], bss_info_conf, err_string);
        if (al_mac.empty()) {
            err_string += (" on " + key);
            return false;
        }

        m_database.add_bss_info_configuration(net::network_utils::mac_from_string(al_mac),
                                              bss_info_conf);
    }
    return true;
}

/**
 * @brief Parse bss_info string into bss_info_conf_struct.
 * 
 * @param[in] bss_info_str String containing bss info configuration.
 * @param[out] bss_info_conf Controller database struct filled with configuration.
 * @param[out] err_string Contains an error description if the function fails.
 * @return al_mac on success, empty string if not.
 */
std::string controller_ucc_listener::parse_bss_info(const std::string &bss_info_str,
                                                    db::bss_info_conf_t &bss_info_conf,
                                                    std::string &err_string)
{
    auto confs = string_utils::str_split(bss_info_str, ' ');

    /*
    The Control API specification defines 8 parameters except for the case of 
    clearing the BSS info stored for a specific operating class, define only
    two parameters: ALID and operating class.
    */
    if ((confs.size() != 8) && (confs.size() != 2)) {
        err_string = "missing configuration";
        return std::string();
    }

    // Alid
    std::string al_mac(confs[0]);
    if (!net::network_utils::is_valid_mac(al_mac)) {
        err_string = "invalid al_mac";
        return std::string();
    }

    bss_info_conf = {};

    // Operating class
    auto &operating_class_str = confs[1];
    if (operating_class_str == "8x") {
        bss_info_conf.operating_class = {81, 83, 84};
    } else if (operating_class_str == "11x") {
        bss_info_conf.operating_class = {115, 116};
    } else if (operating_class_str == "12x") {
        bss_info_conf.operating_class = {124, 125, 126};
    } else {
        err_string = "invalid operating class " + operating_class_str;
        return std::string();
    }

    if (confs.size() == 2) {
        return al_mac;
    }

    // SSID
    bss_info_conf.ssid = confs[2];
    if (bss_info_conf.ssid.size() > WSC::eWscLengths::WSC_MAX_SSID_LENGTH) {
        err_string = "ssid is too long";
        return std::string();
    }

    // Authentication type
    auto &authentication_type_str = confs[3];
    if (!beerocks_ucc_listener::validate_hex_notation(authentication_type_str, 2)) {
        err_string = "invalid authentication type format";
        return std::string();
    }

    uint16_t authentication_type = std::stoi(authentication_type_str, nullptr, 16);

    if (!WSC::eWscAuthValidate::check(authentication_type)) {
        err_string = "invalid authentication type value";
        return std::string();
    }
    bss_info_conf.authentication_type = static_cast<WSC::eWscAuth>(authentication_type);

    // Encryption type
    auto &encryption_type_str = confs[4];
    if (!beerocks_ucc_listener::validate_hex_notation(encryption_type_str, 2)) {
        err_string = "invalid encryption type format";
        return std::string();
    }

    uint16_t encryption_type = std::stoi(encryption_type_str, nullptr, 16);

    if (!WSC::eWscEncrValidate::check(encryption_type)) {
        err_string = "invalid encryption type value";
        return std::string();
    }
    bss_info_conf.encryption_type = static_cast<WSC::eWscEncr>(encryption_type);

    // Network key
    bss_info_conf.network_key = confs[5];
    if (bss_info_conf.network_key.size() > WSC::eWscLengths::WSC_MAX_NETWORK_KEY_LENGTH) {
        err_string = "network key is too long";
        return std::string();
    }

    // Bit 6 of Multi-AP IE's extention attribute, aka "Backhaul BSS"
    auto &bit_6_str = confs[6];
    if (bit_6_str != "0" && bit_6_str != "1") {
        err_string = "invalid bit 6 of Multi-AP IE's extention attribute";
        return std::string();
    }
    uint8_t bss_type = (bit_6_str == "1" ? WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS : 0);

    // Bit 5 of Multi-AP IE's extention attribute, aka "Fronthaul BSS"
    auto &bit_5_str = confs[7];
    if (bit_5_str != "0" && bit_5_str != "1") {
        err_string = "invalid bit 5 of Multi-AP IE's extention attribute";
        return std::string();
    }
    bss_type |= (bit_5_str == "1" ? WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS : 0);

    // Update Bits 6 and 5
    bss_info_conf.bss_type = static_cast<WSC::eWscVendorExtSubelementBssType>(bss_type);

    return al_mac;
}
