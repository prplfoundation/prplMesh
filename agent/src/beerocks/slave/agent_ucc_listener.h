/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __AGENT_UCC_LISTENER_H__
#define __AGENT_UCC_LISTENER_H__

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_ucc_listener.h>
#include <beerocks/tlvf/beerocks_message.h>

#include <mutex>

namespace beerocks {

static const auto DEV_SET_ETH = std::string("eth");

// Forward decleration for backhaul_manager context saving
class backhaul_manager;

class agent_ucc_listener : public beerocks_ucc_listener {
public:
    agent_ucc_listener(backhaul_manager &backhaul_manager_ctx, uint16_t port,
                       const std::string &vendor, const std::string &model,
                       const std::string &bridge_iface, ieee1905_1::CmduMessageTx &cmdu);
    ~agent_ucc_listener(){};

    bool init() override;

    void lock() override { mutex.lock(); }
    void unlock() override { mutex.unlock(); }

    /** @brief Check if reset CAPI command was given.
     *
     * When the DEV_RESET_DEFAULT CAPI command is given, the agent must be held in reset until the
     * DEV_SET_CONFIG command is given. This function returns true when this is the case. It will
     * return true as soon as the DEV_SET_CONFIG command is given. At that point,
     * get_selected_backhaul() will return a valid result.
     */
    bool is_in_reset() const { return m_in_reset; }

    /** @brief Signal that reset is completed.
     *
     * The DEV_RESET_DEFAULT CAPI command should only return when reset is completed, i.e. when
     * the device is ready to accept commands. In particular, when later on a DEV_SET_CONFIG is
     * done, that command should be able to configure the backhaul right away. Therefore, the
     * DEV_RESET_DEFAULT command should only return when the backhaul is connected to the slaves
     * again. This function allows the backhaul to signal that it has reached that state.
     */
    void reset_completed() { m_reset_completed = true; }
    std::string get_selected_backhaul();
    void update_vaps_list(std::string ruid, beerocks_message::sVapsList &vaps);

private:
    std::string fill_version_reply_string() override;
    void clear_configuration() override;
    bool send_cmdu_to_destination(ieee1905_1::CmduMessageTx &cmdu_tx,
                                  const std::string &dest_mac = std::string()) override;
    bool handle_start_wps_registration(const std::string &band, std::string &err_string) override;
    bool handle_dev_set_config(std::unordered_map<std::string, std::string> &params,
                               std::string &err_string) override;
    bool handle_dev_get_param(std::unordered_map<std::string, std::string> &params,
                              std::string &value) override;

    backhaul_manager &m_backhaul_manager_ctx;

    const std::string &m_vendor;
    const std::string &m_model;
    const std::string &m_bridge_iface;
    std::string m_bridge_mac;

    bool m_in_reset        = false;
    bool m_reset_completed = false;
    std::string m_selected_backhaul; // "ETH" or "<RUID of the selected radio>"

    std::mutex mutex;
};

} // namespace beerocks

#endif // __AGENT_UCC_LISTENER_H__
