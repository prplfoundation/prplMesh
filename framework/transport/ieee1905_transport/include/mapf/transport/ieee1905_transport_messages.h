/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MAP_IEEE1905_TRANSPORT_MESSAGES_H_
#define MAP_IEEE1905_TRANSPORT_MESSAGES_H_

#include <bcl/network/socket.h>
#include <mapf/common/message.h>

#include <iomanip>
#include <net/if.h>
#include <netinet/ether.h>
#include <sstream>

#ifndef ETH_P_1905_1
#define ETH_P_1905_1 0x893a
#endif
#ifndef ETH_P_LLDP
#define ETH_P_LLDP 0x88CC
#endif

namespace beerocks {
namespace transport {
namespace messages {

// NOTE: Use the base mapf::Message class
// Remove this once mapf common namespace is merged with beerocks
using mapf::Message;

/**
 * Transport internal message types
 */
enum class Type {
    Invalid                                 = 0,
    CmduRxMessage                           = 1,
    CmduTxMessage                           = 2,
    SubscribeMessage                        = 3,
    CmduTxConfirmationMessage               = 4,
    InterfaceConfigurationQueryMessage      = 5,
    InterfaceConfigurationRequestMessage    = 6,
    InterfaceConfigurationIndicationMessage = 7
};

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
        uint8_t version       = kVersion;
        uint16_t cookie       = 0;   // cookie that maps to CMDU to IEEE1905 message id
        uint8_t dst[ETH_ALEN] = {0}; // destination mac address
        uint8_t src[ETH_ALEN] = {0}; // source mac address
        uint16_t ether_type   = 0;   // e.g. IEEE1905, LLDP, etc.
        uint16_t msg_type     = 0;   // IEEE1905 messageType
        uint8_t relay : 1;           // IEEE1905 relayIndicator
        uint8_t
            preset_message_id : 1; // messageId is already set in the IEEE1905 header (set this bit when sending a Reply)
        uint8_t _reserved : 6;
        uint8_t if_type   = 0; // interface type (use InterfaceType enum)
        uint32_t if_index = 0; // network interface index (set to 0 to let transport decide)
        uint16_t length =
            0; // payload length (including IEEE1905 header, excluding Ethernet header)
    };

    explicit CmduXxMessage(uint32_t type, std::initializer_list<Frame> frames = {})
        : Message(type, frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if (this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().len() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    Metadata *metadata() const { return (Metadata *)frames().back().data(); };

    uint8_t *data() const
    {
        mapf_assert(!frames().empty());

        frames().back().set_size(sizeof(Metadata) + metadata()->length);
        return frames().back().data() + sizeof(Metadata);
    };

    virtual std::ostream &print(std::ostream &os) const
    {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        ss << " metadata:" << std::endl;
        ss << "  version    : " << (unsigned)m->version << std::endl;
        ss << "  cookie     : " << m->cookie << std::endl;
        ss << "  length     : " << m->length << std::endl;
        ss << "  dst        : " << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->dst[0] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->dst[1] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->dst[2] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->dst[3] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->dst[4] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->dst[5] << std::endl;
        ss << "  src        : " << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->src[0] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->src[1] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->src[2] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->src[3] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->src[4] << ":" << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)m->src[5] << std::endl;
        ss << "  ether_type : " << std::hex << std::setfill('0') << std::setw(4) << m->ether_type
           << std::endl;
        ss << "  msg_type   : " << std::hex << std::setfill('0') << std::setw(4) << m->msg_type
           << std::endl;
        ss << "  relay      : " << std::hex << std::setfill('0') << std::setw(1)
           << (unsigned)m->relay << std::endl;
        ss << "  preset_mid : " << std::hex << std::setfill('0') << std::setw(1)
           << (unsigned)m->preset_message_id << std::endl;

        return os << ss.str();
    }
};

class CmduRxMessage : public CmduXxMessage {
public:
    explicit CmduRxMessage(std::initializer_list<Frame> frame = {})
        : CmduXxMessage(uint32_t(Type::CmduRxMessage), frame)
    {
    }
};

class CmduTxMessage : public CmduXxMessage {
public:
    explicit CmduTxMessage(std::initializer_list<Frame> frame = {})
        : CmduXxMessage(uint32_t(Type::CmduTxMessage), frame)
    {
    }
};

class SubscribeMessage : public Message {
    static const uint8_t kVersion = 0;

public:
    /**
     * Maximal number of types for a single subscribe/unsubscribe message
     */
    static constexpr int MAX_SUBSCRIBE_TYPES = 64;

    /**
     * The type of the request
     */
    enum class ReqType : uint8_t {
        INVALID     = 0,
        SUBSCRIBE   = 1,
        UNSUBSCRIBE = 2,
    };

    /**
     * Message type to subscribe/unsubscribe
     */
    union MsgType {
        struct {
            uint32_t internal : 1;        // 0 - external [cmdu], 1 - internal
            uint32_t vendor_specific : 1; // 0 - easymesh, 1 - vendor specific
            uint32_t reserved : 14;       // unused
            uint32_t type : 16;           // message type (opcode)
        } bits;
        uint32_t value = 0;
    };

    struct Metadata {
        uint8_t version       = kVersion;
        ReqType type          = ReqType::INVALID;
        uint8_t msg_types_num = 0;
        MsgType msg_types[MAX_SUBSCRIBE_TYPES];
    };

    explicit SubscribeMessage(std::initializer_list<Frame> frames = {})
        : Message(uint32_t(Type::SubscribeMessage), frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if (this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().len() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    Metadata *metadata() const
    {
        mapf_assert(!frames().empty());

        return (Metadata *)frames().back().data();
    };

    virtual std::ostream &print(std::ostream &os) const
    {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        os << " metadata:" << std::endl;
        os << "  version       : " << static_cast<int>(m->version) << std::endl;
        os << "  type          : " << static_cast<int>(m->type) << std::endl;
        os << "  msg_types_num : " << std::dec << int(m->msg_types_num) << std::endl;
        os << "  msg_types     : " << std::hex << std::setfill('0') << std::setw(4)
           << m->msg_types[0].value;
        for (int i = 1; i < m->msg_types_num; i++) {
            os << ", " << m->msg_types[i].value;
        }
        os << std::endl;

        return os << ss.str();
    }
};

class CmduTxConfirmationMessage : public Message {
    static const uint8_t kVersion = 0;

public:
    struct Metadata {
        uint8_t version = kVersion;
        uint16_t cookie =
            0; // The cookie value copied from the CmduTxMessage that triggered this confirmation
        uint16_t ether_type =
            0; // The ether_type value copied from the CmduTxMessage that triggered this confirmation
        uint16_t msg_type =
            0; // The msg_type value copied from the CmduTxMessage that triggered this confirmation
        uint16_t msg_id =
            0; // The IEEE1905 messageId used by the IEEE1905 Transport to send the CmduTxMessage that triggered this confirmation
    };

    explicit CmduTxConfirmationMessage(std::initializer_list<Frame> frames = {})
        : Message(uint32_t(Type::CmduTxConfirmationMessage), frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if (this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().len() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    Metadata *metadata() const
    {
        mapf_assert(!frames().empty());

        return (Metadata *)frames().back().data();
    };

    virtual std::ostream &print(std::ostream &os) const
    {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        os << " metadata:" << std::endl;
        os << "  version    : " << (unsigned)m->version << std::endl;
        os << "  cookie     : " << m->cookie << std::endl;
        os << "  ether_type : " << std::hex << std::setfill('0') << std::setw(4) << m->ether_type
           << std::endl;
        os << "  msg_type   : " << std::hex << std::setfill('0') << std::setw(4) << m->msg_type
           << std::endl;
        os << "  msg_id     : " << std::hex << std::setfill('0') << std::setw(4) << m->msg_id
           << std::endl;

        return os << ss.str();
    }
};

class InterfaceConfigurationQueryMessage : public Message {
public:
    explicit InterfaceConfigurationQueryMessage(std::initializer_list<Frame> frames = {})
        : Message(uint32_t(Type::InterfaceConfigurationQueryMessage), frames)
    {
    }

    virtual ~InterfaceConfigurationQueryMessage() {}
};

class InterfaceConfigurationMessage : public Message {
    static const uint8_t kVersion   = 0;
    static const int kMaxInterfaces = 32;

public:
    enum Flags {
        ENABLE_IEEE1905_TRANSPORT = 0x00000001, // enable IEEE1905 transport on this interface
        IS_BRIDGE                 = 0x00000002,
    };

    struct Interface {
        // the linux interface index of the specified interface
        char ifname[IF_NAMESIZE];
        // the interface index of the bridge to which this interface belong to (or zero if it does not belong to a bridge)
        char bridge_ifname[IF_NAMESIZE];
        uint32_t flags = 0;
    };

    struct Metadata {
        uint8_t version        = kVersion;
        uint32_t numInterfaces = 0;
        Interface interfaces[kMaxInterfaces];
    };

    explicit InterfaceConfigurationMessage(uint32_t type, std::initializer_list<Frame> frames)
        : Message(type, frames)
    {
        // maximum one frame is allowed (if none are given we will allocate one below)
        mapf_assert(this->frames().size() <= 1);

        if (this->frames().empty()) {
            Message::Frame frame(sizeof(Metadata));
            Add(frame);
        } else if (this->frames().back().len() < sizeof(Metadata)) {
            this->frames().back().set_size(sizeof(Metadata));
        }
    }

    Metadata *metadata() const { return (Metadata *)frames().back().data(); };

    virtual std::ostream &print(std::ostream &os) const
    {
        Message::print(os);

        std::stringstream ss;
        Metadata *m = metadata();
        ss << " metadata:" << std::endl;
        ss << "  version       : " << (unsigned)m->version << std::endl;
        ss << "  numInterfaces : " << (unsigned)m->numInterfaces << std::endl;

        for (int i = 0; i < int(m->numInterfaces) && i < kMaxInterfaces; i++) {
            ss << "  interface " << i << ":"
               << " name: " << m->interfaces[i].ifname
               << " in bridge: " << m->interfaces[i].bridge_ifname << " flags:"
               << ((m->interfaces[i].flags & Flags::ENABLE_IEEE1905_TRANSPORT) ? " transport" : "")
               << ((m->interfaces[i].flags & Flags::IS_BRIDGE) ? " bridge" : "") << std::endl;
        }

        return os << ss.str();
    }
};

class InterfaceConfigurationRequestMessage : public InterfaceConfigurationMessage {
public:
    explicit InterfaceConfigurationRequestMessage(std::initializer_list<Frame> frames = {})
        : InterfaceConfigurationMessage(uint32_t(Type::InterfaceConfigurationRequestMessage),
                                        frames)
    {
    }
};

// same as InterfaceConfigurationRequestMessage - only with different type
class InterfaceConfigurationIndicationMessage : public InterfaceConfigurationMessage {
public:
    explicit InterfaceConfigurationIndicationMessage(std::initializer_list<Frame> frames = {})
        : InterfaceConfigurationMessage(uint32_t(Type::InterfaceConfigurationIndicationMessage),
                                        frames)
    {
    }
};

/**
 * @brief Read and parse internal transport message from a socket.
 * 
 * @param [in] sd Socket of the incoming message.
 * 
 * @return Unique pointer to the received message object or nullptr on error.
 */
std::unique_ptr<Message> read_transport_message(Socket &sd);

/**
 * @brief Send internal message to a socket.
 * 
 * @param [in] sd Socket for sending the message.
 * @param [in] msg The message to send.
 * @param [in] header OPTIONAL: Pointer to a custom header. 
 *                    Calculated automatically if not provided.
 * 
 * @return true on success of false otherwise.
 */
bool send_transport_message(Socket &sd, const Message &msg,
                            const Message::Header *header = nullptr);

} // namespace messages
} // namespace transport
} // namespace beerocks

#endif // MAP_IEEE1905_TRANSPORT_MESSAGES_H_
