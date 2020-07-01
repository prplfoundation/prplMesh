#ifndef socket_refactoring_H
#define socket_refactoring_H

#include <stddef.h>
#include <stdint.h>

#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/netlink.h>
#include <linux/un.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <tlvf/CmduMessageRx.h>
#include <tlvf/common/sMacAddr.h>
#include <tlvf/ieee_1905_1/eMessageType.h>

#include <mapf/common/utils.h>

#include "beerocks_event_loop.h"

namespace beerocks {
namespace sockets {

/**
 * Array of bytes used to hold data received through a socket.
 * Code is programmed to interfaces so it does not care about which implementation is used.
 * Unit tests can use a mock and set different expectations per test (pretend that different data
 * has been received through the socket).
 */
class Buffer {
public:
    virtual ~Buffer()                   = default;
    virtual const uint8_t *data() const = 0;
    virtual size_t size() const         = 0;
    virtual void clear()                = 0;

    uint8_t *data() { return const_cast<uint8_t *>(const_cast<const Buffer *>(this)->data()); }
};

/**
 * First example implementation
 */
class BufferImpl1 : public Buffer {
public:
    BufferImpl1(uint8_t *data, size_t size) : m_data(data), m_size(size) {}
    const uint8_t *data() const override { return m_data; }
    size_t size() const override { return m_size; }
    void clear() override { memset(m_data, 0, size()); }

private:
    uint8_t *m_data;
    size_t m_size;
};

/**
 * Second example implementation
 */
template <size_t Size> class BufferImpl2 : public Buffer {
public:
    const uint8_t *data() const override { return m_data; }
    size_t size() const override { return sizeof(m_data); }
    void clear() override { memset(m_data, 0, size()); }

private:
    uint8_t m_data[Size]{};
};

/**
 * This interface models OS resources implementing the file descriptor interface.
 */
class FileDescriptor {
public:
    static constexpr int invalid_descriptor = -1;

    virtual ~FileDescriptor() = default;
    virtual int fd()          = 0;
};

/**
 * File descriptor implementation.
 * This class is basically a wrapper around an `int fd`
 * This class will be aggregated from all classes modeling OS resources implementing the file
 * descriptor interface.
 */
class FileDescriptorImpl : public FileDescriptor {
public:
    explicit FileDescriptorImpl(int fd) : m_fd(fd)
    {
        // TODO: log error if FileDescriptor::invalid_descriptor == fd;
    }

    /**
     * Delete copy constructor to avoid having multiple copies of the file descriptor
     */
    FileDescriptorImpl(const FileDescriptorImpl &) = delete;

    /**
     * Delete assignment operator to avoid having multiple copies of the file descriptor
     */
    FileDescriptorImpl &operator=(const FileDescriptorImpl &) = delete;

    /**
     * A move constructor allows the resources owned by an rvalue object to be moved into an
     * lvalue without creating its copy.
     */
    FileDescriptorImpl(FileDescriptorImpl &&obj)
    {
        this->m_fd = obj.m_fd;
        obj.m_fd   = FileDescriptor::invalid_descriptor;
    }

    /**
     * The move assignment operator is used to transfer ownership of a file descriptor
     */
    FileDescriptorImpl &operator=(FileDescriptorImpl &&obj)
    {
        // Self-assignment detection
        if (&obj == this) {
            return *this;
        }

        // Release any resource we're holding
        close();

        // Transfer ownership of obj.m_fd to m_fd
        this->m_fd = obj.m_fd;

        return *this;
    }

    /**
     * Close socket on destructor (if it is still open)
     */
    ~FileDescriptorImpl() override { close(); }

    /**
     * @brief Gets file descriptor.
     *
     * File descriptor can be used:
     * - in select(), poll() or epoll() to wait for events on this descriptor.
     * - in send() family of functions to send data through the socket connection.
     * - in recv() family of functions to receive data from the socket connection.
     *
     * @return File descriptor value.
     */
    int fd() override { return m_fd; }

private:
    int m_fd;

    /**
     * @brief Closes file descriptor.
     *
     * If valid, closes file descriptor and then invalidates it.
     *
     * @return True on success and false otherwise (for example, if it was already closed)
     */
    bool close()
    {
        if (FileDescriptor::invalid_descriptor == m_fd) {
            return false;
        }

        int rc = ::close(m_fd);
        m_fd   = FileDescriptor::invalid_descriptor;

        return (0 == rc);
    }
};

/**
 * Wrapper class around sockadr (structure describing a generic socket address)
 */
class SockAddr {
public:
    virtual ~SockAddr()                             = default;
    virtual const struct sockaddr *sockaddr() const = 0;
    virtual const socklen_t length() const          = 0;

    struct sockaddr *sockaddr()
    {
        return const_cast<struct sockaddr *>(const_cast<const SockAddr *>(this)->sockaddr());
    }
};

/**
 * Any object implementing the Socket interface is a FileDescriptor too.
 * This way we give meaning to the action of "closing the socket".
 */
class Socket : public FileDescriptor {
    virtual int setsockopt(int level, int optname, void *optval, socklen_t optlen) = 0;
};

/**
 * Abstract base class for all types of sockets: Raw, UDP, TCP, UDS, ...
 * This implementation class aggregates a FileDescriptor implementation so it has a file
 * descriptor. Methods overridden from FileDescriptor interface delegate on the aggregated
 * implementation.
 * Derived classes provide the file descriptor obtained with a call to socket(), using different
 * family, type and protocol parameters.
 * This class aggregates a FileDescriptor instead of inheriting from one of its implementations to
 * follow the principle of "Favor Aggregation over Inheritance".
 * See https://wiki.c2.com/?UseCompositionAndInterfacesWithoutClassInheritance
 */
class SocketAbstractImpl : public Socket {
public:
    int fd() override { return m_descriptor.fd(); }
    int setsockopt(int level, int optname, void *optval, socklen_t optlen) override
    {
        return ::setsockopt(m_descriptor.fd(), level, optname, optval, optlen);
    }

protected:
    /**
     * Constructor is protected so only derived classes can call it
     */
    explicit SocketAbstractImpl(int fd) : m_descriptor(fd) {}

private:
    FileDescriptorImpl m_descriptor;
};

/**
 * Classes implementing this interface model either the socket connection established at the
 * server side when accept() system call is called or at the client side when connect() is called.
 *
 * The interface defines the methods to send data over a socket and to receive data from a socket.
 */
class Connection {
public:
    virtual ~Connection() = default;

    /**
     * @brief Returns the underlying socket used by this connection.
     *
     * Access to the underlying socket is required to obtain the socket file descriptor with which
     * wait for read or write events using select() or epoll() functions.
     *
     * Connection can be closed by closing the underlying socket.
     *
     * @return Socket used by the connection
     */
    virtual std::shared_ptr<Socket> socket() = 0;

    virtual int receive(Buffer &buffer)                                               = 0;
    virtual int receive_from(Buffer &buffer, SockAddr &address)                       = 0;
    virtual int send(const Buffer &buffer, size_t length)                             = 0;
    virtual int send_to(const Buffer &buffer, size_t length, const SockAddr &address) = 0;
};

class ServerSocket {
public:
    virtual ~ServerSocket()                                       = default;
    virtual std::unique_ptr<Connection> accept(SockAddr &address) = 0;
};

class ClientSocket {
public:
    virtual ~ClientSocket()                                              = default;
    virtual std::unique_ptr<Connection> connect(const SockAddr &address) = 0;
};

class UdsAddress : public SockAddr {
public:
    UdsAddress(const std::string &path = "")
    {
        m_address.sun_family = AF_UNIX;
        mapf::utils::copy_string(m_address.sun_path, path.c_str(), sizeof(m_address.sun_path));
    }

    std::string path() const { return m_address.sun_path; }

    const struct sockaddr *sockaddr() const override
    {
        return reinterpret_cast<const struct sockaddr *>(&m_address);
    }
    const socklen_t length() const override { return m_length; }

private:
    sockaddr_un m_address = {};
    socklen_t m_length    = sizeof(m_address);
};

class InternetAddress : public SockAddr {
public:
    InternetAddress(uint16_t port, uint32_t address = INADDR_ANY)
    {
        m_address.sin_family      = AF_INET;
        m_address.sin_addr.s_addr = address;
        m_address.sin_port        = htons(port);
    }

    const struct sockaddr *sockaddr() const override
    {
        return reinterpret_cast<const struct sockaddr *>(&m_address);
    }
    const socklen_t length() const override { return m_length; }

private:
    sockaddr_in m_address = {};
    socklen_t m_length    = sizeof(m_address);
};

class LinkLevelAddress : public SockAddr {
public:
    LinkLevelAddress() {}

    LinkLevelAddress(uint32_t iface_index, const sMacAddr &mac)
    {
        m_address.sll_family  = AF_PACKET;
        m_address.sll_ifindex = iface_index;
        m_address.sll_halen   = sizeof(sMacAddr);
        std::copy_n(mac.oct, sizeof(sMacAddr), m_address.sll_addr);
    }

    const struct sockaddr *sockaddr() const override
    {
        return reinterpret_cast<const struct sockaddr *>(&m_address);
    }
    const socklen_t length() const override { return m_length; }

private:
    sockaddr_ll m_address = {};
    socklen_t m_length    = sizeof(m_address);
};

class NetlinkAddress : public SockAddr {
public:
    explicit NetlinkAddress(uint32_t groups = 0)
    {
        m_address.nl_family = AF_NETLINK;
        m_address.nl_groups = groups;
    }

    const struct sockaddr *sockaddr() const override
    {
        return reinterpret_cast<const struct sockaddr *>(&m_address);
    }
    const socklen_t length() const override { return m_length; }

private:
    sockaddr_nl m_address = {};
    socklen_t m_length    = sizeof(m_address);
};

/**
 * This class is a wrapper for the socket file descriptor obtained with the accept() system call.
 */
class ConnectedSocket : public SocketAbstractImpl {
public:
    explicit ConnectedSocket(int fd) : SocketAbstractImpl(fd) {}
};

class RawSocket : public SocketAbstractImpl {
public:
    explicit RawSocket(uint16_t protocol = ETH_P_ALL)
        : SocketAbstractImpl(socket(AF_PACKET, SOCK_RAW, htons(protocol)))
    {
    }
};

class UdpSocket : public SocketAbstractImpl {
public:
    UdpSocket() : SocketAbstractImpl(socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) {}
};

class TcpSocket : public SocketAbstractImpl {
public:
    TcpSocket() : SocketAbstractImpl(socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) {}
};

class UdsSocket : public SocketAbstractImpl {
public:
    UdsSocket() : SocketAbstractImpl(socket(AF_UNIX, SOCK_STREAM, 0)) {}
};

class AbstractNetlinkSocket : public SocketAbstractImpl {
protected:
    explicit AbstractNetlinkSocket(uint16_t protocol)
        : SocketAbstractImpl(socket(AF_NETLINK, SOCK_RAW, protocol))
    {
    }
};

class NetlinkRouteSocket : public AbstractNetlinkSocket {
public:
    NetlinkRouteSocket() : AbstractNetlinkSocket(NETLINK_ROUTE) {}
};

/**
 * This class implements the Connection interface with methods that wrap the recv() and recvfrom
 * system calls.
 */
class ConnectionImpl : public Connection {
public:
    explicit ConnectionImpl(const std::shared_ptr<Socket> &socket) : m_socket(socket) {}

    std::shared_ptr<Socket> socket() override { return m_socket; }
    int receive(Buffer &buffer) override
    {
        return ::recv(m_socket->fd(), buffer.data(), buffer.size(), MSG_DONTWAIT);
    }
    int receive_from(Buffer &buffer, SockAddr &address) override
    {
        socklen_t address_length = address.length();
        return ::recvfrom(m_socket->fd(), buffer.data(), buffer.size(), MSG_DONTWAIT,
                          address.sockaddr(), &address_length);
    }
    int send(const Buffer &buffer, size_t length) override
    {
        if (length > buffer.size()) {
            return -1;
        }
        return ::send(m_socket->fd(), buffer.data(), length, MSG_NOSIGNAL);
    }
    int send_to(const Buffer &buffer, size_t length, const SockAddr &address) override
    {
        return ::sendto(m_socket->fd(), buffer.data(), buffer.size(), 0, address.sockaddr(),
                        address.length());
    }

private:
    /**
     * Connected socket used by this connection object.
     */
    std::shared_ptr<Socket> m_socket;
};

class ServerSocketAbstractImpl : public ServerSocket {
public:
    int bind(const SockAddr &address)
    {
        return ::bind(m_socket->fd(), address.sockaddr(), address.length());
    }

    int listen(int backlog) { return ::listen(m_socket->fd(), backlog); }

    std::unique_ptr<Connection> accept(SockAddr &address) override
    {
        socklen_t address_length = address.length();
        int s                    = ::accept(m_socket->fd(), address.sockaddr(), &address_length);
        if (FileDescriptor::invalid_descriptor == s) {
            return nullptr;
        }

        return std::make_unique<ConnectionImpl>(std::make_shared<ConnectedSocket>(s));
    }

protected:
    explicit ServerSocketAbstractImpl(const std::shared_ptr<Socket> &socket) : m_socket(socket) {}
    std::shared_ptr<Socket> m_socket;
};

class ClientSocketAbstractImpl : public ClientSocket {
public:
    int bind(const SockAddr &address)
    {
        return ::bind(m_socket->fd(), address.sockaddr(), address.length());
    }

    // TODO: add connection timeout
    std::unique_ptr<Connection> connect(const SockAddr &address) override
    {
        if (0 != ::connect(m_socket->fd(), address.sockaddr(), address.length())) {
            return nullptr;
        }
        return std::make_unique<ConnectionImpl>(m_socket);
    }

protected:
    explicit ClientSocketAbstractImpl(const std::shared_ptr<Socket> &socket) : m_socket(socket) {}
    std::shared_ptr<Socket> m_socket;
};

class RawServerSocket : public ServerSocketAbstractImpl {
public:
    explicit RawServerSocket(const std::shared_ptr<RawSocket> &socket)
        : ServerSocketAbstractImpl(socket)
    {
    }
};

class UdpServerSocket : public ServerSocketAbstractImpl {
public:
    explicit UdpServerSocket(const std::shared_ptr<UdpSocket> &socket)
        : ServerSocketAbstractImpl(socket)
    {
    }
};

class UdpClientSocket : public ClientSocketAbstractImpl {
public:
    explicit UdpClientSocket(const std::shared_ptr<UdpSocket> &socket)
        : ClientSocketAbstractImpl(socket)
    {
    }
};

class TcpServerSocket : public ServerSocketAbstractImpl {
public:
    explicit TcpServerSocket(const std::shared_ptr<TcpSocket> &socket)
        : ServerSocketAbstractImpl(socket)
    {
    }
};

class TcpClientSocket : public ClientSocketAbstractImpl {
public:
    explicit TcpClientSocket(const std::shared_ptr<TcpSocket> &socket)
        : ClientSocketAbstractImpl(socket)
    {
    }
};

class UdsServerSocket : public ServerSocketAbstractImpl {
public:
    explicit UdsServerSocket(const std::shared_ptr<UdsSocket> &socket)
        : ServerSocketAbstractImpl(socket)
    {
    }
};

class UdsClientSocket : public ClientSocketAbstractImpl {
public:
    explicit UdsClientSocket(const std::shared_ptr<UdsSocket> &socket)
        : ClientSocketAbstractImpl(socket)
    {
    }
};

/**
 * prplmesh-democracy: Do you prefer templates?
 * These template classes would replace previous ServerSocket and ClientSocket derived classes
 */
template <class SocketType> class ServerSocketImpl : public ServerSocketAbstractImpl {
public:
    explicit ServerSocketImpl(const std::shared_ptr<SocketType> &socket)
        : ServerSocketAbstractImpl(socket)
    {
    }
};

template <class SocketType> class ClientSocketImpl : public ClientSocketAbstractImpl {
public:
    explicit ClientSocketImpl(const std::shared_ptr<SocketType> &socket)
        : ClientSocketAbstractImpl(socket)
    {
    }
};

template <class TimeUnits = std::chrono::milliseconds> class Timer : public FileDescriptor {
    /**
     * @brief Schedules the timer for repeated fixed-delay execution, beginning after the
     * specified initial delay.
     *
     * @param delay Delay before timer elapses for the first time.
     * @param period Time between successive timer executions.
     * @return True on success and false otherwise.
     */
    virtual bool schedule(TimeUnits delay, TimeUnits period) = 0;

    /**
     * @brief Terminates this timer.
     *
     * This method may be called repeatedly; the second and subsequent calls have no effect.
     *
     * @return True on success and false otherwise.
     */
    virtual bool cancel() = 0;

    /**
     * @brief Reads the number of expirations that have occurred since the timer was scheduled or
     * last read (whatever happened last).
     *
     * @param number_of_expirations The number of expirations occurred.
     * @return True on success and false otherwise.
     */
    virtual bool read(uint64_t &number_of_expirations) = 0;
};

template <class TimeUnits = std::chrono::milliseconds> class TimerImpl : public Timer<TimeUnits> {
public:
    TimerImpl() : m_descriptor(timerfd_create(CLOCK_MONOTONIC, 0)) {}
    int fd() override { return m_descriptor.fd(); }

    bool schedule(TimeUnits delay, TimeUnits period) override
    {
        return set_time(
            {.it_interval = duration_to_timespec(period), .it_value = duration_to_timespec(delay)});
    }

    virtual bool cancel() override { return set_time({}); }

    bool read(uint64_t &number_of_expirations) override
    {
        ssize_t bytes_read = ::read(fd(), &number_of_expirations, sizeof(number_of_expirations));
        if (bytes_read != sizeof(number_of_expirations)) {
            return false;
        }

        return true;
    }

private:
    FileDescriptorImpl m_descriptor;

    timespec duration_to_timespec(TimeUnits duration)
    {
        auto seconds     = std::chrono::duration_cast<std::chrono::seconds>(duration);
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

        return timespec({.tv_sec  = static_cast<time_t>(seconds.count()),
                         .tv_nsec = static_cast<long>(nanoseconds.count())});
    }

    bool set_time(const itimerspec &value)
    {
        return 0 == timerfd_settime(fd(), 0, &value, nullptr);
    }
};

} // namespace sockets

/**
 * A CMDU handler class is the equivalent in functionality to, for example, each of those methods
 * found in the backhaul manager that are used to handle the different types of messages.
 *
 * To test a particular implementation of CMDU handler, we must know which are its dependencies and
 * what does the handler do with them. All dependencies must be injected as parameters of the
 * handler constructor and the handler code must be programmed to interfaces, not to particular
 * implementations. A dependency that most if not all CMDU handlers will have is the socket
 * connection that the message was received on and that will be used to send the CMDU replies.
 * This way we can write tests for each implementation of the CMDU handler just by using mocks of
 * the dependencies and setting expectations on the mocks, that is, without sending any single
 * byte on a real socket connection.
 */
class CmduHandler {
public:
    virtual ~CmduHandler() = default;

    virtual void handle_message(const sockets::LinkLevelAddress &source_address,
                                ieee1905_1::CmduMessageRx &cmdu_rx) = 0;
};

/**
 * Option B: use a callback function (lambda) instead of class-derived-from-handler-interface.
 * Advantages:
 * - CmduHandler derived classes must be copyable or wrapped in a shared_ptr so they can be
 * installed. However if CmduHandler has been created in the stack, then it is not possible to
 * create a shared_ptr from it. A std::function is copyable
 * - std::function can be set to nullptr when the handler is optional and we don't want to specify
 * it or when we are done with it and want to remove the handler (shared_ptr too).
 * - Context required to run the std::function can be passed in through the capture clause of a
 * lambda function.
 * - It's possible and very easy to create a wrapper std::function around a member method and
 * install it.
 */
using CmduHandlerFunction = std::function<void(const sockets::LinkLevelAddress &source_address,
                                               ieee1905_1::CmduMessageRx &cmdu_rx)>;

/**
 * A message dispatcher is also a message handler so it can be used "instead of" a message handler.
 * This class is a replacement for which otherwise would be a very long switch-case statement. See
 * an example of such a switch statement in backhaul_manager::handle_1905_1_message()
 */
class DynamicCmduDispatcher : public CmduHandler {
public:
    /**
     * @brief Registers a handler for messages of given type.
     *
     * @param message_type The type of CMDU.
     * @param handler Message handler to register.
     * @return True on success and false otherwise (i.e.: there is an already registered handler
     * for messages of given type)
     */
    bool register_handler(ieee1905_1::eMessageType message_type,
                          const std::shared_ptr<CmduHandler> &handler)
    {
        // TODO
        return true;
    }

    /**
     * @brief Removes registered handler for messages of given type.
     *
     * @param message_type The type of CMDU.
     * @return True on success and false otherwise (i.e.: there is no registered handler for
     * messages of given type)
     */
    bool remove_handler(ieee1905_1::eMessageType message_type)
    {
        // TODO
        return true;
    }

    /**
     * @brief Handles CMDU message.
     *
     * Dispatches given message to the handler registered for its type of message, if any.
     *
     * @param source Address of message sender
     * @param cmdu_rx CMDU
     */
    void handle_message(const sockets::LinkLevelAddress &source_address,
                        ieee1905_1::CmduMessageRx &cmdu_rx) override
    {
        // TODO
    }

private:
    std::unordered_map<ieee1905_1::eMessageType, std::shared_ptr<CmduHandler>> m_handlers;
};

/**
 * Another version of the message dispatcher, more similar to backhaul_manager::handle_1905_1_message().
 * At the moment, we don't have any "dynamic" handling of messages so we can use a "static" list of
 * handlers.
 */
class StaticCmduDispatcher : public CmduHandler {
public:
    /**
     * @brief Handles CMDU message.
     *
     * Dispatches given message to the right handler for its type of message, if any.
     *
     * @param source Address of message sender
     * @param cmdu_rx CMDU
     */
    void handle_message(const sockets::LinkLevelAddress &source_address,
                        ieee1905_1::CmduMessageRx &cmdu_rx) override
    {
        // TODO: obtain message_type
        ieee1905_1::eMessageType message_type = 0;
        switch (message_type) {
        case 1:
            handle_message_1(cmdu_rx);
            break;
        case 2:
            handle_message_2(cmdu_rx);
            break;
        case 3:
            handle_message_3(cmdu_rx);
            break;
        default:
            // TODO: log error/warning
            break;
        }
    }

private:
    void handle_message_1(ieee1905_1::CmduMessageRx &cmdu_rx) {}
    void handle_message_2(ieee1905_1::CmduMessageRx &cmdu_rx) {}
    void handle_message_3(ieee1905_1::CmduMessageRx &cmdu_rx) {}
};

/**
 * Implementation of this interface will be used to parse messages out of an array of bytes
 * received through a socket connection.
 *
 * Different implementations of this interface can be provided, depending on if the socket used is
 * message-oriented or stream-oriented and, in the later case, which framing protocol is used to
 * delimiter the start and end of messages.
 *
 * Buffer contents can contain a full message or a fraction of it, depending on how many bytes were
 * ready for read when the recv() call on the socket was issued. Buffer contents might also contain
 * more than one message or one message and a fraction of the next one.
 *
 * If buffer contains a full CMDU, then CMDU parameter is filled in and returns true. This will be
 * always the case when using message-oriented sockets (for example UDS or UDP).
 *
 * On the contrary, when using stream-oriented sockets, buffer might not contain a full message.
 * In this case, this method must save given buffer contents as a fragment of the full message and
 * return false. The rest of the message is supposed to be provided in next calls to the parse()
 * method, but that is not granted and must be checked. An elaborated implementation might also
 * check how much time has elapsed between the arrival of the different fragments of a message
 * and discard rotten ones.
 *
 * To deal with the case where buffer contains more than one message, parsing method shall be
 * called in a loop until it returns false. The byte to start parsing from will be given in the
 * `offset` parameter, initialized to 0 in first call and from then on updated in each loop
 * iteration.
 *
 * Currently, we only send CMDU messages through message-oriented sockets and there are no plans
 * to use message-stream sockets for it so, for the moment, only one implementation is required
 * (the easy one).
 *
 * This interface and its implementations allow the separation of the logic around sockets and the
 * logic around message parsing. The goal of this separation is to be able to test message parsing
 * without having a peer connected at the other end of the wire, sending messages. During testing,
 * we can fill the buffer with any contents we want and pretend that those bytes have been
 * received through the socket connection. This way it is possible to test the parser with very odd
 * combinations of data that would otherwise be very difficult to set and reproduce. The only limit
 * for the tests we can write is our imagination.
 */
class CmduParser {
public:
    virtual ~CmduParser() = default;
    /**
     * Parses a CMDU out of a byte buffer.
     *
     * @param buffer array of bytes containing the message to parse.
     * @param offset position of the first byte in buffer to be parsed.
     * @param length number of bytes in the buffer.
     * @param[out] cmdu_rx parsed message
     * @return True if a message could be parsed out of the buffer and false otherwise (i.e.:
     * because only a fragment of the message was given)
     */
    virtual bool parse_message(const sockets::Buffer &buffer, size_t offset, size_t length,
                               ieee1905_1::CmduMessageRx &cmdu_rx) = 0;
};

class CmduParserImpl : public CmduParser {
public:
    bool parse_message(const sockets::Buffer &buffer, size_t offset, size_t length,
                       ieee1905_1::CmduMessageRx &cmdu_rx) override
    {
        // TODO
        return true;
    }
};

/**
 * This class is used to receive, parse and handle CMDU messages.
 *
 * This class is **testable** since all of its dependencies are injected in the constructor and is
 * coded against interfaces (it is not tied to concrete implementations).
 *
 * An identical class should be created to process other kind of messages like, for example, UCC
 * string configuration commands. Consider creating a template class instead of copying and
 * pasting code.
 */
class CmduProcessor {
public:
    /**
     * Class constructor.
     *
     * @param connection Socket connection on which CMDU messages are received.
     * @param loop Event loop (reactor design pattern) to wait for read events on socket connection.
     * @param parser CMDU message parser to build messages out of a sequence of bytes.
     * @param handler CMDU message handler to handle received messages.
     */
    CmduProcessor(const std::shared_ptr<sockets::Connection> &connection,
                  const std::shared_ptr<EventLoop<sockets::FileDescriptor>> &loop,
                  const std::shared_ptr<CmduParser> &parser,
                  const std::shared_ptr<CmduHandler> &handler)
        : m_connection(connection), m_loop(loop), m_parser(parser), m_handler(handler)
    {
    }

    bool start()
    {
        // TODO
        // setup event loop to register a read-event handler on socket
        /*
        EventHandlers handlers;
        handlers.onRead = [&](int fd, EventLoop *loop) {

            handle_read_event();

            return true;
        };

        m_loop->registerHandlers(m_connection->socket->fd(), -1, handlers);
        */
        return true;
    }

    bool stop()
    {
        // TODO
        // remove installed read-event handler from event loop
        /*
        m_loop->removeHandlers(m_connection->socket()->fd());
        */
        return true;
    }

private:
    void handle_read_event()
    {
        // TODO
        // - call connection.receive() to read the bytes of the message
        // - call parser.parse_message() to parse message out of bytes read.
        // - call handler.handle_message if full message was received and parsed.
        /*
        uint8_t data[1024];
        beerocks::sockets::BufferImpl buffer(data, sizeof(data));
        int bytes_received = m_connection->receive(buffer);

        ieee1905_1::CmduMessageRx cmdu_rx;
        if (m_parser->parse_message(buffer, bytes_received, cmdu_rx)) {
            m_handler->handle_message(cmdu_rx);
        }
        */
    }

private:
    std::shared_ptr<sockets::Connection> m_connection;
    std::shared_ptr<EventLoop> m_loop;
    std::shared_ptr<CmduParser> m_parser;
    std::shared_ptr<CmduHandler> m_handler;
};

class CmduBuilder {
};

} // namespace beerocks

#endif
