/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_MAPF_SOCKET_THREAD_H_
#define _BEEROCKS_MAPF_SOCKET_THREAD_H_

#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_socket_thread.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <tlvf/ieee_1905_1/eMessageType.h>

#include <unordered_set>

// Forward Declarations
namespace mapf {
class Poller;
class LocalBusInterface;
class SubSocket;
} // namespace mapf

namespace beerocks {
namespace btl {
class transport_socket_thread : public socket_thread {
public:
    transport_socket_thread(const std::string &unix_socket_path_ = std::string());
    virtual ~transport_socket_thread();

    virtual bool init() override;
    virtual void set_select_timeout(unsigned msec) override;
    virtual bool work() override;

    bool send_cmdu_to_bus(ieee1905_1::CmduMessageTx &cmdu, const std::string &dst_mac,
                          const std::string &src_mac);

    /**
     * @brief Sends CDMU to transport for dispatching.
     *
     * @param cmdu Control Message Data Unit to send.
     * @param iface_index Network interface index (set to 0 to send on all interfaces).
     * @param dst_mac Destination MAC address.
     * @param src_mac Source MAC address.
     * @return True on success and false otherwise.
     */
    bool send_cmdu_to_bus(ieee1905_1::CmduMessageTx &cmdu, uint32_t iface_index,
                          const std::string &dst_mac, const std::string &src_mac);

protected:
    void add_socket(Socket *s, bool add_to_vector = true) override;
    void remove_socket(Socket *s) override;
    bool read_ready(Socket *s) override;
    bool configure_ieee1905_transport_interfaces(const std::string &bridge_iface,
                                                 const std::vector<std::string> &ifaces);
    bool from_bus(Socket *sd);

    bool bus_subscribe(const std::vector<ieee1905_1::eMessageType> &msg_types);
    bool bus_connect(const std::string &beerocks_temp_path, const bool local_master);
    void bus_connected(Socket *sd);

    /**
     * @brief Sends CDMU to transport for dispatching.
     *
     * @param cmdu Control Message Data Unit to send.
     * @param iface_index Network interface index (set to 0 to send on all interfaces).
     * @param dst_mac Destination MAC address.
     * @param src_mac Source MAC address.
     * @param length Message length.
     * @return True on success and false otherwise.
     */
    bool send_cmdu_to_bus(ieee1905_1::CmduMessage &cmdu, uint32_t iface_index,
                          const std::string &dst_mac, const std::string &src_mac, uint16_t length);

private:
    bool bus_init();
    bool bus_send(ieee1905_1::CmduMessage &cmdu, uint32_t iface_index, const std::string &dst_mac,
                  const std::string &src_mac, uint16_t length);
    bool handle_cmdu_message_bus();

    int poll_timeout_ms = 500;

#ifdef UDS_BUS
    bool skip_filtered_message_type(Socket *sd, ieee1905_1::eMessageType msg_type) override;

    Socket *bus = nullptr;
    std::unique_ptr<SocketServer> bus_server_socket;
    std::unordered_set<ieee1905_1::eMessageType> m_subscribed_messages;
#else
    std::shared_ptr<mapf::LocalBusInterface> bus = nullptr;
    std::shared_ptr<mapf::Poller> poller         = nullptr;
#endif
};
} // namespace btl

} // namespace beerocks

#endif //_BEEROCKS_MAPF_SOCKET_THREAD_H_
