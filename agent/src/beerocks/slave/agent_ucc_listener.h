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
#include <beerocks/tlvf/beerocks_message.h>

#include <mutex>

namespace beerocks {

static const auto DEV_SET_ETH = std::string("eth");

enum class eOnboardingState {
    NOT_IN_PROGRESS,
    WAIT_FOR_RESET,
    RESET_TO_DEFAULT,
    WAIT_FOR_CONFIG,
    IN_PROGRESS,
    SUCCESS,
    FAIL
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

    eOnboardingState get_and_update_onboarding_state();
    void set_onboarding_status(bool success);
    std::string get_selected_backhaul();
    void update_vaps_list(std::string ruid, beerocks_message::sVapsList &vaps);

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

    eOnboardingState m_onboarding_state = eOnboardingState::NOT_IN_PROGRESS;
    std::string m_selected_backhaul; // "ETH" or "<RUID of the selected radio>"
    std::unordered_map<std::string, beerocks_message::sVapsList> vaps_map;

    std::mutex mutex;
};

} // namespace beerocks

#endif // __AGENT_UCC_LISTENER_H__
