/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport_messages.h>

#include <sys/uio.h>

#include <easylogging++.h>

namespace beerocks {
namespace transport {
namespace messages {

// Declaration of static members
constexpr int SubscribeMessage::MAX_SUBSCRIBE_TYPES;

//////////////////////////////////////////////////////////////////////////////
////////////////////////////// Helper Functions //////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static std::unique_ptr<Message>
create_transport_message(Type type, std::initializer_list<messages::Message::Frame> frame)
{
    switch (messages::Type(type)) {
    case messages::Type::CmduRxMessage:
        return std::unique_ptr<messages::CmduRxMessage>{new messages::CmduRxMessage(frame)};
    case messages::Type::CmduTxMessage:
        return std::unique_ptr<messages::CmduTxMessage>{new messages::CmduTxMessage(frame)};
    case messages::Type::SubscribeMessage:
        return std::unique_ptr<messages::SubscribeMessage>{new messages::SubscribeMessage(frame)};
    case messages::Type::CmduTxConfirmationMessage:
        return std::unique_ptr<messages::CmduTxConfirmationMessage>{
            new messages::CmduTxConfirmationMessage(frame)};
    case messages::Type::InterfaceConfigurationQueryMessage:
        return std::unique_ptr<messages::InterfaceConfigurationQueryMessage>{
            new messages::InterfaceConfigurationQueryMessage(frame)};
    case messages::Type::InterfaceConfigurationRequestMessage:
        return std::unique_ptr<messages::InterfaceConfigurationRequestMessage>{
            new messages::InterfaceConfigurationRequestMessage(frame)};
    case messages::Type::InterfaceConfigurationIndicationMessage:
        return std::unique_ptr<messages::InterfaceConfigurationIndicationMessage>{
            new messages::InterfaceConfigurationIndicationMessage(frame)};
    default:
        LOG(WARNING) << "Received unknown message type: " << int(type);
        return std::unique_ptr<messages::Message>{new messages::Message(0, frame)};
    }
}

std::unique_ptr<Message> read_transport_message(Socket &sd)
{
    // Peek into the header to check if the entire message received
    messages::Message::Header header;
    auto bytes_ready = sd.getBytesReady();
    auto read_bytes =
        sd.readBytes(reinterpret_cast<uint8_t *>(&header), sizeof(header), false, sizeof(header));

    if (read_bytes != sizeof(header)) {
        LOG(ERROR) << "Error peeking into the message header: " << read_bytes;
        return nullptr;
    }

    auto discard_pending_bytes = [&]() {
        if (!sd.getBytesReady()) {
            return;
        }

        // Discard all the "ready" bytes in the socket
        // Reading an invalid message magic means that somehow the data synchronization
        // was lost. Since the data is not necessarily aligned to any known size, we have
        // two options here:
        // 1. Safe - Discard 1 byte at a time, until finding the magic word
        // 2. Faster - Discard sizeof(Header) bytes and hope to find a valid header afterwads
        // 2. Fastest - Discard all the bytes and assume the sender will re-send the message
        // For now we'll use the "Faster" method.
        auto discarded_bytes = sd.readBytes(reinterpret_cast<uint8_t *>(&header), sizeof(header),
                                            false, sd.getBytesReady());

        LOG(DEBUG) << "Discarded " << discarded_bytes << " bytes from fd = " << sd.getSocketFd();
    };

    // Validate the header
    if (header.magic != messages::Message::kMessageMagic) {
        LOG(ERROR) << "Invalid message header: magic = 0x" << std::hex << header.magic << std::dec
                   << ", length = " << header.len << ", fd = " << sd.getSocketFd();

        discard_pending_bytes();
        return nullptr;
    }

    // Check if all the message was received
    if (header.len >= uint32_t(bytes_ready)) {
        LOG(DEBUG) << "Message received partially " << bytes_ready << "/" << header.len;
        return nullptr;
    }

    // Validate message length
    if (header.len > messages::Message::kMaxFrameLength) {
        LOG(ERROR) << "Message length is too large: " << header.len << " > "
                   << messages::Message::kMaxFrameLength;

        discard_pending_bytes();
        return nullptr;
    }

    std::unique_ptr<messages::Message> message;

    if (!header.len) {
        message = create_transport_message(Type(header.type), {});
    } else {
        // Read and build the message (blocking operation)
        // TODO: Convert to non-blocking
        messages::Message::Frame frame(size_t(header.len));
        size_t received_bytes = sd.readBytes(frame.data(), header.len, true, header.len);

        if (received_bytes != header.len) {
            LOG(ERROR) << "Received bytes = " << received_bytes
                       << ", Message size = " << header.len;
            return nullptr;
        }

        message = create_transport_message(Type(header.type), {frame});
    }

    LOG_IF(!message, ERROR) << "Failed creating message object for type: " << header.type;
    return message;
}

bool send_transport_message(Socket &sd, const Message &msg, const Message::Header *header)
{
    auto hdr    = (header) ? *header : msg.header();
    iovec iov[] = {{.iov_base = (void *)&hdr, .iov_len = sizeof(hdr)},
                   {.iov_base = (void *)(msg.frame().data()), .iov_len = hdr.len}};

    // Write the header and the data to the socket
    if (writev(sd.getSocketFd(), iov, sizeof(iov) / sizeof(struct iovec)) < 0) {
        LOG(ERROR) << "writev failed: " << strerror(errno);
        return false;
    }

    return true;
}

} // namespace messages
} // namespace transport
} // namespace beerocks
