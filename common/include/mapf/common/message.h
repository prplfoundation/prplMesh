#ifndef __MAPF_COMMON_MESSAGE_H__
#define __MAPF_COMMON_MESSAGE_H__

#include "err.h"
#include <string.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace mapf
{

class Message
{
public:
	static const uint32_t kMessageHeaderVersion = 0x1;
	static const uint32_t kMaxTopicSize = 128;
	static const uint32_t kMaxFrameLength = 0x100000U; // 1MB max for now

	class Frame
	{
	public:
		Frame(size_t len, const void *init_data = nullptr)
			: data_(std::make_shared<std::vector<uint8_t>>(len))
		{
			if (init_data)
				set_data(init_data, len);
		}

		Frame() : Frame(0) {}
		virtual ~Frame() {}

		size_t len() const { return data_->size(); }

		uint8_t* data() const { return data_->data(); }
		std::string str() const { return std::string(reinterpret_cast<char*>(data()), len()); }

		void set_size(size_t size)
		{
			data_->resize(size);
		}

		void set_data(const void* data, size_t len)
		{
			set_size(len);
			std::copy_n((uint8_t*)data, len, data_->data());
		}

		virtual std::ostream& print(std::ostream& os) const {
			return os << "size=" << len() << " use_count=" << data_.use_count();
		}

	private:
		std::shared_ptr<std::vector<uint8_t>> data_ = nullptr;
	}; // class Frame

	struct Header {
		uint32_t version = kMessageHeaderVersion; // header version
		uint32_t len = 0; 		// total length of the message (excluding topic & header)
	};

	Message() {}

	Message(const std::string& topic)
		: topic_(topic) {}

	Message(const std::string& topic, std::initializer_list<Frame> frames)
		: Message(topic)
	{
		for (auto frame:frames)
			Add(frame);
	}

	virtual ~Message() {};

	void Add(Frame& frame)
	{
		frames_.push_back(frame);
	}

	void Clear()
	{
		frames_.clear();
		topic_.clear();
	}

	// Get the first frame
	Frame frame() const { return frames_.empty() ? Frame() : frames_.back(); }

	// Accessors & Mutators
	const Header header() const
	{
		Header hdr;
		for (auto f:frames_)
			hdr.len += f.len();
		return hdr;
	}

	uint32_t version() const { return header().version; }
	uint32_t len() const { return header().len; }

	std::vector<Frame>& frames() const { return frames_; }
	virtual const std::string topic() const { return topic_; }
	virtual void set_topic(const std::string& topic) { topic_ = topic; }

	virtual std::ostream& print(std::ostream& os) const {
		std::stringstream ss;
        ss << " topic   : " << topic() << std::endl;
		ss << " version : " << version() << std::endl;
		ss << " len     : " << len() << std::endl;
		ss << " frames  : " << frames_.size() << std::endl;

		return os << ss.str();
	}

private:
	std::string topic_;
	mutable std::vector<Frame> frames_;
};

inline std::ostream& operator<< (std::ostream& os, const Message::Frame& f) {
	return f.print(os); 
}

inline std::ostream& operator<< (std::ostream& os, const Message& m) {
	return m.print(os); 
} 

} /* namespace mapf */
#endif /* __MAPF_COMMON_MESSAGE_H__ */
