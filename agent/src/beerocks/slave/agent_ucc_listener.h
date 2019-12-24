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

enum class eOnboardingState {
    eONBOARDING_NOT_IN_PROGRESS,
    eONBOARDING_RESET_TO_DEFAULT,
    eONBOARDING_WAIT_FOR_CONFIG,
    eONBOARDING_IN_PROGRESS,
    eONBOARDING_SUCCESS,
    eONBOARDING_FAIL
};

// Forward decleration for backhaul_manager context saving
class backhaul_manager;

class agent_ucc_listener : public beerocks_ucc_listener {
public:
    agent_ucc_listener(backhaul_manager &backhaul_manager_ctx, uint16_t port,
                       const std::string &vendor, const std::string &model,
                       const std::string &bridge_iface);
    ~agent_ucc_listener(){};

    bool init() override;

    void lock() override { mutex.lock(); }
    void unlock() override { mutex.unlock(); }

private:
    std::string fill_version_reply_string() override;
    void clear_configuration() override;
    bool validate_destination_alid(const std::string &dest_alid) override;
    std::shared_ptr<uint8_t> get_buffer_filled_with_cmdu() override;
    bool send_cmdu_to_destination(ieee1905_1::CmduMessageTx &cmdu_tx,
                                  const std::string &dest_mac = std::string()) override;
    bool handle_dev_set_config(std::unordered_map<std::string, std::string> &params,
                               std::string &err_string) override;

    backhaul_manager &m_backhaul_manager_ctx;

    const std::string &m_vendor;
    const std::string &m_model;
    const std::string &m_bridge_iface;
    std::string m_bridge_mac;

    eOnboardingState m_onboarding_state = eOnboardingState::eONBOARDING_NOT_IN_PROGRESS;
    std::string m_selected_backhaul; // "ETH" or "<RUID of the selected radio>"

    std::mutex mutex;
};

} // namespace beerocks

#endif // __AGENT_UCC_LISTENER_H__
