#ifndef __MAPF_BROKER_MONITOR_H__
#define __MAPF_BROKER_MONITOR_H__

#include <iostream>

namespace mapf
{

class BrokerMonitor : public zmq::monitor_t
{
public:
	BrokerMonitor();
	BrokerMonitor(const std::string &name);
	~BrokerMonitor();
	void on_monitor_started();
	void on_event_connected(const zmq_event_t &event_, const char* addr_);
	void on_event_connect_delayed(const zmq_event_t &event_, const char* addr_);
	void on_event_connect_retried(const zmq_event_t &event_, const char* addr_);
	void on_event_listening(const zmq_event_t &event_, const char* addr_);
	void on_event_bind_failed(const zmq_event_t &event_, const char* addr_);
	void on_event_accepted(const zmq_event_t &event_, const char* addr_);
	void on_event_accept_failed(const zmq_event_t &event_, const char* addr_);
	void on_event_closed(const zmq_event_t &event_, const char* addr_);
	void on_event_close_failed(const zmq_event_t &event_, const char* addr_);
	void on_event_disconnected(const zmq_event_t &event_, const char* addr_);
	void on_event_handshake_failed(const zmq_event_t &event_, const char* addr_);
	void on_event_handshake_succeed(const zmq_event_t &event_, const char* addr_);
	void on_event_unknown(const zmq_event_t &event_, const char* addr_);

private:
	const std::string name_;
};

} /* namespace multiap */
#endif /* __MAPF_BROKER_MONITOR_H__ */
