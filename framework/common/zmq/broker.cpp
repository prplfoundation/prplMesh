/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "msglib.h"
#include <mapf/broker/broker.h>
#include <zmq.h>

namespace mapf {

Broker::Broker() : frontend_(new msglib_socket), backend_(new msglib_socket)
{
    context_ = zmq_ctx_new();
    errno_assert(context_);

    frontend_->sd_ = zmq_socket(context_, ZMQ_XSUB);
    errno_assert(frontend_->sd_);

    backend_->sd_ = zmq_socket(context_, ZMQ_XPUB);
    errno_assert(backend_->sd_);
}

Broker::Broker(const std::string &cfg) : Broker() { Init(cfg); }

Broker::~Broker()
{
    int rc = zmq_close(frontend_->sd_);
    errno_assert(rc == 0);
    rc = zmq_close(backend_->sd_);
    errno_assert(rc == 0);
    rc = zmq_ctx_destroy(context_);
    errno_assert(rc == 0);
    delete frontend_;
    delete backend_;
}

void Broker::PrintConfig()
{
    MAPF_INFO("Borker configuration");
    config_.Print();
}

void Broker::Init(const std::string &cfg)
{
    config_ = std::move(BrokerConfig(cfg));
    config_.Parse();
    Bind();
}

void Broker::Bind()
{
    for (auto &frontend : config_.frontend)
        Bind(BrokerSocket::FRONTEND, frontend);
    for (auto &backend : config_.backend)
        Bind(BrokerSocket::BACKEND, backend);
}

void Broker::Bind(BrokerSocket socket_type, const std::string &endpoint)
{
    int rc;

    switch (socket_type) {
    case BrokerSocket::FRONTEND:
        if (config_.security.enable) {
            rc = zmq_setsockopt(frontend_->sd_, ZMQ_CURVE_SERVERKEY,
                                config_.security.pub_public.c_str(), config_.security.key_len);
            errno_assert(rc == 0);
            rc = zmq_setsockopt(frontend_->sd_, ZMQ_CURVE_PUBLICKEY,
                                config_.security.xsub_public.c_str(), config_.security.key_len);
            errno_assert(rc == 0);
            rc = zmq_setsockopt(frontend_->sd_, ZMQ_CURVE_SECRETKEY,
                                config_.security.xsub_secret.c_str(), config_.security.key_len);
            errno_assert(rc == 0);
        }
        if (config_.verbose)
            MAPF_DBG("bind frontend: " + endpoint);
        config_.frontend.insert(endpoint);
        rc = zmq_bind(frontend_->sd_, endpoint.c_str());
        errno_assert(rc == 0);
        break;
    case BrokerSocket::BACKEND:
        if (config_.security.enable) {
            int as_server = 1;
            rc = zmq_setsockopt(backend_->sd_, ZMQ_CURVE_SERVER, &as_server, sizeof(int));
            errno_assert(rc == 0);
            rc = zmq_setsockopt(backend_->sd_, ZMQ_CURVE_SECRETKEY,
                                config_.security.xpub_secret.c_str(), config_.security.key_len);
            errno_assert(rc == 0);
        }
        if (config_.verbose)
            MAPF_DBG("bind backend: " + endpoint);
        config_.backend.insert(endpoint);
        rc = zmq_bind(backend_->sd_, endpoint.c_str());
        errno_assert(rc == 0);
        break;
    default:
        errno_assert(0);
    }
}

void Broker::Run()
{
    //Sanity
    char endpoint[128];
    size_t len = sizeof(endpoint);

    zmq_getsockopt(frontend_->sd_, ZMQ_LAST_ENDPOINT, endpoint, &len);
    if (std::string(endpoint) == "") {
        MAPF_ERR("broker '" + config_.name + "' frontend socket not binded!");
        errno_assert(0);
    }

    len = sizeof(endpoint);
    zmq_getsockopt(backend_->sd_, ZMQ_LAST_ENDPOINT, endpoint, &len);
    if (std::string(endpoint) == "") {
        MAPF_ERR("broker '" + config_.name + "' backend socket not binded!");
        errno_assert(0);
    }

    // Start broker
    if (config_.verbose)
        MAPF_DBG("start proxy (secure=" << std::boolalpha << config_.security.enable << ")");
    zmq_proxy(frontend_->sd_, backend_->sd_, nullptr);
}

} // namespace mapf
