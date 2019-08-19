
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

static bool is_hex_notation(const std::string &s)
{
    return s.compare(0, 2, "0x") == 0 && s.size() > 2 &&
           s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
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
        if ((*std::next(it))[0] == '{') {
            int len_command_str     = (*std::next(it)).length();
            bool prev_space         = false;
            bool prev_curly_bracket = false;
            int count_open_brackets = 0;
            std::string command_str = *std::next(it);
            for (int i = 0; i < len_command_str; i++) {
                auto c = command_str[i];
                if (c == '{') {
                    prev_curly_bracket = true;
                    prev_space         = false;
                    count_open_brackets++;
                    continue;
                }
                if (count_open_brackets == 0) {
                    err_string = "Unbalanced curly brackets in value of param " + param_name + "'";
                    return false;
                } else if (c == '}') {
                    count_open_brackets--;
                    if (count_open_brackets < 0) {
                        err_string =
                            "Unbalanced curly brackets in value of param " + param_name + "'";
                        return false;
                    }
                    prev_space         = false;
                    prev_curly_bracket = false;
                } else if (c == ' ') {
                    prev_space         = true;
                    prev_curly_bracket = false;
                } else {
                    if (prev_space || prev_curly_bracket) {
                        auto token = command_str.substr(i, command_str.find_first_of(" }", i) - i);
                        token.pop_back();
                        //Check if a given string conforms to hex notation.
                        if (is_hex_notation(token) == false) {
                            err_string =
                                "hex notation test failed in value of param " + param_name + "'";
                            return false;
                        }
                        i                  = i + token.length() - 1;
                        prev_space         = false;
                        prev_curly_bracket = false;
                    }
                }
            }
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

        // TODO: Perfrom reset to "name". Currently not needed.
        // NOTE: This action is an asynchronous procedure and will be required to be executed
        //       from task infrastructure.

        // Send back second reply
        reply(sd, cmdu_tx, eWfaCaStatus::COMPLETE);

        break;
    }
    case eWfaCaCommand::DEV_SEND_1905: {
        std::unordered_map<std::string, std::string> params{{"destalid", std::string()},
                                                            {"messagetypevalue", std::string()}};

        const auto mandatory_params_num = params.size();

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

        // Send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        // Check if additional tlvs were added to command. If yes, then needs to parse the tlv and
        // send it as it.
        if (mandatory_params_num < params.size()) {

            if (!cmdu_tx.create(g_mid, ieee1905_1::eMessageType(message_type))) {
                LOG(ERROR) << "cmdu creation of type 0x" << message_type_str << ", has failed";
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
                break;
            }

            // TODO: Add the TLVs from the command

            son_actions::send_cmdu_to_agent(agent_sd, cmdu_tx);

            // If not using TLVs from the command, create CMDU from existing buffer
        } else {
            auto certification_tx_buffer = database.get_certification_tx_buffer();
            if (!certification_tx_buffer) {
                LOG(ERROR) << "certification_tx_buffer is not allocated";
            }
            ieee1905_1::CmduMessageTx certification_cmdu_tx(
                certification_tx_buffer.get() + sizeof(beerocks::message::sUdsHeader),
                message::MESSAGE_BUFFER_LENGTH - sizeof(beerocks::message::sUdsHeader));

            if (!create_cmdu(certification_cmdu_tx, ieee1905_1::eMessageType(message_type))) {
                LOG(ERROR) << "Failed to create CMDU";
                reply(sd, cmdu_tx, eWfaCaStatus::ERROR, err_internal);
                break;
            }

            if (!son_actions::send_cmdu_to_agent(agent_sd, certification_cmdu_tx)) {
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

        // NOTE: Uncertainty if param names: "backhaul", "bss_info{1..N}" are mandatory

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

        // TODO: Find out what to do with value of params: "backhaul", bss_info{1..N}, "name".

        // Send back first reply
        if (!reply(sd, cmdu_tx, eWfaCaStatus::RUNNING)) {
            LOG(ERROR) << "failed to send reply";
            break;
        }

        // TODO: Configure device "name". Currently not needed.
        // NOTE: This action is an asynchronous procedure and will be required to be executed
        //       from task infrastructure.

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
 * @brief Create CMDU to send when receiving WFA-CA command "DEV_SEND_1905".
 * This function may create the CMDU from scratch or use the certification buffer which loaded with
 * preperared from advance.
 * 
 * @param[in,out] cmdu_tx CmduTx message object created from certification_tx_buffer.
 * @param[in] message_type: CMDU message type to create. 
 * @return true if successful, false if not.
 */
bool wfa_ca::create_cmdu(ieee1905_1::CmduMessageTx &cmdu_tx, ieee1905_1::eMessageType message_type)
{
    std::shared_ptr<ieee1905_1::cCmduHeader> cmdu_header;

    int tlv_type;

    switch (message_type) {
    case ieee1905_1::eMessageType::TOPOLOGY_QUERY_MESSAGE: {
        cmdu_header = cmdu_tx.create(g_mid, ieee1905_1::eMessageType::TOPOLOGY_QUERY_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        break;
    }
    case ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE: {

        cmdu_header =
            cmdu_tx.create(g_mid, ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        break;
    }
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
                    return false;
                }
            } else if (tlv_type == int(wfa_map::eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT)) {
                if (!cmdu_tx.addClass<wfa_map::tlvTransmitPowerLimit>()) {
                    LOG(ERROR) << "addClass tlvTransmitPowerLimit has failed";
                    return false;
                }
            }
            tlv_type = cmdu_tx.getNextTlvType();
        }
        break;
    }
    default:
        LOG(ERROR) << "Missing handler for message_type=" << std::hex << int(message_type);
        break;
    }

    if (!cmdu_header) {
        return false;
    }

    // Force mid
    cmdu_header->message_id() = g_mid;

    return true;
}
