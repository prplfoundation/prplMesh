
#include "wfa_ca.h"

#include "son_actions.h"

#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>

#include <tlvf/wfa_map/eTlvTypeMap.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>

#include <easylogging++.h>

#include <algorithm>
#include <iomanip>

#define FORBIDDEN_CHARS "$#&*()[];/\\<>?|`~=+"

using namespace beerocks;

static uint16_t g_mid = UINT16_MAX / 2 + 1;

/* Common Error Messages */
static const std::string err_internal = "Internal error";

/**
 * @brief Checks for forbidden characters inside WFA-CA command
 * 
 * @param[in] str WFA-CA command.
 * @return String containing all the forbidden characters the have been found.
 */
static std::string check_forbidden_chars(const std::string &str)
{
    std::string forbidden_chars_found;
    auto found = str.find_first_of(FORBIDDEN_CHARS);
    while (found != std::string::npos) {
        forbidden_chars_found += str[found];
        found = str.find_first_of(FORBIDDEN_CHARS, found + 1);
    }
    return forbidden_chars_found;
}

/**
 * @brief Checks that the given string is in a hex notation
 * 
 * @param[in] str String to check hex notation.
 * @param[in] expected_octets Number of expected octets (Optional).
 * @return true if string is valid hex notation, else false.
 */
static bool validate_hex_notation(const std::string &str, uint8_t expected_octets = 0)
{
    // Each value must start with 0x
    if (str.substr(0, 2) != "0x") {
        return false;
    }

    // The number of digits must be even
    if (str.size() % 2 != 0) {
        return false;
    }

    // Expected octets validation
    // "str.size() - 2" ignore "0x" prefix.
    // division by 2 because 2 chars are equal to one octet
    if (expected_octets != 0 && (str.size() - 2) / 2 != expected_octets) {
        return false;
    }

    // Check if string has non-hex character
    if (str.find_first_not_of("0123456789abcdefABCDEF", 2) != std::string::npos) {
        return false;
    }

    return true;
}

/**
 * @brief Checks that the given string is in binary notation
 * 
 * @param[in] str String to check binary notation.
 * @return true if string is in valid binary notation, else false.
 */
static bool validate_binary_notation(const std::string &str)
{
    // Each value must start with 0b
    if (str.substr(0, 2) != "0b") {
        return false;
    }

    if ((str.size() - 2) % 8 != 0) {
        return false;
    }

    // Check if string contains not only 0 and 1
    if (str.find_first_not_of("01", 2) != std::string::npos) {
        return false;
    }
    return true;
}

/**
 * @brief Checks that the given string is in decimal notation
 * 
 * @param[in] str String to check decimal notation.
 * @return true if string is in valid decimal notation, else false.
 */
static bool validate_decimal_notation(const std::string &str)
{
    // Each value must start with 0b
    if (str.substr(0, 2) != "0d") {
        return false;
    }

    // Check if string has non-decimal character
    if (str.find_first_not_of("0123456789", 2) != std::string::npos) {
        return false;
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
static std::string parse_bss_info(const std::string &bss_info_str,
                                  db::bss_info_conf_t &bss_info_conf, std::string &err_string)
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
    if (!validate_hex_notation(authentication_type_str, 2)) {
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
    if (!validate_hex_notation(encryption_type_str, 2)) {
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

/**
 * @brief Convert enum of WFA-CA status to string.
 * 
 * @param[in] status Enum of WFA-CA status. 
 * @return Status as string.
 */
const std::string wfa_ca::wfa_ca_status_to_string(eWfaCaStatus status)
{
    switch (status) {
    case eWfaCaStatus::RUNNING:
        return "RUNNING";
    case eWfaCaStatus::INVALID:
        return "INVALID";
    case eWfaCaStatus::ERROR:
        return "ERROR";
    case eWfaCaStatus::COMPLETE:
        return "COMPLETE";
    default:
        LOG(ERROR) << "Status argument doesn't have an enum";
        break;
    }
    return std::string();
}

/**
 * @brief Convert string of WFA-CA command into enum
 * 
 * @param[in] command String of WFA-CA command.
 * @return Command enum. If command is not recognized returns eWfaCaCommand::WFA_CA_COMMAND_MAX.
 */
wfa_ca::eWfaCaCommand wfa_ca::wfa_ca_command_from_string(std::string command)
{
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "CA_GET_VERSION") {
        return eWfaCaCommand::CA_GET_VERSION;
    } else if (command == "DEVICE_GET_INFO") {
        return eWfaCaCommand::DEVICE_GET_INFO;
    } else if (command == "DEV_RESET_DEFAULT") {
        return eWfaCaCommand::DEV_RESET_DEFAULT;
    } else if (command == "DEV_SEND_1905") {
        return eWfaCaCommand::DEV_SEND_1905;
    } else if (command == "DEV_SET_CONFIG") {
        return eWfaCaCommand::DEV_SET_CONFIG;
    } else if (command == "DEV_GET_PARAMETER") {
        return eWfaCaCommand::DEV_GET_PARAMETER;
    }

    return eWfaCaCommand::WFA_CA_COMMAND_MAX;
}

/**
 * @brief Generate WFA-CA reply message and send it.
 * 
 * @param[in] sd Socket to send the reply to.
 * @param[in] cmdu_tx CmduTx message object to create the message with.
 * @param[in] status Status of the reply.
 * @param[in] description Optional, Description to add to the reply.
 * @return true if successful, false if not.
 */
bool wfa_ca::reply(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx, eWfaCaStatus status,
                   const std::string &description)
{
    std::string reply_string = "status," + wfa_ca_status_to_string(status);

    if (!description.empty()) {
        reply_string += ",";
        if (status == eWfaCaStatus::INVALID || status == eWfaCaStatus::ERROR) {
            reply_string += "errorCode,";
        }
        reply_string += description;
    }

    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_WFA_CA_CONTROLLER_RESPONSE>(
            cmdu_tx);

    if (!response) {
        LOG(ERROR) << "Failed building cACTION_BML_WFA_CA_CONTROLLER_RESPONSE message!";
        return false;
    }

    if (!response->set_reply(reply_string)) {
        LOG(ERROR) << "setting reply has failed!";
        return false;
    }

    return message_com::send_cmdu(sd, cmdu_tx);
}

/**
 * @brief Parse the strings vector which containing parameter names, and values into unordered map
 * which will contain the parameter name as a map key, and paramaeter value as map value.
 * The function receives as an input unordered map of parameters that are mandatory, which will
 * include non-mandatory parameters as output.
 * 
 * @param[in] command_tokens A vector containing commands parameters name and values.
 * @param[in,out] params An unordered map containing mandatory parameters on input, and parsed 
 * command as output.
 * @param[out] err_string Contains an error description if the function fails.
 * @return true if successful, false if not.
 */
bool wfa_ca::parse_params(const std::vector<std::string> &command_tokens,
                          std::unordered_map<std::string, std::string> &params,
                          std::string &err_string)
{
    // create a copy of all mandatory param names
    std::list<std::string> mandatory_params;
    for (const auto &param_name : params) {
        mandatory_params.push_back(param_name.first);
    }

    size_t mandatory_params_cnt = 0;
    for (auto it = std::next(command_tokens.begin()); it != command_tokens.end(); it++) {
        std::string param_name = *it;
        std::transform(param_name.begin(), param_name.end(), param_name.begin(), ::tolower);
        if (params.find(param_name) != params.end()) {
            mandatory_params_cnt++;
        }

        if (std::next(it) == command_tokens.end()) {
            err_string = "missing param value for param '" + param_name + "'";
            return false;
        }

        params[param_name] = *std::next(it);
        it++;
    }

    if (mandatory_params_cnt == mandatory_params.size()) {
        return true;
    }

    // Generate an err string if missing mandatory param
    err_string = "missing mandatory params: ";
    for (const std::string &param_name : mandatory_params) {
        if ((params.find(param_name))->second.empty()) {
            err_string += param_name + ", ";
        }
    }

    // Remove last comma and space
    err_string.pop_back();
    err_string.pop_back();

    return false;
}

/**
 * @brief Receives WFA-CA command, preform it and returns a reply to the sender.
 * 
 * @param[in] sd Socket of the sender.
 * @param[in] beerocks_header Beerocks message header.
 * @param[in] cmdu_rx CmduRx Message object, containing the command request.
 * @param[in] cmdu_tx CmduTx Message object of the controller, to be used when replying.
 * @param[in] database Controllers database 
 * @param[in] tasks Controlles tasks pool.
 */
void wfa_ca::handle_wfa_ca_message(
    Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
    task_pool &tasks)
{
    auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_WFA_CA_CONTROLLER_REQUEST>();

    if (!request) {
        LOG(ERROR) << "addClass cACTION_BML_WFA_CA_CONTROLLER_REQUEST failed";
        return;
    }

    std::string command_str(request->command());

    LOG(DEBUG) << "Received ACTION_BML_WFA_CA_CONTROLLER_REQUEST, command=\"" << command_str
               << "\"";

    std::string err_string;

    auto forbidden_chars = check_forbidden_chars(command_str);
    if (!forbidden_chars.empty()) {
        err_string = "command contain illegal chars";
        LOG(ERROR) << err_string << ": " << forbidden_chars;
        reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
        return;
    }

    auto cmd_tokens_vec = string_utils::str_split(command_str, ',');

    if (cmd_tokens_vec.empty()) {
        err_string = "empty command";
        LOG(ERROR) << err_string;
        reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
        return;
    }

    auto &command_type_str = *cmd_tokens_vec.begin();
    auto command_type      = wfa_ca_command_from_string(command_type_str);

    switch (command_type) {
    case eWfaCaCommand::CA_GET_VERSION: {
        // send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        // Send back second reply
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE, std::string("version,") + BEEROCKS_VERSION);
        break;
    }
    case eWfaCaCommand::DEVICE_GET_INFO: {

        // send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        // Send back second reply
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE,
              std::string("vendor,") + database.settings_vendor() + std::string(",model,") +
                  database.settings_model() + std::string(",version,") + BEEROCKS_VERSION);
        break;
    }
    case eWfaCaCommand::DEV_GET_PARAMETER: {

        // For controller certification, the following command is received to get the ALid:
        // DUT (127.0.0.1:5000) ---> dev_get_parameter,program,map,parameter,ALid

        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        std::string alid;
        if (!net::network_utils::linux_iface_get_mac("br-lan", alid))
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, "FAIL");
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE, std::string("aLid,") + alid);
        break;
    }
    case eWfaCaCommand::DEV_RESET_DEFAULT: {
        // NOTE: Note sure this parameters are actually needed. There is a controversial
        // between TestPlan and CAPI specification regarding if these params are required.
        std::unordered_map<std::string, std::string> params{
            // {"name", std::string()},
            // {"program", std::string()},
            // {"devrole", std::string()},
            // {"type", std::string()}
        };

        if (!parse_params(cmd_tokens_vec, params, err_string)) {
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        // Input check
        if (params.find("devrole") != params.end()) {
            if (params["devrole"] != "controller" && params["devrole"] != "agent") {
                err_string = "invalid param value '" + params["devrole"] +
                             "' for param name 'devrole', accepted values can be only 'controller'"
                             " or 'agent";
                LOG(ERROR) << err_string;
                reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
                break;
            }
        }

        if (params.find("program") != params.end()) {
            if (params["program"] != "map") {
                err_string = "invalid param value '" + params["map"] +
                             "' for param name 'program', accepted value can be only 'map'";
                LOG(ERROR) << err_string;
                reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
                break;
            }
        }

        if (params.find("type") != params.end()) {
            if (params["type"] != "test bed" && params["type"] != "DUT") {
                err_string = "invalid param value '" + params["type"] +
                             "' for param name 'type', accepted values can be only 'test bed' or"
                             " 'dut'";
                LOG(ERROR) << err_string;
                reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
                break;
            }
        }

        // TODO: Find out what to do with value of param "name".

        // send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        database.clear_bss_info_configuration();

        // Send back second reply
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE);

        break;
    }
    case eWfaCaCommand::DEV_SEND_1905: {
        std::unordered_map<std::string, std::string> params{{"destalid", std::string()},
                                                            {"messagetypevalue", std::string()}};

        if (!parse_params(cmd_tokens_vec, params, err_string)) {
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        // Input check
        auto &dest_alid = params["destalid"];
        std::transform(dest_alid.begin(), dest_alid.end(), dest_alid.begin(), ::tolower);
        Socket *agent_sd = database.get_node_socket(dest_alid);

        if (agent_sd == nullptr) {
            err_string = "invalid param value '" + params["destalid"] +
                         "' for param name 'DestALID', agent not found";
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        auto &message_type_str = params["messagetypevalue"];
        auto message_type      = (uint16_t)(std::stoul(message_type_str, nullptr, 16));
        if (!ieee1905_1::eMessageTypeValidate::check(message_type)) {
            err_string = "invalid param value '0x" + message_type_str +
                         "' for param name 'MessageTypeValue', message type not found";
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        // TLV check
        std::list<tlv_hex_t> tlv_hex_list;
        if (!get_send_1905_1_tlv_hex_list(tlv_hex_list, params, err_string)) {
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        // Send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        // Check if CMDU message type has preprepared CMDU which can be loaded
        auto certification_tx_buffer = database.get_certification_tx_buffer();
        if (!certification_tx_buffer) {
            LOG(ERROR) << "certification_tx_buffer is not allocated";
            reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
            break;
        }
        ieee1905_1::CmduMessageTx external_cmdu_tx(
            certification_tx_buffer.get() + sizeof(beerocks::message::sUdsHeader),
            message::MESSAGE_BUFFER_LENGTH - sizeof(beerocks::message::sUdsHeader));
        if (create_cmdu(external_cmdu_tx, ieee1905_1::eMessageType(message_type), database,
                        err_string)) {
            // Success
            if (!son_actions::send_cmdu_to_agent(agent_sd, external_cmdu_tx)) {
                LOG(ERROR) << "Failed to send CMDU";
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
                break;
            }

        } else if (!err_string.empty()) {
            // Failure
            LOG(ERROR) << "cmdu creation of type 0x" << message_type_str << ", has failed";
            reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_string);
            break;

        } else {
            // CMDU was not loaded from preprepared buffer, and need to be created manually, and
            // use TLV data from the command (if exists)

            if (!cmdu_tx.create(g_mid, ieee1905_1::eMessageType(message_type))) {
                LOG(ERROR) << "cmdu creation of type 0x" << message_type_str << ", has failed";
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
                break;
            }

            // We need to fill in CmduMessage's class vector. However, we really don't care about
            // the classes, and we don't want any swapping to be done. Still, we need something
            // to make sure the length of the tlvs is taken into account.
            // Therefore, use the tlvPrefilledData class, which can update the end pointer.
            auto tlvs = cmdu_tx.addClass<tlvPrefilledData>();
            if (!tlvs) {
                LOG(ERROR) << "addClass tlvPrefilledData has failed";
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
                break;
            }

            if (!tlvs->add_tlvs_from_list(tlv_hex_list, err_string)) {
                LOG(ERROR) << err_string;
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_string);
                break;
            }

            LOG(INFO) << "Send TLV CMDU " << message_type << " with " << tlv_hex_list.size()
                      << " TLVs, length " << cmdu_tx.getMessageLength();

            if (!son_actions::send_cmdu_to_agent(agent_sd, cmdu_tx)) {
                LOG(ERROR) << "Failed to send CMDU";
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
                break;
            }
        }

        std::string description = "mid,0x" + string_utils::int_to_hex_string(g_mid++, 4);

        // Send back second reply
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE, description);
        break;
    }
    case eWfaCaCommand::DEV_SET_CONFIG: {
        // NOTE: Note sure this parameters are actually needed. There is a controversial
        // between TestPlan and CAPI specification regarding if these params are required.
        std::unordered_map<std::string, std::string> params{
            // {"name", std::string()},
            // {"program", std::string()},
        };

        if (!parse_params(cmd_tokens_vec, params, err_string)) {
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        // Input check
        if (params.find("program") != params.end()) {
            if (params["program"] != "map") {
                err_string = "invalid param value '" + params["map"] +
                             "' for param name 'program', accepted value can be only 'map'";
                LOG(ERROR) << err_string;
                reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
                break;
            }
        }

        if (params.find("backhaul") != params.end()) {
            err_string = "parameter 'backhaul' is relevant only on an agent";
            LOG(ERROR) << err_string;
            reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
            break;
        }

        for (uint8_t bss_info_idx = 1; bss_info_idx < UINT8_MAX; bss_info_idx++) {
            std::string key("bss_info");
            key += std::to_string(bss_info_idx);
            if (params.find(key) == params.end()) {
                if (bss_info_idx == 1) {
                    err_string = "command has no bss_info configuration";
                    LOG(ERROR) << err_string;
                    reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
                    return;
                }
                break;
            }

            db::bss_info_conf_t bss_info_conf;
            auto al_mac = parse_bss_info(params[key], bss_info_conf, err_string);
            if (al_mac.empty()) {
                err_string += (" on " + key);
                LOG(ERROR) << err_string;
                reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_string);
                return;
            }

            database.add_bss_info_configuration(net::network_utils::mac_from_string(al_mac),
                                                bss_info_conf);
        }

        // Send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        // Send back second reply
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE);

        break;
    }
    default: {
        auto err_description = "Invalid WFA-CA command type: '" + command_type_str + "'";
        LOG(ERROR) << err_description;
        reply(sd, cmdu_tx, eWfaCaStatus::INVALID, err_description);
        break;
    }
    }
}

/**
 * @brief Create CMDU manually by loading CMDU from preperared buffer, or getting the data to fill 
 * TLVs from the database.
 * If the function fails but err_string string is empty, then it means that the CMDU was not handled
 * by this function, and the caller will need to create the CMDU by itself.
 * 
 * @param[in,out] cmdu_tx CmduTx message object created from certification_tx_buffer.
 * @param[in] message_type CMDU message type to create.
 * @param[in] database Controllers database.
 * @param[out] err_string Contains an error description if the function fails.
 * @return true if CMDU was successfully loaded, false with empty err_string if CMDU was not loaded,
 * but didn't fail, false with non-empty err_string on failure.
 */
bool wfa_ca::create_cmdu(ieee1905_1::CmduMessageTx &cmdu_tx, ieee1905_1::eMessageType message_type,
                         db &database, std::string &err_string)
{
    std::shared_ptr<ieee1905_1::cCmduHeader> cmdu_header;

    int tlv_type;

    switch (message_type) {
    case ieee1905_1::eMessageType::CHANNEL_SELECTION_REQUEST_MESSAGE: {

        cmdu_header = cmdu_tx.load();
        if (!cmdu_header) {
            LOG(ERROR) << "load cmdu has failed";
            return false;
        }

        tlv_type = cmdu_tx.getNextTlvType();
        while (tlv_type != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
            if (tlv_type == int(wfa_map::eTlvTypeMap::TLV_CHANNEL_PREFERENCE)) {
                if (!cmdu_tx.addClass<wfa_map::tlvChannelPreference>()) {
                    LOG(ERROR) << "addClass tlvChannelPreference has failed";
                    err_string = err_internal;
                    return false;
                }
            } else if (tlv_type == int(wfa_map::eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT)) {
                if (!cmdu_tx.addClass<wfa_map::tlvTransmitPowerLimit>()) {
                    LOG(ERROR) << "addClass tlvTransmitPowerLimit has failed";
                    err_string = err_internal;
                    return false;
                }
            }
            tlv_type = cmdu_tx.getNextTlvType();
        }
        break;
    }
    default:
        // If message type handler is not implemented, then return false with empty error, so the
        // caller would know that it was not handled.
        err_string = std::string();
        return false;
    }

    if (!cmdu_header) {
        err_string = err_internal;
        return false;
    }

    // Force mid
    cmdu_header->message_id() = g_mid;

    return true;
}

/**
 * @brief Parse command "DEV_SEND_1905" TLVs parameters into an ordered list of TLV structure
 * "tlv_hex_t", and does data validation.
 * 
 * @param[in,out] tlv_hex_list An ordered list of TLVs of type "tlv_hex_t".
 * @param[in] params An unordered map containing parsed parameters of a CAPI command. 
 * @param[out] err_string Contains an error description if the function fails.
 * @return true if successful, false if not.
 */
bool wfa_ca::get_send_1905_1_tlv_hex_list(std::list<tlv_hex_t> &tlv_hex_list,
                                          std::unordered_map<std::string, std::string> &params,
                                          std::string &err_string)
{
    static const int max_tlv                          = 256; // Magic number
    size_t tlv_idx                                    = 0;
    bool skip                                         = false;
    bool finish                                       = false;
    static const std::vector<std::string> tlv_members = {"tlv_type", "tlv_length", "tlv_value"};

    do {
        tlv_hex_t tlv_hex;
        for (size_t tlv_member_idx = 0; tlv_member_idx < tlv_members.size(); tlv_member_idx++) {
            std::string lookup_str =
                tlv_members[tlv_member_idx] + (tlv_idx > 0 ? std::to_string(tlv_idx) : "");
            auto it = params.find(lookup_str);
            if (it == params.end()) {
                if (tlv_idx == 0 && tlv_member_idx == 0) {
                    // Skipping TLV with no index (index 0), checking TLV index 1
                    skip = true;
                    break;
                } else if (tlv_idx > 0 && tlv_member_idx == 0) {
                    // No indexed TLVs found, finish
                    finish = true;
                    break;
                } else {
                    err_string = "missing param name '" + lookup_str + "'";
                    return false;
                }
            }

            // Type
            if (tlv_member_idx == 0) {
                tlv_hex.type = &it->second;
                if (!validate_hex_notation(*tlv_hex.type, 1)) {
                    err_string = "param name '" + lookup_str + "' has invalid hex notation value";
                    return false;
                }

                // Length
            } else if (tlv_member_idx == 1) {
                tlv_hex.length = &it->second;
                if (!validate_hex_notation(*tlv_hex.length, 2)) {
                    err_string = "param name '" + lookup_str + "' has invalid hex notation value";
                    return false;
                }

                // Value
            } else if (tlv_member_idx == 2) {
                // The hex values contains '{' and '}' delimiters, but we don't care about them.
                it->second.erase(std::remove(it->second.begin(), it->second.end(), '{'),
                                 it->second.end());
                it->second.erase(std::remove(it->second.begin(), it->second.end(), '}'),
                                 it->second.end());
                tlv_hex.value = &it->second;

                // Validate hex notation on list of values separated by space
                auto values = string_utils::str_split(it->second, ' ');
                for (auto &value : values) {
                    if (!(validate_hex_notation(value) || net::network_utils::is_valid_mac(value) ||
                          validate_binary_notation(value) || validate_decimal_notation(value))) {
                        err_string =
                            "param name '" + lookup_str + "' has value with invalid format";
                        return false;
                    }
                }
            } else {
                LOG(ERROR) << "Illegal tlv_member_idx value: " << int(tlv_member_idx);
                err_string = err_internal;
                return false;
            }
        }

        if (finish) {
            return true;
        }

        tlv_idx++;

        if (skip) {
            skip = false;
            continue;
        }

        tlv_hex_list.push_back(tlv_hex);

    } while (tlv_idx < max_tlv);

    LOG(ERROR) << "Reached stop condition";
    err_string = err_internal;
    return false;
}

/**
 * @brief Writes hex string into the class buffer.
 * 
 * @param[in] value string in hex notation.
 * @param[out] length Length of current tlv.
 * @return false if length smaller than data, else true.
 */
bool tlvPrefilledData::add_tlv_value_hex_string(const std::string &value, uint16_t &length)
{
    // iterate every two chars (1 octet) and write it to buffer
    // start with char_idx = 2 to skip "0x"
    for (size_t char_idx = 2; char_idx < value.size(); char_idx += 2) {
        if (length == 0) {
            return false;
        }
        *m_buff_ptr__ = std::stoi(value.substr(char_idx, 2), nullptr, 16);
        m_buff_ptr__++;
        length--;
    }
    return true;
}

/**
 * @brief Writes a decimal string into the class buffer.
 * 
 * @param[in] value Decimal string.
 * @param[out] length Length of current tlv.
 * @return false if length smaller than data, else true.
 */
bool tlvPrefilledData::add_tlv_value_decimal_string(const std::string &value, uint16_t &length)
{
    // Convert to hex string
    std::stringstream ss;
    size_t num_of_bytes = 1;
    // We don't really know how to identify if it is one or two (or more) bytes.
    // Assume 2 bytes if it has more than 3 digits
    if (value.size() > 5) {
        num_of_bytes++;
    }
    ss << "0x" << std::setw(num_of_bytes * 2) << std::setfill('0') << std::hex
       << std::stoi(value.substr(2, value.length()));

    return add_tlv_value_hex_string(ss.str(), length);
}

/**
 * @brief Writes a binary string into the class buffer.
 * 
 * @param[in] value Binary string.
 * @param[out] length Length of current tlv.
 * @return false if length smaller than data, else true.
 */
bool tlvPrefilledData::add_tlv_value_binary_string(const std::string &value, uint16_t &length)
{
    for (size_t char_idx = 2; char_idx < value.size(); char_idx += 8) {
        if (length == 0) {
            return false;
        }
        *m_buff_ptr__ = std::stoi(value.substr(char_idx, 8), nullptr, 2);
        m_buff_ptr__++;
        length--;
    }
    return true;
}

/**
 * @brief Writes a mac string into the class buffer.
 * 
 * @param[in] value mac address.
 * @param[out] length of current tlv.
 * @return false if length smaller than data, else true.
 */
bool tlvPrefilledData::add_tlv_value_mac(const std::string &value, uint16_t &length)
{
    for (size_t char_idx = 0; char_idx < value.size(); char_idx += 3) {
        if (length == 0) {
            return false;
        }
        *m_buff_ptr__ = std::stoi(value.substr(char_idx, 2), nullptr, 16);
        m_buff_ptr__++;
        length--;
    }
    return true;
}

/**
 * @brief Writes TLVs from TLVs list 'tlv_hex_list' into the class buffer.
 * 
 * @param[in] tlv_hex_list An ordered list of TLVs of type "tlv_hex_t".
 * @param[out] err_string Contains an error description if the function fails.
 * @return true if successful, false if not.
 */
bool tlvPrefilledData::add_tlvs_from_list(std::list<wfa_ca::tlv_hex_t> &tlv_hex_list,
                                          std::string &err_string)
{
    for (const auto &tlv : tlv_hex_list) {

        uint8_t type = std::stoi(*tlv.type, nullptr, 16);

        uint16_t length = std::stoi(*tlv.length, nullptr, 16);

        // "+3" = size of type and length fields
        if (getBuffRemainingBytes() < unsigned(length + 3)) {
            err_string = "not enough space on buffer";
            return false;
        }

        *m_buff_ptr__ = type;
        m_buff_ptr__++;
        *m_buff_ptr__ = uint8_t(length >> 8);
        m_buff_ptr__++;
        *m_buff_ptr__ = uint8_t(length);
        m_buff_ptr__++;

        auto values = string_utils::str_split(*tlv.value, ' ');
        for (auto value : values) {
            //Do conversion if needed
            if (value[1] == 'x') {
                if (!add_tlv_value_hex_string(value, length)) {
                    err_string = "length smaller than data";
                    return false;
                }
            } else if (value[2] == ':') {
                if (!add_tlv_value_mac(value, length)) {
                    err_string = "length smaller than data";
                    return false;
                }
            } else if (value[1] == 'd') {
                if (!add_tlv_value_decimal_string(value, length)) {
                    err_string = "length smaller than data";
                    return false;
                }
            } else if (value[1] == 'b') {
                if (!add_tlv_value_binary_string(value, length)) {
                    err_string = "length smaller than data";
                    return false;
                }
            }
        }

        if (length != 0) {
            err_string = "data smaller than length";
            return false;
        }
    }
    return true;
}
