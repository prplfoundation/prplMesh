#ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY1_H__
#define __MAPF_COMMON_TEST_MESSAGES_DUMMY1_H__

#include <mapf/common/message.h>
#include <iostream>

namespace mapf
{

class Dummy1Message : public Message
{
public:
	static const std::string kTopicPrefix;
	Dummy1Message(const std::string& topic)
		: Message(topic)
	{
		std::cout << "Building Dummy1Message, prefix=" << kTopicPrefix << std::endl;
	}

	Dummy1Message(const std::string& topic, std::initializer_list<Message::Frame> frames)
		: Message(topic, frames) {}

	virtual ~Dummy1Message()
	{
		std::cout << "Destroying Dummy1Message, prefix=" << kTopicPrefix << std::endl;
	}
};

}

#endif //ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY1_H__
