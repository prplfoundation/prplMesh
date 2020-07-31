/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_SOCKETS_H_
#define BCL_NETWORK_SOCKETS_H_

#include "file_descriptor_impl.h"

#include <stdint.h>

#include <net/if.h>

namespace beerocks {
namespace net {

/**
 * Array of bytes used to hold data received through a socket.
 * Code is programmed to interfaces so it does not care about which implementation is used.
 * Unit tests can use a mock and set different expectations per test (pretend that different data
 * has been received through the socket).
 */
class Buffer {
public:
    virtual ~Buffer()                   = default;
    virtual const uint8_t *data() const = 0;
    virtual size_t size() const         = 0;
    virtual void clear()                = 0;

    uint8_t *data() { return const_cast<uint8_t *>(const_cast<const Buffer *>(this)->data()); }
};

/**
 * Sockets are OS resources implementing the file descriptor interface. The way this fact is
 * modeled is by extending the FileDescriptor interface.
 */
class Socket : public FileDescriptor {
public:
    /**
     * Wrapper class around sockaddr (structure describing a generic socket address)
     */
    class Address {
    public:
        /**
         * @brief Class destructor
         */
        virtual ~Address() = default;

        /**
         * @brief Returns address of sockaddr structure.
         *
         * @return address of sockaddr.
         */
        virtual const struct sockaddr *sockaddr() const = 0;

        /**
         * @brief Returns the length of the sockaddr structure.
         *
         * @return length of sockaddr
         */
        virtual socklen_t length() const = 0;

        /**
         * @brief Returns address of sockaddr structure.
         *
         * This is the non-const version of the method with the same name.
         *
         * @return address of sockaddr.
         */
        struct sockaddr *sockaddr()
        {
            /**
             * This is a way to "Avoid Duplication in const and Non-const Member Function" as
             * described in "Effective C++, 3rd ed" by Scott Meyers.
             * The two casts and function call may be ugly but they're correct and the method is
             * implemented in the interface class, so available to all implementation classes for free.
             */
            return const_cast<struct sockaddr *>(const_cast<const Address *>(this)->sockaddr());
        }
    };

    /**
     * Classes implementing this interface model either the socket connection established at the
     * server side when accept() system call is called or at the client side when connect() is called.
     *
     * The interface defines the methods to send data over a socket and to receive data from a socket.
     */
    class Connection {
    public:
        /**
         * @brief Class destructor
         */
        virtual ~Connection() = default;

        /**
         * @brief Returns the underlying socket used by this connection.
         *
         * Access to the underlying socket is required to obtain the socket file descriptor with which
         * wait for read or write events using select() or epoll() functions.
         *
         * @return Socket used by the connection
         */
        virtual std::shared_ptr<Socket> socket() = 0;

        /**
         * @brief Receives data through the socket connection.
         *
         * @param[out] buffer Buffer to hold received data.
         * @param[in] offset Position into the buffer to start receiving data.
         * @return Number of bytes received, -1 on failure.
         */
        virtual int receive(Buffer &buffer, size_t offset = 0) = 0;

        /**
         * @brief Receives data through the socket connection.
         *
         * @param[out] buffer Buffer to hold received data.
         * @param[out] address Address where the data came from.
         * @return Number of bytes received, -1 on failure.
         */
        virtual int receive_from(Buffer &buffer, Address &address) = 0;

        /**
         * @brief Sends data through the socket connection.
         *
         * @param[in] buffer Buffer holding data to send.
         * @param[in] length Number of bytes to send.
         * @return Number of bytes transmitted, -1 on failure.
         */
        virtual int send(const Buffer &buffer, size_t length) = 0;

        /**
         * @brief Sends data through the socket connection.
         *
         * @param[in] buffer Buffer holding data to send.
         * @param[in] length Number of bytes to be transmitted.
         * @param[in] address Destination address.
         * @return Number of bytes transmitted, -1 on failure.
         */
        virtual int send_to(const Buffer &buffer, size_t length, const Address &address) = 0;
    };
};

class ServerSocket {
public:
    /**
     * @brief Class destructor
     */
    virtual ~ServerSocket() = default;

    /**
     * @brief Accepts a connection request.
     *
     * @param address Address of the peer socket.
     * @return First connection request on the queue of pending connections for the listening
     * socket.
     */
    virtual std::unique_ptr<Socket::Connection> accept(Socket::Address &address) = 0;
};

class ClientSocket {
public:
    /**
     * @brief Class destructor
     */
    virtual ~ClientSocket() = default;

    /**
     * @brief Connects the socket to the address specified.
     *
     * @param address Destination address.
     * @return Connection established with peer socket.
     */
    virtual std::unique_ptr<Socket::Connection> connect(const Socket::Address &address) = 0;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_SOCKETS_H_ */
