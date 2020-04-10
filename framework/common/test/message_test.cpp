/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "config.h"
#include "messages/dummy_messages.h"
#include <iostream>
#include <mapf/common/message.h>
#include <mapf/common/message_factory.h>
#include <mapf/common/message_maker.h>
#include <mapf/local_bus.h>
#include <thread>
#include <unistd.h>

namespace mapf {

class MessageTest1 : public Message {
public:
    static const std::string kTopicPrefix;
    MessageTest1(const std::string &topic) : Message(topic)
    {
        std::cout << "Building MessageTest1, prefix=" << kTopicPrefix << std::endl;
    }

    MessageTest1(const std::string &topic, std::initializer_list<Message::Frame> frames)
        : Message(topic, frames)
    {
    }

    virtual ~MessageTest1()
    {
        std::cout << "Destroying MessageTest1, prefix=" << kTopicPrefix << std::endl;
    }
};
const std::string MessageTest1::kTopicPrefix = "test1.";

class MessageTest2 : public Message {
public:
    static const std::string kTopicPrefix;
    MessageTest2(const std::string &topic) : Message(topic)
    {
        std::cout << "Building MessageTest2, prefix=" << kTopicPrefix << std::endl;
    }

    MessageTest2(const std::string &topic, std::initializer_list<Message::Frame> frames)
        : Message(topic, frames)
    {
    }

    virtual ~MessageTest2()
    {
        std::cout << "Destroying MessageTest2, prefix=" << kTopicPrefix << std::endl;
    }
};
const std::string MessageTest2::kTopicPrefix = "test2.";
} // namespace mapf

void test1()
{
    std::cout << "START test1" << std::endl;
    mapf::Message::Frame f1;
    mapf::Message::Frame f2(512);
    std::string str("valkiry");
    mapf::Message::Frame f3(str.size(), str.c_str());
    std::cout << f3.data() << std::endl;
    f3.set_data("1234567890", sizeof("1234567890"));
    std::cout << f3.data() << std::endl;
    mapf::Message::Frame f4  = f3;
    mapf::Message::Frame *f5 = new mapf::Message::Frame(f4);
    std::cout << "f1: " << f1 << std::endl;
    std::cout << "f2: " << f2 << std::endl;
    std::cout << "f3: " << f3 << std::endl;
    std::cout << "f4: " << f4 << std::endl;
    delete f5;
    std::cout << "f1: " << f1 << std::endl;
    std::cout << "f2: " << f2 << std::endl;
    std::cout << "f3: " << f3 << std::endl;
    std::cout << "f4: " << f4 << std::endl;
    std::cout << "END test1" << std::endl;
}

void test2_thread_handler(mapf::Message::Frame f)
{
    int count = 10;

    while (count--) {
        //sleep(1);
        std::cout << "thread f: " << f << std::endl;
    }

    std::cout << "thread exiting" << std::endl;
}

void test2()
{
    std::cout << "START test2" << std::endl;
    std::thread *thread_obj;
    {
        mapf::Message::Frame f1(100);
        std::cout << "MAIN: f1 before: " << f1 << std::endl;
        thread_obj = new std::thread(test2_thread_handler, f1);
        std::cout << "MAIN: f1 after: " << f1 << std::endl;
    }
    std::cout << "MAIN went out of scope" << std::endl;
    thread_obj->join();
    delete thread_obj;
    std::cout << "END test2" << std::endl;
}

void test3()
{
    std::cout << "START test3" << std::endl;
    mapf::Message m1("kratos");
    mapf::Message::Frame f1, f2, f3;
    mapf::Message m2("boy", {f1, f2, f3});
    std::cout << "m2: " << m2 << std::endl;

    std::cout << "m1: " << m1 << std::endl;
    {
        mapf::Message::Frame f1;
        std::cout << "f1 before adding: " << f1 << std::endl;
        m1.Add(f1);
        std::cout << "f1 after adding: " << f1 << std::endl;
        std::cout << "in scope m1: " << m1 << std::endl;
        for (auto f : m1.frames()) {
            std::cout << "frame: " << f << std::endl;
        }
    }
    std::cout << "after scope f1: " << m1.frames().data()[0] << std::endl;
    std::cout << "after scope m1: " << m1 << std::endl;
    for (auto f : m1.frames()) {
        std::cout << "frame: " << f << std::endl;
    }
    std::cout << "END test3" << std::endl;
}

void test4()
{
    mapf::MessageFactory::Instance().DumpMakers();
    mapf::Message::Frame f1(kTopic.size(), kTopic.data());
    std::cout << "f1: " << f1 << std::endl;
    {
        auto m1 = mapf::MessageFactory::Instance().Create("test1.thanos", {f1});
        std::cout << "m1 = " << *m1 << std::endl;
        std::cout << "f1: " << f1 << std::endl;
        auto m2 = mapf::MessageFactory::Instance().Create("test2.ironman");
        std::cout << "m2 = " << *m2 << std::endl;
        auto m3 = mapf::MessageFactory::Instance().Create("base", {f1});
        std::cout << "m3 = " << *m3 << std::endl;
        std::cout << "f1: " << f1 << std::endl;
    }
    std::cout << "f1: " << f1 << std::endl;
}

int main()
{
    /*mapf::Logger::Instance().LoggerInit("message_test");
	mapf::LocalBusInterface bus(mapf::Context::Instance());
	bus.subscriber().Subscribe<mapf::Dummy1Message>();
	bus.subscriber().Subscribe<mapf::Dummy2Message>();
	mapf::MessageFactory::Instance().DumpMakers();
	std::cout << "Subscriber: " << bus.subscriber() << std::endl;*/

    test1();
    test2();
    test3();
    test4();
    return 0;
}
