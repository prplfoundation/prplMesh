/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef SOCKET_H
#define SOCKET_H

#if __linux
#elif __unix
#elif __posix
#else
#ifndef IS_WINDOWS
#define IS_WINDOWS
#endif
#endif

#ifdef IS_WINDOWS
#include <WinSock2.h>
#include <windows.h>
typedef unsigned __int64 ssize_t;
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class Socket {
public:
    Socket(SOCKET s, long readTimeout = 1000);
    Socket(SOCKET s, std::string peer_ip, int port_port, long readTimeout = 1000);
    Socket(const std::string &uds_path = std::string(), long readTimeout = 1000);
    virtual ~Socket();

    enum SocketMode { SocketModeBlocking, SocketModeNonBlocking };

    ssize_t getBytesReady();
    ssize_t readBytes(uint8_t *buf, size_t buf_size, bool blocking, size_t buf_len = 0,
                      bool isPeek = false);
    ssize_t writeBytes(const uint8_t *buf, size_t buf_len, int port = 0,
                       struct sockaddr_in addr_in = {});
    ssize_t writeString(std::string msg)
    {
        return writeBytes((const uint8_t *)msg.c_str(), (size_t)msg.length());
    }
    bool setWriteTimeout(long msec);
    bool setReadTimeout(long msec);
    void closeSocket();
    bool isOpen();
    SOCKET getSocketFd() { return m_socket; }
    std::string getError()
    {
        return m_error;
        m_error.clear();
    }
    std::string getPeerIP() { return m_peer_ip; }
    int getPeerPort() { return m_peer_port; }
    std::string getUdsPath() { return m_uds_path; }
    void setPeerMac(std::string mac) { m_peer_mac = mac; }
    std::string getPeerMac() { return m_peer_mac; }

    bool isAcceptedSocket() { return m_accepted_socket; }
    void setIsServer() { m_is_server = true; }

#ifndef IS_WINDOWS
    size_t getBytesWritePending();
#endif

protected:
    friend class SocketServer;
    friend class SocketSelect;
    SOCKET m_socket = INVALID_SOCKET;
    std::string m_error;
    std::string m_peer_ip;
    std::string m_uds_path;
    int m_peer_port = 0;
    std::string m_peer_mac;
    bool m_accepted_socket  = false;
    bool m_external_handler = false;
    bool m_is_server        = false;

private:
    static int m_ref;
};

class SocketClient : public Socket {
public:
    SocketClient(const std::string &uds_path, long readTimeout = 1000);
    SocketClient(const std::string &host, int port, int connect_timeout_msec = -1,
                 long readTimeout = 1000);
};

class SocketServer : public Socket {
public:
    SocketServer() {}
    SocketServer(const std::string &uds_path, int connections,
                 SocketMode mode = SocketModeBlocking);
    SocketServer(int port, int connections, SocketMode mode = SocketModeBlocking);
    Socket *acceptConnections();
};

class SocketSelect {
public:
    SocketSelect();
    ~SocketSelect();
    void setTimeout(timeval *tval);
    void addSocket(Socket *s);
    void removeSocket(Socket *s);
    void clearReady(Socket *s);
    Socket *at(size_t idx);
    int selectSocket();
    bool readReady(const Socket *s);
    bool readReady(size_t idx);
    int count() { return (int)m_socketVec.size(); }
    bool isBlocking() { return m_isBlocking; }
    std::string getError()
    {
        return m_error;
        m_error.clear();
    }

private:
    bool m_isBlocking;
    std::vector<Socket *> m_socketVec;
    fd_set m_socketSet;
    timeval *m_socketTval;
    std::string m_error;
};

#endif
