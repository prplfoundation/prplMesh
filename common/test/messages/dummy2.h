#ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY2_H__
#define __MAPF_COMMON_TEST_MESSAGES_DUMMY2_H__

#include <mapf/common/message.h>
#include <iostream>

namespace mapf
{

class Dummy2Message : public Message
{
public:
	static const std::string kTopicPrefix;
	Dummy2Message(const std::string& topic)
		: Message(topic)
	{
		std::cout << "Building Dummy2Message, prefix=" << kTopicPrefix << std::endl;
	}

	Dummy2Message(const std::string& topic, std::initializer_list<Message::Frame> frames)
		: Message(topic, frames) {}

	virtual ~Dummy2Message()
	{
		std::cout << "Destroying Dummy2Message, prefix=" << kTopicPrefix << std::endl;
	}
};

}

#endif //#ifndef __MAPF_COMMON_TEST_MESSAGES_DUMMY2_H__
