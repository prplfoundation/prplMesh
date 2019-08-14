/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_SOCKET_THREAD_H_
#define _BEEROCKS_SOCKET_THREAD_H_

#include "beerocks_message_structs.h"
#include "beerocks_thread_base.h"
#include "network/socket.h"

#include <tlvf/CmduMessageRx.h>
#include <tlvf/CmduMessageTx.h>

#define DEFAULT_SELECT_TIMEOUT_MS 500

namespace beerocks {
// Forward Declaration
namespace btl {
class transport_socket_thread;
}

class socket_thread : public thread_base {
public:
    socket_thread(const std::string &unix_socket_path_ = std::string());
    virtual ~socket_thread();
    void set_server_max_connections(int connections);
    virtual void set_select_timeout(unsigned msec);

    virtual bool init() override;
    virtual bool work() override;

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) = 0;
    virtual bool custom_message_handler(Socket *sd, uint8_t *rx_buffer, size_t rx_buffer_size)
    {
        return false;
    }
    virtual std::string print_cmdu_types(const message::sUdsHeader *cmdu_header)
    {
        return std::string();
    }
    virtual void before_select() {}
    virtual void after_select(bool timeout) {}
    virtual void socket_connected(Socket *sd);
    virtual bool socket_disconnected(Socket *sd) = 0;
    virtual int server_port() { return -1; }

    virtual void add_socket(Socket *s, bool add_to_vector = true) { select.addSocket(s); }
    virtual void remove_socket(Socket *s) { select.removeSocket(s); }
    inline void clear_ready(Socket *s) { select.clearReady(s); }
    virtual bool read_ready(Socket *s) { return select.readReady(s); }

    ieee1905_1::CmduMessageTx cmdu_tx;

private:
    // Only mapf_socket thread need to access these functions and memebers, other classes should not
    // TODO: redesign the socket thread in order to not use "friend" - Vitaly
    friend class btl::transport_socket_thread;

    int socket_disconnected_uds(Socket *sd);
    bool handle_cmdu_message_uds(Socket *sd);
    bool verify_cmdu(message::sUdsHeader *uds_header);

    std::string unix_socket_path;
    uint8_t rx_buffer[message::MESSAGE_BUFFER_LENGTH];
    uint8_t tx_buffer[message::MESSAGE_BUFFER_LENGTH];
    ieee1905_1::CmduMessageRx cmdu_rx;

    std::unique_ptr<SocketServer> server_socket;
    std::vector<Socket *> sockets;

    ///////////////////////////////////////////////

    int server_max_connections;
    SocketSelect select;
};

} // namespace beerocks

#endif //_BEEROCKS_SOCKET_THREAD_H_
