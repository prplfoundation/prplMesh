/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY2_H__
#define __MAPF_COMMON_TEST_MESSAGES_DUMMY2_H__

#include <iostream>
#include <mapf/common/message.h>

namespace mapf {

class Dummy2Message : public Message {
public:
    static const std::string kTopicPrefix;
    Dummy2Message(const std::string &topic) : Message(topic)
    {
        std::cout << "Building Dummy2Message, prefix=" << kTopicPrefix << std::endl;
    }

    Dummy2Message(const std::string &topic, std::initializer_list<Message::Frame> frames)
        : Message(topic, frames)
    {
    }

    virtual ~Dummy2Message()
    {
        std::cout << "Destroying Dummy2Message, prefix=" << kTopicPrefix << std::endl;
    }
};
}

#endif //#ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY2_H__
