/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_MASTER_THREAD_H
#define _SON_MASTER_THREAD_H

#include "controller_ucc_listener.h"
#include "db/db.h"
#include "tasks/optimal_path_task.h"
#include "tasks/task_pool.h"

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_socket_thread.h>
#include <bcl/network/network_utils.h>

#include <mapf/common/encryption.h>
#include <tlvf/ieee_1905_1/tlvWscM1.h>
#include <tlvf/ieee_1905_1/tlvWscM2.h>
#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>

#include <btl/btl.h>

#include <cstddef>
#include <ctime>
#include <stdint.h>

namespace son {
class master_thread : public beerocks::btl::transport_socket_thread {

public:
    master_thread(const std::string &master_uds_, db &database_);
    virtual ~master_thread();

    virtual bool init() override;
    virtual bool work() override;

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual void before_select() override;
    virtual void after_select(bool timeout) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

private:
    bool handle_cmdu_1905_1_message(const std::string &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_control_message(const std::string &src_mac,
                                     std::shared_ptr<beerocks::beerocks_header> beerocks_header);
    void handle_cmdu_control_ieee1905_1_message(const std::string &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_intel_slave_join(const std::string &src_mac,
                                 std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
                                 ieee1905_1::CmduMessageRx &cmdu_rx,
                                 ieee1905_1::CmduMessageTx &cmdu_tx);
    bool
    handle_non_intel_slave_join(const std::string &src_mac,
                                std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
                                std::shared_ptr<ieee1905_1::tlvWscM1> tlvwscM1,
                                std::string bridge_mac, std::string radio_mac,
                                ieee1905_1::CmduMessageTx &cmdu_tx);

    bool construct_combined_infra_metric();

    // 1905 messages handlers
    bool handle_cmdu_1905_autoconfiguration_search(const std::string &src_mac,
                                                   ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_autoconfiguration_WSC(const std::string &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_link_metric_response(const std::string &src_mac,
                                               ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_ap_metric_response(const std::string &src_mac,
                                             ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_channel_preference_report(const std::string &src_mac,
                                                    ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_channel_selection_response(const std::string &src_mac,
                                                     ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_operating_channel_report(const std::string &src_mac,
                                                   ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_topology_discovery(const std::string &src_mac,
                                             ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_ack_message(const std::string &src_mac,
                                      ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_higher_layer_data_message(const std::string &src_mac,
                                                    ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_steering_completed_message(const std::string &src_mac,
                                                     ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_client_steering_btm_report_message(const std::string &src_mac,
                                                             ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_topology_notification(const std::string &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);

    bool autoconfig_wsc_parse_radio_caps(
        std::string radio_mac, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps);
    // Autoconfig encryption support
    bool autoconfig_wsc_add_m2(std::shared_ptr<ieee1905_1::tlvWscM1> m1,
                               const wireless_utils::sBssInfoConf *bss_info_conf);
    bool autoconfig_wsc_add_m2_encrypted_settings(std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                                  WSC::cConfigData &config_data,
                                                  uint8_t authkey[32], uint8_t keywrapkey[16]);
    bool autoconfig_wsc_authentication(std::shared_ptr<ieee1905_1::tlvWscM1> m1,
                                       std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                       uint8_t authkey[32]);
    bool autoconfig_wsc_calculate_keys(std::shared_ptr<ieee1905_1::tlvWscM1> m1,
                                       std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                       const mapf::encryption::diffie_hellman &dh,
                                       uint8_t authkey[32], uint8_t keywrapkey[16]);

    db &database;
    task_pool tasks;
    std::unique_ptr<beerocks::controller_ucc_listener> m_controller_ucc_listener;
};

} // namespace son

#endif
