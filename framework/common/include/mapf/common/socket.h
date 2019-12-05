/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_SOCKET_H__
#define __MAPF_COMMON_SOCKET_H__

#include <iostream>
#include <mapf/common/context.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <mapf/common/message.h>
#include <mapf/common/message_maker.h>
#include <memory>

namespace mapf {

struct msglib_socket;
class Socket {
    friend inline std::ostream &operator<<(std::ostream &os, const Socket &socket);

public:
    Socket();
    virtual ~Socket();

    // -1 - endless, 0 - one shot, >0 - num of retries
    int Connect(const std::string &addr, int max_retries = -1);

    void Close();

    virtual std::ostream &Print(std::ostream &s) const = 0;
    virtual bool Pollable() const                      = 0;
    int fd() const;
    msglib_socket *handle() const { return sock; };
    static bool SyncRequired();

protected:
    msglib_socket *sock;
};

class PubSocket : public Socket {
public:
    PubSocket(Context &ctx);
    std::ostream &Print(std::ostream &s) const override;
    bool Pollable() const override { return false; }
    bool Send(const std::unique_ptr<Message> &msg, int flags = 0);
    bool Send(const Message &msg, int flags = 0);
    size_t Send(void *buf, size_t len, int flags); //TODO - change to private

private:
    PubSocket();
    static std::string padTopic(const std::string &topic);
    static const char topic_pad_char = '\0';
};

class SubSocket : public Socket {
public:
    SubSocket(Context &ctx);

    std::ostream &Print(std::ostream &s) const override;

    bool Pollable() const override { return true; }

    /** (Un)Subscribe APIs - prefer templated versions (uses factory) */

    int Subscribe(std::string const &topic);
    template <class T> int Subscribe(std::string const &topic = T::kTopicPrefix)
    {
        static MessageMaker<T> maker(T::kTopicPrefix);
        return Subscribe(topic);
    }
    int Subscribe(const std::initializer_list<std::string> &topics);
    /**
     * MMZ
     * Parameters that are not modified inside the method should be passed as
     * const reference
     */
    template <class T> int Subscribe(std::initializer_list<std::string> topics)
    {
        mapf_assert(topics.size() > 0);
        for (const auto &topic : topics)
          /**
           * MMZ
           * Method return value is ignored
           */
            Subscribe<T>(topic);
        /**
         * MMZ
         * Method should return a value
         */
    }

    int Unsubscribe(const std::string &topic);
    template <class T> int Unsubscribe(const std::string &topic = T::kTopicPrefix)
    {
        return Unsubscribe(topic);
    }
    /**
     * MMZ
     * Parameters that are not modified inside the method should be passed as
     * const reference
     */
    int Unsubscribe(std::initializer_list<std::string> topics);
    template <class T> int Unsubscribe(std::initializer_list<std::string> topics)
    {
        mapf_assert(topics.size() > 0);
        /**
         * MMZ
         * Method returns on first iteration
         */
        for (const auto &topic : topics)
            return Unsubscribe<T>(topic);
        /**
         * MMZ
         * Method should return a value
         */
    }

    /** Receive APIs - prefer unique_ptr versions (uses factory) */
    bool Receive(Message &msg, int flags = 0);
    std::unique_ptr<Message> Receive(int flags = 0);
    ssize_t Receive(void *buf, size_t len, int flags = 0); //TODO - change to private
    bool Receive(Message::Frame &frame, int flags = 0);    //TODO - change to private

private:
    SubSocket();
    std::vector<std::string>::iterator FindSubscription(const std::string &topic);
    bool More() const;
    std::unique_ptr<std::string> ReceiveTopic(int flags);
    std::unique_ptr<Message::Header> ReceiveHeader(int flags);
    Message::Frame ReceiveFrames(size_t total_len, int flags);
    void EraseSubscription(const std::string &topic);
    void AddSubscription(const std::string &topic);

    std::vector<std::string> topics_;
};

inline std::ostream &operator<<(std::ostream &os, const Socket &socket) { return socket.Print(os); }

} //namespace mapf

#endif /* __MAPF_COMMON_SOCKET_H__ */
