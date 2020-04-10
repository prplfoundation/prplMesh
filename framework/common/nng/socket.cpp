/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "msglib.h"
#include <algorithm>
#include <iomanip>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <mapf/common/message_factory.h>
#include <mapf/common/socket.h>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include <unistd.h>

//#define MAPF_DEBUG
#ifdef MAPF_DEBUG
#define DBG MAPF_DBG
#else
#define DBG(...)
#endif

namespace mapf {

/* Socket API */

Socket::Socket() : sock(new msglib_socket) {}

Socket::~Socket()
{
    Close();
    delete sock;
}

bool Socket::SyncRequired() { return false; }

int Socket::Connect(const std::string &addr, int max_retries)
{
    mapf_assert(!addr.empty());
    int rc, retries = 0;
    do {
        rc = nng_dial(sock->sd_, addr.c_str(), nullptr, 0);
        if (rc == 0)
            break;
        MAPF_DBG("nng_dial failed: " << nng_strerror(rc) << " retry #" << retries);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (max_retries < 0 || retries++ < max_retries);

    MAPF_ERR_IF(rc, "nng_dial failed");
    return rc;
}

void Socket::Close()
{
    //failure occures only when socket is already closed, so no need to check return value
    nng_close(sock->sd_);
}

int Socket::fd() const
{
    int fd    = -1;
    size_t sz = sizeof(fd);
    int rc    = nng_getopt(sock->sd_, NNG_OPT_RECVFD, &fd, &sz);
    MAPF_ERR_IF(rc, "Can't get fd for socket. errno=" << nng_strerror(rc));

    return fd;
}

/* PUB socket API */
PubSocket::PubSocket(Context &ctx)
{
    int rc = nng_pub0_open(&sock->sd_);
    mapf_assert(rc == 0);
}

size_t PubSocket::Send(void *buf, size_t len, int flags)
{
    int rc = nng_send(sock->sd_, buf, len, flags);
    if (rc == 0) {
        return len;
    } else {
        return -1;
    }
}

bool PubSocket::Send(const Message &msg, int flags)
{
    mapf_assert(msg.version() == Message::kMessageHeaderVersion);
    mapf_assert(msg.len() <= Message::kMaxFrameLength);

    std::string paddedTopic = padTopic(msg.topic());

    uint totalLen = paddedTopic.length();
    for (auto frame : msg.frames()) {
        totalLen += frame.len();
    }

    uint8_t buf[totalLen];
    int copied = 0;
    memcpy(buf, (void *)paddedTopic.data(), paddedTopic.length());
    copied = paddedTopic.length();
    for (auto frame : msg.frames()) {
        memcpy(buf + copied, frame.data(), frame.len());
        copied += frame.len();
    }
    int rc = nng_send(sock->sd_, buf, copied, flags);

    return rc == 0;
}

bool PubSocket::Send(const std::unique_ptr<Message> &msg, int flags)
{
    mapf_assert(msg);
    return Send(*msg, flags);
}

std::string PubSocket::padTopic(const std::string &topic)
{
    std::ostringstream s;
    s << topic << std::setfill(PubSocket::topic_pad_char)
      << std::setw(Message::kMaxTopicSize - topic.length()) << '\0';
    return s.str();
}

std::ostream &PubSocket::Print(std::ostream &s) const
{
    s << "PUB socket, fd=" << fd() << std::endl;
    return s;
}

/* SUB socket API */
SubSocket::SubSocket(Context &ctx)
{
    int rc = nng_sub0_open(&sock->sd_);
    mapf_assert(rc == 0);
}

int SubSocket::Subscribe(const std::string &topic)
{
    errno  = 0;
    int rc = nng_setopt(sock->sd_, NNG_OPT_SUB_SUBSCRIBE, topic.c_str(), topic.length());
    if (rc) {
        MAPF_ERR("Subscribe " << topic << " failed, errno=" << nng_strerror(rc));
        return rc;
    }

    AddSubscription(topic);
    return 0;
}

int SubSocket::Subscribe(const std::initializer_list<std::string> &topics)
{
    for (auto &topic : topics) {
        Subscribe(topic);
    }
    return 0;
}

int SubSocket::Unsubscribe(const std::string &topic)
{
    if (topic.empty())
        return 0;
    int rc = nng_setopt(sock->sd_, NNG_OPT_SUB_UNSUBSCRIBE, topic.c_str(), topic.length());
    if (rc == 0)
        EraseSubscription(topic);
    return rc;
}

int SubSocket::Unsubscribe(std::initializer_list<std::string> topics)
{
    for (auto &topic : topics)
        Unsubscribe(topic);

    return 0;
}

ssize_t SubSocket::Receive(void *buf, size_t len, int flags)
{
    void *alloc_buf = nullptr;
    size_t size;
    int rc = nng_recv(sock->sd_, &alloc_buf, &size, NNG_FLAG_ALLOC);

    memcpy(buf, alloc_buf, int(len));
    if (rc != 0) {
        if (errno == EAGAIN)
            return 0;
        return -1;
    }

    return size;
}

bool SubSocket::Receive(Message::Frame &frame, int flags)
{
    int nbytes = Receive(frame.data(), frame.len(), flags);
    if (nbytes == -1) {
        MAPF_ERR("nng_recv failed with error " << strerror(errno));
        return false;
    }

    return true;
}

bool SubSocket::Receive(Message &msg, int flags)
{
    msg.Clear();

    void *buf = nullptr;
    size_t size;
    int rc = nng_recv(sock->sd_, &buf, &size, NNG_FLAG_ALLOC); //buffer is allocated by nanomsg
    if (rc != 0) {
        MAPF_ERR("Can't allocate buffer for message");
        return false;
    }
    msg.set_topic(std::string((char *)buf, 0, Message::kMaxTopicSize));
    Message::Frame frame(
        size,
        (uint8_t *)buf +
            Message::
                kMaxTopicSize); //The constructor copies the data into the frame internal buffer
    msg.Add(frame);
    nng_free(buf, size);

    DBG("message received");
    return true;
}

std::unique_ptr<Message> SubSocket::Receive(int flags)
{
    void *buf = nullptr;
    size_t size;
    int rc = nng_recv(sock->sd_, &buf, &size, NNG_FLAG_ALLOC); //buffer is allocated by nanomsg
    if (rc != 0) {
        MAPF_ERR("Can't allocate buffer for message");
        return nullptr;
    }
    std::string topic = std::string((char *)buf, 0, Message::kMaxTopicSize);
    int nbytes        = size - Message::kMaxTopicSize;
    Message::Frame frame(
        nbytes,
        (uint8_t *)buf +
            Message::
                kMaxTopicSize); //The constructor copies the data into the frame internal buffer
    nng_free(buf, size);
    DBG("message received");
    return MessageFactory::Instance().Create(topic, {frame});
}

bool SubSocket::More() const { return false; }
std::unique_ptr<std::string> SubSocket::ReceiveTopic(int flags) { return nullptr; }
std::unique_ptr<Message::Header> SubSocket::ReceiveHeader(int flags) { return nullptr; }
Message::Frame SubSocket::ReceiveFrames(size_t total_len, int flags) { return Message::Frame(); }

std::vector<std::string>::iterator SubSocket::FindSubscription(const std::string &topic)
{
    return std::find_if(topics_.begin(), topics_.end(),
                        [&topic](const std::string &item) { return item == topic; });
}

void SubSocket::AddSubscription(const std::string &topic)
{
    auto it = FindSubscription(topic);
    if (it == topics_.end())
        topics_.push_back(topic);
}

void SubSocket::EraseSubscription(const std::string &topic)
{
    auto it = FindSubscription(topic);
    if (it != topics_.end())
        topics_.erase(it);
}

std::ostream &SubSocket::Print(std::ostream &s) const
{
    s << "SUB socket, fd = " << fd() << ". topics:";
    for (const auto &topic : topics_) {
        s << std::endl << topic;
    }

    return s;
}

} // namespace mapf
