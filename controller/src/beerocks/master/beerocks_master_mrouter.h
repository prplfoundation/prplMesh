/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MASTER_MESSAGE_ROUTER_H__
#define __MASTER_MESSAGE_ROUTER_H__

#include <btl.h>
#include <beerocks/tlvf/beerocks_message.h>
#include <list>

namespace beerocks {
class master_mrouter : public btl::transport_socket_thread {
public:
    master_mrouter(const std::string &mrouter_uds_, const std::string &master_uds_,
                   const std::string &bridge_iface_name);
    ~master_mrouter();

    virtual bool init() override;

protected:
    virtual int server_port() override;
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void on_thread_stop() override;
    virtual void socket_connected(Socket *sd) override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual void after_select(bool timeout) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    typedef struct {
        SocketClient *master = nullptr;
        std::string slave_bridge_mac;
    } sMasterSockets;

    std::string local_bridge_mac;

    std::list<std::shared_ptr<sMasterSockets>> master_sockets;
    std::string master_uds;
};

} //namespace beerocks

#endif // __master_MESSAGE_ROUTER_FACTORY_H__
