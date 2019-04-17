#ifndef MAP_IEEE1905_TRANSPORT_MESSAGES_H_
#define MAP_IEEE1905_TRANSPORT_MESSAGES_H_

#include <mapf/common/message.h>

#include <netinet/ether.h>
#include <iomanip>
#include <sstream>

#ifndef ETH_P_1905_1
#define ETH_P_1905_1  0x893a
#endif
#ifndef ETH_P_LLDP
#define ETH_P_LLDP    0x88CC
#endif

namespace mapf {

// Notes:
//
// This class will use one and only one Frame!!!
// Do not use this class directly - please use CmduTxMessage and CmduRxMessage instead.
class CmduXxMessage : public Message {
    static const uint8_t kVersion = 0;

public:
    enum InterfaceType {
        IF_TYPE_NONE = 0,
        IF_TYPE_NET,
        IF_TYPE_LOCAL_BUS,
        IF_TYPE_TUNNEL,
    };

    struct Metadata {
        uint8_t version = kVersion;
        uint16_t cookie = 0;            // cookie that maps to CMDU to IEEE1905 message id
        uint8_t dst[ETH_ALEN] = { 0 };  // destination mac address
        uint8_t src[ETH_ALEN] = { 0 };  // source mac address
        uint16_t ether_type = 0;        // e.g. IEEE1905, LLDP, etc.
        uint16_t msg_type = 0;          // IEEE1905 messageType
        uint8_t relay:1;                // IEEE1905 relayIndicator
        uint8_t preset_message_id:1;    // messageId is already set in the IEEE1905 header (set this bit when sending a Reply)
        uint8_t _reserved:6;
        uint8_t if_type = 0;            // interface type (use InterfaceType enum)
        uint32_t if_index = 0;          // network interface index (set to 0 to let transport decide)
        uint16_t length = 0;            // payload length (including IEEE1905 header, excluding Ethernet header)
    };

    CmduXxMessage()
        : CmduXxMessage("", {})
    {
    }

    CmduXxMessage(const std::string& topic)
        : CmduXxMessage(topic, {})
    {
    }

    CmduXxMessage(const std::string& topic, std::initializer_list<Frame> frames)
        : Message(topic, frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if(this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().size() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    virtual const std::string topic_prefix() const = 0;

    virtual const std::string topic() const {
        return build_topic(topic_prefix(), metadata()->ether_type, metadata()->msg_type);
    }

    Metadata *metadata() const {
        return (Metadata *)frames().back().get();
    };

    uint8_t *data() const {
        mapf_assert(!frames().empty());

        frames().back().set_size(sizeof(Metadata) + metadata()->length);
        return frames().back().get() + sizeof(Metadata);
    };

    virtual std::ostream& print(std::ostream& os) const {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        ss << " metadata:" << std::endl;
        ss << "  version    : " << (unsigned)m->version << std::endl;
        ss << "  cookie     : " << m->cookie << std::endl;
        ss << "  length     : " << m->length << std::endl;
        ss << "  dst        : " << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->dst[0] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->dst[1] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->dst[2] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->dst[3] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->dst[4] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->dst[5] << std::endl;
        ss << "  src        : " << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->src[0] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->src[1] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->src[2] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->src[3] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->src[4] << ":"
                                << std::hex << std::setfill('0') << std::setw(2) << (unsigned)m->src[5] << std::endl;
        ss << "  ether_type : " << std::hex << std::setfill('0') << std::setw(4) << m->ether_type << std::endl;
        ss << "  msg_type   : " << std::hex << std::setfill('0') << std::setw(4) << m->msg_type << std::endl;
        ss << "  relay      : " << std::hex << std::setfill('0') << std::setw(1) << (unsigned)m->relay << std::endl;
        ss << "  preset_mid : " << std::hex << std::setfill('0') << std::setw(1) << (unsigned)m->preset_message_id << std::endl;

        return os << ss.str();
    }

protected:
    static const std::string build_topic(const std::string prefix, uint16_t ether_type, uint16_t msg_type) {
        std::ostringstream topic;
        topic << prefix << std::setfill('0') << std::setw(4) << std::hex << ether_type << "." << std::setfill('0') << std::setw(4) << std::hex << msg_type;
        return topic.str();
    }
};



class CmduRxMessage : public CmduXxMessage {
    using CmduXxMessage::CmduXxMessage; // inherit base class constructors

public:
    static const std::string kTopicPrefix;

    virtual const std::string topic_prefix() const {
        return kTopicPrefix;
    }

    static const std::string ieee1905_topic(uint16_t msg_type) {
        return build_topic(kTopicPrefix, ETH_P_1905_1, msg_type);
    }

    static const std::string lldp_topic() {
        return build_topic(kTopicPrefix, ETH_P_LLDP, 0x0000);
    }
};



class CmduTxMessage : public CmduXxMessage {
    using CmduXxMessage::CmduXxMessage; // inherit base class constructors

public:
    static const std::string kTopicPrefix;

    virtual const std::string topic_prefix() const {
        return kTopicPrefix;
    }
};



class CmduTxConfirmationMessage : public Message {
    static const uint8_t kVersion = 0;

public:
    static const std::string kTopicPrefix;

    struct Metadata {
        uint8_t version = kVersion;
        uint16_t cookie = 0;            // The cookie value copied from the CmduTxMessage that triggered this confirmation
        uint16_t ether_type = 0;        // The ether_type value copied from the CmduTxMessage that triggered this confirmation
        uint16_t msg_type = 0;          // The msg_type value copied from the CmduTxMessage that triggered this confirmation
        uint16_t msg_id = 0;            // The IEEE1905 messageId used by the IEEE1905 Transport to send the CmduTxMessage that triggered this confirmation
    };

    CmduTxConfirmationMessage()
        : CmduTxConfirmationMessage("", {})
    {
    }

    CmduTxConfirmationMessage(const std::string& topic)
        : CmduTxConfirmationMessage(topic, {})
    {
    }

    CmduTxConfirmationMessage(const std::string& topic, std::initializer_list<Frame> frames)
        : Message(topic, frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if(this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().size() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    virtual const std::string topic_prefix() const {
        return kTopicPrefix;
    }

    virtual const std::string topic() const {
        return build_topic(topic_prefix(), metadata()->ether_type, metadata()->msg_type);
    }

    static const std::string ieee1905_topic(uint16_t msg_type) {
        return build_topic(kTopicPrefix, ETH_P_1905_1, msg_type);
    }

    Metadata *metadata() const {
        mapf_assert(!frames().empty());

        return (Metadata *)frames().back().get();
    };

    virtual std::ostream& print(std::ostream& os) const {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        os << " metadata:" << std::endl;
        os << "  version    : " << (unsigned)m->version << std::endl;
        os << "  cookie     : " << m->cookie << std::endl;
        os << "  ether_type : " << std::hex << std::setfill('0') << std::setw(4) << m->ether_type << std::endl;
        os << "  msg_type   : " << std::hex << std::setfill('0') << std::setw(4) << m->msg_type << std::endl;
        os << "  msg_id     : " << std::hex << std::setfill('0') << std::setw(4) << m->msg_id << std::endl;

        return os << ss.str();
    }

protected:
    static const std::string build_topic(const std::string prefix, uint16_t ether_type, uint16_t msg_type) {
        std::ostringstream topic;
        topic << prefix << std::setfill('0') << std::setw(4) << std::hex << ether_type << "." << std::setfill('0') << std::setw(4) << std::hex << msg_type;
        return topic.str();
    }
};


class InterfaceConfigurationQueryMessage : public Message {
    using Message::Message; // inherit base class constructors

public:
    static const std::string kTopicPrefix;

    virtual const std::string topic_prefix() const {
        return kTopicPrefix;
    }
};

class InterfaceConfigurationRequestMessage : public Message {
    static const uint8_t kVersion = 0;
    static const int kMaxInterfaces = 32;

public:
    static const std::string kTopicPrefix;

    enum Flags {
        ENABLE_IEEE1905_TRANSPORT = 0x00000001,           // enable IEEE1905 transport on this interface
        IS_BRIDGE = 0x00000002,
    };

    struct Interface {
        uint32_t if_index = 0; // the linux interface index of the specified interface
        uint32_t bridge_if_index = 0; // the interface index of the bridge to which this interface belong to (or zero if it does not belong to a bridge)
        uint32_t flags = 0;
    };

    struct Metadata {
        uint8_t version = kVersion;
        uint32_t numInterfaces = 0;
        Interface interfaces[kMaxInterfaces];
    };

    InterfaceConfigurationRequestMessage()
        : InterfaceConfigurationRequestMessage("", {})
    {
    }

    InterfaceConfigurationRequestMessage(const std::string& topic)
        : InterfaceConfigurationRequestMessage(topic, {})
    {
    }

    InterfaceConfigurationRequestMessage(const std::string& topic, std::initializer_list<Frame> frames)
        : Message(topic, frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if(this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().size() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    virtual const std::string topic_prefix() const {
        return kTopicPrefix;
    }

    virtual const std::string topic() const {
        return topic_prefix();
    }

    Metadata *metadata() const {
        return (Metadata *)frames().back().get();
    };

    virtual std::ostream& print(std::ostream& os) const {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        ss << " metadata:" << std::endl;
        ss << "  version       : " << (unsigned)m->version << std::endl;
        ss << "  numInterfaces : " << (unsigned)m->numInterfaces << std::endl;

        for (int i = 0; i < int(m->numInterfaces) && i < kMaxInterfaces; i++) {
            ss << "  interface " << i << ":" 
               << " if_index: " << m->interfaces[i].if_index 
               << " in bridge: " << m->interfaces[i].bridge_if_index 
               << " flags:" 
               << ((m->interfaces[i].flags & Flags::ENABLE_IEEE1905_TRANSPORT) ? " transport" : "")
               << ((m->interfaces[i].flags & Flags::IS_BRIDGE) ? " bridge" : "")
               << std::endl;
        }

        return os << ss.str();
    }
};


// same as InterfaceConfigurationRequestMessage - only with different topic
class InterfaceConfigurationIndicationMessage : public InterfaceConfigurationRequestMessage {
    using InterfaceConfigurationRequestMessage::InterfaceConfigurationRequestMessage; // inherit base class constructors

public:
    static const std::string kTopicPrefix;

    virtual const std::string topic_prefix() const {
        return kTopicPrefix;
    }
};

}; //namespace

#endif // MAP_IEEE1905_TRANSPORT_MESSAGES_H_
