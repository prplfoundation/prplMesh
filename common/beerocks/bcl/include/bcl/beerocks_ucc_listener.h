/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __BEEROCKS_UCC_LISTENER_H__
#define __BEEROCKS_UCC_LISTENER_H__

#include <bcl/beerocks_socket_thread.h>

#include <list>
#include <string>
#include <unordered_map>

namespace beerocks {

// Defined by CAPI specifications:
static constexpr uint8_t UCC_REPLY_RUNNING_TIMEOUT_SEC  = 1;
static constexpr uint8_t UCC_REPLY_COMPLETE_TIMEOUT_SEC = 120;

class beerocks_ucc_listener : public socket_thread {
public:
    beerocks_ucc_listener(uint16_t port, ieee1905_1::CmduMessageTx &cmdu);
    ~beerocks_ucc_listener(){};

protected:
    bool init() override;

    // Helper functions
    static std::string check_forbidden_chars(const std::string &str);
    static bool validate_hex_notation(const std::string &str, uint8_t expected_octets = 0);
    static bool validate_binary_notation(const std::string &str);
    static bool validate_decimal_notation(const std::string &str);

    // Virtual functions
    virtual void lock()                                                                = 0;
    virtual void unlock()                                                              = 0;
    virtual std::string fill_version_reply_string()                                    = 0;
    virtual void clear_configuration()                                                 = 0;
    virtual bool validate_destination_alid(const std::string &dest_alid)               = 0;
    virtual bool send_cmdu_to_destination(ieee1905_1::CmduMessageTx &cmdu_tx,
                                          const std::string &dest_mac = std::string()) = 0;
    virtual bool handle_dev_set_config(std::unordered_map<std::string, std::string> &params,
                                       std::string &err_string)                        = 0;
    virtual bool handle_dev_get_param(std::unordered_map<std::string, std::string> &params,
                                      std::string &value)                              = 0;

    enum class eUccListenerRunOn : uint8_t {
        CONTROLLER,
        AGENT,
        NONE,
    };
    eUccListenerRunOn m_ucc_listener_run_on = eUccListenerRunOn::NONE;

private:
    // socket thread functions
    int server_port() override;
    void socket_connected(Socket *sd) override;
    bool socket_disconnected(Socket *sd) override;
    bool custom_message_handler(Socket *sd, uint8_t *rx_buffer, size_t rx_buffer_size) override;

    void before_select() override { unlock(); }
    void after_select(bool timeout) override { lock(); }

    // not need this function but it is pure virtual so must be overridden.
    bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override { return true; }

    // Helper functions
    enum class eWfaCaCommand : uint8_t {
        CA_GET_VERSION,
        DEVICE_GET_INFO,
        DEV_GET_PARAMETER,
        DEV_RESET_DEFAULT,
        DEV_SEND_1905,
        DEV_SET_CONFIG,
        WFA_CA_COMMAND_MAX,
    };
    static eWfaCaCommand wfa_ca_command_from_string(std::string command);

    enum class eWfaCaStatus : uint8_t { RUNNING, INVALID, ERROR, COMPLETE };
    static const std::string wfa_ca_status_to_string(eWfaCaStatus status);

    static bool parse_params(const std::vector<std::string> &command_tokens,
                             std::unordered_map<std::string, std::string> &params,
                             std::string &err_string);

    struct tlv_hex_t {
        std::string *type   = nullptr;
        std::string *length = nullptr;
        std::string *value  = nullptr;
    };
    static bool get_send_1905_1_tlv_hex_list(std::list<tlv_hex_t> &tlv_hex_list,
                                             std::unordered_map<std::string, std::string> &params,
                                             std::string &err_string);

    // Class functions
    void handle_wfa_ca_command(const std::string &command);
    bool reply_ucc(eWfaCaStatus status, const std::string &description = std::string());

    friend class tlvPrefilledData;

    // Variables
    ieee1905_1::CmduMessageTx &m_cmdu_tx;
    Socket *m_ucc_sd = nullptr;
    uint16_t m_port;
};

class tlvPrefilledData : public BaseClass {
public:
    tlvPrefilledData(uint8_t *buff, size_t buff_len, bool parse = false)
        : BaseClass(buff, buff_len, parse)
    {
        m_init_succeeded = true;
    };
    tlvPrefilledData(std::shared_ptr<BaseClass> base, bool parse = false)
        : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse)
    {
        m_init_succeeded = true;
    };
    ~tlvPrefilledData(){};

    // No swapping is needed for a prefilled TLV list
    void class_swap() override{};
    // No finalize is needed for a prefilled TLV list
    bool finalize() override { return true; };
    static size_t get_initial_size() { return 0; };

    bool add_tlv_value_hex_string(const std::string &value, uint16_t &length);
    bool add_tlv_value_decimal_string(const std::string &value, uint16_t &length);
    bool add_tlv_value_binary_string(const std::string &value, uint16_t &length);
    bool add_tlv_value_mac(const std::string &value, uint16_t &length);
    bool add_tlvs_from_list(std::list<beerocks_ucc_listener::tlv_hex_t> &tlv_hex_list,
                            std::string &err_string);
};
} // namespace beerocks

#endif // __BEEROCKS_UCC_LISTENER_H__
