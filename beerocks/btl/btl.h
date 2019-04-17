/*
* INTEL CONFIDENTIAL
* Copyright 2019 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#ifndef _BEEROCKS_MAPF_SOCKET_THREAD_H_
#define _BEEROCKS_MAPF_SOCKET_THREAD_H_

#include <beerocks/bcl/beerocks_socket_thread.h>
#include <beerocks/bcl/beerocks_message_structs.h>

#include <tlvf/ieee_1905_1/eMessageType.h>
#include <beerocks/tlvf/beerocks_message.h>

// Forward Declarations
namespace mapf {
    class Poller;
    class LocalBusInterface;
    class SubSocket;
}

namespace beerocks
{
    namespace btl
    {
        class transport_socket_thread : public socket_thread
        {
            public:
                transport_socket_thread(const std::string& unix_socket_path_ = std::string());
                virtual ~transport_socket_thread();

                virtual bool init() override;
                virtual void set_select_timeout(unsigned msec) override;
                virtual bool work() override;

            protected:

                void add_socket(Socket *s, bool add_to_vector = true) override;
                void remove_socket(Socket *s) override;
                bool read_ready(Socket *s) override;
                bool configure_ieee1905_transport_interfaces(const std::string& bridge_iface, const std::vector<std::string>& ifaces);
                bool from_bus(Socket *sd);

                bool bus_subscribe(const std::vector<ieee1905_1::eMessageType>& msg_types);
                bool bus_connect(const std::string& beerocks_temp_path, const bool local_master);
                void bus_connected(Socket *sd);

                bool send_cmdu_to_bus(ieee1905_1::CmduMessageTx& cmdu, const std::string& dst_mac, const std::string& src_mac);
                bool send_cmdu_to_bus(ieee1905_1::CmduMessage& cmdu, const std::string& dst_mac, const std::string& src_mac, uint16_t length);

                static const std::string MULTICAST_MAC_ADDR;

            private:
                void bus_init();
                bool bus_send(ieee1905_1::CmduMessage& cmdu, const std::string& dst_mac, const std::string& src_mac, uint16_t length);
                bool handle_cmdu_message_bus();

                int poll_timeout_ms = 500;

#ifdef UDS_BUS
                Socket* bus = nullptr;
#else
                std::shared_ptr<mapf::LocalBusInterface> bus = nullptr;
                std::shared_ptr<mapf::Poller> poller = nullptr;
#endif
        };
    } // namespace btl

} // namespace beerocks

#endif //_BEEROCKS_MAPF_SOCKET_THREAD_H_
