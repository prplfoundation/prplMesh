/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_POLLER_H__
#define __MAPF_COMMON_POLLER_H__

#include <iostream>
#include <mapf/common/message.h>
#include <mapf/common/socket.h>
#include <poll.h>
#include <vector>

#define MAPF_POLLIN POLLIN
#define MAPF_POLLOUT POLLOUT
#define MAPF_POLLERR POLLERR
#define MAPF_POLLPRI POLLPRI

namespace mapf {

struct msglib_pollitems;

class Poller {
public:
    Poller();
    virtual ~Poller();
    Poller(const Poller &) = delete;
    Poller &operator=(const Poller &) = delete;

    int Add(const Socket &socket, short events = MAPF_POLLIN);
    int Modify(const Socket &socket, short events);
    int Remove(const Socket &socket);
    int Add(int fd, short events = MAPF_POLLIN);
    int Modify(int fd, short events);
    int Remove(int fd);

    int getLinuxfd(const Socket &socket);
    int Poll(long timeout = -1);
    short CheckEvent(const Socket &socket);
    short CheckEvent(int fd);

    void PrintItems() const;

private:
    int Remove(const Socket *socket, int fd);

    msglib_pollitems *items_;
};

} /* namespace mapf */
#endif /* __MAPF_COMMON_POLLER_H__ */
