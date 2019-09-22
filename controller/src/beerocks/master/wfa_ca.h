/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _WFA_CA_H_
#define _WFA_CA_H_

#include "db/db.h"
#include "tasks/task_pool.h"

#include <unordered_map>

using namespace son;

class wfa_ca {
public:
    static void
    handle_wfa_ca_message(Socket *sd,
                          std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
                          ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx,
                          db &database, task_pool &tasks);

private:
    enum class eWfaCaStatus : uint8_t { RUNNING, INVALID, ERROR, COMPLETE };
    static const std::string wfa_ca_status_to_string(eWfaCaStatus status);

    enum class eWfaCaCommand : uint8_t {
        CA_GET_VERSION,
        DEVICE_GET_INFO,
        DEV_GET_PARAMETER,
        DEV_RESET_DEFAULT,
        DEV_SEND_1905,
        DEV_SET_CONFIG,
        WFA_CA_COMMAND_MAX
    };
    static eWfaCaCommand wfa_ca_command_from_string(std::string command);

    static bool reply(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx, eWfaCaStatus status,
                      const std::string &description = std::string());

    static bool parse_params(const std::vector<std::string> &command_tokens,
                             std::unordered_map<std::string, std::string> &params,
                             std::string &err_string);

    static bool create_cmdu(ieee1905_1::CmduMessageTx &cmdu_tx,
                            ieee1905_1::eMessageType message_type, db &database,
                            std::string &err_string);

    struct tlv_hex_t {
        std::string *type   = nullptr;
        std::string *length = nullptr;
        std::string *value  = nullptr;
    };
    static bool get_send_1905_1_tlv_hex_list(std::list<tlv_hex_t> &tlv_hex_list,
                                             std::unordered_map<std::string, std::string> &params,
                                             std::string &err_string);

    friend class tlvPrefilledData;
};

class tlvPrefilledData : public BaseClass {
public:
    tlvPrefilledData(uint8_t *buff, size_t buff_len, bool parse = false, bool swap_needed = false)
        : BaseClass(buff, buff_len, parse, swap_needed)
    {
        m_init_succeeded = true;
    };
    tlvPrefilledData(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false)
        : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
    {
        m_init_succeeded = true;
    };
    ~tlvPrefilledData(){};

    // No swapping is needed for a prefilled TLV list
    void class_swap(){};
    static size_t get_initial_size() { return 0; };

    bool add_tlv_value_hex_string(const std::string &value, uint16_t &length);
    bool add_tlv_value_decimal_string(const std::string &value, uint16_t &length);
    bool add_tlv_value_binary_string(const std::string &value, uint16_t &length);
    bool add_tlv_value_mac(const std::string &value, uint16_t &length);
    bool add_tlvs_from_list(std::list<wfa_ca::tlv_hex_t> &tlv_hex_list, std::string &err_string);
};

#endif
