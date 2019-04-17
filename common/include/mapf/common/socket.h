#ifndef __MAPF_COMMON_SOCKET_H__
#define __MAPF_COMMON_SOCKET_H__

#include "context.h"
#include "message.h"
#include "message_maker.h"
#include "logger.h"
#include "err.h"
#include <zmq.h>
#include <iostream>
#include <memory>

namespace mapf
{

class Socket
{
	friend inline std::ostream& operator << (std::ostream& os, const Socket& socket);
public:
	Socket() {};
	virtual ~Socket()
	{
		Close();
	};

	int Connect(const std::string& addr)
	{
		mapf_assert(!addr.empty());
		return zmq_connect(ptr_, addr.c_str());
	}

	int Disconnect(const std::string& addr)
	{
		return zmq_disconnect(ptr_, addr.c_str());
	}

	void Close()
	{
		if (ptr_ == nullptr)
			return; // already closed
		int rc = zmq_close(ptr_);
		errno_assert(rc == 0);
		ptr_ = nullptr;
	}

	virtual std::ostream& Print(std::ostream& s) const = 0;
	virtual bool Pollable() const = 0;

	/** Accessors & Mutators */
	void* handle() const { return ptr_; }

	bool connected() const
	{
		return endpoint().empty() == false;
	}

	std::string endpoint() const // returns the last endpoint to which the socket connected
	{
		char buf[32];
		size_t len = sizeof(buf);
		int rc = zmq_getsockopt(ptr_, ZMQ_LAST_ENDPOINT, buf, &len);
		return (rc == 0) ? std::string(buf, len - 1) : std::string(0);
	}

protected:
	Socket(void* ptr) : ptr_(ptr) {}

	void* ptr_ = nullptr;
};

class PubSocket : public Socket
{
public:
	PubSocket(Context& ctx) : Socket(zmq_socket(ctx.context(), ZMQ_PUB))
	{
		mapf_assert(ptr_);
	}

	std::ostream& Print(std::ostream& s) const override
	{
		s << "PUB socket, " << (connected() ? "connected" : "not connected") <<  " (" << endpoint() << ")" << std::endl;
		return s;
	}

	bool Pollable() const override { return false; }

	bool Send(const std::unique_ptr<Message>& msg, int flags = 0);
	bool Send(const Message& msg, int flags = 0);
	size_t Send(const void *buf, size_t len, int flags = 0); //TODO - change to private
	bool Send(const Message::Frame& frame, int flags = 0);   //TODO - change to private

private:
	PubSocket();
};

class SubSocket : public Socket
{
public:
	SubSocket(Context& ctx) : Socket(zmq_socket(ctx.context(), ZMQ_SUB))
	{
		mapf_assert(ptr_);
	}

	std::ostream& Print(std::ostream& s) const override
	{
		s << "SUB socket, " << (connected() ? "connected" : "not connected") <<  " (" << endpoint() << ") topics:";
		for (const auto& topic : topics_) {
			s << std::endl << topic;
		}

		return s;
	}

	bool Pollable() const override { return true; }

	/** (Un)Subscribe APIs - prefer templated versions (uses factory) */

	int Subscribe(std::string const& topic);
	template <class T> int Subscribe(std::string const& topic = T::kTopicPrefix)
	{
		static MessageMaker<T> maker(T::kTopicPrefix);
		return Subscribe(topic);
	}
	int Subscribe(const std::initializer_list<std::string>& topics);
    template <class T> int Subscribe(std::initializer_list<std::string> topics)
    {
		mapf_assert(topics.size() > 0);
		for (const auto& topic : topics)
			Subscribe<T>(topic);
    }

	int Unsubscribe(const std::string& topic);
	template <class T> int Unsubscribe(const std::string& topic = T::kTopicPrefix)
	{
		return Unsubscribe(topic);
	}
	int Unsubscribe(std::initializer_list<std::string> topics);
	template <class T> int Unsubscribe(std::initializer_list<std::string> topics)
    {
		mapf_assert(topics.size() > 0);
		for (const auto& topic:topics)
			return Unsubscribe<T>(topic);
    }

	/** Receive APIs - prefer unique_ptr versions (uses factory) */
	bool Receive(Message& msg, int flags = 0);
	std::unique_ptr<Message> Receive(int flags = 0);
	ssize_t Receive(void *buf, size_t len, int flags = 0); //TODO - change to private
	bool Receive(Message::Frame& frame, int flags = 0);   //TODO - change to private

private:
	SubSocket();
	bool More() const;
	std::unique_ptr<std::string> ReceiveTopic(int flags = 0);
	std::unique_ptr<Message::Header> ReceiveHeader(int flags = 0);
	Message::Frame ReceiveFrames(size_t max_len, int flags = 0);
	std::vector<std::string>::iterator FindSubscription(const std::string& topic);
	void EraseSubscription(const std::string& topic);
	void AddSubscription(const std::string& topic);

	std::vector<std::string> topics_;
};

inline std::ostream& operator << (std::ostream& os, const Socket& socket)
{
	return socket.Print(os);
}

} //namespace mapf

#endif /* __MAPF_COMMON_SOCKET_H__ */
