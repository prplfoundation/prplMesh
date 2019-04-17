#include <mapf/broker/broker_monitor.h>

namespace multiap
{

BrokerMonitor::BrokerMonitor()
	: zmq::monitor_t()
	, name_("default")
{
}

BrokerMonitor::BrokerMonitor(const std::string &name)
	: zmq::monitor_t()
	, name_(name)
{
}

BrokerMonitor::~BrokerMonitor()
{
}

void BrokerMonitor::on_monitor_started()
{
	//LOG(name_ << ": " << __builtin_FUNCTION());
}
void BrokerMonitor::on_event_connected(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_connect_delayed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_connect_retried(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_listening(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_bind_failed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_accepted(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_accept_failed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_closed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_close_failed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_disconnected(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_handshake_failed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_handshake_succeed(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}
void BrokerMonitor::on_event_unknown(const zmq_event_t &event_, const char* addr_)
{
	LOG(name_ << ": " << __builtin_FUNCTION() << " event " << event_.value << " address " << addr_);
}

} /* namespace multiap */
