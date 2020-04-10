/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "config.h"
#include <exception>
#include <mapf/broker/broker.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <mapf/common/poller.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class PollerTest {
public:
    PollerTest(mapf::Context &ctx, const char *name = "")
        : name_(name), pub_(ctx), sub_(ctx), sub2_(ctx)
    {
        int rc = pub_.Connect(kPubAddr);
        mapf_assert(rc == 0);
        rc = sub_.Connect(kSubAddr);
        mapf_assert(rc == 0);
        rc = sub_.Subscribe(kTopic);
        mapf_assert(rc == 0);
        rc = sub2_.Connect(kSubAddr);
        mapf_assert(rc == 0);
        rc = sub2_.Subscribe(kTopic2);
        mapf_assert(rc == 0);
        std::cout << "\nTEST START: " << name_ << "\n\tsub1 socket: " << sub_
                  << "\nsub2 socket: " << sub2_ << "\npublisher socket: " << pub_ << std::endl;
    }

    void SlowJoinerMultipleAttempts()
    {
        int attempts = 0, max_attempts = 100, rc;

        std::cout << "sending first message multiple attempts (slow joiner fix)" << std::endl;
        // slow joiner syndrom - send multiple times untill received
        while (attempts++ < max_attempts) {
            Send();
            rc = poller_.Poll(10);
            if (rc > 0) {
                mapf_assert(poller_.CheckEvent(sub_) == MAPF_POLLIN ||
                            poller_.CheckEvent(sub2_) == MAPF_POLLIN);

                break;
            }
        }
        mapf_assert(attempts < max_attempts);
        Recv();
        std::cout << "finished first message send after " << attempts << " attempts" << std::endl;
    }

    void Run()
    {
        int rc;
        MAPF_INFO("START");
        rc = poller_.Add(sub_);
        mapf_assert(rc == 0);
        rc = poller_.Add(sub2_);
        mapf_assert(rc == 0);
        SlowJoinerMultipleAttempts();
        Send();
        Recv();

        // verify poll doesn't return false events
        rc = poller_.Poll(10);

        // Send and receive once more
        std::cout << "sending second message" << std::endl;
        Send();

        // Verify delete
        rc = poller_.Remove(sub_);
        mapf_assert(rc == 0);
        rc = poller_.Remove(sub2_);
        mapf_assert(rc == 0);
        // poll should return 0
        rc = poller_.Poll();
        mapf_assert(rc == 0);

        // verify add again
        rc = poller_.Add(sub_);
        mapf_assert(rc == 0);
        rc = poller_.Add(sub2_);
        mapf_assert(rc == 0);
        rc = poller_.Poll();
        mapf_assert(rc > 0);
        Recv();
        MAPF_INFO(name_ << ": finished second message send (single attempt)");
    }

protected:
    void Send()
    {
        mapf::Message::Frame data(kData.size(), kData.data());
        mapf::Message msg(kTopic, {data});
        mapf::Message::Frame data2(kData2.size(), kData2.data());
        mapf::Message msg2(kTopic2, {data2});

        MAPF_INFO("send message #1: " << msg << "\nFrame: " << msg.frame().str());
        bool rc = pub_.Send(msg);
        mapf_assert(rc == true);

        MAPF_INFO("send message #2: " << msg2 << "\nFrame: " << msg2.frame().str());
        rc = pub_.Send(msg2);
        mapf_assert(rc == true);
    }

    void Recv()
    {
        mapf::Message msg;

        int received = 0;
        while (received < 2) {
            int rc = poller_.Poll();
            mapf_assert(rc > 0);

            MAPF_INFO("rc = " << rc);
            poller_.PrintItems();
            bool ret = false;
            if (poller_.CheckEvent(sub_)) {
                ret = sub_.Receive(msg);
                mapf_assert(ret);
                MAPF_INFO("received message: " << msg << "\nFrame: " << msg.frame().str());
                received++;
            }
            if (poller_.CheckEvent(sub2_)) {
                ret = sub2_.Receive(msg);
                mapf_assert(ret);
                MAPF_INFO("received message: " << msg << "\nFrame: " << msg.frame().str());
                received++;
            }
        }
    }

    const std::string name_;
    mapf::PubSocket pub_;
    mapf::SubSocket sub_;
    mapf::SubSocket sub2_;
    mapf::Poller poller_;

private:
    PollerTest();
};

void sighandler(int signum)
{
    MAPF_INFO("PID #" << getpid() << ": Interrupt Signal (" << signum << ") received.");
    // if we got abort(), it means there was an assert. Send abort to all sub processes and exit
    if (signum == SIGABRT)
        kill(0, SIGABRT);
    exit(signum);
}

int start_broker()
{
    pid_t pid = fork();
    mapf_assert(pid >= 0);

    if (pid == 0) {
        signal(SIGTERM, sighandler);
        MAPF_INFO("broker starting");
        mapf::Broker broker;
        broker.Bind(mapf::BrokerSocket::FRONTEND, kPubAddr);
        broker.Bind(mapf::BrokerSocket::BACKEND, kSubAddr);
        broker.Run();
    }
    return pid;
}

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("poller_test");
    signal(SIGABRT, sighandler);

    pid_t pid = start_broker();
    MAPF_INFO("broker pid = " << pid);

    mapf::Context &ctx = mapf::Context::Instance();

    PollerTest test(ctx);
    test.Run();

    kill(pid, SIGTERM);
    return 0;
}
