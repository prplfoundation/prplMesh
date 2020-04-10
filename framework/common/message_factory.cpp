/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <algorithm>
#include <iostream>
#include <map>
#include <mapf/common/err.h>
#include <mapf/common/message.h>
#include <mapf/common/message_factory.h>
#include <mapf/common/message_maker.h>
#include <string.h>

namespace mapf {

MessageFactory &MessageFactory::Instance()
{
    static MessageFactory factory;
    return factory;
}

void MessageFactory::RegisterMaker(const std::string &topic_prefix, MessageMakerBase *maker)
{
    auto it = Find(topic_prefix);
    mapf_assert(it == makers_.end()); // we don't allow multiple makers for the same topic prefix
    makers_[topic_prefix] = maker;
}

std::unique_ptr<Message> MessageFactory::Create(const std::string &topic) const
{
    auto it = Find(topic);
    if (it != makers_.end()) {
        MessageMakerBase *maker = it->second;
        return maker->Create(topic);
    }
    return std::unique_ptr<Message>{new Message(topic)};
}

std::unique_ptr<Message> MessageFactory::Create(const std::string &topic,
                                                std::initializer_list<Message::Frame> frames) const
{
    auto it = Find(topic);
    if (it != makers_.end()) {
        MessageMakerBase *maker = it->second;
        return maker->Create(topic, frames);
    }
    return std::unique_ptr<Message>{new Message(topic, frames)};
}

std::map<std::string, MessageMakerBase *>::const_iterator
MessageFactory::Find(const std::string &topic) const
{
    return std::find_if(makers_.begin(), makers_.end(),
                        [&topic](const std::pair<std::string, MessageMakerBase *> &m) {
                            return (topic.find(m.first) == 0);
                        });
}

} /* namespace mapf */
