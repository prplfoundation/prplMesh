/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/network_utils.h>
#include <btl/btl.h>
#include <easylogging++.h>

using namespace beerocks::btl;
using namespace beerocks::net;

transport_socket_thread::transport_socket_thread(const std::string &unix_socket_path_)
    : socket_thread(unix_socket_path_)
{
    transport_socket_thread::set_select_timeout(DEFAULT_SELECT_TIMEOUT_MS);
}

transport_socket_thread::~transport_socket_thread() {}

bool transport_socket_thread::init()
{
    if (!broker_init()) {
        THREAD_LOG(ERROR) << "broker_init failed";
        return false;
    }

    if (!socket_thread::init()) {
        THREAD_LOG(ERROR) << "Failed to init socket_thread";
        return false;
    }

    return true;
}

void transport_socket_thread::set_select_timeout(unsigned msec) { poll_timeout_ms = msec; }

bool transport_socket_thread::send_cmdu_to_broker(ieee1905_1::CmduMessageTx &cmdu_tx,
                                                  const std::string &dst_mac,
                                                  const std::string &src_mac,
                                                  const std::string &iface_name)
{
    if (!cmdu_tx.finalize()) {
        THREAD_LOG(ERROR) << "finalize failed";
        return false;
    }

    return send_cmdu_to_broker(cmdu_tx, dst_mac, src_mac, cmdu_tx.getMessageLength(), iface_name);
}

bool transport_socket_thread::send_cmdu_to_broker(ieee1905_1::CmduMessage &cmdu,
                                                  const std::string &dst_mac,
                                                  const std::string &src_mac, uint16_t length,
                                                  const std::string &iface_name)
{
    // This method should be used by Message Routers only. It is used to forward CMDU messages from UDS socket to the BUS.
    LOG_IF(!m_broker, FATAL) << "Broker is not connected!";

    // set bridge address
    if (src_mac.empty()) {
        THREAD_LOG(ERROR) << "src_mac is empty!";
        return false;
    }

    if (dst_mac.empty()) {
        THREAD_LOG(ERROR) << "dst_mac is empty!";
        return false;
    }

    return broker_send(cmdu, iface_name, dst_mac, src_mac, length);
}
