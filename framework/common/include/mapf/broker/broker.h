/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_BROKER_H__
#define __MAPF_BROKER_H__

#include <iostream>
#include <mapf/broker/broker_config.h>
#include <mapf/common/config.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>

namespace mapf {
struct msglib_socket;

enum class BrokerSocket : int { FRONTEND, BACKEND };

class Broker {
public:
    Broker();

    explicit Broker(const std::string &cfg);
    Broker(const Broker &) = delete;
    Broker &operator=(const Broker &) = delete;

    ~Broker();

    void Bind(BrokerSocket socket_type, const std::string &endpoint);
    void Run();
    void PrintConfig();

private:
    void Init(const std::string &cfg);
    void Bind();

    BrokerConfig config_;

protected:
    msglib_socket *frontend_;
    msglib_socket *backend_;
    void *context_;
};

} /* namespace mapf */

#endif /* __MAPF_BROKER_H__ */
