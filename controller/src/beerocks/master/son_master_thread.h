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
#include <tlvf/WSC/configData.h>
#include <tlvf/WSC/m1.h>
#include <tlvf/WSC/m2.h>
#include <tlvf/ieee_1905_1/tlvWsc.h>
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
                                 beerocks::beerocks_header &beerocks_header,
                                 ieee1905_1::CmduMessageTx &cmdu_tx);
    bool
    handle_non_intel_slave_join(const std::string &src_mac,
                                std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
                                WSC::m1 &m1, std::string bridge_mac, std::string radio_mac,
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
    bool handle_cmdu_1905_ap_capability_report(const std::string &src_mac,
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
    bool handle_cmdu_1905_client_capability_report_message(const std::string &src_mac,
                                                           ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_topology_notification(const std::string &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);

    bool autoconfig_wsc_parse_radio_caps(
        std::string radio_mac, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps);
    // Autoconfig encryption support
    bool autoconfig_wsc_add_m2(WSC::m1 &m1, const wireless_utils::sBssInfoConf *bss_info_conf);
    bool autoconfig_wsc_add_m2_encrypted_settings(WSC::m2::config &m2_cfg,
                                                  WSC::configData &config_data, uint8_t authkey[32],
                                                  uint8_t keywrapkey[16]);
    bool autoconfig_wsc_authentication(WSC::m1 &m1, WSC::m2 &m2, uint8_t authkey[32]);
    void autoconfig_wsc_calculate_keys(WSC::m1 &m1, WSC::m2::config &m2_cfg,
                                       const mapf::encryption::diffie_hellman &dh,
                                       uint8_t authkey[32], uint8_t keywrapkey[16]);

    bool is_prplmesh(const std::string &mac);

    db &database;
    task_pool tasks;
    beerocks::controller_ucc_listener m_controller_ucc_listener;
};

} // namespace son

#endif
