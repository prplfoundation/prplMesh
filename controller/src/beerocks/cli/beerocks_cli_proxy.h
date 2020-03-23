/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __CLI_PROXY_H__
#define __CLI_PROXY_H__

#include <bcl/beerocks_socket_thread.h>
#include <beerocks/tlvf/beerocks_message.h>

namespace beerocks {
class cli_proxy : public socket_thread {
public:
    cli_proxy(std::string master_uds_);
    ~cli_proxy();

    virtual bool init() override;

protected:
    virtual int server_port() override;
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void on_thread_stop() override;
    virtual void socket_connected(Socket *sd) override;
    virtual bool socket_disconnected(Socket *sd) override;

private:
    typedef struct {
        Socket *cli_client   = nullptr;
        SocketClient *master = nullptr;
    } sMasterSockets;

    std::vector<sMasterSockets *> master_sockets;
    std::string master_uds;

    void release_sockets();
};
} // namespace beerocks

#endif // __CLI_PROXY_H__
