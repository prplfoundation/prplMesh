/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NETLINK_SOCKET_H__
#define __BWL_NETLINK_SOCKET_H__

#include <functional>
#include <memory>

/**
 * Forward declarations
 */
struct nl_sock;
struct nl_msg;

namespace bwl {

/**
 * @brief C++ wrapper class around the core library of the Netlink Protocol Library Suite (libnl).
 *
 * The Netlink socket is Linux kernel interface used for inter-process communication (IPC) between
 * both the kernel and userspace processes.
 *
 * The Netlink core library contains C functions implementing the fundamentals required to use the
 * Netlink protocol such as socket handling, message construction and parsing, and sending and
 * receiving of data.
 *
 * This is a very simple C++ wrapper class around the Netlink core library. Its aim is to
 * facilitate the use of the library from C++ applications. The class can be used as is or extended
 * by derived classes to support extended versions of the netlink protocol like, for example, the
 * generic netlink protocol.
 *
 * Known derived classes: nl_genl_socket.
 */
class netlink_socket {

public:
    /**
     * @brief Class constructor.
     *
     * Allocates the Netlink socket structure.
     *
     * @param protocol Netlink protocol to use. Possible values are listed in <linux/netlink.h>
     */
    explicit netlink_socket(int protocol);

    /**
     * @brief Class destructor.
     *
     * Deallocates the Netlink socket structure.
     */
    virtual ~netlink_socket() = default;

    /**
     * @brief Connects the Netlink socket.
     *
     * Creates a new Netlink socket and binds it to the protocol specified in class constructor.
     *
     * @return True on success and false otherwise (e.g.: socket is already connected).
     */
    virtual bool connect();

    /**
     * @brief Closes the Netlink socket.
     *
     * Call this method to terminate connection and free resources.
     */
    virtual void close();

    /**
     * @brief Performs a request/response transaction.
     *
     * Requires Netlink socket to be connected.
     *
     * Creates a new Netlink message and sends it over the previously connected Netlink socket.
     * Then waits until response message is received and handles it.
     *
     * Message to send is created using provided creation callback function and received message
     * is parsed using provided handling callback function.
     *
     * @param[in] msg_create Message creation callback function (called on message to be sent).
     * @param[in] msg_handle Message handling callback function (called on received message).
     *
     * @return True on success and false otherwise.
     */
    virtual bool send_receive_msg(std::function<bool(struct nl_msg *msg)> msg_create,
                                  std::function<void(struct nl_msg *msg)> msg_handle);

protected:
    /**
     * Netlink socket structure containing the netlink socket and all related attributes,
     * including the actual file descriptor.
     *
     * This member is protected so derived classes can use it to extend or refine the
     * functionality provided by this class.
     */
    std::unique_ptr<struct nl_sock, void (*)(struct nl_sock *)> m_nl_socket;

private:
    /**
     * Netlink protocol to use. Possible values are listed in <linux/netlink.h>
     */
    int m_protocol;
};

} // namespace bwl

#endif
