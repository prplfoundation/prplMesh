/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "msglib.h"
#include <algorithm>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <mapf/common/poller.h>

namespace mapf {

struct msglib_pollitems {
    std::vector<pollfd> items_;

    std::vector<pollfd>::iterator Find(const Socket &socket) { return Find(socket.fd()); }

    std::vector<pollfd>::iterator Find(int fd)
    {
        return std::find_if(items_.begin(), items_.end(),
                            [&fd](const pollfd &item) { return item.fd == fd; });
    }
};

Poller::Poller() : items_(new msglib_pollitems) {}

Poller::~Poller() { delete items_; }

int Poller::Add(int fd, short events)
{
    errno = 0;
    MAPF_INFO("Adding polling socket " << fd);
    if (fd < 0) {
        errno = EINVAL;
        MAPF_ERR("Invalid fd=" << fd << ", can't add to poller");
        return -1;
    }

    auto it = items_->Find(fd);
    if (it == items_->items_.end()) {
        items_->items_.push_back({fd, events, 0});
    } else {
        errno = EEXIST;
        MAPF_ERR("fd " << fd << " already added, skipping...");
        return -1;
    }

    return 0;
}

int Poller::Add(const Socket &socket, short events)
{
    errno = 0;
    if (socket.Pollable() == false) {
        errno = EINVAL;
        MAPF_ERR("socket " << socket << " not pollable!");
        return -1;
    }
    return Add(socket.fd(), events);
}

int Poller::Modify(const Socket &socket, short events) { return Modify(socket.fd(), events); }

int Poller::Modify(int fd, short events)
{
    errno   = 0;
    auto it = items_->Find(fd);
    if (it != items_->items_.end()) {
        it->events = events;
    } else {
        errno = ENONET;
        MAPF_ERR("fd " << fd << " not added, skipping...");
        return -1;
    }

    return 0;
}

int Poller::Remove(const Socket &socket) { return Remove(socket.fd()); }

int Poller::Remove(int fd)
{
    errno   = 0;
    auto it = items_->Find(fd);
    if (it != items_->items_.end()) {
        items_->items_.erase(it);
        return 0;
    } else {
        MAPF_ERR("fd " << fd << " not added, skipping...");
        errno = ENOENT;
        return -1;
    }
}

int Poller::Poll(long timeout)
{
    int result = 0;
    if (items_->items_.empty())
        return 0;
    errno = 0;
    result += poll(items_->items_.data(), items_->items_.size(), timeout);

    return result;
}

short Poller::CheckEvent(const Socket &socket) { return CheckEvent(socket.fd()); }

short Poller::CheckEvent(int fd)
{
    auto it = items_->Find(fd);
    if (it != items_->items_.end())
        return (*it).revents;

    MAPF_ERR("fd " << fd << " not added, skipping...");
    errno = ENOENT;
    return -1;
}

void Poller::PrintItems() const
{
    MAPF_INFO("Poll Items:");
    for (auto item : items_->items_)
        MAPF_INFO("socket. fd:" << item.fd << ", events: " << item.events
                                << ", revents: " << item.revents);
}

} /* namespace mapf */
