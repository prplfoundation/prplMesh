#include <mapf/common/message.h>
#include <mapf/common/message_factory.h>
#include <mapf/common/message_maker.h>
#include <mapf/common/err.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <map>

namespace mapf
{

MessageFactory& MessageFactory::Instance()
{
	static MessageFactory factory;
	return factory;
}

void MessageFactory::RegisterMaker(const std::string& topic_prefix, MessageMakerBase *maker)
{
	auto it = Find(topic_prefix);
	mapf_assert(it == makers_.end()); // we don't allow multiple makers for the same topic prefix
	makers_[topic_prefix] = maker;
}

std::unique_ptr<Message>
MessageFactory::Create(const std::string& topic) const
{
	auto it = Find(topic);
	if (it != makers_.end()) {
		MessageMakerBase *maker = it->second;
		return maker->Create(topic);
	}
	return std::unique_ptr<Message>{new Message(topic)};
}

std::unique_ptr<Message>
MessageFactory::Create(const std::string& topic, std::initializer_list<Message::Frame> frames) const
{
	auto it = Find(topic);
	if (it != makers_.end()) {
		MessageMakerBase *maker = it->second;
		return maker->Create(topic, frames);
	}
	return std::unique_ptr<Message>{new Message(topic, frames)};
}

std::map<std::string, MessageMakerBase*>::const_iterator
MessageFactory::Find(const std::string& topic) const
{
	return std::find_if(makers_.begin(), makers_.end(),
						[&topic](const std::pair<std::string, MessageMakerBase*> &m) {
					   return (topic.find(m.first) == 0);
					});
}

} /* namespace mapf */
