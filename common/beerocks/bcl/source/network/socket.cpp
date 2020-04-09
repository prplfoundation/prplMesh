/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/socket.h>

#include <errno.h>

#ifdef IS_WINDOWS
typedef int socklen_t;
#else
#include <fcntl.h>
#include <linux/sockios.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <bcl/beerocks_string_utils.h>

#define closesocket close
#define ioctlsocket ioctl
#endif

#include <easylogging++.h>

int Socket::m_ref = 0;

Socket::Socket(const std::string &uds_path, long readTimeout)
{
#ifdef IS_WINDOWS
    if (!uds_path.empty()) {
        m_error = std::string("unix socket not supported");
        return;
    }

    //
    if (m_ref == 0) {
        WSADATA info;
        if (WSAStartup(MAKEWORD(2, 0), &info)) {
            m_error = std::string("WSAStartup() failed");
            return;
        }
    }
    m_ref++;
    //

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET) {
        m_error = std::string("socket() failed");
    }

#else // Linux
    if (!uds_path.empty()) {
        m_uds_path = uds_path;
        m_socket   = socket(AF_UNIX, SOCK_STREAM, 0);
    } else {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
    }
    if (m_socket == INVALID_SOCKET) {
        m_error = uds_path + " -> socket() failed";
    }
#endif

    if (readTimeout != 0)
        setReadTimeout(readTimeout);
}

Socket::Socket(SOCKET s, long readTimeout)
{
    m_socket           = s;
    m_external_handler = true;

    if (readTimeout != 0)
        setReadTimeout(readTimeout);
}

Socket::Socket(SOCKET s, std::string peer_ip, int port_port, long readTimeout)
{
    m_socket    = s;
    m_peer_ip   = peer_ip;
    m_peer_port = port_port;

    if (readTimeout != 0)
        setReadTimeout(readTimeout);
}

Socket::~Socket()
{
    if (m_external_handler)
        return;
    closeSocket();
    if (m_is_server && (!m_uds_path.empty())) {
        remove(m_uds_path.c_str());
    }
#ifdef IS_WINDOWS
    if (m_ref > 0)
        m_ref--;
    if (m_ref == 0)
        WSACleanup();
#endif
}

bool Socket::setWriteTimeout(long msec)
{
    if (m_socket == INVALID_SOCKET) {
        return false;
    }
    timeval tval;
    tval.tv_sec  = 0;
    tval.tv_usec = 1000 * msec;
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tval, sizeof(timeval)) < 0) {
        return false;
    }
    return true;
}

bool Socket::setReadTimeout(long msec)
{
    if (m_socket == INVALID_SOCKET) {
        return false;
    }

#ifdef IS_WINDOWS
    DWORD timeout = msec * 1000 * 1000;
    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout) < 0) {
        return false;
    }
#else
    long sec  = 0;
    long usec = 0;
    if (msec == 0) {
        sec  = (msec / 1000);
        usec = (msec % 1000) * 1000;
    }

    struct timeval tv;
    tv.tv_sec  = sec;
    tv.tv_usec = usec;
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv) < 0) {
        return false;
    }
#endif

    return true;
}

void Socket::closeSocket()
{
    if ((m_socket != INVALID_SOCKET) && ((!m_external_handler) || (m_accepted_socket))) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

bool Socket::isOpen() { return (m_socket != INVALID_SOCKET); }

ssize_t Socket::getBytesReady()
{
    u_long cnt = 0;
    if (m_socket != INVALID_SOCKET) {
        ioctlsocket(m_socket, FIONREAD, &cnt);
    }
    return (ssize_t)cnt;
}

ssize_t Socket::readBytes(uint8_t *buf, size_t buf_size, bool blocking, size_t buf_len, bool isPeek)
{
    if (m_socket == INVALID_SOCKET) {
        return 0;
    }

    ssize_t len = 0;
    if (buf_len == 0) {
        buf_len = getBytesReady();
        if (buf_len == 0)
            buf_len = buf_size;
    }

    if (buf_len > buf_size) {
        LOG(WARNING) << "message truncated, buffer too small!!! buf_size=" << buf_size
                     << " buf_len=" << buf_len;
        buf_len = buf_size;
    }

    // If the user requested to PEEK into the data, add the MSG_PEEK flag
    int flags = isPeek ? MSG_PEEK : 0;

    // Add the MSG_DONTWAIT flag to prevent blocking on the recv() call
    if (!blocking) {
        flags |= MSG_DONTWAIT;
    }

    len = recv(m_socket, (char *)buf, (int)buf_len, flags);

    if (len < 0) {
        LOG(ERROR) << "Error reading from socket (" << m_socket << "): " << strerror(errno);
    }

    return len;
}

ssize_t Socket::writeBytes(const uint8_t *buf, size_t buf_len, int port, struct sockaddr_in addr_in)
{
    if (m_socket == INVALID_SOCKET) {
        return 0;
    }

    if (port) {
        return sendto(m_socket, (const char *)buf, (int)buf_len, 0, (struct sockaddr *)&addr_in,
                      sizeof(addr_in));
    } else {

#ifdef IS_WINDOWS
        int flags = 0;
#else
        int flags = MSG_NOSIGNAL;
#endif

        return send(m_socket, (const char *)buf, (int)buf_len, flags);
    }
}

SocketServer::SocketServer(const std::string &uds_path, int connections, SocketMode mode)
    : Socket(uds_path)
{
#ifdef IS_WINDOWS
    if (!uds_path.empty()) {
        m_error = std::string("unix socket not supported");
    }
#else // Linux

    if (m_socket == INVALID_SOCKET) {
        return;
    }

    // Server socket is always internally managed
    m_external_handler = false;

    sockaddr_un addr;
    m_uds_path = uds_path;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    beerocks::string_utils::copy_string(addr.sun_path, uds_path.c_str(), sizeof(addr.sun_path));

    if (mode == SocketModeNonBlocking) {
        u_long arg = 1;
        ioctlsocket(m_socket, FIONBIO, &arg);
    }

    setIsServer();
    remove(uds_path.c_str());

    if (bind(m_socket, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        m_error  = "bind() to " + uds_path + " failed, error: " + strerror(errno);
        return;
    }
    listen(m_socket, connections);

#endif
}

SocketServer::SocketServer(int port, int connections, SocketMode mode)
{
    sockaddr_in addr;

    if (m_socket == INVALID_SOCKET) {
        return;
    }

    // Server socket is always internally managed
    m_external_handler = false;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET; // windows --> PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

#ifdef IS_WINDOWS
    BOOL enable_b = TRUE;
    char *enable  = (char *)&enable_b;
#else
    int enable_b = 1;
    int *enable  = &enable_b;
#endif
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, enable, sizeof(enable)) < 0) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }

    setIsServer();

    if (mode == SocketModeNonBlocking) {
        u_long arg = 1;
        ioctlsocket(m_socket, FIONBIO, &arg);
    }
    if (bind(m_socket, (sockaddr *)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        m_error  = std::string("bind() failed: ") + strerror(errno);
        return;
    }
    listen(m_socket, connections);
}

Socket *SocketServer::acceptConnections()
{
    Socket *socket_ret = NULL;
    SOCKET s;
    sockaddr_in addr;
    socklen_t addrsize = sizeof(addr);

    memset(&addr, 0, sizeof(addr));

    s = accept(m_socket, (struct sockaddr *)&addr, &addrsize);
    if (s == INVALID_SOCKET) {
#ifdef IS_WINDOWS
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            m_error = std::string("accept() failed: ") + strerror(errno);
        }
#else
        m_error = std::string("accept() failed: ") + strerror(errno);
#endif
    } else {
        socket_ret = new Socket(s, std::string(inet_ntoa(addr.sin_addr)), int(addr.sin_port));
        socket_ret->m_accepted_socket = true;

        if (!m_uds_path.empty())
            socket_ret->m_uds_path = m_uds_path;
    }
    return socket_ret;
}

SocketClient::SocketClient(const std::string &uds_path, long readTimeout)
    : Socket(uds_path, readTimeout)
{
#ifdef IS_WINDOWS
    m_error = std::string("unix socket not supported");
#else
    if (m_socket == INVALID_SOCKET) {
        m_error = "socket != INVALID_SOCKET";
        return;
    }

    m_uds_path = uds_path;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    beerocks::string_utils::copy_string(addr.sun_path, uds_path.c_str(), sizeof(addr.sun_path));

    if (::connect(m_socket, (sockaddr *)&addr, sizeof(addr))) {
        m_error = "connect() to " + uds_path + " failed: " + strerror(errno);
    }
#endif
}

SocketClient::SocketClient(const std::string &host, int port, int connect_timeout_msec,
                           long readTimeout)
    : Socket(std::string(), readTimeout)
{
    sockaddr_in addr;

    if (m_socket == INVALID_SOCKET) {
        m_error = "socket != INVALID_SOCKET";
        return;
    }

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);
    memset(&(addr.sin_zero), 0, 8);

#ifdef IS_WINDOWS
    hostent *he;
    if ((he = gethostbyname(host.c_str())) == 0) {
        m_error = "no such host:" + host;
        return;
    }
    addr.sin_addr = *((in_addr *)he->h_addr);
#else
    // get host address
    if (inet_pton(AF_INET, host.c_str(), &(addr.sin_addr)) != 1) {
        // check if can resolve name //
        sockaddr addr_;
        if (!getnameinfo(&addr_, sizeof(addr_), (char *)host.c_str(), host.length(), NULL, 0, 0)) {
            m_error = "no such host:" + host;
            return;
        }
        addr.sin_addr = ((struct sockaddr_in *)&addr_)->sin_addr;
    }
#endif

    m_peer_ip   = host;
    m_peer_port = port;

    if (connect_timeout_msec < 0) {
        if (::connect(m_socket, (sockaddr *)&addr, sizeof(sockaddr))) {
            m_error = "connect() to " + host + " failed: " + strerror(errno);
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        return;
    }

#ifdef IS_WINDOWS
    unsigned long flags = 1;
    if (ioctlsocket(m_socket, FIONBIO, &flags) != NO_ERROR) {
        m_error = "can't set socket flags";
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }
#else
    // check connection for none block connect //
    int flags = fcntl(m_socket, F_GETFL, 0);
    if (flags < 0) {
        m_error = "can't read socket flags";
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }
    flags = flags | O_NONBLOCK;
    if (fcntl(m_socket, F_SETFL, flags) != 0) {
        m_error = "set O_NONBLOCK failed";
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }
#endif

    ::connect(m_socket, (sockaddr *)&addr, sizeof(sockaddr));

    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_socket, &set);
    struct timeval timeout;
    timeout.tv_sec  = (connect_timeout_msec / 1000);
    timeout.tv_usec = 1000 * (connect_timeout_msec % 1000);
    int max_s       = int(m_socket) + 1;
    int ret         = select(max_s, NULL, &set, NULL, &timeout);
    if (ret != 1) {
        m_error = "connection timeout!";
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }

#ifdef IS_WINDOWS
    ioctlsocket(m_socket, FIONBIO, &flags);
#else
    flags = (flags & ~O_NONBLOCK);
    fcntl(m_socket, F_SETFL, flags);
#endif
}

SocketSelect::SocketSelect()
{
    m_socketTval = NULL;
    m_isBlocking = true;
}

SocketSelect::~SocketSelect()
{
    for (auto s : m_socketVec) {
        if (s != nullptr && s->isAcceptedSocket()) {
            delete s;
        }
    }
    m_socketVec.clear();
}

void SocketSelect::setTimeout(timeval *tval)
{
    if (tval) {
        if (m_socketTval == NULL) {
            m_socketTval = new timeval;
        }
        m_socketTval->tv_sec  = tval->tv_sec;
        m_socketTval->tv_usec = tval->tv_usec;
        m_isBlocking          = false;
    } else {
        if (m_socketTval) {
            delete m_socketTval;
            m_socketTval = NULL;
            m_isBlocking = true;
        }
    }
}

void SocketSelect::addSocket(Socket *s)
{
    if (!s)
        return;

    // Make sure the socket in not in the select
    for (auto soc : m_socketVec) {
        if (soc == s) {
            return;
        }
    }

    m_socketVec.push_back(s);
}

void SocketSelect::removeSocket(Socket *s)
{
    unsigned i;
    if (s) {
        for (i = 0; i < m_socketVec.size(); i++) {
            if (m_socketVec[i] == s) {
                break;
            }
        }
        std::vector<Socket *>::iterator it;
        if (i < m_socketVec.size()) {
            m_socketVec.erase(m_socketVec.begin() + i);
        }
    }
}

void SocketSelect::clearReady(Socket *s)
{
    if (s) {
        FD_CLR(s->m_socket, &m_socketSet);
    }
}

int SocketSelect::selectSocket()
{
    int max_s = 0;
    FD_ZERO(&m_socketSet);
    for (unsigned i = 0; i < m_socketVec.size(); i++) {
        FD_SET(m_socketVec[i]->m_socket, &m_socketSet);
        if (max_s < m_socketVec[i]->m_socket)
            max_s = (int)m_socketVec[i]->m_socket;
    }
    // create a copy of m_socketTval for select() //
    timeval timeout;
    timeval *p_timeout;
    if (m_socketTval) {
        timeout   = *m_socketTval;
        p_timeout = &timeout;
    } else {
        p_timeout = nullptr;
    }
    return select(max_s + 1, &m_socketSet, (fd_set *)0, (fd_set *)0, p_timeout);
}

Socket *SocketSelect::at(size_t idx)
{
    if (idx < m_socketVec.size()) {
        return m_socketVec[idx];
    } else {
        return NULL;
    }
}

bool SocketSelect::readReady(const Socket *s)
{
    if ((s != nullptr) && (s->m_socket != INVALID_SOCKET)) {
        return (FD_ISSET(s->m_socket, &m_socketSet)) ? true : false;
    } else {
        return false;
    }
}

bool SocketSelect::readReady(size_t idx)
{
    if (idx < m_socketVec.size()) {
        return readReady(m_socketVec[idx]);
    } else {
        return false;
    }
}

#ifndef IS_WINDOWS
size_t Socket::getBytesWritePending()
{
    int pending = 0;
    if (m_socket != INVALID_SOCKET) {
        ioctl(m_socket, SIOCOUTQ, &pending);
    }
    return (size_t)pending;
}
#endif
