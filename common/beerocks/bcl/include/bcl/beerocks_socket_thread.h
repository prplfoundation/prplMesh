/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
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

#include <chrono>

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
    virtual bool skip_filtered_message_type(ieee1905_1::eMessageType msg_type) { return false; }
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

    void skip_next_select_timeout();

    // This function needs to be used on code parts in before/after select functions, where we
    // suspect that its operation might take a long time (> select_timeout) and will not finish the
    // thread operation before the select timeout should have been expired if the thread had ideal
    // operation time of zero.
    std::chrono::steady_clock::time_point awake_timeout()
    {
        // Set the awake timeout to 50% of the select timeout to create a buffer of 50%
        // to let the thread finish its work until getting to Select again.
        return m_select_wake_up_time + std::chrono::milliseconds(int(0.50 * m_select_timeout_msec));
    }

    ieee1905_1::CmduMessageTx cmdu_tx;
    ieee1905_1::CmduMessageTx cert_cmdu_tx;
    const std::string unix_socket_path;

private:
    // Only mapf_socket thread need to access these functions and memebers, other classes should not
    // TODO: redesign the socket thread in order to not use "friend" - Vitaly
    friend class btl::transport_socket_thread;

    int socket_disconnected_uds(Socket *sd);
    bool handle_cmdu_message_uds(Socket *sd);
    bool verify_cmdu(message::sUdsHeader *uds_header);

    uint8_t rx_buffer[message::MESSAGE_BUFFER_LENGTH];
    uint8_t tx_buffer[message::MESSAGE_BUFFER_LENGTH];
    uint8_t cert_tx_buffer[message::MESSAGE_BUFFER_LENGTH];
    ieee1905_1::CmduMessageRx cmdu_rx;

    std::unique_ptr<SocketServer> server_socket;
    std::vector<Socket *> sockets;

    ///////////////////////////////////////////////

    int server_max_connections;
    SocketSelect select;

    uint32_t m_select_timeout_msec                              = 0;
    bool m_skip_next_select_timeout                             = false;
    std::chrono::steady_clock::time_point m_select_wake_up_time = std::chrono::steady_clock::now();
};

} // namespace beerocks

#endif //_BEEROCKS_SOCKET_THREAD_H_
