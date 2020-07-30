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

namespace beerocks {
namespace btl {
class transport_socket_thread : public socket_thread {
public:
    transport_socket_thread(const std::string &unix_socket_path_ = std::string());
    virtual ~transport_socket_thread();

    virtual bool init() override;
    virtual void set_select_timeout(unsigned msec) override;
    virtual bool work() override;

    /**
     * @brief Sends CDMU to transport for dispatching.
     *
     * @param cmdu Control Message Data Unit to send.
     * @param dst_mac Destination MAC address.
     * @param src_mac Source MAC address.
     * @param iface_name Name of the network interface to use (set to empty string to send on all
     * available interfaces).
     * @return True on success and false otherwise.
     */
    bool send_cmdu_to_broker(ieee1905_1::CmduMessageTx &cmdu, const std::string &dst_mac,
                             const std::string &src_mac, const std::string &iface_name = "");

protected:
    void add_socket(Socket *s, bool add_to_vector = true) override;
    bool configure_ieee1905_transport_interfaces(const std::string &bridge_iface,
                                                 const std::vector<std::string> &ifaces);

    bool from_broker(Socket *sd);

    bool broker_connect(const std::string &beerocks_temp_path, const bool local_master);
    bool broker_subscribe(const std::vector<ieee1905_1::eMessageType> &msg_types);

    /**
     * @brief Sends CDMU to transport for dispatching.
     *
     * @param cmdu Control Message Data Unit to send.
     * @param dst_mac Destination MAC address.
     * @param src_mac Source MAC address.
     * @param length Message length.
     * @param iface_name Name of the network interface to use (set to empty string to send on all
     * available interfaces).
     * @return True on success and false otherwise.
     */
    bool send_cmdu_to_broker(ieee1905_1::CmduMessage &cmdu, const std::string &dst_mac,
                             const std::string &src_mac, uint16_t length,
                             const std::string &iface_name = "");

private:
    bool broker_init();
    bool broker_send(ieee1905_1::CmduMessage &cmdu, const std::string &iface_name,
                     const std::string &dst_mac, const std::string &src_mac, uint16_t length);
    bool handle_cmdu_message_broker();

    int poll_timeout_ms = 500;

    std::unique_ptr<SocketClient> m_broker;
};
} // namespace btl

} // namespace beerocks

#endif //_BEEROCKS_MAPF_SOCKET_THREAD_H_
