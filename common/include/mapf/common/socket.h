#ifndef __MAPF_COMMON_SOCKET_H__
#define __MAPF_COMMON_SOCKET_H__

#include <mapf/common/context.h>
#include <mapf/common/message.h>
#include <mapf/common/message_maker.h>
#include <mapf/common/logger.h>
#include <mapf/common/err.h>
#include <iostream>
#include <memory>

namespace mapf
{

struct msglib_socket;
class Socket
{
	friend inline std::ostream& operator << (std::ostream& os, const Socket& socket);
public:
	Socket();
	virtual ~Socket();

	int Connect(const std::string& addr, bool retry = true);

	void Close();

	virtual std::ostream& Print(std::ostream& s) const = 0;
	virtual bool Pollable() const = 0;
	int fd() const;
	msglib_socket *handle() const { return sock; };
	static bool SyncRequired();

protected:
	msglib_socket *sock;
};

class PubSocket : public Socket
{
public:
	PubSocket(Context& ctx);
	std::ostream& Print(std::ostream& s) const override;
	bool Pollable() const override { return false; }
	bool Send(const std::unique_ptr<Message>& msg, int flags = 0);
	bool Send(const Message& msg, int flags = 0);
	size_t Send(void *buf, size_t len, int flags); //TODO - change to private

private:
	PubSocket();
	static std::string padTopic(const std::string& topic);
	static const char topic_pad_char = '\0';
};

class SubSocket : public Socket
{
public:
	SubSocket(Context& ctx);

	std::ostream& Print(std::ostream& s) const override;

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
	std::vector<std::string>::iterator FindSubscription(const std::string& topic);
	bool More() const;
	std::unique_ptr<std::string> ReceiveTopic(int flags);
	std::unique_ptr<Message::Header> ReceiveHeader(int flags);
	Message::Frame ReceiveFrames(size_t total_len, int flags);
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
