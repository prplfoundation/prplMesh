/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY1_H__
#define __MAPF_COMMON_TEST_MESSAGES_DUMMY1_H__

#include <iostream>
#include <mapf/common/message.h>

namespace mapf {

class Dummy1Message : public Message {
public:
    static const std::string kTopicPrefix;
    Dummy1Message(const std::string &topic) : Message(topic)
    {
        std::cout << "Building Dummy1Message, prefix=" << kTopicPrefix << std::endl;
    }

    Dummy1Message(const std::string &topic, std::initializer_list<Message::Frame> frames)
        : Message(topic, frames)
    {
    }

    virtual ~Dummy1Message()
    {
        std::cout << "Destroying Dummy1Message, prefix=" << kTopicPrefix << std::endl;
    }
};
} // namespace mapf

#endif //ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY1_H__
