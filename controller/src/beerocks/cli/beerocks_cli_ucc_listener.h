/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __CLI_UCC_LISTENER_H__
#define __CLI_UCC_LISTENER_H__

#include "../bml/bml_defs.h"
#include <beerocks/bcl/beerocks_socket_thread.h>

#include <mutex>

namespace beerocks {
class cli_ucc_listener : public socket_thread {
public:
    cli_ucc_listener(const std::string &beerocks_conf_path, uint16_t port);
    ~cli_ucc_listener();

    virtual bool init() override;

private:
    virtual int server_port() override;
    virtual void socket_connected(Socket *sd) override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual bool custom_message_handler(Socket *sd, uint8_t *rx_buffer,
                                        size_t rx_buffer_size) override;

    // not need this function but it is pure virtual so must be overridden.
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override
    {
        return true;
    }

    static void wfa_ca_reply_to_ucc_cb(BML_CTX ctx, const char *buffer);

    bool connect();
    void disconnect();
    void reply_ucc(const char *buffer);

    // Variable
    std::string m_beerocks_conf_path;
    BML_CTX m_ctx    = nullptr;
    Socket *m_ucc_sd = nullptr;
    std::mutex m_ucc_sd_mutex;
    uint16_t m_port;

    // bool open_socket_server(in_addr_t addr, uint16_t port);
    // void close_socket_server();
};
} // namespace beerocks

#endif // __CLI_UCC_LISTENER_H__
