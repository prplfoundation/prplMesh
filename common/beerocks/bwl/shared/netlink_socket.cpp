/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "netlink_socket.h"

#include <netlink/msg.h>
#include <netlink/netlink.h>

#include <easylogging++.h>

namespace bwl {

netlink_socket::netlink_socket(int protocol)
    : m_nl_socket(nl_socket_alloc(), nl_socket_free), m_protocol(protocol)
{
    LOG_IF(!m_nl_socket, ERROR) << "Failed to allocate netlink socket!";
}

bool netlink_socket::connect()
{
    if (!m_nl_socket) {
        LOG(ERROR) << "Cannot use unallocated socket!";
        return false;
    }

    // Connect the socket
    if (nl_connect(m_nl_socket.get(), m_protocol) != 0) {
        LOG(ERROR) << "Failed to connect netlink socket!";
        return false;
    }

    return true;
}

void netlink_socket::close()
{
    if (!m_nl_socket) {
        LOG(ERROR) << "Cannot use unallocated socket!";
    }

    // Terminate connection and close socket
    nl_close(m_nl_socket.get());
}

bool netlink_socket::send_receive_msg(std::function<bool(struct nl_msg *msg)> msg_create,
                                      std::function<void(struct nl_msg *msg)> msg_handle)
{
    if (!m_nl_socket) {
        LOG(ERROR) << "Cannot use unallocated socket!";
        return false;
    }

    // The Netlink message to send
    std::unique_ptr<nl_msg, void (*)(nl_msg *)> nl_message(nlmsg_alloc(), nlmsg_free);
    if (!nl_message) {
        LOG(ERROR) << "Failed creating netlink message!";
        return false;
    }

    // The Netlink callback set
    std::unique_ptr<nl_cb, void (*)(nl_cb *)> nl_callback(nl_cb_alloc(NL_CB_DEFAULT), nl_cb_put);
    if (!nl_callback) {
        LOG(ERROR) << "Failed creating netlink callback!";
        return false;
    }

    // Termination flag for the loop that receives the response messages. Possible values are:
    // error == 1: initial value, request message has not be sent yet
    // error == 0: response has been successfully received
    // error < 0: some error occurred while receiving response
    // Final value is used to compute the result code of this method.
    int error = 1;

    // Create standard callbacks
    static auto nl_err_cb = [](struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) -> int {
        int *error = (int *)arg;
        *error     = err->error;
        LOG(ERROR) << "Failed to process netlink message! Error: " << *error;
        return NL_STOP;
    };

    static auto nl_finish_cb = [](struct nl_msg *msg, void *arg) -> int {
        int *error = (int *)arg;
        *error     = 0;
        return NL_SKIP;
    };

    static auto nl_ack_cb = [](struct nl_msg *msg, void *arg) -> int {
        int *error = (int *)arg;
        *error     = 0;
        return NL_STOP;
    };

    // Passing a lambda with capture is not supported for standard C function
    // pointers. As a workaround, we create a static (but thread local) wrapper
    // function that calls the capturing lambda function.
    static thread_local std::function<int(struct nl_msg * msg, void *arg)> nl_handler_cb_wrapper;
    nl_handler_cb_wrapper = [&](struct nl_msg *msg, void *arg) -> int {
        msg_handle(msg);
        return NL_SKIP;
    };
    auto nl_handler_cb = [](struct nl_msg *msg, void *arg) -> int {
        return nl_handler_cb_wrapper(msg, arg);
    };

    // Call the user's message create function
    if (!msg_create(nl_message.get())) {
        LOG(ERROR) << "User's netlink create function failed!";
        return false;
    }

    // Set the callbacks
    nl_cb_err(nl_callback.get(), NL_CB_CUSTOM, nl_err_cb, &error); // error
    nl_cb_set(nl_callback.get(), NL_CB_FINISH, NL_CB_CUSTOM, nl_finish_cb,
              &error);                                                        // finish
    nl_cb_set(nl_callback.get(), NL_CB_ACK, NL_CB_CUSTOM, nl_ack_cb, &error); // ack
    nl_cb_set(nl_callback.get(), NL_CB_VALID, NL_CB_CUSTOM, nl_handler_cb,
              nullptr); // response handler

    // Send the netlink message
    int rc = nl_send_auto_complete(m_nl_socket.get(), nl_message.get());
    if (rc < 0) {
        LOG(ERROR) << "Failed to send netlink message! Error: " << rc;
        return false;
    }

    // Receive the response messages
    // Note that call to nl_recvmsgs() is blocking and loop terminates when one of these
    // conditions is met:
    // - nl_recvmsgs() fails (because internal call to nl_recv() in turn fails)
    // - One of the callback functions sets error to 0 (ok)
    // - One of the callback functions sets error to a value lower than 0 (error)
    // Loop is required just in case more than one message is received. Handling callback must
    // process them all.
    while (error > 0) {
        int rc = nl_recvmsgs(m_nl_socket.get(), nl_callback.get());
        if (rc < 0) {
            LOG(ERROR) << "Failed to receive netlink messages! Error: " << rc;
            return false;
        }
    }

    // Return true on success and false otherwise
    return (0 == error);
}

} // namespace bwl
