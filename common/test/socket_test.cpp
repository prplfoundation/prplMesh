#include "config.h"
#include <mapf/common/message_factory.h>
#include <mapf/common/socket.h>
#include <mapf/common/logger.h>
#include <mapf/common/err.h>
#include <mapf/broker/broker.h>
#include <stdio.h>
#include <unistd.h>
#include <exception>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <getopt.h>
MAPF_INITIALIZE_LOGGER
class SocketTest
{
public:
	struct SocketTestConfig {
		unsigned iterations = 1;
		unsigned max_attempts = 1000;
		unsigned init_delay = 100;
		bool verbose = false;
	};

	SocketTest(mapf::Context& context, SocketTestConfig *cfg = nullptr, const char *name = "")
		: name_(name)
		, pub_(context)
		, sub_(context)
		, item_({sub_.handle(), 0, ZMQ_POLLIN, 0})
	{
		if (cfg)
			cfg_ = SocketTestConfig(*cfg);
		usleep(cfg_.init_delay);
		int rc = pub_.Connect(kPubAddr);
		mapf_assert(rc == 0);
		rc = sub_.Connect(kSubAddr);
		mapf_assert(rc == 0);
		rc = sub_.Subscribe(kTopic);
		mapf_assert(rc == 0);
		mapf::MessageFactory::Instance().DumpMakers();
	}

	void SlowJoinerMultipleAttempts()
	{
		int attempts = 0, max_attempts = cfg_.max_attempts, rc;

		if (cfg_.verbose)
			std::cout << "sending first message multiple attempts (slow joiner fix)" << std::endl;
		/* slow joiner syndrom - send multiple times untill received */
		while (attempts++ < max_attempts) {
			Send();
			rc = zmq_poll(&item_, 1, 10); //poll minimal timeout
			if (rc > 0) {
				mapf_assert(item_.revents == ZMQ_POLLIN);
				break;
			}
		}
		mapf_assert(attempts < max_attempts);
		Recv();
		if (cfg_.verbose)
			std::cout << "finished first message send after " << attempts << " attempts" << std::endl;
	}

	void Run()
	{
		for (int i = 0; i < int(cfg_.iterations); i++) {
			if (cfg_.verbose) {
				std::cout << "\n#" << i << ": TEST START: " << name_  << std::endl;
				std::cout << "subscriber socket: " << sub_ << "\npublisher socket: " << pub_ << std::endl;
			}

			SlowJoinerMultipleAttempts();
			//sleep(1);

			// verify poll doesn't return false events
			int rc = zmq_poll(&item_, 1, 10);
			mapf_assert(rc == 0);

			// Send and receive once more
			if (cfg_.verbose)
				std::cout << "sending second message" << std::endl;
			Send();
			rc = zmq_poll(&item_, 1, 10);
			mapf_assert(rc == 1);
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
	zmq_pollitem_t item_;

private:
	SocketTest();
};

// test sending single string
class SocketTestString : public SocketTest
{
public:
	SocketTestString(mapf::Context& ctx, SocketTestConfig *cfg = nullptr) : SocketTest(ctx, cfg, "SocketTestString") {}

	void Send() override
	{
		int rc = pub_.Send(kTopic.data(), kTopic.size());
		mapf_assert(rc >= 0);
	}

	void Recv() override
	{
		char buf[256];
		int nbytes = sub_.Receive(buf, sizeof(buf));
		mapf_assert(nbytes > 0);
		if (cfg_.verbose)
			MAPF_INFO(name_ << ": received message: " << std::string(static_cast<char*>(buf), nbytes));
	}
};

// test sending single frame
class SocketTestFrame : public SocketTest
{
public:
	SocketTestFrame(mapf::Context& ctx, SocketTestConfig *cfg = nullptr) : SocketTest(ctx, cfg, "SocketTestFrame") {}

	void Send() override
	{
		mapf::Message::Frame topic(kTopic.size(), kTopic.data());
		bool rc = pub_.Send(topic);
		mapf_assert(rc == true);
	}

	void Recv() override
	{
		mapf::Message::Frame topic(kTopic.size());
		memset(topic.get(), 0, topic.len());
		bool rc = sub_.Receive(topic);
		mapf_assert(rc == true);
		if (cfg_.verbose)
			MAPF_INFO(name_ << ": received frame " << topic << " data: " << topic.get());
	}
};

// test sending a message with a single frame
class SocketTestMessage : public SocketTest
{
public:
	SocketTestMessage(mapf::Context& ctx, SocketTestConfig *cfg = nullptr) : SocketTest(ctx, cfg, "SocketTestMessage") {}

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
class SocketTestMessageMult : public SocketTest
{
public:
	SocketTestMessageMult(mapf::Context& ctx, SocketTestConfig *cfg = nullptr) : SocketTest(ctx, cfg, "SocketTestMessageMult") {}

	void Send() override
	{
		mapf::Message::Frame data1(kData.size(), kData.data());
		mapf::Message::Frame data2(kData2.size(), kData2.data());
		mapf::Message msg(kTopic, {data1, data2});
		
		bool rc = pub_.Send(msg);
		auto frames = [&msg]() {
				std::string frames;
				for (auto f:msg.frames())
					frames += f.str() + ", ";
				return frames;
			};
		if (cfg_.verbose)
			MAPF_INFO("sending multi frame message:\n\tMessage: " << msg << "\n\tFrames: " << frames());
		mapf_assert(rc == true);
	}

	void Recv() override
	{
		mapf::Message msg;
		bool rc = sub_.Receive(msg);
		mapf_assert(rc == true);
		if (cfg_.verbose)
			MAPF_INFO("received multi frame message:\n\tMessage: " << msg << "\n\tFrame: " << msg.frame().str());
	}
};


// test sending a message with a 2 frames, receive with MessageFactory
class SocketTestMessageMultFactory : public SocketTest
{
public:
	SocketTestMessageMultFactory(mapf::Context& ctx, SocketTestConfig *cfg = nullptr)
		: SocketTest(ctx, cfg, "SocketTestMessageMultFactory") {}

	void Send() override
	{
		mapf::Message::Frame data1(kData.size(), kData.data());
		mapf::Message::Frame data2(kData2.size(), kData2.data());
		auto msg = mapf::MessageFactory::Instance().Create(kTopic, {data1, data2});

		bool rc = pub_.Send(msg);
		auto frames = [&msg]() {
				std::string frames;
				for (auto f:msg->frames())
					frames += f.str() + ", ";
				return frames;
			};
		if (cfg_.verbose)
			MAPF_INFO("sending multi frame message:\n\tMessage: " << *msg << "\n\tFrames: " << frames());
		mapf_assert(rc == true);
	}

	void Recv() override
	{
		auto msg = sub_.Receive();
		mapf_assert(msg);
		if (cfg_.verbose)
			MAPF_INFO("received multi frame message:\n\tMessage: " << *msg << "\n\tFrame: " << msg->frame().str());
	}
};

int test_zmq_direct()
{
	void *ctx = zmq_ctx_new();
	mapf_assert(ctx);

	void *pub = zmq_socket(ctx, ZMQ_PUB);
	mapf_assert(pub);
	int rc = zmq_connect(pub, kPubAddr);
	mapf_assert(rc == 0);

	//sleep(1);
	void *sub = zmq_socket(ctx, ZMQ_SUB);
	mapf_assert(sub);
	rc = zmq_connect(sub, kSubAddr);
	mapf_assert(rc == 0);
	//sleep(1);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, kTopic.c_str(), kTopic.length());
	mapf_assert(rc == 0);

	sleep(1);
	rc = zmq_send(pub, kTopic.c_str(), kTopic.length(), 0);
	std::cout << "kTopic=" << kTopic << " len=" << kTopic.length() << " rc=" << rc << std::endl;
	mapf_assert(rc == int(kTopic.length()));
	//sleep(1);

	zmq_pollitem_t item = {sub, 0, ZMQ_POLLIN, 0};
	rc = zmq_poll(&item, 1, -1);
	mapf_assert(rc == 1);

	char buf[256];
	rc = zmq_recv(sub, buf, 256, 0);
	std::cout << buf << std::endl;
	return 0;
}

SocketTest::SocketTestConfig g_cfg;
std::string g_test = "all";

void PrintHelp()
{
    std::cout <<
			"-d/--delay <us>: Init delay (wait before start test)\n"
            "-i/--iterations <num>: Iterations for each test case\n"
            "-a/--attempts <num>: Max attempts for slow joiner WA\n"
			"-t/--test <name>: Test to run (all | string | frame | message | mult | factory)\n"
            "-v/--verbose: Enable verbose printing\n"
			"-h/--help: Show help\n";
    exit(1);
}

void ProcessArgs(int argc, char** argv)
{
    const char* const short_opts = "i:a:t:d:vh";
    const option long_opts[] = {
            {"iterations", 1, nullptr, 'i'},
			{"delay", 1, nullptr, 'd'},
            {"attempts", 1, nullptr, 'a'},
			{"test", 1, nullptr, 't'},
            {"verbose", 0, nullptr, 'v'},
            {"help", 0, nullptr, 'h'},
            {nullptr, 0, nullptr, 0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
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
			if (g_test != "all" && g_test != "string" && g_test != "frame" && g_test != "message" && g_test != "mult" && g_test != "factory") {
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

int start_broker(SocketTest::SocketTestConfig& cfg)
{
	pid_t pid = fork();
	mapf_assert(pid >=0);

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

	mapf::Context& ctx = mapf::Context::Instance();

	if (g_test == "all" || g_test == "string") {
		SocketTestString test_string(ctx, &g_cfg);
		test_string.Run();
	}

	if (g_test == "all" || g_test == "frame") {
		SocketTestFrame test_frame(ctx, &g_cfg);
		test_frame.Run();
	}
	
	if (g_test == "all" || g_test == "message") {
		SocketTestMessage test_message(ctx, &g_cfg);
		test_message.Run();
	}

	if (g_test == "all" || g_test == "mult") {
		SocketTestMessageMult test_multi(ctx, &g_cfg);
		test_multi.Run();
	}

	if (g_test == "all" || g_test == "factory") {
		SocketTestMessageMultFactory test_factory(ctx, &g_cfg);
		test_factory.Run();
	}
	
	kill(pid, SIGTERM);
	return 0;
}
