/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_MESSAGE_MAKER_H__
#define __MAPF_COMMON_MESSAGE_MAKER_H__

#include "message_factory.h"
#include <iostream>
#include <memory>
#include <string.h>

namespace mapf {

class Message;

class MessageMakerBase {
public:
    virtual std::unique_ptr<Message> Create(const std::string &topic) const                     = 0;
    virtual std::unique_ptr<Message> Create(const std::string &topic,
                                            std::initializer_list<Message::Frame> frames) const = 0;
    virtual const std::string &topic_prefix() const                                             = 0;
    virtual ~MessageMakerBase() {}
};

template <typename T> class MessageMaker : public MessageMakerBase {
public:
    MessageMaker(const std::string &topic_prefix) : topic_prefix_(topic_prefix)
    {
        MessageFactory::Instance().RegisterMaker(topic_prefix, this);
    }

    virtual std::unique_ptr<Message> Create(const std::string &topic) const
    {
        return std::unique_ptr<T>{new T(topic)};
    }

    virtual std::unique_ptr<Message> Create(const std::string &topic,
                                            std::initializer_list<Message::Frame> frames) const
    {
        return std::unique_ptr<T>{new T(topic, frames)};
    }

    virtual const std::string &topic_prefix() const { return topic_prefix_; }

private:
    const std::string topic_prefix_;
};

} /* namespace mapf */
#endif /* __MAPF_COMMON_MESSAGE_MAKER_H__ */
