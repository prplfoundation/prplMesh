/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "multiap_common.h"
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
MAPF_INITIALIZE_LOGGER
namespace mapf
{

class MultiApAgent : public MultiApBase
{
	using MultiApBase::MultiApBase;
	
	const uint16_t kTxMsgType = 0x8005;
	const uint16_t kRxMsgType = 0x8004;
	const Ieee1905CmduHeader kCmduHdr = {0, 0, ntohs(kTxMsgType), 0, 0, 0xc0};
	uint16_t kCookie = 0;

public:
	static bool kStop;

	void Recv()
	{
		auto msg = bus_.subscriber().Receive();
		mapf_assert(msg);
		if (auto cmdu_rx_msg = dynamic_cast<CmduRxMessage *>(msg.get())) {
			//kCookie = cmdu_rx_msg->metadata()->cookie;
			MAPF_INFO("received CmduRxMessage: " << *cmdu_rx_msg << " with cookie " << kCookie);
		} else {
			MAPF_ERR("received non CmduRxMessage:\n\tMessage: " << *msg << "\n\tFrame: " << msg->frame().str());
		}
	}

	void Send()
	{
		CmduTxMessage msg;
		memcpy(msg.metadata()->dst, "\x01\x80\xc2\x00\x00\x13", 6);
		msg.metadata()->ether_type = ETH_P_1905_1;
		msg.metadata()->msg_type = kTxMsgType;
		msg.metadata()->length = sizeof(kCmduHdr) + 32; //header + 32 bytes of zeros
		//msg.metadata()->cookie = kCookie;
		memcpy(msg.data(), &kCmduHdr, sizeof(kCmduHdr));
		MAPF_INFO("sending CmduTxMessage: " << msg);
		int rc = bus_.publisher().Send(msg);
		mapf_assert(rc == true);
	}

	void Run()
	{
		Subscribe(kRxMsgType);
		while (!kStop) {
			sleep(1);
			if (Poll(100)) {
				Recv();
				Send();
			}
		}
		MAPF_INFO("stopped..");
	}
};

bool MultiApAgent::kStop = false;
 
};

void signal_handler(int signum)
{
	MAPF_INFO("PID #" << getpid() << ": Interrupt Signal (" << signum << ") received.");
	mapf::MultiApAgent::kStop = true;
}

int main()
{
	mapf::Logger::Instance().LoggerInit("agent");
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGSTOP, signal_handler);

	MAPF_INFO("multiap agent start");
	mapf::MultiApAgent agent;
	agent.Run();
	MAPF_INFO("multiap agent end");

	return 0;
}
