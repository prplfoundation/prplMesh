/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <exception>
#include <mapf/broker/broker.h>
#include <mapf/common/poller.h>
#include <mapf/local_bus.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class LocalBusTestMessage : public mapf::Message {
public:
    static const std::string kTopicPrefix;
    LocalBusTestMessage(const std::string &topic) : Message(topic) {}
    LocalBusTestMessage(const std::string &topic, std::initializer_list<Message::Frame> frames)
        : Message(topic, frames)
    {
    }
    virtual ~LocalBusTestMessage() {}
};
const std::string LocalBusTestMessage::kTopicPrefix = "test.";
const std::string kTopic(LocalBusTestMessage::kTopicPrefix + "1");
const std::string kData1("0123456789");
const std::string kData2("ABCDEFGHIJKLMNOP");

class LocalBusTest {
public:
    LocalBusTest() : lbus_(mapf::Context::Instance())
    {
        lbus_.Init();
        int rc = lbus_.subscriber().Subscribe<LocalBusTestMessage>();
        mapf_assert(rc == 0);
        rc = poller_.Add(lbus_.subscriber());
        mapf_assert(rc == 0);
        lbus_.Sync();
        mapf_assert(rc == 0);
        // print makers - should be LocalBusTest topic prefix only
        mapf::MessageFactory::Instance().DumpMakers();
    }

    void Run()
    {
        Send();
        Poll();
        Recv();
    }

protected:
    virtual void Send()
    {
        // sends a message created by the factory
        mapf::Message::Frame data1(kData1.size(), kData1.data());
        mapf::Message::Frame data2(kData2.size(), kData2.data());
        auto msg = mapf::MessageFactory::Instance().Create(kTopic, {data1, data2});
        mapf_assert(msg);

        bool rc = lbus_.publisher().Send(msg);
        mapf_assert(rc == true);
        auto frames = [&msg]() {
            std::string frames;
            for (auto f : msg->frames())
                frames += f.str() + ", ";
            return frames;
        };
        MAPF_INFO("sending multi frame message:\n\tMessage: " << *msg
                                                              << "\n\tFrames: " << frames());
    }

    virtual void Poll()
    {
        int rc = poller_.Poll();
        mapf_assert(rc == 1);
    }

    virtual void Recv()
    {

        // receive message using socket factory APIs
        auto msg = lbus_.subscriber().Receive();
        assert(msg);
        MAPF_INFO("received message:\n\tMessage: " << *msg << "\n\tFrame: " << msg->frame().str());
    }

private:
    mapf::LocalBusInterface lbus_;
    mapf::Poller poller_;
};

void signal_handler(int signum)
{
    MAPF_INFO("PID #" << getpid() << ": Interrupt Signal (" << signum << ") received.");
    exit(signum);
}

int local_bus()
{
    pid_t pid = fork();
    mapf_assert(pid >= 0);

    if (pid == 0) {
        signal(SIGTERM, signal_handler);
        MAPF_INFO("staring local bus broker");
        mapf::Broker localbus(mapf::kLocalBusConf);
        localbus.PrintConfig();
        localbus.Run();
    }
    usleep(1000); //wait till broker is ready...
    return pid;
}

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("local_bus_test");
    pid_t pid = local_bus();
    MAPF_INFO("local_bus pid = " << pid);

    LocalBusTest test;
    test.Run();

    kill(pid, SIGTERM);
    return 0;
}
