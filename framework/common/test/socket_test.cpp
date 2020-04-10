/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "config.h"
#include <exception>
#include <getopt.h>
#include <mapf/broker/broker.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <mapf/common/message_factory.h>
#include <mapf/common/socket.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class SocketTest {
public:
    struct SocketTestConfig {
        unsigned iterations   = 1;
        unsigned max_attempts = 1000;
        unsigned init_delay   = 0;
        bool verbose          = false;
    };

    SocketTest(mapf::Context &context, SocketTestConfig *cfg = nullptr, const char *name = "")
        : name_(name), pub_(context), sub_(context)
    {
        if (cfg)
            cfg_ = SocketTestConfig(*cfg);
        int rc = pub_.Connect(kPubAddr);
        mapf_assert(rc == 0);
        rc = sub_.Connect(kSubAddr);
        mapf_assert(rc == 0);
        rc = sub_.Subscribe(kTopic);
        mapf_assert(rc == 0);
        mapf::MessageFactory::Instance().DumpMakers();
    }

    void Run()
    {
        for (int i = 0; i < int(cfg_.iterations); i++) {
            if (cfg_.verbose) {
                std::cout << "\n#" << i << ": TEST START: " << name_ << std::endl;
                std::cout << "subscriber socket: " << sub_ << "\npublisher socket: " << pub_
                          << std::endl;
            }

            if (mapf::Socket::SyncRequired()) {
                usleep(200000); //handle slow joiner syndrom - best effort:)
            }

            // Send and receive once more
            if (cfg_.verbose)
                std::cout << "sending second message" << std::endl;
            Send();
            Recv();
            if (cfg_.verbose) {
                std::cout << "finished second message send (single attempt)" << std::endl;
                std::cout << "#" << i << ": TEST PASSED: " << name_ << std::endl;
            }
        }
    }

protected:
    virtual void Recv() = 0;
    virtual void Send() = 0;

    const std::string name_;
    SocketTestConfig cfg_;
    mapf::PubSocket pub_;
    mapf::SubSocket sub_;

private:
    SocketTest();
};

// test sending single string
class SocketTestString : public SocketTest {
public:
    SocketTestString(mapf::Context &ctx, SocketTestConfig *cfg = nullptr)
        : SocketTest(ctx, cfg, "SocketTestString")
    {
    }

    void Send() override
    {
        int rc = pub_.Send((void *)(kTopic.data()), kTopic.size(), 0);
        mapf_assert(rc >= 0);
    }

    void Recv() override
    {
        char buf[256];
        int nbytes = sub_.Receive(buf, sizeof(buf));
        mapf_assert(nbytes > 0);
        if (cfg_.verbose)
            MAPF_INFO(name_ << ": received message: "
                            << std::string(static_cast<char *>(buf), nbytes));
    }
};

// test sending single frame
class SocketTestFrame : public SocketTest {
public:
    SocketTestFrame(mapf::Context &ctx, SocketTestConfig *cfg = nullptr)
        : SocketTest(ctx, cfg, "SocketTestFrame")
    {
    }

    void Send() override
    {
        bool rc = pub_.Send((void *)kTopic.data(), (size_t)kTopic.size(), 0);
        mapf_assert(rc == true);
    }

    void Recv() override
    {
        mapf::Message::Frame topic(kTopic.size());
        memset(topic.data(), 0, topic.len());
        bool rc = sub_.Receive(topic);
        mapf_assert(rc == true);
        if (cfg_.verbose)
            MAPF_INFO(name_ << ": received frame " << topic << " data: " << topic.data());
    }
};

// test sending a message with a single frame
class SocketTestMessage : public SocketTest {
public:
    SocketTestMessage(mapf::Context &ctx, SocketTestConfig *cfg = nullptr)
        : SocketTest(ctx, cfg, "SocketTestMessage")
    {
    }

    void Send() override
    {
        mapf::Message::Frame data(kData.size(), kData.data());
        mapf::Message::Frame data2(kData2.size(), kData2.data());
        mapf::Message msg(kTopic, {data, mapf::Message::Frame(0), data2});
        bool rc = pub_.Send(msg);
        mapf_assert(rc == true);
    }

    void Recv() override
    {
        mapf::Message msg;
        bool rc = sub_.Receive(msg);
        mapf_assert(rc == true);
        if (cfg_.verbose)
            MAPF_INFO("received single frame message: " << msg << "\nFrame: " << msg.frame().str());
    }
};

// test sending a message with a 2 frames
class SocketTestMessageMult : public SocketTest {
public:
    SocketTestMessageMult(mapf::Context &ctx, SocketTestConfig *cfg = nullptr)
        : SocketTest(ctx, cfg, "SocketTestMessageMult")
    {
    }

    void Send() override
    {
        mapf::Message::Frame data1(kData.size(), kData.data());
        mapf::Message::Frame data2(kData2.size(), kData2.data());
        mapf::Message msg(kTopic, {data1, data2});

        bool rc     = pub_.Send(msg);
        auto frames = [&msg]() {
            std::string frames;
            for (auto f : msg.frames())
                frames += f.str() + ", ";
            return frames;
        };
        if (cfg_.verbose)
            MAPF_INFO("sending multi frame message:\n\tMessage: " << msg
                                                                  << "\n\tFrames: " << frames());
        mapf_assert(rc == true);
    }

    void Recv() override
    {
        mapf::Message msg;
        bool rc = sub_.Receive(msg);
        mapf_assert(rc == true);
        if (cfg_.verbose)
            MAPF_INFO("received multi frame message:\n\tMessage: " << msg << "\n\tFrame: "
                                                                   << msg.frame().str());
    }
};

// test sending a message with a 2 frames, receive with MessageFactory
class SocketTestMessageMultFactory : public SocketTest {
public:
    SocketTestMessageMultFactory(mapf::Context &ctx, SocketTestConfig *cfg = nullptr)
        : SocketTest(ctx, cfg, "SocketTestMessageMultFactory")
    {
    }

    void Send() override
    {
        mapf::Message::Frame data1(kData.size(), kData.data());
        mapf::Message::Frame data2(kData2.size(), kData2.data());
        auto msg = mapf::MessageFactory::Instance().Create(kTopic, {data1, data2});

        bool rc     = pub_.Send(msg);
        auto frames = [&msg]() {
            std::string frames;
            for (auto f : msg->frames())
                frames += f.str() + ", ";
            return frames;
        };
        if (cfg_.verbose)
            MAPF_INFO("sending multi frame message:\n\tMessage: " << *msg
                                                                  << "\n\tFrames: " << frames());
        mapf_assert(rc == true);
    }

    void Recv() override
    {
        auto msg = sub_.Receive();
        mapf_assert(msg);
        if (cfg_.verbose)
            MAPF_INFO("received multi frame message:\n\tMessage: " << *msg << "\n\tFrame: "
                                                                   << msg->frame().str());
    }
};

SocketTest::SocketTestConfig g_cfg;
std::string g_test = "all";

void PrintHelp()
{
    std::cout << "-d/--delay <us>: Init delay (wait before start test)\n"
                 "-i/--iterations <num>: Iterations for each test case\n"
                 "-a/--attempts <num>: Max attempts for slow joiner WA\n"
                 "-t/--test <name>: Test to run (all | string | frame | message | mult | factory)\n"
                 "-v/--verbose: Enable verbose printing\n"
                 "-h/--help: Show help\n";
    exit(1);
}

void ProcessArgs(int argc, char **argv)
{
    const char *const short_opts = "i:a:t:d:vh";
    const option long_opts[]     = {{"iterations", 1, nullptr, 'i'}, {"delay", 1, nullptr, 'd'},
                                {"attempts", 1, nullptr, 'a'},   {"test", 1, nullptr, 't'},
                                {"verbose", 0, nullptr, 'v'},    {"help", 0, nullptr, 'h'},
                                {nullptr, 0, nullptr, 0}};

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt) {
        case 'd':
            g_cfg.init_delay = std::stoi(optarg);
            std::cout << "Init Delay[us]: " << g_cfg.init_delay << std::endl;
            break;
        case 'i':
            g_cfg.iterations = std::stoi(optarg);
            std::cout << "Iterations: " << g_cfg.iterations << std::endl;
            break;
        case 'a':
            g_cfg.max_attempts = std::stoi(optarg);
            std::cout << "Max Attempts (slow joiner): " << g_cfg.max_attempts << std::endl;
            break;
        case 't':
            g_test = std::string(optarg);
            if (g_test != "all" && g_test != "string" && g_test != "frame" && g_test != "message" &&
                g_test != "mult" && g_test != "factory") {
                std::cout << "Invalid test name: " << g_test << std::endl;
                PrintHelp();
            }
            std::cout << "Test: " << g_test << std::endl;
            break;
        case 'v':
            g_cfg.verbose = true;
            std::cout << "Verbose\n";
            break;
        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            PrintHelp();
            break;
        }
    }
}

void sighandler(int signum)
{
    MAPF_INFO("PID #" << getpid() << ": Interrupt Signal (" << signum << ") received.");
    // if we got abort(), it means there was an assert. Send abort to all sub processes and exit
    if (signum == SIGABRT)
        kill(0, SIGABRT);
    exit(signum);
}

int start_broker(SocketTest::SocketTestConfig &cfg)
{
    pid_t pid = fork();
    mapf_assert(pid >= 0);

    if (pid == 0) {
        signal(SIGTERM, sighandler);
        if (cfg.verbose)
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
    mapf::Logger::Instance().LoggerInit("socket_test");
    ProcessArgs(argc, argv);

    signal(SIGABRT, sighandler);
    pid_t pid = start_broker(g_cfg);

    mapf::Context &ctx = mapf::Context::Instance();

    if (g_test == "all" || g_test == "string") {
        MAPF_INFO("Socket test string start");
        SocketTestString test_string(ctx, &g_cfg);
        test_string.Run();
    }

    if (g_test == "all" || g_test == "frame") {
        MAPF_INFO("Socket test frame start");
        SocketTestFrame test_frame(ctx, &g_cfg);
        test_frame.Run();
    }

    if (g_test == "all" || g_test == "message") {
        MAPF_INFO("Socket test message start");
        SocketTestMessage test_message(ctx, &g_cfg);
        test_message.Run();
    }

    if (g_test == "all" || g_test == "mult") {
        MAPF_INFO("Socket test message mult start");
        SocketTestMessageMult test_multi(ctx, &g_cfg);
        test_multi.Run();
    }

    if (g_test == "all" || g_test == "factory") {
        MAPF_INFO("Socket test message mult factory start");
        SocketTestMessageMultFactory test_factory(ctx, &g_cfg);
        test_factory.Run();
    }

    kill(pid, SIGTERM);
    return 0;
}
