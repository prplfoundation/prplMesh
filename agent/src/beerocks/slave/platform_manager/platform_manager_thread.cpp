/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "platform_manager_thread.h"

#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

#include <bpl/bpl_dhcp.h>

#include <net/if.h> // if_nametoindex

using namespace beerocks::net;

namespace beerocks {
namespace platform_manager {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define SELECT_TIMEOUT_MSC 500

static const uint8_t s_arrZeroMac[]  = {0, 0, 0, 0, 0, 0};
static const uint8_t s_arrBCastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define ARP_NOTIF_INTERVAL (90000)                  // 1.5 minutes
#define ARP_CLEAN_INTERVAL (ARP_NOTIF_INTERVAL * 2) // 2 notif. intervals

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static beerocks::eFreqType bpl_band_to_freq_type(int bpl_band)
{
    if (bpl_band == BPL_RADIO_BAND_2G) {
        return beerocks::eFreqType::FREQ_24G;
    } else if (bpl_band == BPL_RADIO_BAND_5G) {
        return beerocks::eFreqType::FREQ_5G;
    } else if (bpl_band == BPL_RADIO_BAND_AUTO) {
        return beerocks::eFreqType::FREQ_AUTO;
    } else {
        return beerocks::eFreqType::FREQ_UNKNOWN;
    }
}

static bool fill_platform_settings(
    std::string iface_name,
    std::shared_ptr<beerocks_message::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE> msg,
    main_thread::platform_common_conf_t &platform_common_conf,
    std::unordered_map<std::string, std::shared_ptr<beerocks_message::sWlanSettings>>
        &iface_wlan_params_map,
    Socket *sd)
{
    if (bpl::cfg_get_beerocks_credentials(BPL_RADIO_FRONT, msg->platform_settings().front_ssid,
                                          msg->platform_settings().front_pass,
                                          msg->platform_settings().front_security_type) < 0) {
        LOG(ERROR) << "Failed reading front Wi-Fi credentials!";
        return false;
    }

    LOG(DEBUG) << "Front Credentials:"
               << " ssid=" << msg->platform_settings().front_ssid
               << " sec=" << msg->platform_settings().front_security_type << " pass=***";

    if (bpl::cfg_get_beerocks_credentials(BPL_RADIO_BACK, msg->platform_settings().back_ssid,
                                          msg->platform_settings().back_pass,
                                          msg->platform_settings().back_security_type) < 0) {
        LOG(ERROR) << "Failed reading Wi-Fi back credentials!";
        return false;
    }

    int mem_only_psk = bpl::cfg_get_security_policy();
    if (mem_only_psk < 0) {
        LOG(ERROR) << "Failed reading Wi-Fi security policy!";
        return false;
    }

    msg->platform_settings().mem_only_psk = mem_only_psk;

    LOG(DEBUG) << "Back Credentials:"
               << " ssid=" << msg->platform_settings().back_ssid
               << " sec=" << msg->platform_settings().back_security_type
               << " mem_only_psk=" << int(msg->platform_settings().mem_only_psk) << " pass=***";

    struct bpl::BPL_WLAN_PARAMS params;
    if (bpl::cfg_get_wifi_params(iface_name.c_str(), &params) < 0) {
        LOG(ERROR) << "Failed reading '" << iface_name << "' parameters!";
        return false;
    }
    /* update message */
    msg->wlan_settings().band_enabled = params.enabled;
    msg->wlan_settings().channel      = params.channel;
    string_utils::copy_string(msg->wlan_settings().ssid, params.ssid,
                              beerocks::message::WIFI_SSID_MAX_LENGTH);
    string_utils::copy_string(msg->wlan_settings().pass, params.passphrase,
                              beerocks::message::WIFI_PASS_MAX_LENGTH);
    string_utils::copy_string(msg->wlan_settings().security_type, params.security,
                              beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH);

    LOG(DEBUG) << "wlan settings:"
               << " ssid=" << msg->wlan_settings().ssid
               << " sec=" << msg->wlan_settings().security_type << " pass=***"
               << " channel=" << int(msg->wlan_settings().channel);

    // initialize wlan params cache
    //erase interface cache from map if exists
    iface_wlan_params_map.erase(iface_name);
    auto params_ptr = std::make_shared<beerocks_message::sWlanSettings>();
    if (!params_ptr) {
        LOG(ERROR) << "Failed creating shared pointer";
        return false;
    }

    params_ptr->band_enabled = params.enabled;
    params_ptr->channel      = params.channel;
    string_utils::copy_string(params_ptr->ssid, params.ssid,
                              beerocks::message::WIFI_SSID_MAX_LENGTH);
    string_utils::copy_string(params_ptr->pass, params.passphrase,
                              beerocks::message::WIFI_PASS_MAX_LENGTH);
    string_utils::copy_string(params_ptr->security_type, params.security,
                              beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH);

    iface_wlan_params_map[iface_name] = params_ptr;

    LOG(DEBUG) << "iface=" << iface_name << " added to wlan params change check";

    if (!platform_common_conf.conf_initialized) {

        const int back_vaps_buff_len =
            BPL_BACK_VAPS_GROUPS * BPL_BACK_VAPS_IN_GROUP * BPL_MAC_ADDR_OCTETS_LEN;
        char back_vaps[back_vaps_buff_len];

        if ((platform_common_conf.onboarding = bpl::cfg_is_onboarding()) < 0) {
            LOG(ERROR) << "Failed reading 'onboarding'";
            return false;
        }
        if ((platform_common_conf.rdkb_extensions = bpl::cfg_get_rdkb_extensions()) < 0) {
            LOG(ERROR) << "Failed reading 'rdkb_extensions'";
            return false;
        }
        if ((platform_common_conf.band_steering = bpl::cfg_get_band_steering()) < 0) {
            LOG(ERROR) << "Failed reading 'band_steering'";
            return false;
        }
        if ((platform_common_conf.client_roaming = bpl::cfg_get_client_roaming()) < 0) {
            LOG(ERROR) << "Failed reading 'client_roaming";
            return false;
        }
        if ((platform_common_conf.local_master = bpl::cfg_is_master()) < 0) {
            LOG(ERROR) << "Failed reading 'local_master'";
            return false;
        }
        if ((platform_common_conf.operating_mode = bpl::cfg_get_operating_mode()) < 0) {
            LOG(ERROR) << "Failed reading 'operating_mode'";
            return false;
        }
        if ((platform_common_conf.certification_mode = bpl::cfg_get_certification_mode()) < 0) {
            LOG(ERROR) << "Failed reading 'certification_mode'";
            return false;
        }
        if ((platform_common_conf.stop_on_failure_attempts =
                 bpl::cfg_get_stop_on_failure_attempts()) < 0) {
            LOG(ERROR) << "Failed reading 'stop_on_failure_attempts'";
            return false;
        }
        if ((platform_common_conf.dfs_reentry = bpl::cfg_get_dfs_reentry()) < 0) {
            LOG(ERROR) << "Failed reading 'dfs_reentry'";
            return false;
        }

        if (bpl::cfg_get_backhaul_params(&platform_common_conf.backhaul_max_vaps,
                                         &platform_common_conf.backhaul_network_enabled,
                                         &platform_common_conf.backhaul_preferred_radio_band) < 0) {
            LOG(ERROR) << "Failed reading 'backhaul_max_vaps, backhaul_network_enabled, "
                          "backhaul_preferred_radio_band'!";
        }

        if (bpl::cfg_get_backhaul_vaps(back_vaps, back_vaps_buff_len) < 0) {
            LOG(ERROR) << "Failed reading beerocks backhaul_vaps parameters!";
            return false;
        }

        // set local_gw flag
        platform_common_conf.local_gw =
            (platform_common_conf.operating_mode == BPL_OPER_MODE_GATEWAY ||
             platform_common_conf.operating_mode == BPL_OPER_MODE_GATEWAY_WISP);

        // fill backhaul vaps
        auto *p             = back_vaps;
        int num_of_elements = sizeof(beerocks_message::sPlatformSettings::backhaul_vaps_bssid) /
                              sizeof(beerocks_message::sPlatformSettings::backhaul_vaps_bssid[0]);
        for (int i = 0; i < num_of_elements; i++) {
            std::copy_n(p, BPL_MAC_ADDR_OCTETS_LEN,
                        msg->platform_settings().backhaul_vaps_bssid[i].oct);
            p += BPL_MAC_ADDR_OCTETS_LEN;
        }

        // mark initialization flag
        platform_common_conf.conf_initialized = true;
    }

    msg->platform_settings().onboarding          = uint8_t(platform_common_conf.onboarding);
    msg->platform_settings().dfs_reentry_enabled = uint8_t(platform_common_conf.dfs_reentry);
    msg->platform_settings().rdkb_extensions_enabled =
        uint8_t(platform_common_conf.rdkb_extensions);
    msg->platform_settings().client_band_steering_enabled =
        uint8_t(platform_common_conf.band_steering);
    msg->platform_settings().client_optimal_path_roaming_enabled =
        uint8_t(platform_common_conf.client_roaming);
    msg->platform_settings().client_optimal_path_roaming_prefer_signal_strength_enabled =
        0; // TODO add platform DB flag
    msg->platform_settings().client_11k_roaming_enabled =
        uint8_t(platform_common_conf.client_roaming || platform_common_conf.band_steering);
    msg->platform_settings().local_gw           = uint8_t(platform_common_conf.local_gw);
    msg->platform_settings().operating_mode     = uint8_t(platform_common_conf.operating_mode);
    msg->platform_settings().certification_mode = uint8_t(platform_common_conf.certification_mode);
    msg->platform_settings().stop_on_failure_attempts =
        uint8_t(platform_common_conf.stop_on_failure_attempts);
    msg->platform_settings().local_master      = uint8_t(platform_common_conf.local_master);
    msg->platform_settings().backhaul_max_vaps = uint8_t(platform_common_conf.backhaul_max_vaps);
    msg->platform_settings().backhaul_network_enabled =
        uint8_t(platform_common_conf.backhaul_network_enabled);
    msg->platform_settings().backhaul_preferred_radio_band =
        uint8_t(bpl_band_to_freq_type(platform_common_conf.backhaul_preferred_radio_band));

    msg->platform_settings().load_balancing_enabled   = 0; // for v1.3 TODO read from CAL DB
    msg->platform_settings().service_fairness_enabled = 0; // for v1.3 TODO read from CAL DB

    LOG(DEBUG) << "iface " << iface_name << " settings:";
    LOG(DEBUG) << "onboarding: " << (unsigned)msg->platform_settings().onboarding;
    LOG(DEBUG) << "client_band_steering_enabled: "
               << (unsigned)msg->platform_settings().client_band_steering_enabled;
    LOG(DEBUG) << "client_optimal_path_roaming_enabled: "
               << (unsigned)msg->platform_settings().client_optimal_path_roaming_enabled;
    LOG(DEBUG) << "client_optimal_path_roaming_prefer_signal_strength_enabled: "
               << (unsigned)msg->platform_settings()
                      .client_optimal_path_roaming_prefer_signal_strength_enabled;
    LOG(DEBUG) << "band_enabled: " << (unsigned)msg->wlan_settings().band_enabled;
    LOG(DEBUG) << "local_gw: " << (unsigned)msg->platform_settings().local_gw;
    LOG(DEBUG) << "local_master: " << (unsigned)msg->platform_settings().local_master;
    LOG(DEBUG) << "dfs_reentry_enabled: " << (unsigned)msg->platform_settings().dfs_reentry_enabled;
    LOG(DEBUG) << "backhaul_preferred_radio_band: "
               << (unsigned)msg->platform_settings().backhaul_preferred_radio_band;

    return true;
}

std::string extern_query_db(std::string parameter)
{
    std::string ret;
    if (bpl::bpl_init() < 0) {
        ret = "Failed to initialize BPL!";
    } else {
        if (parameter == "is_master") {
            ret = (bpl::cfg_is_master() > 0 ? "true" : "false");
        } else if (parameter == "is_gateway") {
            auto operating_mode = bpl::cfg_get_operating_mode();
            ret                 = (operating_mode == BPL_OPER_MODE_GATEWAY ||
                           operating_mode == BPL_OPER_MODE_GATEWAY_WISP
                       ? "true"
                       : "false");
        } else if (parameter == "is_onboarding") {
            ret = (bpl::cfg_is_onboarding() > 0 ? "true" : "false");
        } else {
            ret = "Error, bad parameter.\n"
                  "Allowed parameters: \n"
                  " is_master \n"
                  " is_gateway \n"
                  " is_onboarding \n";
        }
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

main_thread::main_thread(config_file::sConfigSlave config_, logging &logger_)
    : socket_thread(config_.temp_path + std::string(BEEROCKS_PLAT_MGR_UDS)), config(config_),
      logger(logger_)
{
    set_select_timeout(SELECT_TIMEOUT_MSC);
    thread_name        = "platform_manager";
    enable_arp_monitor = (config.enable_arp_monitor == "1");

    std::memset(&bpl_iface_status, 0, sizeof(bpl::BPL_INTERFACE_STATUS_NOTIFICATION));

    //copy the AP interface names and station interface names for the BPL to send the interface status
    auto now = std::chrono::steady_clock::now();
    int i    = 0;
    for (int j = 0; j < IRE_MAX_SLAVES && i < BPL_NUM_OF_INTERFACES; ++j) {
        if (config.hostap_iface[j].empty())
            continue;

        auto status = std::make_shared<slave_iface_status>(i, beerocks::eRadioStatus::INVALID, now);
        if (!status) {
            LOG(ERROR) << "failed to allocate shared pointer, i=" << i;
            continue;
        }

        string_utils::copy_string(bpl_iface_status.ifname[i], config.hostap_iface[j].c_str(),
                                  BPL_IFNAME_LEN);
        bpl_iface_status_map[config.hostap_iface[j]] = status;
        ap_ifaces.insert(config.hostap_iface[j]);
        i++;
    }
    for (int j = 0; j < IRE_MAX_SLAVES && i < BPL_NUM_OF_INTERFACES; ++j) {
        if (config.sta_iface[j].empty())
            continue;

        auto status = std::make_shared<slave_iface_status>(i, beerocks::eRadioStatus::INVALID, now);
        if (!status) {
            LOG(ERROR) << "failed to allocate shared pointer, i=" << i;
            continue;
        }

        string_utils::copy_string(bpl_iface_status.ifname[i], config.sta_iface[j].c_str(),
                                  BPL_IFNAME_LEN);
        bpl_iface_status_map[config.sta_iface[j]] = status;
        i++;
    }

    auto status = std::make_shared<slave_iface_status>(i, beerocks::eRadioStatus::INVALID, now);
    if (!status) {
        LOG(ERROR) << "failed to allocate shared pointer, i=" << i;
        return;
    }
    bpl_iface_status_map[config.backhaul_wire_iface] = status;
    string_utils::copy_string(bpl_iface_status.ifname[i], config.backhaul_wire_iface.c_str(),
                              BPL_IFNAME_LEN);
}

main_thread::~main_thread() { on_thread_stop(); }

void main_thread::add_slave_socket(Socket *sd, const std::string &iface_name)
{
    // Lock the slaves socket map
    std::unique_lock<std::mutex> lock(m_mtxSlaves);

    m_mapSlaves[sd] = iface_name;
}

void main_thread::del_slave_socket(Socket *sd)
{
    // Lock the slaves socket map
    std::unique_lock<std::mutex> lock(m_mtxSlaves);

    // Remove the socket from the connections map
    m_mapSlaves.erase(sd);

    // Also check if that was the backhaul manager
    if (m_pBackhaulManagerSlave == sd)
        m_pBackhaulManagerSlave = nullptr;
}

bool main_thread::send_cmdu_safe(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx)
{
    // Lock the slaves socket map
    std::unique_lock<std::mutex> lock(m_mtxSlaves);

    if (m_mapSlaves.find(sd) == m_mapSlaves.end()) {
        LOG(ERROR) << "Attempted send to invalid socket slave: " << sd;
        return false;
    }

    message_com::send_cmdu(sd, cmdu_tx);
    return true;
}

std::string main_thread::get_hostap_iface_name_from_slave_socket(Socket *sd)
{
    // Lock the slaves socket map
    std::unique_lock<std::mutex> lock(m_mtxSlaves);

    auto slave_it = m_mapSlaves.find(sd);
    if (slave_it == m_mapSlaves.end()) {
        LOG(ERROR) << "Invalid socket slave: " << sd;
        return std::string();
    }

    return slave_it->second; // iface_name
}

Socket *main_thread::get_slave_socket_from_hostap_iface_name(const std::string &iface)
{
    auto it_slave = std::find_if(
        m_mapSlaves.begin(), m_mapSlaves.end(),
        [&iface](const std::pair<Socket *, std::string> &slave) { return iface == slave.second; });

    if (it_slave != m_mapSlaves.end()) {
        return it_slave->first;
    }

    return nullptr;
}

Socket *main_thread::get_backhaul_socket()
{
    // Lock the slaves socket map
    std::unique_lock<std::mutex> lock(m_mtxSlaves);

    // If a slave containing the backhaul manager registered, return its' socket.
    // If not, return the first socket from the connection map
    Socket *sd = nullptr;

    if (m_pBackhaulManagerSlave)
        sd = m_pBackhaulManagerSlave;
    else if (m_mapSlaves.size())
        sd = m_mapSlaves.begin()->first;

    return (sd);
}

void main_thread::load_iface_params(const std::string &strIface, beerocks::eArpSource eType)
{
    // Ignore empty interfaces
    if (strIface.empty()) {
        LOG(ERROR) << "strIface is empty!";
        return;
    }

    LOG(DEBUG) << "load_iface_params(), Interface " << strIface;

    m_mapIfaces[strIface] = {eType}; // Struct initialization
}

std::string main_thread::bridge_iface_from_mac(const sMacAddr &sMac)
{
    char iface_name[BPL_ARP_IFACE_NAME_LEN];

    // Read the interface name using BPL
    if (bpl::arp_get_bridge_iface(config.bridge_iface.c_str(), sMac.oct, iface_name) == -1) {
        return {};
    }

    return std::string(iface_name);
}

void main_thread::send_dhcp_notification(std::string op, std::string mac, std::string ip,
                                         std::string hostname)
{
    LOG(DEBUG) << "DHCP Event: " << op << ", mac: " << mac << ", ip: " << ip
               << ", hostname: " << hostname;
    auto dhcp_notif = message_com::create_vs_message<
        beerocks_message::cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION>(cmdu_tx);

    if (dhcp_notif == nullptr) {
        LOG(ERROR) << "Failed building ACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION message!";
        return;
    }

    // Build the DHCP notification message
    if (op == "add")
        dhcp_notif->op() = beerocks_message::eDHCPOp_Add;
    else if (op == "del")
        dhcp_notif->op() = beerocks_message::eDHCPOp_Del;
    else if (op == "old")
        dhcp_notif->op() = beerocks_message::eDHCPOp_Old;

    dhcp_notif->mac()  = network_utils::mac_from_string(mac);
    dhcp_notif->ipv4() = network_utils::ipv4_from_string(ip);
    string_utils::copy_string(dhcp_notif->hostname(0), hostname.c_str(), message::NODE_NAME_LENGTH);

    // Get a slave socket
    Socket *sd = get_backhaul_socket();

    if (sd) {
        message_com::send_cmdu(sd, cmdu_tx);
    }
}

bool main_thread::init()
{
    // Initialize the BPL (Beerocks Platform Library)
    if (bpl::bpl_init() < 0) {
        LOG(ERROR) << "Failed to initialize BPL!";
        return (false);
    }

    // Bridge & Backhaul interface
    is_onboarding_on_init = bpl::cfg_is_onboarding();
    if (is_onboarding_on_init) {
        LOG(DEBUG) << "Onboarding state.";
        for (int slave_num = 0; slave_num < IRE_MAX_SLAVES; slave_num++) {
            load_iface_params(config.sta_iface[slave_num], beerocks::ARP_SRC_WIRELESS_BACK);
        }
    } else {
        LOG(DEBUG) << "Non-onboarding state.";
        load_iface_params(config.bridge_iface, beerocks::ARP_SRC_ETH_FRONT);
        load_iface_params(config.backhaul_wire_iface, beerocks::ARP_SRC_ETH_BACK);
        for (int slave_num = 0; slave_num < IRE_MAX_SLAVES; slave_num++) {
            load_iface_params(config.sta_iface[slave_num], beerocks::ARP_SRC_WIRELESS_BACK);
        }
    }

    // Start the async work queue
    if (!work_queue.start()) {
        LOG(ERROR) << "Failed starting asynchronous work queue";
        return false;
    }

    return socket_thread::init();
}

bool main_thread::work()
{
    bool bret = socket_thread::work();

    return bret;
}

bool main_thread::slave_iface_status_check(const std::string &iface_name, uint8_t status,
                                           bool operational)
{
    bool bret = false;
    auto now  = std::chrono::steady_clock::now();
    auto it   = bpl_iface_status_map.find(iface_name);
    if (it != bpl_iface_status_map.end() && it->second != nullptr) {

        if (it->second->status != status) {
            bret = true;
        }

        //update the latest values in the map
        it->second->last_seen   = now;
        it->second->status      = status;
        it->second->operational = operational;
        return bret;
    }
    return false;
}

bool main_thread::platform_operational_state_check()
{
    bool new_operational_state = true;

    for (auto iface_status : bpl_iface_status_map) {
        if (ap_ifaces.find(iface_status.first) == ap_ifaces.end())
            continue; // iface is not an ap
        new_operational_state = (iface_status.second->operational) ? new_operational_state : false;
    }

    bool ret             = platform_operational != new_operational_state;
    platform_operational = new_operational_state;
    return ret;
}

bool main_thread::wlan_params_changed_check()
{
    bool any_slave_changed = false;
    for (auto &elm : bpl_iface_wlan_params_map) {
        if (elm.second == nullptr) {
            LOG(ERROR) << "invalid map - pointer to NULL";
            return false;
        }
        bool wlan_params_changed = false;
        struct bpl::BPL_WLAN_PARAMS params;
        if (bpl::cfg_get_wifi_params(elm.first.c_str(), &params) < 0) {
            LOG(ERROR) << "Failed reading '" << elm.first << "' parameters!";
            return false;
        }

        if (elm.second->band_enabled != params.enabled) {
            elm.second->band_enabled = params.enabled;
            LOG(DEBUG) << "band_enabled changed";
            wlan_params_changed = true;
        }
        if (elm.second->channel != params.channel) {
            elm.second->channel = params.channel;
            LOG(DEBUG) << "channel changed";
            wlan_params_changed = true;
        }
        if (std::string(elm.second->ssid).compare(std::string(params.ssid))) {
            LOG(DEBUG) << "ssid changed, cached=" << std::string(elm.second->ssid) << ", new="
                       << std::string(params.ssid, beerocks::message::WIFI_SSID_MAX_LENGTH);
            string_utils::copy_string(elm.second->ssid, params.ssid,
                                      beerocks::message::WIFI_SSID_MAX_LENGTH);
            wlan_params_changed = true;
        }
        if (std::string(elm.second->pass).compare(std::string(params.passphrase))) {
            LOG(DEBUG) << "pass changed";
            string_utils::copy_string(elm.second->pass, params.passphrase,
                                      beerocks::message::WIFI_PASS_MAX_LENGTH);
            wlan_params_changed = true;
        }
        if (std::string(elm.second->security_type).compare(std::string(params.security))) {
            LOG(DEBUG) << "security_type changed, cached=" << std::string(elm.second->security_type)
                       << ", new="
                       << std::string(params.security,
                                      beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH);
            string_utils::copy_string(elm.second->security_type, params.security,
                                      beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH);
            wlan_params_changed = true;
        }

        if (wlan_params_changed) {
            any_slave_changed = true;
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            notification->wlan_settings().band_enabled = elm.second->band_enabled;
            notification->wlan_settings().channel      = elm.second->channel;
            string_utils::copy_string(notification->wlan_settings().ssid, elm.second->ssid,
                                      beerocks::message::WIFI_SSID_MAX_LENGTH);
            string_utils::copy_string(notification->wlan_settings().pass, elm.second->pass,
                                      beerocks::message::WIFI_PASS_MAX_LENGTH);
            string_utils::copy_string(notification->wlan_settings().security_type,
                                      elm.second->security_type,
                                      beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH);

            Socket *sd = get_slave_socket_from_hostap_iface_name(elm.first);
            if (!sd) {
                LOG(ERROR) << "failed to get slave socket from iface=" << elm.first;
                continue;
            }

            send_cmdu_safe(sd, cmdu_tx);
            LOG(DEBUG) << "wlan_params_changed - cmdu msg sent, iface=" << elm.first
                       << " cmdu msg sent, sd=" << intptr_t(sd);
        }
    }
    return any_slave_changed;
}

void main_thread::send_slave_iface_status_to_bpl(bool bforce)
{
    if (config.enable_bpl_iface_status_notifications != "1") {
        return;
    }
    for (auto &elm : bpl_iface_status_map) {
        auto saved_status = elm.second;
        if (saved_status != nullptr) {
            bpl_iface_status.status[saved_status->index] = saved_status->status;
            bpl_iface_status.operational_status          = platform_operational;
        }
    }

    // Notify the SL asynchronously
    work_queue.enqueue<void>(
        [](bpl::BPL_INTERFACE_STATUS_NOTIFICATION bpl_iface_status) {
            bpl::cfg_notify_iface_status(&bpl_iface_status);
        },
        bpl_iface_status);
}

void main_thread::on_thread_stop()
{
    LOG(DEBUG) << "Platform_manager on_thread_stop()";

    LOG(DEBUG) << "Stopping asynchronous work queue...";
    work_queue.stop(true);

    if (bpl::dhcp_mon_stop() == false) {
        LOG(ERROR) << "Failed stopping DHCP Monitor!";
    } else {
        LOG(DEBUG) << "DHCP Monitor Stopped.";
    }

    // Stop the ARP Monitor
    stop_arp_monitor();

    if (m_pDHCPMonSocket) {
        remove_socket(m_pDHCPMonSocket);
        delete m_pDHCPMonSocket;
        m_pDHCPMonSocket = nullptr;
    }

    bpl::bpl_close();
    LOG(DEBUG) << "Closed BPL.";

    bpl_iface_status_map.clear();

    bpl_iface_wlan_params_map.clear();
}

bool main_thread::socket_disconnected(Socket *sd)
{
    auto it = m_mapSlaves.find(sd);
    if (it == m_mapSlaves.end()) {
        LOG(INFO) << "non slave socket disconnected! (probably backhaul manager)";
        return true;
    }
    LOG(DEBUG) << "slave socket_disconnected, iface=" << m_mapSlaves[sd] << ", sd=" << sd
               << ", is_platform_operational = " << platform_operational;

    // we should have only one per sd
    bpl_iface_wlan_params_map.erase(m_mapSlaves[sd]);

    del_slave_socket(sd);

    if (platform_operational) {
        platform_common_conf.conf_initialized = false;
    }

    return (true);
}

std::string main_thread::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

bool main_thread::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    if (beerocks_header->action() != beerocks_message::ACTION_PLATFORM) {
        LOG(ERROR) << "Unknown message, action: " << int(beerocks_header->action());
        return true;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST: {
        LOG(TRACE) << "ACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST failed";
            return false;
        }
        // Interface params
        std::string strIfaceName = std::string(request->iface_name(message::IFACE_NAME_LENGTH));
        LOG(DEBUG) << "Registering slave with interface = " << strIfaceName;

        add_slave_socket(sd, strIfaceName);

        work_queue.enqueue<void>([this, strIfaceName, sd]() {
            size_t headroom = sizeof(beerocks::message::sUdsHeader);
            size_t buffer_size =
                headroom + message_com::get_vs_cmdu_size_on_buffer<
                               beerocks_message::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE>();
            uint8_t tx_buffer[buffer_size];
            ieee1905_1::CmduMessageTx cmdu_tx(tx_buffer + headroom, sizeof(tx_buffer) - headroom);

            //Response message (empty for now)
            auto register_response = message_com::create_vs_message<
                beerocks_message::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE>(cmdu_tx);

            if (register_response == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return;
            }

            std::chrono::steady_clock::time_point retry_timer = std::chrono::steady_clock::now();
            uint8_t retry_cnt                                 = 0;

            register_response->valid() = 0;
            do {
                if (std::chrono::steady_clock::now() > retry_timer) {
                    LOG(TRACE) << "Trying to read settings of iface:" << strIfaceName
                               << ", attempt=" << int(retry_cnt);
                    if (fill_platform_settings(strIfaceName, register_response,
                                               platform_common_conf, bpl_iface_wlan_params_map,
                                               sd)) {
                        register_response->valid() = 1;
                    } else {
                        LOG(INFO) << "Reading settings of iface:" << strIfaceName
                                  << ", attempt=" << int(retry_cnt) << " has failed!";
                        if (++retry_cnt == PLATFORM_READ_CONF_MAX_ATTEMPTS)
                            break;
                        retry_timer = std::chrono::steady_clock::now() +
                                      std::chrono::seconds(PLATFORM_READ_CONF_RETRY_SEC);
                    }
                }
            } while (!register_response->valid());

            LOG(DEBUG) << "sending ACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE to " << strIfaceName
                       << " sd=" << intptr_t(sd);
            send_cmdu_safe(sd, cmdu_tx);
        });

    } break;

    case beerocks_message::ACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL: {
        LOG(TRACE) << "ACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL failed";
            return false;
        }
        logger.set_log_level_state((beerocks::eLogLevel)request->params().log_level,
                                   request->params().enable);

    } break;

    case beerocks_message::ACTION_PLATFORM_ARP_QUERY_REQUEST: {
        LOG(TRACE) << "ACTION_PLATFORM_ARP_QUERY_REQUEST";

        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_PLATFORM_ARP_QUERY_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_ARP_QUERY_REQUEST failed";
            return false;
        }

        // Send a probe request to the ARP monitor
        if (!m_ctxArpMon) {
            LOG(WARNING) << "ARP monitor NOT active...";
            break;
        }

        if (bpl::arp_mon_probe(m_ctxArpMon, request->params().mac.oct, request->params().ipv4.oct,
                               beerocks_header->id()) != 0) {
            LOG(DEBUG) << "ARP probe failed!";
            break;
        }

        // Add the MAC to the arp entries map
        if (m_mapArpEntries.find(request->params().mac) == m_mapArpEntries.end()) {
            auto pArpEntry = std::make_shared<SArpEntry>();

            // Only the IP address is initialized at this point
            pArpEntry->ip = network_utils::uint_ipv4_from_array(&request->params().ipv4.oct);
            pArpEntry->iface_index = -1;
            pArpEntry->last_seen   = std::chrono::steady_clock::now();

            // LOG(DEBUG) << "UINT64 --> MAC: " << network_utils::mac_to_string(
            //     (const uint8_t*)pArpEntry->ip);

            LOG(DEBUG) << "Adding MAC " << request->params().mac << " to the ARP list...";

            m_mapArpEntries[request->params().mac] = pArpEntry;
        }

    } break;

    case beerocks_message::ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST: {
        // Request message
        LOG(TRACE) << "ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST";

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        memset(response->params().user_password, 0, message::USER_PASS_LEN);

        char pass[BPL_USER_PASS_LEN];

        if (bpl::cfg_get_administrator_credentials(pass) < 0) {
            LOG(ERROR) << "Failed reading administrator credentials!";
            response->result() = 0;
        } else {
            response->result() = 1;
        }

        string_utils::copy_string(response->params().user_password, pass, message::USER_PASS_LEN);

        // Sent with unsafe because BML is reachable only on platform thread
        message_com::send_cmdu(sd, cmdu_tx);

        //clear the pwd in the memory
        memset(&pass, 0, sizeof(pass));
        //memset might be optimized and compiler might not set it 0 if its not used after memset
        *(volatile char *)pass = *(volatile char *)pass;

    } break;
    case beerocks_message::ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE failed";
            return false;
        }
        if (!master_version.empty() && !slave_version.empty()) {
            string_utils::copy_string(response->versions().master_version, master_version.c_str(),
                                      message::VERSION_LENGTH);
            string_utils::copy_string(response->versions().slave_version, slave_version.c_str(),
                                      message::VERSION_LENGTH);
            response->result() = 1;
        } else {
            response->result() = 0;
        }

        message_com::send_cmdu(sd, cmdu_tx);

    } break;
    case beerocks_message::ACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION failed";
            return false;
        }
        std::string master_version(notification->versions().master_version);
        std::string slave_version(notification->versions().slave_version);

        // Notify the SL asynchronously
        work_queue.enqueue<void>([master_version, slave_version]() {
            LOG(DEBUG) << "call bpl_version_mismatch_notification"
                       << ", master = " << master_version << ", slave = " << slave_version;

            bpl::cfg_notify_fw_version_mismatch();
        });

    } break;
    case beerocks_message::ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION failed";
            return false;
        }
        master_version.assign(notification->versions().master_version);
        slave_version.assign(notification->versions().slave_version);
    } break;
    case beerocks_message::ACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION: {

        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION failed";
            return false;
        }
        bool check_wifi_ap =
            slave_iface_status_check(notification->iface_name_ap(message::IFACE_NAME_LENGTH),
                                     notification->status_ap(), notification->status_operational());
        bool check_wifi_bh     = false;
        bool check_wired_bh    = false;
        bool check_operational = platform_operational_state_check();
        if (!platform_common_conf.local_gw && notification->is_bh_manager()) {
            check_wifi_bh = slave_iface_status_check(
                notification->iface_name_bh(message::IFACE_NAME_LENGTH), notification->status_bh());
            check_wired_bh = slave_iface_status_check(config.backhaul_wire_iface,
                                                      notification->status_bh_wired());
        }
        //if the status has changed from prev status or the timer has expired, send the status to BPL
        if (check_wifi_ap || check_wifi_bh || check_wired_bh || check_operational) {
            LOG(INFO)
                << "ACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION - change in iface status "
                << " iface_name_ap: " << notification->iface_name_ap(message::IFACE_NAME_LENGTH)
                << " iface_name_bh: " << notification->iface_name_bh(message::IFACE_NAME_LENGTH)
                << " is_bh_manager: " << (int)notification->is_bh_manager()
                << " BH_wire_iface: " << config.backhaul_wire_iface
                << " status_bh_wired: " << (int)notification->status_bh_wired() << " check_wifi_ap "
                << check_wifi_ap << " check_wifi_bh " << check_wifi_bh << " check_wired_bh "
                << check_wired_bh << " check_operational " << check_operational;

            if (check_operational) {
                LOG(INFO) << "***platform operational state chenged to: "
                          << (platform_operational ? "true" : "false");
                if (!platform_operational) {
                    platform_common_conf.conf_initialized = false;
                }

                // Response message
                auto notification = message_com::create_vs_message<
                    beerocks_message::cACTION_PLATFORM_OPERATIONAL_NOTIFICATION>(cmdu_tx);
                if (notification == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return false;
                }

                notification->operational() = platform_operational;

                send_cmdu_safe(sd, cmdu_tx);
            }

            send_slave_iface_status_to_bpl(true);
        }
    } break;

    case beerocks_message::ACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION";
        auto notification = beerocks_header->addClass<
            beerocks_message::
                cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass "
                          "cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION "
                          "failed";
            return false;
        }
        if (notification->is_backhaul_manager()) {
            LOG(DEBUG) << "slave is backhaul manager, updating";
            m_pBackhaulManagerSlave = sd;

            // Start ARP monitor
            if (enable_arp_monitor) {
                if (!init_arp_monitor()) {
                    LOG(ERROR) << "can't start ARP monitor";
                    return false;
                }
            }

            // Start DHCP monitor
            if (platform_common_conf.local_gw) {
                if (!init_dhcp_monitor()) {
                    LOG(ERROR) << "can't start DHCP monitor";
                    return false;
                }
            }
        }
    } break;

    case beerocks_message::ACTION_PLATFORM_ONBOARD_QUERY_REQUEST: {
        LOG(TRACE) << "ACTION_PLATFORM_ONBOARD_QUERY_REQUEST";
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building ONBOARD RESPONSE message!";
            return false;
        }
        response->params().onboarding = bpl::cfg_is_onboarding();

        // Sent with unsafe because BML is reachable only on platform thread
        message_com::send_cmdu(sd, cmdu_tx);

    } break;

    case beerocks_message::ACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST: {
        LOG(TRACE) << "ACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST";
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        // Read the value from CAL
        response->local_master() = bpl::cfg_is_master();

        // Sent with unsafe because BML is reachable only on platform thread
        message_com::send_cmdu(sd, cmdu_tx);

    } break;

    case beerocks_message::ACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST: {
        // Request message
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST failed";
            break;
        }

        //TODO use vap_id, for now assume vap_id == MAIN_VAP
        LOG(TRACE) << "ACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST, vap_id="
                   << int(request->vap_id());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        auto fill_credentials_msg = [](const int radio_dir, char *msg_ssid, char *msg_pass,
                                       uint8_t &msg_sec) -> bool {
            memset(msg_ssid, 0, message::WIFI_SSID_MAX_LENGTH);
            memset(msg_pass, 0, message::WIFI_PASS_MAX_LENGTH);

            char ssid[BPL_SSID_LEN];
            char pass[BPL_PASS_LEN];
            char sec[BPL_SEC_LEN];

            if (bpl::cfg_get_beerocks_credentials(radio_dir, ssid, pass, sec) < 0) {
                LOG(ERROR) << "Failed reading Wi-Fi credentials!";
                return false;
            } else {
                std::string sec_string(sec);
                if (sec_string == BPL_WLAN_SEC_NONE_STR) {
                    msg_sec = beerocks_message::eWiFiSec_None;
                } else if (sec_string == BPL_WLAN_SEC_WEP64_STR) {
                    msg_sec = beerocks_message::eWiFiSec_WEP64;
                } else if (sec_string == BPL_WLAN_SEC_WEP128_STR) {
                    msg_sec = beerocks_message::eWiFiSec_WEP128;
                } else if (sec_string == BPL_WLAN_SEC_WPA_PSK_STR) {
                    msg_sec = beerocks_message::eWiFiSec_WPA_PSK;
                } else if (sec_string == BPL_WLAN_SEC_WPA2_PSK_STR) {
                    msg_sec = beerocks_message::eWiFiSec_WPA2_PSK;
                } else if (sec_string == BPL_WLAN_SEC_WPA_WPA2_PSK_STR) {
                    msg_sec = beerocks_message::eWiFiSec_WPA_WPA2_PSK;
                } else {
                    msg_sec = beerocks_message::eWiFiSec_None;
                    LOG(WARNING) << "Unsupported Wi-Fi Security: " << sec_string;
                    return (false);
                }
            }

            string_utils::copy_string(msg_ssid, ssid, message::WIFI_SSID_MAX_LENGTH);
            string_utils::copy_string(msg_pass, pass, message::WIFI_PASS_MAX_LENGTH);

            //clear the pwd in the memory
            memset(&pass, 0, sizeof(pass));

            //memset might be optimized and compiler might not set it 0 if its not used after memset
            *(volatile char *)pass = *(volatile char *)pass;

            return true;
        };

        response->result() =
            fill_credentials_msg(BPL_RADIO_FRONT, response->front_params().ssid,
                                 response->front_params().pass, response->front_params().sec);
        response->result() =
            fill_credentials_msg(BPL_RADIO_BACK, response->front_params().ssid,
                                 response->front_params().pass, response->front_params().sec);

        LOG(INFO) << "fSSID: " << response->front_params().ssid
                  << " fPASS: " << response->front_params().pass
                  << " fSEC: " << response->front_params().sec;
        LOG(INFO) << "bSSID: " << response->front_params().ssid
                  << " bPASS: " << response->front_params().pass
                  << " bSEC: " << response->front_params().sec;

        // Sent with unsafe because BML is reachable only on platform thread
        message_com::send_cmdu(sd, cmdu_tx);

    } break;

    case beerocks_message::ACTION_PLATFORM_ONBOARD_SET_REQUEST: {
        LOG(TRACE) << "ACTION_PLATFORM_ONBOARD_SET_REQUEST";
        // Request message
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_PLATFORM_ONBOARD_SET_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_ONBOARD_SET_REQUEST failed";
            break;
        }

        LOG(INFO) << "Set onboarding to " << std::to_string(request->params().onboarding);
        bpl::cfg_set_onboarding(int(request->params().onboarding));

        LOG(INFO) << "Success onboarding " << std::to_string(bpl::cfg_is_onboarding());
        // No response message is needed
    } break;

    case beerocks_message::ACTION_PLATFORM_WPS_ONBOARDING_REQUEST: {
        LOG(TRACE) << "ACTION_PLATFORM_WPS_ONBOARDING_REQUEST";

        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_PLATFORM_WPS_ONBOARDING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_PLATFORM_WPS_ONBOARDING_REQUEST failed";
            break;
        }

        std::string iface = request->iface_name(message::IFACE_NAME_LENGTH);

        work_queue.enqueue<void>(
            [this](std::string iface) {
                bpl::cfg_notify_onboarding_completed("SSID", "PASSWORD", "SECURITY", iface.c_str(),
                                                     0);
            },
            iface);
    } break;

    case beerocks_message::ACTION_PLATFORM_DEVICE_INFO_GET_REQUEST: {
        // Request message
        LOG(TRACE) << "ACTION_PLATFORM_DEVICE_INFO_GET_REQUEST";

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        auto &params = response->params();
        memset(&params, 0, sizeof(params));

        bpl::BPL_DEVICE_INFO bpl_device_info;

        if (bpl::cfg_get_device_info(&bpl_device_info) < 0) {
            LOG(ERROR) << "Failed reading device information!";
            response->result() = 0;
        } else {
            response->result() = 1;
        }

        string_utils::copy_string(params.manufacturer, bpl_device_info.manufacturer,
                                  message::DEV_INFO_STR_MAX_LEN);
        string_utils::copy_string(params.model_name, bpl_device_info.model_name,
                                  message::DEV_INFO_STR_MAX_LEN);
        string_utils::copy_string(params.serial_number, bpl_device_info.serial_number,
                                  message::DEV_INFO_STR_MAX_LEN);

        // LAN
        string_utils::copy_string(params.lan_iface_name, bpl_device_info.lan_iface_name,
                                  message::IFACE_NAME_LENGTH);
        params.lan_ip_address   = bpl_device_info.lan_ip_address;
        params.lan_network_mask = bpl_device_info.lan_network_mask;

        // WAN
        string_utils::copy_string(params.wan_iface_name, bpl_device_info.wan_iface_name,
                                  message::IFACE_NAME_LENGTH);
        params.wan_ip_address   = bpl_device_info.wan_ip_address;
        params.wan_network_mask = bpl_device_info.wan_network_mask;

        // Sent with unsafe because BML is reachable only on platform thread
        message_com::send_cmdu(sd, cmdu_tx);

    } break;

    case beerocks_message::ACTION_PLATFORM_ERROR_NOTIFICATION: {
        auto error =
            beerocks_header->addClass<beerocks_message::cACTION_PLATFORM_ERROR_NOTIFICATION>();
        if (error == nullptr) {
            LOG(ERROR) << "addClass failed";
            break;
        }

        uint32_t error_code = error->code();
        std::string error_data(error->data(0));

        // Notify the SL asynchronously
        work_queue.enqueue<void>([error_code, error_data]() {
            LOG(DEBUG) << "PLATFORM ERROR NOTIFICATION - Code: " << error_code
                       << ", Data: " << error_data;

            bpl::cfg_notify_error(error_code, error_data.c_str());
        });

    } break;
    default: {
        LOG(ERROR) << "Unknown PLATFORM_MANAGER message, action_op: "
                   << int(beerocks_header->action_op());
        return (false);
    }
    }

    return true;
}

bool main_thread::handle_arp_monitor()
{
    auto arp_notif =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION>(
            cmdu_tx);
    if (arp_notif == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    // Process the message
    bpl::BPL_ARP_MON_ENTRY entry;
    if (bpl::arp_mon_process(m_ctxArpMon, &entry) != 0) {
        LOG(ERROR) << "Failed processing ARP monitor message!";
        return (false);
    }

    // Ignore IPs outside the monitored network, zeroed MACs or invalid state
    if (((network_utils::uint_ipv4_from_array(entry.ip) & m_uiArpMonMask) !=
         (m_uiArpMonIP & m_uiArpMonMask)) ||
        (!memcmp(entry.mac, s_arrZeroMac, sizeof(entry.mac))) ||
        (!memcmp(entry.mac, s_arrBCastMac, sizeof(entry.mac))) ||
        (entry.state == beerocks::ARP_NUD_FAILED)) {

        // LOG(DEBUG) << "Ignoring ARP from: "
        //            << network_utils::ipv4_to_string(entry.ip) << " ("
        //            << entry.mac << ")"
        //            << ", state: " << int(entry.state)
        //            << ", type: " << int(entry.type);

        return true;
    }

    // Copy entry values
    std::copy_n(entry.mac, sizeof(sMacAddr::oct), arp_notif->params().mac.oct);
    std::copy_n(entry.ip, sizeof(sIpv4Addr::oct), arp_notif->params().ipv4.oct);
    arp_notif->params().iface_idx = entry.iface_idx;
    arp_notif->params().state     = entry.state;
    arp_notif->params().source    = entry.source;
    arp_notif->params().type      = entry.type;

    // After processing the message, copy the ipv4 and mac as strings
    std::string client_ipv4 = network_utils::ipv4_to_string(arp_notif->params().ipv4);
    std::string client_mac  = network_utils::mac_to_string(arp_notif->params().mac);

    Socket *sd       = nullptr;
    auto iIfaceIndex = arp_notif->params().iface_idx;

    char if_name_buffer[IF_NAMESIZE];
    if (!if_indextoname(iIfaceIndex, if_name_buffer)) {
        LOG(ERROR) << "Failed to find iface of iface_index" << int(iIfaceIndex);
        return false;
    }

    std::string strIfaceName(if_name_buffer);

    sd = get_slave_socket_from_hostap_iface_name(strIfaceName);

    // Use the Backhaul Manager Slave as the default destination
    if ((sd == nullptr) && ((sd = get_backhaul_socket()) == nullptr)) {
        LOG(WARNING) << "Failed obtaining slave socket";
        return false;
    }

    auto it_iface = m_mapIfaces.find(strIfaceName);
    if (it_iface != m_mapIfaces.end()) {
        auto &pIfaceParams         = it_iface->second;
        arp_notif->params().source = pIfaceParams.eType;

    } else if (entry.type != ARP_TYPE_DELNEIGH) {

        std::string mac  = network_utils::mac_to_string(arp_notif->params().mac);
        std::string ipv4 = network_utils::ipv4_to_string(arp_notif->params().ipv4);
        LOG(WARNING) << "Interface index " << int(iIfaceIndex) << " not found! mac=" << mac
                     << ", ipv4=" << ipv4;
        return (false);
    }

    std::string source = ((arp_notif->params().source == beerocks::ARP_SRC_ETH_BACK) ||
                          (arp_notif->params().source == beerocks::ARP_SRC_WIRELESS_BACK))
                             ? "BACK"
                             : "FRONT";

    // Send the message to the Slave
    LOG(INFO) << "ARP - Interface: " << strIfaceName
              << ", State: " << int(arp_notif->params().state) << ", IP: " << client_ipv4 << " ("
              << client_mac << ")"
              << ", Source: " << source << ", Type: " << int(arp_notif->params().type);

    // Check if the master should be notified
    bool fSendNotif = true;
    if (entry.type != ARP_TYPE_DELNEIGH) {
        auto pArpEntry = m_mapArpEntries.find(arp_notif->params().mac);

        if (pArpEntry != m_mapArpEntries.end()) {

            auto now                = std::chrono::steady_clock::now();
            auto last_seen_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                          now - pArpEntry->second->last_seen)
                                          .count();

            // Check for IP/Inteface changes
            if ((pArpEntry->second->ip !=
                 network_utils::uint_ipv4_from_array(arp_notif->params().ipv4.oct)) ||
                (pArpEntry->second->iface_index != int(arp_notif->params().iface_idx)) ||
                (last_seen_duration >= ARP_NOTIF_INTERVAL)) {

                // Update the entry
                pArpEntry->second->ip =
                    network_utils::uint_ipv4_from_array(arp_notif->params().ipv4.oct);
                pArpEntry->second->iface_index = arp_notif->params().iface_idx;
                pArpEntry->second->last_seen   = now;

                LOG(DEBUG) << "Client last seen " << last_seen_duration
                           << " milliseconds ago. Sending notification!";

            } else {
                // Do NOT send notification
                fSendNotif = false;

                LOG(DEBUG) << "Client last seen " << last_seen_duration
                           << " milliseconds ago. Skipping notification...";
            }
        }
    }

    // Send the message to the slave
    if (sd && fSendNotif) {
        send_cmdu_safe(sd, cmdu_tx);
    }

    return (true);
}

bool main_thread::handle_arp_raw()
{
    // Skip invalid ARP packets
    bpl::BPL_ARP_MON_ENTRY entry;
    int task_id = bpl::arp_mon_process_raw_arp(m_ctxArpMon, &entry);
    if (task_id == 0) {
        // task-id equals to 0  means nodes list is empty
        // not an error, but no further work is required
        return true;
    } else if (task_id < 0) {
        return false;
    }

    auto arp_resp =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ARP_QUERY_RESPONSE>(
            cmdu_tx, task_id);

    if (arp_resp == nullptr) {
        LOG(ERROR) << "Failed building cACTION_PLATFORM_ARP_QUERY_RESPONSE message!";
        return false;
    }

    // Copy entry values
    std::copy_n(entry.mac, sizeof(arp_resp->params().mac.oct), arp_resp->params().mac.oct);
    std::copy_n(entry.ip, sizeof(arp_resp->params().ipv4.oct), arp_resp->params().ipv4.oct);
    arp_resp->params().iface_idx = entry.iface_idx;
    arp_resp->params().state     = entry.state;
    arp_resp->params().source    = entry.source;

    std::string strIface = bridge_iface_from_mac(arp_resp->params().mac);

    if (strIface.empty())
        return (false);

    // Find the interface index and source
    SIfaceParams *pIfaceParams = nullptr;
    auto it                    = m_mapIfaces.find(strIface);
    if (it != m_mapIfaces.end()) {
        pIfaceParams = &it->second;
    }

    int iIfaceIndex = if_nametoindex(strIface.c_str());

    if (iIfaceIndex == 0) {
        LOG(WARNING) << "Failed reading interface index for (" << strIface
                     << "): " << strerror(errno);

        return false;
    }

    // Update ARP response parameters
    arp_resp->params().iface_idx = (pIfaceParams) ? iIfaceIndex : 0;
    arp_resp->params().source =
        (pIfaceParams) ? uint8_t(pIfaceParams->eType) : uint8_t(beerocks::ARP_SRC_ETH_FRONT);

    std::string strSource =
        (pIfaceParams && ((pIfaceParams->eType == beerocks::ARP_SRC_ETH_BACK) ||
                          (pIfaceParams->eType == beerocks::ARP_SRC_WIRELESS_BACK)))
            ? "BACK"
            : "FRONT";

    LOG(DEBUG) << "Discovered IP: " << network_utils::ipv4_to_string(arp_resp->params().ipv4)
               << " (" << arp_resp->params().mac << ") on '" << strIface << "' (" << strSource
               << ")";

    // Update ARP entry parameters
    auto pArpEntry = m_mapArpEntries.find(arp_resp->params().mac);

    if (pArpEntry != m_mapArpEntries.end()) {
        pArpEntry->second->ip = network_utils::uint_ipv4_from_array(arp_resp->params().ipv4.oct);
        pArpEntry->second->iface_index = arp_resp->params().iface_idx;
        pArpEntry->second->last_seen   = std::chrono::steady_clock::now();
    } else {
        // This should not happen since the client is added to the list on query request...
        LOG(WARNING) << "MAC " << arp_resp->params().mac
                     << " was NOT found in the ARP entries list...";
    }

    // Get a slave socket
    Socket *sd = get_backhaul_socket();

    if (sd) {
        LOG(TRACE) << "ACTION_PLATFORM_ARP_QUERY_RESPONSE mac=" << arp_resp->params().mac
                   << " task_id=" << task_id;
        send_cmdu_safe(sd, cmdu_tx);
    }

    return (true);
}

void main_thread::arp_entries_cleanup()
{
    if (!enable_arp_monitor || !m_mapArpEntries.size())
        return;

    auto now = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_tpArpEntriesCleanup).count() <
        ARP_CLEAN_INTERVAL) {
        return;
    }

    m_tpArpEntriesCleanup = now;

    for (auto it = m_mapArpEntries.begin(); it != m_mapArpEntries.end();) {

        auto last_seen_duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second->last_seen)
                .count();

        // If the client wasn't seen --> erase it
        if (last_seen_duration >= ARP_NOTIF_INTERVAL) {
            LOG(INFO) << "Removing client with MAC " << (const uint8_t *)&it->first
                      << " due to inactivity of " << last_seen_duration << " milliseconds.";

            it = m_mapArpEntries.erase(it);
        } else {
            it++;
        }
    }
}

void main_thread::after_select(bool timeout)
{
    // On timeout
    if (timeout) {
        // Cleanup old arp entries...
        arp_entries_cleanup();
        return;
    }

    // DHCP Monitor
    if (m_pDHCPMonSocket && read_ready(m_pDHCPMonSocket)) {
        clear_ready(m_pDHCPMonSocket);
        bpl::dhcp_mon_handle_event();
    }

    // ARP Monitor
    if (m_pArpMonSocket && read_ready(m_pArpMonSocket)) {
        clear_ready(m_pArpMonSocket);
        if (!handle_arp_monitor()) {
            LOG(ERROR) << "handle_arp_monitor failed, restarting ARP monitor";
            if (!restart_arp_monitor()) {
                LOG(ERROR) << "failed to restart ARP monitor";
            }
        }
    }

    // ARP Monitor - RAW Socket
    if (m_pArpRawSocket && read_ready(m_pArpRawSocket)) {
        clear_ready(m_pArpRawSocket);
        if (!handle_arp_raw()) {
            LOG(ERROR) << "handle_arp_raw failed, restarting ARP monitor";
            if (!restart_arp_monitor()) {
                LOG(ERROR) << "failed to restart ARP monitor";
            }
        }
    }

    // check if wlan params changed
    wlan_params_changed_check();
}

bool main_thread::init_arp_monitor()
{
    if (!m_ctxArpMon) {
        LOG(DEBUG) << "Starting ARP Monitor...";

        // Read IP/Netmask of the monitored interface
        network_utils::raw_iface_info info;
        if (!network_utils::get_raw_iface_info(config.bridge_iface, info)) {
            LOG(ERROR) << "Failed reading '" + config.bridge_iface + "' information";
            return false;
        }

        int ret = bpl::arp_mon_start(&m_ctxArpMon, config.bridge_iface.c_str());
        if (ret < 0) {
            // If arp_monitor failed to start, continue without it. It might failed due to
            // insufficient permissions. Detailed error message is printed inside.
            if (ret == -int(bpl::eErrorCode::OPERATION_NOT_SUPPORTED)) {
                LOG(INFO) << "Skip starting ARP monitor (not supported)";
                return (true);
            }
            LOG(ERROR) << "Failed starting ARP monitor!";
            return (false);
        }

        m_uiArpMonIP   = info.ipa.s_addr;
        m_uiArpMonMask = info.nmask.s_addr;

        // Create wrapper socket classes
        if (!m_pArpRawSocket) {
            m_pArpRawSocket = new Socket(bpl::arp_mon_get_raw_arp_fd(m_ctxArpMon));
            add_socket(m_pArpRawSocket);
        }

        if (!m_pArpMonSocket) {
            m_pArpMonSocket = new Socket(bpl::arp_mon_get_fd(m_ctxArpMon));
            add_socket(m_pArpMonSocket);
        }

        LOG(DEBUG) << "ARP Monitor started on interface '" << config.bridge_iface << "' ("
                   << network_utils::ipv4_to_string(m_uiArpMonIP) << "/"
                   << network_utils::ipv4_to_string(m_uiArpMonMask) << ")";

        // Initialize the ARP entries cleanup timestamp
        m_tpArpEntriesCleanup = std::chrono::steady_clock::now();
    }

    return true;
}

void main_thread::stop_arp_monitor()
{
    if (m_ctxArpMon) {
        bpl::arp_mon_stop(m_ctxArpMon);
        LOG(DEBUG) << "ARP Monitor Stopped.";
        m_ctxArpMon = nullptr;
    }

    if (m_pArpRawSocket) {
        remove_socket(m_pArpRawSocket);
        delete m_pArpRawSocket;
        m_pArpRawSocket = nullptr;
    }

    if (m_pArpMonSocket) {
        remove_socket(m_pArpMonSocket);
        delete m_pArpMonSocket;
        m_pArpMonSocket = nullptr;
    }
}

bool main_thread::restart_arp_monitor()
{
    stop_arp_monitor();

    if (!init_arp_monitor()) {
        LOG(ERROR) << "can't start ARP monitor";
        return false;
    }

    return true;
}

bool main_thread::init_dhcp_monitor()
{
    static auto dhcp_monitor_cb_wrapper = [&](std::string op, std::string mac, std::string ip,
                                              std::string hostname) {
        send_dhcp_notification(op, mac, ip, hostname);
    };

    if (!m_pDHCPMonSocket) {

        int dhcp_mon_fd = bpl::dhcp_mon_start(
            [](const char *op, const char *mac, const char *ip, const char *hostname) {
                dhcp_monitor_cb_wrapper(op, mac, ip, hostname);
            });
        if (dhcp_mon_fd < 0) {
            if (dhcp_mon_fd == -int(bpl::eErrorCode::OPERATION_NOT_SUPPORTED)) {
                LOG(INFO) << "Skip starting DHCP monitor (not supported)";
                return (true);
            }
            LOG(ERROR) << "Failed starting DHCP monitor: " << dhcp_mon_fd;
            return (false);
        }

        m_pDHCPMonSocket = new Socket(dhcp_mon_fd);
        add_socket(m_pDHCPMonSocket);
        LOG(DEBUG) << "DHCP Monitor Started... sd=" << intptr_t(m_pDHCPMonSocket);
    }

    return true;
}

} // namespace platform_manager
} // namespace beerocks
