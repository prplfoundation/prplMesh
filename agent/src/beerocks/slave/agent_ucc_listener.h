/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __AGENT_UCC_LISTENER_H__
#define __AGENT_UCC_LISTENER_H__

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_ucc_listener.h>

#include <mutex>

namespace beerocks {
class agent_ucc_listener : public beerocks_ucc_listener {
public:
    agent_ucc_listener(uint16_t port, const std::string &vendor, const std::string &model,
                       const std::string &bridge_iface, SocketClient **controller_sd);
    ~agent_ucc_listener(){};

    bool init() override;

    void lock() override { mutex.lock(); }
    void unlock() override { mutex.unlock(); }

    bool dev_reset_default_needed() { return m_dev_reset_default; }
    bool dev_reset_default_inprogress() { return m_dev_reset_default_inprogress; }
    void set_dev_reset_default_inprogress()
    {
        m_dev_reset_default            = false;
        m_dev_reset_default_inprogress = true;
    }
    void set_dev_reset_default_complete()
    {
        m_dev_reset_default_inprogress = false;
        reply_ucc(eWfaCaStatus::COMPLETE);
    }

private:
    std::string fill_version_reply_string() override;
    void clear_configuration() override;
    Socket *get_dev_send_1905_destination_socket(const std::string &dest_alid) override;
    std::shared_ptr<uint8_t> get_buffer_filled_with_cmdu() override;
    bool send_cmdu_to_destination(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx) override;
    bool handle_dev_set_config(std::unordered_map<std::string, std::string> &params,
                               std::string &err_string) override;

    const std::string &m_vendor;
    const std::string &m_model;
    const std::string &m_bridge_iface;
    std::string m_bridge_mac;
    SocketClient **const m_controller_sd;
    bool m_dev_reset_default            = false;
    bool m_dev_reset_default_inprogress = false;

    std::mutex mutex;
};

} // namespace beerocks

#endif // __AGENT_UCC_LISTENER_H__
