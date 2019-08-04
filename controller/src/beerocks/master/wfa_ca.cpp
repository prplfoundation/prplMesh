
#include "wfa_ca.h"

#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>

#include <easylogging++.h>

#include <algorithm>

#define FORBIDDEN_CHARS "$#&*()[]{};/\\<>?|`~=+"

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
            err_string += "'" + param_name + "', ";
        }
    }

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
        // TODO
        break;
    }
    case eWfaCaCommand::DEVICE_GET_INFO: {
        // TODO
        break;
    }
    case eWfaCaCommand::DEV_GET_PARAMETER: {
        // TODO
        break;
    }
    case eWfaCaCommand::DEV_RESET_DEFAULT: {

        /* 
        * This command is used to reset the device to its default program specific configuration, 
        * as well as remove any cached profiles, keys and credentials.
        *
        * Parameters:
        * 
        * Param Name    | Values                        | Description
        * ----------------------------------------------------------------------------------------
        * "devrole"     | string ("controller"/"agent") | Device role    
        * "name"        | string                        | Device name    
        * "program"     | string ("map")                | Program name
        * "type"        | string ("test bed"/"dut")     | Type of the device
        * 
        * Return Values: 
        *   None.
        *
        * Example:
        *   UCC: dev_reset_default,
        *   CA:status,RUNNING
        *   CA:status,COMPLETE
        * 
        *   UCC: dev_reset_default,
        *   CA:status,RUNNING
        *   CA:status,COMPLETE
        */

        // NOTE: Note sure this parameters are actually needed. There is a controversial
        // between TestPlan and CAPI specification regarding if these params are required.
        static std::unordered_map<std::string, std::string> params{
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
            if (params["type"] != "test bed" && params["type"] != "dut") {
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
        //TODO
    }
    case eWfaCaCommand::DEV_SET_CONFIG: {
        /*
        * This command is used to configure the device with its configuration parameters.
        * 
        * Parameters:
        * 
        * Param Name    | Values                        | Description
        * ----------------------------------------------------------------------------------------
        * "backhaul"    | string:                       | Set a single backhual interface to 
        *               |  ruid (ex: 11:22:33:44:55:66) | Ethernet or Wi-Fi. For Wi-Fi, a RUID 
        *               |  eth                          | associated with its frequency band 
        *               |                               | (2.4GHz, 5GHz Low, 5GHz High) is provided.
        *               |                               | For Ethernet, the device should return 
        *               |                               | status,COMPLETE until it completes 
        *               |                               | Ethernet onboarding.  
        *               |                               |
        * "bss_info1"   | string:                       | BSS initialization data used by controller
        *               |   WTS_REPLACE_DEST_ALID       | to configure agent's fronthaul radio.
        *               |   8x Multi-AP-2G-1 0x0020     | The value is a list of fields separated by
        *               |   0x0008 maprocks1 0 1        | space that stands for:
        *               |                               | WTS_REPLACE_DEST_ALID, operating class,
        *               |                               | SSID, authentication type (WPA2-PSK),
        *               |                               | encryption type(AES-CCMP), network key, 
        *               |                               | bit 6 of Multi-AP IE's extension
        *               |                               | attribute, bit 5 of Multi-AP IE's,
        *               |                               | bit 5 of Multi-AP IE's extension
        *               |                               | attribute. To clear the BSS info stored
        *               |                               | a specific operating class, only the
        *               |                               | device ALID and operating class are 
        *               |                               | retained.
        *               |                               |
        * "bss_info2"   | string                        |  
        * "bss_infoN"   | string                        |  
        * "name"        | string (ex: "dev1")           | Device name
        * "program"     | string ("map)                 | Program name
        * 
        * Return values:
        *   None.
        * 
        * 
        * Example:
        *   UCC: dev_set_config,name,agt1,program,map,backhaul,0x00904C2A11B7
        *   CA:status,RUNNING
        *   CA:status,COMPLETE
        */

        // NOTE: Note sure this parameters are actually needed. There is a controversial
        // between TestPlan and CAPI specification regarding if these params are required.
        static std::unordered_map<std::string, std::string> params{
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
