/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_MESSAGE_FACTORY_H__
#define __MAPF_COMMON_MESSAGE_FACTORY_H__

#include "message.h"
#include <iostream>
#include <map>
#include <memory>
#include <string.h>

namespace mapf {

class MessageMakerBase;

class MessageFactory {
public:
    static MessageFactory &Instance();
    void RegisterMaker(const std::string &topic_prefix, MessageMakerBase *maker);
    std::unique_ptr<Message> Create(const std::string &topic) const;
    std::unique_ptr<Message> Create(const std::string &topic,
                                    std::initializer_list<Message::Frame> frames) const;

    void DumpMakers()
    {
        std::cout << "Factory registered Message Makers:" << std::endl;
        for (auto m : makers_)
            std::cout << m.first << std::endl;
    }

private:
    std::map<std::string, MessageMakerBase *>::const_iterator Find(const std::string &topic) const;

    // map of all Message Makers where the key is the topic prefix
    std::map<std::string, MessageMakerBase *> makers_;
};

} /* namespace mapf */
#endif /* __MAPF_COMMON_MESSAGE_FACTORY_H__ */
