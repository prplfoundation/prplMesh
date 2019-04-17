#include <mapf/broker/broker_interface.h>

namespace mapf
{

int BrokerInterface::syncCount = 0;
void BrokerInterface::Init()
{
	int rc = config_.Parse();
	mapf_assert(rc == 0);
	for (auto& backend:config_.backend) {
		MAPF_INFO("connect " << config_.name << "@" << backend);
		rc = sub_.Connect(backend);
		mapf_assert(rc == 0);
	}
	for (auto& frontend:config_.frontend) {
		MAPF_INFO("connect "  << config_.name << "@" << frontend);
		pub_.Connect(frontend);
		mapf_assert(rc == 0);
	}
}

void BrokerInterface::PrintConfig()
{
	MAPF_DBG("BrokerInterface configuration");
	config_.Print();
}

void BrokerInterface::Sync()
{
	syncCount++;
	const std::string& msg_topic = sync_topic();
	Message tx_msg(msg_topic);
	int rc, attempts = 0, max_attempts = 1000;
	Poller poller;

	rc = sub_.Subscribe(msg_topic);
	mapf_assert(rc == 0);
	rc = poller.Add(sub_);
	mapf_assert(rc == 0);

	while (attempts++ < max_attempts) {
		SyncSend(tx_msg);
		rc = poller.Poll(1);
		if (rc > 0) {
			mapf_assert(poller.CheckEvent(sub_) == MAPF_POLLIN);
			if (SyncRecv(msg_topic))
				break;
		}
	}
	mapf_assert(attempts < max_attempts);
	rc = sub_.Unsubscribe(msg_topic);
	mapf_assert(rc == 0);
}

void BrokerInterface::SyncSend(Message& msg)
{
	bool rc = pub_.Send(msg);
	mapf_assert(rc == true);
}

bool BrokerInterface::SyncRecv(const std::string & msg_topic)
{
	Message msg;
	bool rc = sub_.Receive(msg);
	return rc && (msg_topic == msg.topic());
}

} /* namespace mapf */
