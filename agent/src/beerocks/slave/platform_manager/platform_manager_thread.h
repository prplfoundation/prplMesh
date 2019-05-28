/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _PLATFORM_MANAGER_THREAD_H
#define _PLATFORM_MANAGER_THREAD_H

#include <beerocks/bcl/beerocks_async_work_queue.h>
#include <beerocks/bcl/beerocks_config_file.h>
#include <beerocks/bcl/beerocks_logging.h>
#include <beerocks/bcl/beerocks_socket_thread.h>

#include "beerocks/tlvf/beerocks_message_common.h"

#include <bpl_arp.h>
#include <bpl_cfg.h>

#include <unordered_map>
#include <unordered_set>

namespace beerocks {
namespace platform_manager {

extern bool extern_set_iface_state(std::string iface, bool state);
extern bool extern_set_radio_state(std::string iface, bool state); //to be removed in WLANRTSYS-6776
extern std::string extern_query_db(std::string parameter);

class main_thread : public socket_thread {

public:
    main_thread(config_file::sConfigSlave config_, logging &logger_);
    ~main_thread();

    struct platform_common_conf_t {
        bool conf_initialized = false;
        int rdkb_extensions;
        int band_steering;
        int client_roaming;
        int onboarding;
        int wired_backhaul;
        int operating_mode;
        int local_gw;
        int local_master;
        int backhaul_max_vaps;
        int backhaul_network_enabled;
        int backhaul_prefered_radio_band;
        int dfs_reentry;
        int passive_mode;
    };

    virtual bool init() override;
    virtual bool work() override;

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual void after_select(bool timeout);
    virtual void on_thread_stop() override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;

    bool handle_arp_monitor();
    bool handle_arp_raw();

private:
    void add_slave_socket(Socket *sd, const std::string &iface_name);
    void del_slave_socket(Socket *sd);
    bool send_cmdu_safe(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx);
    std::string get_hostap_iface_name_from_slave_socket(Socket *sd);
    Socket *get_slave_socket_from_hostap_iface_name(const std::string &iface);

    Socket *get_backhaul_socket();
    void load_iface_params(const std::string &strIface, beerocks::eArpSource eType);
    std::string bridge_iface_from_mac(const net::sMacAddr &sMac);
    void send_dhcp_notification(std::string op, std::string mac, std::string ip,
                                std::string hostname);
    void arp_entries_cleanup();
    bool init_dhcp_monitor();
    bool init_arp_monitor();

    void send_slave_iface_status_to_bpl(bool bforce = false);
    bool slave_iface_status_check(const std::string &iface_name, uint8_t status,
                                  bool operational = false);
    bool platform_operational_state_check();
    bool wlan_params_changed_check();

private:
    const int PLATFORM_READ_CONF_RETRY_SEC    = 10;
    const int PLATFORM_READ_CONF_MAX_ATTEMPTS = 10;

    config_file::sConfigSlave config;

    struct SIfaceParams {
        beerocks::eArpSource eType;
    };

    struct SArpEntry {
        uint32_t ip;
        int iface_index;
        std::chrono::steady_clock::time_point last_seen;
    };

    // Connected slaves (socket/interface index)
    std::unordered_map<Socket *, std::string> m_mapSlaves; // value=iface_name
    std::mutex m_mtxSlaves;

    // Interfaces
    std::unordered_map<std::string, SIfaceParams> m_mapIfaces; // key=iface_name

    // ARP entries by source MAC (uint64_t) address
    std::unordered_map<uint64_t, std::shared_ptr<SArpEntry>> m_mapArpEntries;
    std::chrono::steady_clock::time_point m_tpArpEntriesCleanup;

    // Pointer to the backhaul manager slave
    Socket *m_pBackhaulManagerSlave = nullptr;

    BPL_ARP_MON_CTX m_ctxArpMon = nullptr;
    Socket *m_pArpMonSocket     = nullptr;
    Socket *m_pArpRawSocket     = nullptr;
    Socket *m_pDHCPMonSocket    = nullptr;

    uint32_t m_uiArpMonIP;
    uint32_t m_uiArpMonMask;

    logging &logger;
    bool enable_arp_monitor;
    bool is_onboarding_on_init;

    std::string master_version;
    std::string slave_version;

    beerocks::async_work_queue work_queue;

    struct slave_iface_status {
        int index;
        int status;
        bool operational = false;
        std::chrono::steady_clock::time_point last_seen;

        slave_iface_status(int index_, int status_,
                           std::chrono::steady_clock::time_point last_seen_)
            : index(index_), status(status_), last_seen(last_seen_)
        {
        }
    };

    bool platform_operational = false;
    std::unordered_map<std::string, std::shared_ptr<slave_iface_status>> bpl_iface_status_map;
    std::unordered_map<std::string, std::shared_ptr<beerocks_message::sWlanSettings>>
        bpl_iface_wlan_params_map;
    std::unordered_set<std::string> ap_ifaces;
    BPL_INTERFACE_STATUS_NOTIFICATION bpl_iface_status;

    platform_common_conf_t platform_common_conf;
};

} // namespace platform_manager
} // namespace beerocks

#endif // _PLATFORM_MANAGER_THREAD_H
