/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_MAPF_SOCKET_THREAD_H_
#define _BEEROCKS_MAPF_SOCKET_THREAD_H_

#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_socket_thread.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <tlvf/ieee_1905_1/eMessageType.h>

// Forward Declarations
namespace mapf {
class Poller;
class LocalBusInterface;
class SubSocket;
} // namespace mapf

namespace beerocks {
namespace btl {
class transport_socket_thread : public socket_thread {
public:
    transport_socket_thread(const std::string &unix_socket_path_ = std::string());
    virtual ~transport_socket_thread();

    virtual bool init() override;
    virtual void set_select_timeout(unsigned msec) override;
    virtual bool work() override;

protected:
    void add_socket(Socket *s, bool add_to_vector = true) override;
    void remove_socket(Socket *s) override;
    bool read_ready(Socket *s) override;
    bool configure_ieee1905_transport_interfaces(const std::string &bridge_iface,
                                                 const std::vector<std::string> &ifaces);
    bool from_bus(Socket *sd);

    bool bus_subscribe(const std::vector<ieee1905_1::eMessageType> &msg_types);
    bool bus_connect(const std::string &beerocks_temp_path, const bool local_master);
    void bus_connected(Socket *sd);

    bool send_cmdu_to_bus(ieee1905_1::CmduMessageTx &cmdu, const std::string &dst_mac,
                          const std::string &src_mac);
    bool send_cmdu_to_bus(ieee1905_1::CmduMessage &cmdu, const std::string &dst_mac,
                          const std::string &src_mac, uint16_t length);

    static const std::string MULTICAST_MAC_ADDR;

private:
    bool bus_init();
    bool bus_send(ieee1905_1::CmduMessage &cmdu, const std::string &dst_mac,
                  const std::string &src_mac, uint16_t length);
    bool handle_cmdu_message_bus();

    int poll_timeout_ms = 500;

#ifdef UDS_BUS
    Socket *bus = nullptr;
    std::unique_ptr<SocketServer> bus_server_socket;
#else
    std::shared_ptr<mapf::LocalBusInterface> bus = nullptr;
    std::shared_ptr<mapf::Poller> poller         = nullptr;
#endif
};
} // namespace btl

} // namespace beerocks

#endif //_BEEROCKS_MAPF_SOCKET_THREAD_H_
