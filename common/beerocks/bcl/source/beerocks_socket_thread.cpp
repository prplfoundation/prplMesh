/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_backport.h>
#include <bcl/beerocks_socket_thread.h>
#include <easylogging++.h>

#include <bcl/beerocks_utils.h>

#include <tlvf/CmduMessageRx.h>
#include <tlvf/ieee_1905_1/eTlvType.h>

using namespace beerocks;

#define DEFAULT_MAX_SOCKET_CONNECTIONS 10
#define TX_BUFFER_UDS (tx_buffer + sizeof(beerocks::message::sUdsHeader))
#define TX_BUFFER_UDS_SIZE (sizeof(tx_buffer) - sizeof(beerocks::message::sUdsHeader))
#define CERT_TX_BUFFER_UDS (cert_tx_buffer + sizeof(beerocks::message::sUdsHeader))
#define CERT_TX_BUFFER_UDS_SIZE (sizeof(cert_tx_buffer) - sizeof(beerocks::message::sUdsHeader))

socket_thread::socket_thread(const std::string &unix_socket_path_)
    : thread_base(), cmdu_tx(TX_BUFFER_UDS, TX_BUFFER_UDS_SIZE),
      cert_cmdu_tx(CERT_TX_BUFFER_UDS, CERT_TX_BUFFER_UDS_SIZE),
      unix_socket_path(unix_socket_path_), cmdu_rx(rx_buffer + sizeof(message::sUdsHeader),
                                                   sizeof(rx_buffer) - sizeof(message::sUdsHeader)),
      server_socket(nullptr), server_max_connections(DEFAULT_MAX_SOCKET_CONNECTIONS)
{
    memset(TX_BUFFER_UDS, 0, TX_BUFFER_UDS_SIZE);
    socket_thread::set_select_timeout(500);
}

socket_thread::~socket_thread() {}

void socket_thread::set_server_max_connections(int connections)
{
    server_max_connections = connections;
}

bool socket_thread::init()
{
    if (server_socket) {
        remove_socket(server_socket.get());
        server_socket->closeSocket();
        server_socket.reset(nullptr);
    }

    if (unix_socket_path.empty()) {
        int port = server_port();
        if (port > 0) {
            server_socket = std::make_unique<SocketServer>(port, server_max_connections);
            if (!server_socket) {
                THREAD_LOG(ERROR) << "server_socket == nullptr";
                return false;
            } else if (!server_socket->getError().empty()) {
                THREAD_LOG(ERROR) << "failed to create server_socket, error:"
                                  << server_socket->getError();
                return false;
            } else {
                THREAD_LOG(DEBUG) << "new SocketServer on TCP port " << port;
            }
        }
    } else {
        server_socket = std::make_unique<SocketServer>(unix_socket_path, server_max_connections);
        if (!server_socket) {
            THREAD_LOG(ERROR) << "server_socket == nullptr";
            return false;
        } else if (!server_socket->getError().empty()) {
            THREAD_LOG(ERROR) << "failed to create server_socket, error:"
                              << server_socket->getError();
            return false;
        } else {
            THREAD_LOG(DEBUG) << "new SocketServer on UDS " << unix_socket_path;
        }
    }

    if (server_socket) {
        if (!server_socket->getError().empty()) {
            THREAD_LOG(ERROR) << "new SocketServer: " << server_socket->getError()
                              << " port=" << int(server_port());
            return false;
        }
        add_socket(server_socket.get(), false);
    }

    return true;
}

void socket_thread::set_select_timeout(unsigned msec)
{
    struct timeval tv;
    tv.tv_sec  = (msec / 1000);
    tv.tv_usec = (1000 * (msec % 1000));
    select.setTimeout(&tv);
}

void socket_thread::socket_connected(Socket *sd)
{
    if (sd == nullptr) {
        THREAD_LOG(ERROR) << "sd == nullptr";
    } else {
        add_socket(sd);
    }
}

int socket_thread::socket_disconnected_uds(Socket *sd)
{
    // Dummy read for disconnected check

    ssize_t available_bytes = sd->readBytes(rx_buffer, sizeof(rx_buffer), false, 1,
                                            true); // try to read 1 byte, non-blocking
    if (available_bytes > 0) {
        return 0;
    } else if ((available_bytes < 0) && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        // In case the read operation failed due to timeout, don't close the socket
        THREAD_LOG(ERROR) << "Got event on socket but read operation timedout! sd=" << intptr_t(sd);
        return -1;
    }

    // handle disconnection
    if (socket_disconnected(sd)) {
        remove_socket(sd);
        sd->closeSocket();
        if (sd->isAcceptedSocket()) {
            delete sd;
        }
    }

    return 1;
}

bool socket_thread::handle_cmdu_message_uds(Socket *sd)
{
    size_t available_bytes;

    // Check if UDS Header exists
    available_bytes = sd->readBytes(rx_buffer, sizeof(rx_buffer), true, sizeof(message::sUdsHeader),
                                    true); // PEEK UDS Header, blocking

    if (available_bytes < sizeof(message::sUdsHeader)) {
        THREAD_LOG(ERROR) << "available bytes = " << available_bytes
                          << " is less then sizeof(sUdsHeader)=" << sizeof(message::sUdsHeader);
        return false;
    }

    // Header ready
    message::sUdsHeader *uds_header = (message::sUdsHeader *)rx_buffer;
    size_t message_size             = uds_header->length + sizeof(message::sUdsHeader);

    // Try to read all message
    available_bytes =
        sd->readBytes(rx_buffer, sizeof(rx_buffer), true, message_size); // blocking read

    if (available_bytes != message_size) {
        THREAD_LOG(ERROR) << "available bytes = " << available_bytes
                          << " message_size = " << message_size;
        return false;
    }

    if (!verify_cmdu(uds_header)) {
        THREAD_LOG(ERROR) << "unable to verify cmdu!";
        return false;
    }

    if (!cmdu_rx.parse()) {
        THREAD_LOG(ERROR) << "parsing cmdu failure, rx_buffer" << std::hex << rx_buffer << std::dec
                          << ", uds_header->length=" << int(uds_header->length);
        return false;
    }

    if (!handle_cmdu(sd, cmdu_rx)) {
        return false;
    }

    return true;
}

// FIXME - WLANRTSYS-6360 - should be moved to transport
bool socket_thread::verify_cmdu(message::sUdsHeader *uds_header)
{
    ieee1905_1::sTlvHeader *tlv = reinterpret_cast<ieee1905_1::sTlvHeader *>(
        (uint8_t *)uds_header + sizeof(message::sUdsHeader) +
        ieee1905_1::cCmduHeader::get_initial_size());
    ieee1905_1::sTlvHeader *first_tlv = tlv;

    bool ret = true;

    uint8_t type    = tlv->type;
    uint16_t length = tlv->length;

    do {

        swap_16(length);
        if (static_cast<ieee1905_1::eTlvType>(type) == ieee1905_1::eTlvType::TLV_VENDOR_SPECIFIC) {
            auto tlv_vendor_specific = ieee1905_1::tlvVendorSpecific(
                (uint8_t *)tlv, length + sizeof(ieee1905_1::sTlvHeader), true);
            if (!tlv_vendor_specific.isInitialized()) {
                LOG(ERROR) << "tlvVendorSpecific init() failure";
                ret = false;
                break;
            }

            if (tlv_vendor_specific.vendor_oui() ==
                ieee1905_1::tlvVendorSpecific::eVendorOUI::OUI_INTEL) {
                // assuming that the magic is the first data on the beerocks header
                auto beerocks_magic = *(uint32_t *)(tlv_vendor_specific.payload());
                swap_32(beerocks_magic);
                if (beerocks_magic != message::MESSAGE_MAGIC) {
                    THREAD_LOG(WARNING) << "mismatch magic " << std::hex << int(beerocks_magic)
                                        << " != " << int(message::MESSAGE_MAGIC) << std::dec;
                    ret = false;
                    break;
                }

            } else {
                THREAD_LOG(INFO) << "Not an Intel vendor specific message!";
            }

            // cancel the swap we did
            tlv_vendor_specific.class_swap();
        } else if (static_cast<ieee1905_1::eTlvType>(type) ==
                       ieee1905_1::eTlvType::TLV_END_OF_MESSAGE &&
                   length == 0) {
            return true;
        }

        // set the next tlv
        tlv  = (ieee1905_1::sTlvHeader *)((uint8_t *)tlv + sizeof(ieee1905_1::sTlvHeader) + length);
        type = tlv->type;
        length = tlv->length;

    } while ((uint8_t *)tlv <= (uint8_t *)first_tlv + uds_header->length);

    THREAD_LOG(ERROR) << "TLV end of message not found! tlv_type=" << int(tlv->type)
                      << ", tlv_length=" << int(tlv->length) << ", "
                      << print_cmdu_types(uds_header);

    std::ptrdiff_t available_bytes = uds_header->length + sizeof(message::sUdsHeader);
    LOG(DEBUG) << "hex_dump (" + std::to_string(available_bytes) + " bytes):" << std::endl
               << utils::dump_buffer((uint8_t *)uds_header, available_bytes);
    return ret;
}

bool socket_thread::work()
{
    before_select();

    int sel_ret = select.selectSocket();
    if (sel_ret < 0) {
        // Do not fail for the following "errors"
        if (errno == EAGAIN || errno == EINTR) {
            THREAD_LOG(DEBUG) << "Select returned: " << strerror(errno);
            return true;
        }

        THREAD_LOG(ERROR) << "select error: " << strerror(errno);
        return false;
    }

    after_select(bool(sel_ret == 0));

    if (sel_ret == 0) {
        return true;
    }
    //If something happened on the server socket, then its an incoming connection
    if (server_socket && read_ready(server_socket.get())) {
        clear_ready(server_socket.get());
        Socket *sd = server_socket->acceptConnections();
        if (sd == nullptr || (!server_socket->getError().empty())) {
            THREAD_LOG(ERROR) << "acceptConnections == nullptr: " << server_socket->getError();
            return false;
        } else {
            if (unix_socket_path.empty()) {
                THREAD_LOG(DEBUG) << "new connection from ip=" << sd->getPeerIP()
                                  << " port=" << sd->getPeerPort() << " sd=" << intptr_t(sd);
            } else {
                THREAD_LOG(DEBUG) << "new connection on " << unix_socket_path
                                  << " sd=" << intptr_t(sd);
            }
            socket_connected(sd);
        }
    }

    int sockets_count;
    int i = 0;
    do {
        sockets_count = select.count();
        for (i = 0; i < sockets_count; i++) {
            if (read_ready(select.at(i))) {
                Socket *sd = select.at(i);
                if (!sd) {
                    THREAD_LOG(WARNING)
                        << "sd at select with index i=" << int(i) << " is nullptr, skipping";
                    continue;
                }

                // '0' - socket not disconnected (bytes to read), '1' - socket disconnected, '-1' - error
                auto ret = socket_disconnected_uds(sd);
                if (ret != 0) {
                    // breaking instead of continue because socket_disconnected_uds() may erase element from Select Socket Vector while iterating it
                    break;
                }

                if (custom_message_handler(sd, rx_buffer, sizeof(rx_buffer))) {
                    continue;
                }

                if (!handle_cmdu_message_uds(sd)) {
                    continue;
                }
            }
        }
        // The loop should go over all the sockets. In case something break the for loop before it ended,
        // start iterating over the sockets again.
    } while (i < sockets_count);
    return true;
}
