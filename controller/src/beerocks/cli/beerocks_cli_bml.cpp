/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_cli_bml.h"
#include "bml_utils.h"

#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <unordered_map>

using namespace beerocks;
using namespace net;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void fill_conn_map_node(
    std::unordered_multimap<std::string, std::shared_ptr<cli_bml::conn_map_node_t>> &conn_map_nodes,
    struct BML_NODE *node)
{
    auto n                         = std::make_shared<cli_bml::conn_map_node_t>();
    n->type                        = node->type;
    n->state                       = node->state;
    n->channel                     = node->channel;
    n->bw                          = node->bw;
    n->channel_ext_above_secondary = node->channel_ext_above_secondary;
    n->rx_rssi                     = node->rx_rssi;
    n->mac                         = network_utils::mac_to_string(node->mac);
    n->ip_v4                       = network_utils::ipv4_to_string(node->ip_v4);
    n->name.assign(node->name[0] ? node->name : "N/A");

    if (node->type != BML_NODE_TYPE_CLIENT) { // GW or IRE

        n->gw_ire.backhaul_mac = network_utils::mac_to_string(node->data.gw_ire.backhaul_mac);

        // RADIO
        int radio_length = sizeof(node->data.gw_ire.radio) / sizeof(node->data.gw_ire.radio[0]);
        for (int i = 0; i < radio_length; i++) {
            if (node->data.gw_ire.radio[i].channel != 0 && node->data.gw_ire.radio[i].ap_active) {
                auto r           = std::make_shared<cli_bml::conn_map_node_t::gw_ire_t::radio_t>();
                r->channel       = node->data.gw_ire.radio[i].channel;
                r->cac_completed = node->data.gw_ire.radio[i].cac_completed;
                r->bw            = node->data.gw_ire.radio[i].bw;
                r->channel_ext_above_secondary =
                    node->data.gw_ire.radio[i].channel_ext_above_secondary;
                r->radio_identifier =
                    network_utils::mac_to_string(node->data.gw_ire.radio[i].radio_identifier);
                r->radio_mac = network_utils::mac_to_string(node->data.gw_ire.radio[i].radio_mac);
                r->ifname.assign(node->data.gw_ire.radio[i].iface_name);

                // VAP
                int vap_length = sizeof(node->data.gw_ire.radio[i].vap) /
                                 sizeof(node->data.gw_ire.radio[i].vap[0]);
                for (int j = 0; j < vap_length; j++) {
                    auto vap_mac =
                        network_utils::mac_to_string(node->data.gw_ire.radio[i].vap[j].bssid);
                    if (vap_mac != network_utils::ZERO_MAC_STRING) {
                        auto v =
                            std::make_shared<cli_bml::conn_map_node_t::gw_ire_t::radio_t::vap_t>();
                        v->bssid = vap_mac;
                        v->ssid.assign(node->data.gw_ire.radio[i].vap[j].ssid[0]
                                           ? node->data.gw_ire.radio[i].vap[j].ssid
                                           : std::string("N/A"));
                        v->backhaul_vap = node->data.gw_ire.radio[i].vap[j].backhaul_vap;
                        r->vap.push_back(v);
                    }
                }
                n->gw_ire.radio.push_back(r);
            }
        }
    }

    conn_map_nodes.insert({network_utils::mac_to_string(node->parent_bssid), n});
}

static std::string &ind_inc(std::string &ind)
{
    static const std::string basic_ind("    "); // 4 spaces
    ind += basic_ind;
    return ind;
}

static std::string &ind_dec(std::string &ind)
{
    ind.erase(ind.end() - 4, ind.end()); // erase last 4 space chars
    return ind;
}

static std::string node_type_to_conn_map_string(uint8_t type)
{
    std::string ret;

    switch (type) {
    case BML_NODE_TYPE_GW:
        ret = "GW_BRIDGE:";
        break;
    case BML_NODE_TYPE_IRE:
        ret = "IRE_BRIDGE:";
        break;
    case BML_NODE_TYPE_CLIENT:
        ret = "CLIENT:";
        break;

    default:
        ret = "N/A";
    }

    return ret;
}

static void bml_utils_dump_conn_map(
    std::unordered_multimap<std::string, std::shared_ptr<cli_bml::conn_map_node_t>> &conn_map_nodes,
    const std::string &parent_bssid, const std::string ind, std::stringstream &ss)
{
    std::string ind_str = ind;

    // ss << "***" << " parent mac: " << parent_bssid << "***" << std::endl;

    auto range = conn_map_nodes.equal_range(parent_bssid);
    for (auto it = range.first; it != range.second; it++) {
        auto node = it->second;

        // ss << "***" << " node mac: " << node->mac << "***" << std::endl;

        // CLIENT
        if (node->type == BML_NODE_TYPE_CLIENT) {
            ss << ind_inc(ind_str) << node_type_to_conn_map_string(node->type)
               << " mac: " << node->mac << ", ipv4: " << node->ip_v4 << ", name: " << node->name;
            if (node->channel) { // channel != 0
                ss << ", ch: " << std::to_string(node->channel) << ", bw: "
                   << utils::convert_bandwidth_to_int((beerocks::eWiFiBandwidth)node->bw)
                   << utils::convert_channel_ext_above_to_string(node->channel_ext_above_secondary,
                                                                 (beerocks::eWiFiBandwidth)node->bw)
                   << ", rx_rssi: " << std::to_string(node->rx_rssi);
            }
            ss << std::endl;

        } else { //PLATFORM

            // IRE BACKHAUL
            if (node->type == BML_NODE_TYPE_IRE) {
                ss << ind_inc(ind_str) << "IRE_BACKHAUL:"
                   << " mac: " << node->gw_ire.backhaul_mac
                   << ", ch: " << std::to_string(node->channel) << ", bw: "
                   << utils::convert_bandwidth_to_int((beerocks::eWiFiBandwidth)node->bw)
                   << utils::convert_channel_ext_above_to_string(node->channel_ext_above_secondary,
                                                                 (beerocks::eWiFiBandwidth)node->bw)
                   //<< ", rx_rssi: "       << std::to_string(node->rx_rssi)
                   << std::endl;
            }

            // BRIDGE
            if (parent_bssid != network_utils::ZERO_MAC_STRING)
                ind_inc(ind_str);

            ss << ind_str << node_type_to_conn_map_string(node->type) << " name: " << node->name
               << ", mac: " << node->mac << ", ipv4: " << node->ip_v4 << std::endl;

            // ETHERNET
            auto eth_sw_mac_binary = network_utils::mac_from_string(node->mac);
            ++eth_sw_mac_binary.oct[5]; // generate eth address from bridge address
            auto eth_mac = network_utils::mac_to_string(eth_sw_mac_binary);
            ss << ind_inc(ind_str) << "ETHERNET:"
               << " mac: " << eth_mac << std::endl;
            // add clients which are connected to the Ethernet
            bml_utils_dump_conn_map(conn_map_nodes, eth_mac, ind_str, ss);

            // RADIO
            for (auto radio : node->gw_ire.radio) {

                ss << ind_str << "RADIO: " << radio->ifname << " mac: " << radio->radio_mac
                   << ", ch: "
                   << (radio->channel != 255 ? std::to_string(radio->channel) : std::string("N/A"))
                   << ((son::wireless_utils::is_dfs_channel(radio->channel) &&
                        !radio->cac_completed)
                           ? std::string("(CAC)")
                           : std::string())
                   << ", bw: "
                   << utils::convert_bandwidth_to_int((beerocks::eWiFiBandwidth)radio->bw)
                   << utils::convert_channel_ext_above_to_string(
                          radio->channel_ext_above_secondary, (beerocks::eWiFiBandwidth)radio->bw)
                   << ", freq: "
                   << std::to_string(beerocks::utils::wifi_channel_to_freq(radio->channel)) << "MHz"
                   << std::endl;

                // VAP
                ind_inc(ind_str);
                uint8_t j = 0;
                for (auto vap = radio->vap.begin(); vap != radio->vap.end(); vap++) {
                    if ((*vap)->bssid != network_utils::ZERO_MAC_STRING) {
                        ss << ind_str << std::string((*vap)->backhaul_vap ? "b" : "f") << "VAP["
                           << std::to_string(j) << "]:"
                           << " " << radio->ifname << "." << std::to_string(j)
                           << " bssid: " << (*vap)->bssid << ", ssid: " << (*vap)->ssid
                           << std::endl;
                        // add clients which are connected to the vap
                        bml_utils_dump_conn_map(conn_map_nodes, (*vap)->bssid, ind_str, ss);
                        j++;
                    }
                }
                ind_dec(ind_str);
            }
        }
        ind_str = ind; // return the indentation to original level
    }
}

#ifdef BEEROCKS_RDKB
static void steering_set_group_string_to_struct(const std::string &str_cfg_2,
                                                const std::string &str_cfg_5,
                                                BML_STEERING_AP_CONFIG &cfg2,
                                                BML_STEERING_AP_CONFIG &cfg5)
{
    auto v_str_cfg_2 = string_utils::str_split(str_cfg_2, ',');
    auto v_str_cfg_5 = string_utils::str_split(str_cfg_5, ',');
    for (const auto &elem : v_str_cfg_2) {
        std::cout << "v_str_cfg_2 : " << elem << std::endl;
    }

    for (const auto &elem : v_str_cfg_5) {
        std::cout << "v_str_cfg_5 : " << elem << std::endl;
    }
    std::copy_n(network_utils::mac_from_string(v_str_cfg_2[0]).oct, BML_MAC_ADDR_LEN, cfg2.bssid);
    std::copy_n(network_utils::mac_from_string(v_str_cfg_5[0]).oct, BML_MAC_ADDR_LEN, cfg5.bssid);
    cfg2.utilCheckIntervalSec   = string_utils::stoi(v_str_cfg_2[1]);
    cfg5.utilCheckIntervalSec   = string_utils::stoi(v_str_cfg_5[1]);
    cfg2.utilAvgCount           = string_utils::stoi(v_str_cfg_2[2]);
    cfg5.utilAvgCount           = string_utils::stoi(v_str_cfg_5[2]);
    cfg2.inactCheckIntervalSec  = string_utils::stoi(v_str_cfg_2[3]);
    cfg5.inactCheckIntervalSec  = string_utils::stoi(v_str_cfg_5[3]);
    cfg2.inactCheckThresholdSec = string_utils::stoi(v_str_cfg_2[4]);
    cfg5.inactCheckThresholdSec = string_utils::stoi(v_str_cfg_5[4]);

    sMacAddr bssid2;
    sMacAddr bssid5;
    std::copy_n(cfg2.bssid, sizeof(sMacAddr::oct), bssid2.oct);
    std::copy_n(cfg5.bssid, sizeof(sMacAddr::oct), bssid5.oct);

    std::cout << "cfg2.bssid = " << bssid2 << std::endl
              << "cfg2.utilCheckIntervalSec = " << cfg2.utilCheckIntervalSec << std::endl
              << "cfg2.utilAvgCount = " << cfg2.utilAvgCount << std::endl
              << "cfg2.inactCheckIntervalSec = " << cfg2.inactCheckIntervalSec << std::endl
              << "cfg2.inactCheckThresholdSec = " << cfg2.inactCheckThresholdSec << std::endl
              << "___________________________________________________" << std::endl
              << "cfg5.bssid = " << bssid5 << std::endl
              << "cfg5.utilCheckIntervalSec = " << cfg5.utilCheckIntervalSec << std::endl
              << "cfg5.utilAvgCount = " << cfg5.utilAvgCount << std::endl
              << "cfg5.inactCheckIntervalSec = " << cfg5.inactCheckIntervalSec << std::endl
              << "cfg5.inactCheckThresholdSec = " << cfg5.inactCheckThresholdSec << std::endl;
    return;
}
static void steering_client_set_string_to_struct(const std::string &str_config,
                                                 BML_STEERING_CLIENT_CONFIG &config)
{
    auto v_str_config = string_utils::str_split(str_config, ',');

    config.snrProbeHWM      = string_utils::stoi(v_str_config[0]);
    config.snrProbeLWM      = string_utils::stoi(v_str_config[1]);
    config.snrAuthHWM       = string_utils::stoi(v_str_config[2]);
    config.snrAuthLWM       = string_utils::stoi(v_str_config[3]);
    config.snrInactXing     = string_utils::stoi(v_str_config[4]);
    config.snrHighXing      = string_utils::stoi(v_str_config[5]);
    config.snrLowXing       = string_utils::stoi(v_str_config[6]);
    config.authRejectReason = string_utils::stoi(v_str_config[7]);

    std::cout << "config.snrProbeHWM = " << config.snrProbeHWM << std::endl
              << "config.snrProbeLWM = " << config.snrProbeLWM << std::endl
              << "config.snrAuthHWM = " << config.snrAuthHWM << std::endl
              << "config.snrAuthLWM = " << config.snrAuthLWM << std::endl
              << "config.snrInactXing = " << config.snrInactXing << std::endl
              << "config.snrHighXing = " << config.snrHighXing << std::endl
              << "config.snrLowXing = " << config.snrLowXing << std::endl
              << "config.authRejectReason = " << config.authRejectReason << std::endl;
    return;
}
#endif //BEEROCKS_RDKB
//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SocketClient *cli_bml::m_analyzer_socket = nullptr;

cli_bml::cli_bml(std::string beerocks_conf_path_)
{
    setFunctionsMapAndArray();
    beerocks_conf_path = beerocks_conf_path_;

    // Set BML easylogging context
    auto log_storage = el::Helpers::storage();
    bml_set_local_log_context((void *)&log_storage);

    //set select timeout
    timeval tval;
    tval.tv_sec  = 0;
    tval.tv_usec = 1000 * 2000; // 2sec
    select.setTimeout(&tval);
}

cli_bml::~cli_bml() { disconnect(); }

bool cli_bml::connect()
{
    disconnect(); // Disconnect if already connected
    const char *c = beerocks_conf_path.c_str();
    int ret       = bml_connect(&ctx, c, this);
    printBmlReturnVals("bml_connect", ret);
    return (ret == BML_RET_OK ? true : false);
}

void cli_bml::disconnect()
{
    is_analyzer = false;
    if (ctx != nullptr) {
        int ret = bml_disconnect(ctx);
        printBmlReturnVals("bml_disconnect", ret);
        ctx = nullptr;
    }
    if (m_analyzer_socket != nullptr) {
        select.removeSocket(m_analyzer_socket);
        m_analyzer_socket->closeSocket();
        delete m_analyzer_socket;
        m_analyzer_socket = nullptr;
    }
}

bool cli_bml::is_connected()
{
    if (!ctx) {
        return false;
    }
    return true;
}

//
// Help functions
//

void cli_bml::setFunctionsMapAndArray()
{
    helpLineSize = 0;

    //Setting up functions map : <function_name>,
    //                           <help_args>,<help>,
    //                           <funcPtr>,
    //                           <minNumOfArgs>,<maxNumOfArgs>,
    //                           <type1>,<type2>,...,<type10>

    insertCommandToMap("bml_help", "", "get supported commands", &cli::help_caller, 0, 0);
    insertCommandToMap("bml_connect", "", "connect to libbml ",
                       static_cast<pFunction>(&cli_bml::connect_caller), 0, 0);
    insertCommandToMap("bml_onboard_status", "",
                       "prints '1' if the platform is in Oboarding state, '0' otherwise",
                       static_cast<pFunction>(&cli_bml::onboard_status_caller), 0, 0);
    insertCommandToMap("bml_ping", "",
                       "Check whether the connection with the beerocks platform is alive and well",
                       static_cast<pFunction>(&cli_bml::ping_caller), 0, 0);
    insertCommandToMap("bml_nw_map_register_update_cb", "[<x>]",
                       "Registers a callback functions for the network map update operation, call "
                       "with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::nw_map_register_update_cb_caller), 0, 1,
                       STRING_ARG);
    insertCommandToMap("bml_nw_map_query", "", "Query the beerocks for the latest network map",
                       static_cast<pFunction>(&cli_bml::nw_map_query_caller), 0, 0);
    insertCommandToMap("bml_conn_map", "", "dump the latest network map",
                       static_cast<pFunction>(&cli_bml::bml_connection_map_caller), 0, 0);
    insertCommandToMap("bml_stat_register_cb", "[<x>]",
                       "Registers a callback function to periodic statistics update from the "
                       "beerocks platform, call with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::stat_register_cb_caller), 0, 1, STRING_ARG);
    insertCommandToMap("bml_events_register_cb", "[<x>]",
                       "Registers a callback function to events "
                       "update from the beerocks platform, call "
                       "with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::events_register_cb_caller), 0, 1,
                       STRING_ARG);
    insertCommandToMap(
        "bml_set_wifi_credentials", "<new_ssid> <new_pass> [<security>] [<vap_id>] [<force>]",
        "Sets new ssid and password for the given VAP."
        " Security values: 0=No Wi-Fi Security, 1=WEP-64 Encryption, 2=WEP-128 Encryption, "
        "3=WPA-Personal, 4=WPA2-Personal, 5=WPA-WPA2-Personal Encryption. Default value 4."
        " Vap default vaule 0."
        " [<1(True) or 0(False)>] for force. Default value 1",
        static_cast<pFunction>(&cli_bml::set_wifi_credentials_caller), 2, 5, STRING_ARG, STRING_ARG,
        INT_ARG, INT_ARG, INT_ARG);
    insertCommandToMap("bml_get_wifi_credentials", "[<vap_id>]",
                       "Get SSID and security type for the given VAP (or Vap=0 by default)",
                       static_cast<pFunction>(&cli_bml::get_wifi_credentials_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_set_onboarding_state", "<enable 0/1>", "Sets onboarding state.",
                       static_cast<pFunction>(&cli_bml::set_onboarding_state_caller), 1, 1,
                       INT_ARG);
    insertCommandToMap("bml_get_onboarding_state", "", "Get onboarding state.",
                       static_cast<pFunction>(&cli_bml::get_onboarding_state_caller), 0, 0);
    insertCommandToMap("bml_wps_onboarding", "[<iface>]",
                       "Start WPS onboarding process on 'iface' or all ifaces in case of empty.",
                       static_cast<pFunction>(&cli_bml::wps_onboarding_caller), 0, 1, STRING_ARG);
    insertCommandToMap("bml_get_device_info", "", "Get device information",
                       static_cast<pFunction>(&cli_bml::get_device_info_caller), 0, 0);
    insertCommandToMap("bml_get_bml_version", "", "prints bml version",
                       static_cast<pFunction>(&cli_bml::get_bml_version_caller), 0, 0);
    insertCommandToMap("bml_get_master_slave_versions", "",
                       "prints beerocks master & slave versions",
                       static_cast<pFunction>(&cli_bml::get_master_slave_versions_caller), 0, 0);
    insertCommandToMap(
        "bml_enable_legacy_client_roaming", "[<1 or 0>]",
        "if input was given - enable/disable legacy client roaming, prints current value",
        static_cast<pFunction>(&cli_bml::enable_legacy_client_roaming_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_enable_client_roaming", "[<1 or 0>]",
                       "if input was given - enable/disable client roaming, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_client_roaming_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap(
        "bml_enable_client_roaming_prefer_signal_strength", "[<1 or 0>]",
        "if input was given - enable/disable client roaming prefer signal strength, prints current "
        "value",
        static_cast<pFunction>(&cli_bml::enable_client_roaming_prefer_signal_strength_caller), 0, 1,
        INT_ARG);
    insertCommandToMap("bml_enable_client_band_steering", "[<1 or 0>]",
                       "if input was given - enable/disable band steering, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_client_band_steering_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_enable_ire_roaming", "[<1 or 0>]",
                       "if input was given - enable/disable ire roaming, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_ire_roaming_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_enable_load_balancer", "[<1 or 0>]",
                       "if input was given - enable/disable load balancer, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_load_balancer_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_enable_service_fairness", "[<1 or 0>]",
                       "if input was given - enable/disable service fairness, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_service_fairness_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_enable_dfs_reentry", "[<1 or 0>]",
                       "if input was given - enable/disable service fairness, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_dfs_reentry_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_certification_mode", "[<1 or 0>]",
                       "if input was given - enable/disable certification mode, "
                       " else prints current value",
                       static_cast<pFunction>(&cli_bml::enable_certification_mode_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_set_log_level", "<module_name> <log_level> <1 or 0> [<mac>]",
                       "turn 'on/off' (1 or 0) 'log_level' ('i'- info, 'd' - debug, 'e' - error, "
                       "'f' - fatal, 't' - trace, 'w' - warning, 'a' - all) on 'module_name' "
                       "(master/ slave/ monitor/ platform/ all) with 'mac' (default - all slaves) ",
                       static_cast<pFunction>(&cli_bml::set_log_level_caller), 3, 4, STRING_ARG,
                       STRING_ARG, INT_ARG, STRING_ARG);
    insertCommandToMap("bml_restricted_channels_set_global", "<restricted channls>",
                       "Set global 'restricted channels' seperated by commas",
                       static_cast<pFunction>(&cli_bml::set_global_restricted_channels_caller), 1,
                       1, STRING_ARG);
    insertCommandToMap("bml_restricted_channels_get_global", "", "Get global restricted channels",
                       static_cast<pFunction>(&cli_bml::get_global_restricted_channels_caller), 0,
                       0);
    insertCommandToMap("bml_restricted_channels_set_slave", "<restricted channels>",
                       "Set slave 'mac' 'restricted channels' seperated by commas",
                       static_cast<pFunction>(&cli_bml::set_slave_restricted_channels_caller), 2, 2,
                       STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "bml_restricted_channels_get_slave", "<mac>", "Get restricted channels from slave 'mac'",
        static_cast<pFunction>(&cli_bml::get_slave_restricted_channels_caller), 1, 1, STRING_ARG);
    insertCommandToMap("bml_trigger_topology_discovery", "<al_mac (mac format)>",
                       "trigger topology query towards 'al_mac'",
                       static_cast<pFunction>(&cli_bml::bml_trigger_topology_discovery_caller), 1,
                       1, STRING_ARG);
    insertCommandToMap(
        "bml_trigger_channel_selection",            // command name
        "<al_mac (mac format)> <ruid(mac format)>", // command args list
        "trigger channel selection procedure"       // command description
        "on agent 'agent_ruid'",
        static_cast<pFunction>(&cli_bml::bml_channel_selection_caller), // caller function
        2, 2,                                                           // min,max args number
        STRING_ARG, STRING_ARG);                                        // args types

#ifdef BEEROCKS_RDKB
    insertCommandToMap("bml_rdkb_steering_set_group", "<steeringGroupIndex> <cfg_2> <cfg_5>",
                       "cfg2/5 = <bssid>, <utilCheckIntervalSec>, <utilAvgCount>, "
                       "<inactCheckIntervalSec>, <inactCheckThresholdSec> (without spaces between "
                       "commas) ",
                       static_cast<pFunction>(&cli_bml::bml_rdkb_steering_set_group_caller), 3, 3,
                       INT_ARG, STRING_ARG, STRING_ARG);

    insertCommandToMap(
        "bml_rdkb_steering_client_set", "<steeringGroupIndex> <bssid> <client_mac> [<config>]",
        "config = <snrProbeHWM>, <snrProbeLWM>, <snrAuthHWM>, <snrAuthLWM>, <snrInactXing>, "
        "<snrHighXing>, <snrLowXing>, <authRejectReason> (without spaces between commas) "
        "if 'config' is not given, client will be removed ",
        static_cast<pFunction>(&cli_bml::bml_rdkb_steering_client_set_caller), 3, 4, INT_ARG,
        STRING_ARG, STRING_ARG, STRING_ARG);
    insertCommandToMap("bml_rdkb_steering_event_register", "[<x>]",
                       "Registers a callback function to events update from the beerocks platform, "
                       "call with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::bml_rdkb_steering_event_register_caller), 0,
                       1, STRING_ARG);

    insertCommandToMap("bml_rdkb_steering_client_measure",
                       "<steeringGroupIndex> <bssid> <client_mac>", "",
                       static_cast<pFunction>(&cli_bml::bml_rdkb_steering_client_measure_caller), 3,
                       3, INT_ARG, STRING_ARG, STRING_ARG);
#endif
    //bool insertCommandToMap(std::string command, std::string help_args, std::string help,  pFunction funcPtr, uint8_t minNumOfArgs, uint8_t maxNumOfArgs,
}

void cli_bml::printBmlReturnVals(const std::string &func_name, int ret_val)
{
    std::cout << func_name << ": ";
    switch (ret_val) {
    case BML_RET_OK: {
        std::cout << "return value is: BML_RET_OK, Success status" << std::endl;
        break;
    }
    case -BML_RET_OP_FAILED: {
        std::cout << "return value is: BML_RET_OP_FAILED, Operation failed" << std::endl;
        break;
    }
    case -BML_RET_INVALID_ARGS: {
        std::cout << "return value is: BML_RET_INVALID_ARGS, Invalid arguments" << std::endl;
        break;
    }
    case -BML_RET_MEM_FAIL: {
        std::cout << "return value is: BML_RET_MEM_FAIL, Memory allocation failed" << std::endl;
        break;
    }
    case -BML_RET_INIT_FAIL: {
        std::cout << "return value is: BML_RET_INIT_FAIL, BML Initialization failed" << std::endl;
        break;
    }
    case -BML_RET_CONNECT_FAIL: {
        std::cout << "return value is: BML_RET_CONNECT_FAIL, Connection to Beerocks failed"
                  << std::endl;
        break;
    }
    case -BML_RET_NO_DATA: {
        std::cout << "return value is: BML_RET_NO_DATA, No more data available" << std::endl;
        break;
    }
    case -BML_RET_INVALID_DATA: {
        std::cout << "return value is: BML_RET_INVALID_DATA, Unexpected data" << std::endl;
        break;
    }
    case -BML_RET_TIMEOUT: {
        std::cout << "return value is: BML_RET_TIMEOUT, Operation timed out" << std::endl;
        break;
    }
    default: {
        std::cout << "return value is: " << ret_val << " Unknown return value" << std::endl;
    }
    }
}

void cli_bml::map_query_cb(const struct BML_NODE_ITER *node_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);

    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }

    char *p = pThis->print_buffer;

    struct BML_NODE *current_node;
    if (node_iter->first() != BML_RET_OK) {
        std::cout << "map_query_cb: node_iter.first() != BML_RET_OK, map_query_cb stops"
                  << std::endl;
        return;
    }
    current_node = node_iter->get_node();
    if (to_console) {
        bml_utils_node_to_string(current_node, pThis->print_buffer, PRINT_BUFFER_LENGTH);
        std::cout << pThis->print_buffer << std::endl;
    } else {
        //Add MARK line
        if (current_node->type == BML_NODE_TYPE_GW) {
            std::stringstream ss;
            ss << "MARK" << std::endl;
            ss.seekp(0, std::ios::end);
            std::stringstream::pos_type offset = ss.tellp();
            int free_buffer_len                = PRINT_BUFFER_LENGTH - (p - pThis->print_buffer);

            if (offset > free_buffer_len) {
                std::cout << "ERROR: MARK sign > print_buffer available length" << std::endl;
                return;
            } else {
                beerocks::string_utils::copy_string(p, ss.str().c_str(),
                                                    sizeof(pThis->print_buffer));
                p += offset;
            }
        }
        p += bml_utils_node_to_string(current_node, p, PRINT_BUFFER_LENGTH);
    }
    while (node_iter->next() == BML_RET_OK) {
        current_node = node_iter->get_node();
        if (to_console) {
            bml_utils_node_to_string(current_node, pThis->print_buffer, PRINT_BUFFER_LENGTH);
            std::cout << pThis->print_buffer << std::endl;
        } else {
            std::ptrdiff_t size = p - pThis->print_buffer;
            p += bml_utils_node_to_string(current_node, p, PRINT_BUFFER_LENGTH - size);
        }
    }

    if (!to_console) {
        //send_message to m_analyzer_socket
        std::ptrdiff_t size = p - pThis->print_buffer;
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::connection_map_cb(const struct BML_NODE_ITER *node_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);

    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }

    struct BML_NODE *current_node;
    if (node_iter->first() != BML_RET_OK) {
        std::cout << "map_query_cb: node_iter.first() != BML_RET_OK, map_query_cb stops"
                  << std::endl;
        return;
    }

    current_node = node_iter->get_node();
    if (current_node) {
        fill_conn_map_node(pThis->conn_map_nodes, current_node);

        while (node_iter->next() == BML_RET_OK) {
            current_node = node_iter->get_node();
            fill_conn_map_node(pThis->conn_map_nodes, current_node);
        }
    }

    if (node_iter->last_node) {
        std::stringstream ss;
        std::string ind;
        if (pThis->conn_map_nodes.empty()) {
            ss << "Connection map is empty..." << std::endl;
        } else {
            bml_utils_dump_conn_map(pThis->conn_map_nodes, network_utils::ZERO_MAC_STRING, ind, ss);
            pThis->conn_map_nodes.clear();
        }
        // Printing the connection map
        std::cout << std::endl << ss.str();
        //No need to wait anymore - this is the last fragment
        pThis->pending_response = false;
    }
}

void cli_bml::map_query_to_console_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_query_cb(node_iter, true);
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
    pThis->pending_response = false;
}

void cli_bml::connection_map_to_console_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::connection_map_cb(node_iter, true);
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
}

void cli_bml::map_query_to_socket_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_query_cb(node_iter, false);
}

void cli_bml::map_update_cb(const struct BML_NODE_ITER *node_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }

    struct BML_NODE *current_node;
    if (node_iter->first() != BML_RET_OK) {
        std::cout << "map_update_cb: node_iter.first() != BML_RET_OK, map_update_cb stops"
                  << std::endl;
        return;
    }
    current_node = node_iter->get_node();
    int size     = bml_utils_node_to_string(current_node, pThis->print_buffer, PRINT_BUFFER_LENGTH);
    if (to_console) {
        std::cout << pThis->print_buffer << std::endl;
    } else {
        //send_message to m_analyzer_socket
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::map_update_to_console_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_update_cb(node_iter, true);
}

void cli_bml::map_update_to_socket_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_update_cb(node_iter, false);
}

void cli_bml::stats_update_cb(const struct BML_STATS_ITER *stats_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(stats_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
    char *p = pThis->print_buffer;

    struct BML_STATS *current_stats;
    if (stats_iter->first() != BML_RET_OK) {
        std::cout << "stats_update_cb: stats_iter->first() != BML_RET_OK, stats_update_cb stops"
                  << std::endl;
        return;
    }
    current_stats = stats_iter->get_node();
    if (to_console) {
        bml_utils_stats_to_string(current_stats, pThis->print_buffer, PRINT_BUFFER_LENGTH);
        std::cout << pThis->print_buffer << std::endl;
    } else {
        p += bml_utils_stats_to_string_raw(current_stats, p, PRINT_BUFFER_LENGTH);
    }

    while (stats_iter->next() == BML_RET_OK) {
        current_stats = stats_iter->get_node();
        if (to_console) {
            bml_utils_stats_to_string(current_stats, pThis->print_buffer, PRINT_BUFFER_LENGTH);
            std::cout << pThis->print_buffer << std::endl;
        } else {
            std::ptrdiff_t size = p - pThis->print_buffer;
            p += bml_utils_stats_to_string_raw(current_stats, p, PRINT_BUFFER_LENGTH - size);
        }
    }

    if (!to_console) {
        //send_message to m_analyzer_socket
        std::ptrdiff_t size = p - pThis->print_buffer;
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::stats_update_to_console_cb(const struct BML_STATS_ITER *stats_iter)
{
    cli_bml::stats_update_cb(stats_iter, true);
}

void cli_bml::stats_update_to_socket_cb(const struct BML_STATS_ITER *stats_iter)
{
    cli_bml::stats_update_cb(stats_iter, false);
}

void cli_bml::events_update_cb(const struct BML_EVENT *event, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(event->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
    char *p = pThis->print_buffer;

    if (to_console) {
        bml_utils_event_to_string(event, pThis->print_buffer, PRINT_BUFFER_LENGTH);
        std::cout << pThis->print_buffer << std::endl;
    } else {
        p += bml_utils_event_to_string(event, p, PRINT_BUFFER_LENGTH);
    }

    if (!to_console) {
        //send_message to m_analyzer_socket
        std::ptrdiff_t size = p - pThis->print_buffer;
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::events_update_to_console_cb(const struct BML_EVENT *event)
{
    cli_bml::events_update_cb(event, true);
}

void cli_bml::events_update_to_socket_cb(const struct BML_EVENT *event)
{
    cli_bml::events_update_cb(event, false);
}

//
// Caller functions
//
int cli_bml::connect_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return connect();
}

int cli_bml::onboard_status_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return onboard_status();
}

int cli_bml::ping_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return ping();
}

int cli_bml::nw_map_register_update_cb_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return nw_map_register_update_cb();
    return nw_map_register_update_cb(args.stringArgs[0]);
}

int cli_bml::nw_map_query_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return nw_map_query();
}

int cli_bml::bml_connection_map_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return connection_map();
}

int cli_bml::stat_register_cb_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return stat_register_cb();
    return stat_register_cb(args.stringArgs[0]);
}

int cli_bml::events_register_cb_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return events_register_cb();
    return events_register_cb(args.stringArgs[0]);
}

int cli_bml::set_wifi_credentials_caller(int numOfArgs)
{
    if (numOfArgs == 2)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1]);
    else if (numOfArgs == 3)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    else if (numOfArgs == 4)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.intArgs[2],
                                    args.intArgs[3]);
    else if (numOfArgs == 5)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.intArgs[2],
                                    args.intArgs[3], args.intArgs[4]);
    else
        return -1;
}

int cli_bml::get_wifi_credentials_caller(int numOfArgs)
{
    if (numOfArgs == 0)
        return get_wifi_credentials();
    else if (numOfArgs == 1)
        return get_wifi_credentials(args.intArgs[0]);
    else
        return -1;
}

int cli_bml::set_onboarding_state_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return set_onboarding_state(args.intArgs[0]);
}

int cli_bml::get_onboarding_state_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return get_onboarding_state();
}

int cli_bml::wps_onboarding_caller(int numOfArgs)
{
    if (numOfArgs == 0)
        return wps_onboarding();
    else
        return wps_onboarding(args.stringArgs[0]);
}

int cli_bml::get_device_info_caller(int numOfArgs) { return get_device_info(); }

int cli_bml::get_bml_version_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return get_bml_version();
}

int cli_bml::get_master_slave_versions_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return get_master_slave_versions();
}

int cli_bml::enable_legacy_client_roaming_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_legacy_client_roaming();
    return enable_legacy_client_roaming(args.intArgs[0]);
}

int cli_bml::enable_client_roaming_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_roaming();
    return enable_client_roaming(args.intArgs[0]);
}

int cli_bml::enable_client_roaming_prefer_signal_strength_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_roaming_prefer_signal_strength();
    return enable_client_roaming_prefer_signal_strength(args.intArgs[0]);
}

int cli_bml::enable_client_band_steering_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_band_steering();
    return enable_client_band_steering(args.intArgs[0]);
}

int cli_bml::enable_ire_roaming_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_ire_roaming();
    return enable_ire_roaming(args.intArgs[0]);
}

int cli_bml::enable_load_balancer_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_load_balancer();
    return enable_load_balancer(args.intArgs[0]);
}

int cli_bml::enable_service_fairness_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_service_fairness();
    return enable_service_fairness(args.intArgs[0]);
}

int cli_bml::enable_dfs_reentry_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_dfs_reentry();
    return enable_dfs_reentry(args.intArgs[0]);
}

int cli_bml::enable_certification_mode_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_certification_mode();
    return enable_certification_mode(args.intArgs[0]);
}

int cli_bml::set_log_level_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return set_log_level(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    } else if (numOfArgs == 4) {
        return set_log_level(args.stringArgs[0], args.stringArgs[1], args.intArgs[2],
                             args.stringArgs[3]);
    }
    return -1;
}

int cli_bml::set_global_restricted_channels_caller(int numOfArgs)
{
    std::cout << "numOfArgs= " << int(numOfArgs) << std::endl;
    if (numOfArgs == 1) {
        std::cout << "args.stringArgs[0] " << args.stringArgs[0] << std::endl;
        return set_global_restricted_channels(args.stringArgs[0]);
    }
    return -1;
}
int cli_bml::get_global_restricted_channels_caller(int numOfArgs)
{
    if (numOfArgs == 0) {
        return get_global_restricted_channels();
    }
    return -1;
}

int cli_bml::set_slave_restricted_channels_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return set_slave_restricted_channels(args.stringArgs[0], args.stringArgs[1]);
    }
    return -1;
}

int cli_bml::get_slave_restricted_channels_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_slave_restricted_channels(args.stringArgs[0]);
    }
    return -1;
}

int cli_bml::bml_trigger_topology_discovery_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return topology_discovery(args.stringArgs[0]);
    }
    return -1;
}

int cli_bml::bml_channel_selection_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return channel_selection(args.stringArgs[0], args.stringArgs[1]);
    }
    return -1;
}
#ifdef BEEROCKS_RDKB
int cli_bml::bml_rdkb_steering_set_group_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return steering_set_group(args.intArgs[0], args.stringArgs[1], args.stringArgs[2]);
    }
    return -1;
}
int cli_bml::bml_rdkb_steering_client_set_caller(int numOfArgs)
{
    if (numOfArgs == 4) {
        return steering_client_set(args.intArgs[0], args.stringArgs[1], args.stringArgs[2],
                                   args.stringArgs[3]);
    } else if (numOfArgs == 3) {
        return steering_client_set(args.intArgs[0], args.stringArgs[1], args.stringArgs[2]);
    }
    return -1;
}
int cli_bml::bml_rdkb_steering_event_register_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return steering_event_register();
    return steering_event_register(args.stringArgs[0]);
}

int cli_bml::bml_rdkb_steering_client_measure_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return steering_client_measure(args.intArgs[0], args.stringArgs[1], args.stringArgs[2]);
    }
    return -1;
}
#endif //BEEROCKS_RDKB

/**
 * caller function for set_dcs_continuous_scan_enable
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_enable_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return set_dcs_continuous_scan_enable(args.stringArgs[0], args.intArgs[1]);
    }
    return -1;
}

/**
 * caller function for get_dcs_continuous_scan_enable
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_enable_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_dcs_continuous_scan_enable(args.stringArgs[0]);
    }
    return -1;
}

/**
 * caller function for set_dcs_continuous_scan_params
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_params_caller(int numOfArgs)
{
    std::string radio_mac(network_utils::WILD_MAC_STRING);
    int32_t dwell_time    = BML_CHANNEL_SCAN_INVALID_PARAM;
    int32_t interval_time = BML_CHANNEL_SCAN_INVALID_PARAM;
    std::string channel_pool;

    std::string::size_type pos;
    //[radio_mac=<radio_mac> dwell_time=<dwell_time> interval_time='<interval_time>']"
    for (int i = 1; i < numOfArgs; i++) { //first optional arg
        if ((pos = args.stringArgs[i].find("radio_mac=")) != std::string::npos) {
            radio_mac = args.stringArgs[i].substr(pos + sizeof("radio_mac"));
        } else if ((pos = args.stringArgs[i].find("dwell_time=")) != std::string::npos) {
            dwell_time = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("dwell_time")));
        } else if ((pos = args.stringArgs[i].find("interval_time=")) != std::string::npos) {
            interval_time =
                string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("interval_time")));
        } else if ((pos = args.stringArgs[i].find("channel_pool=")) != std::string::npos) {
            channel_pool = args.stringArgs[i].substr(pos + sizeof("channel_pool"));
        }
    }

    if (numOfArgs > 1) {
        return set_dcs_continuous_scan_params(radio_mac, dwell_time, interval_time, channel_pool);
    }
    return -1;
}

/**
 * caller function for get_dcs_continuous_scan_params
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_params_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_dcs_continuous_scan_params(args.stringArgs[0]);
    }
    return -1;
}

/**
 * caller function for start_dcs_single_scan
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::start_dcs_single_scan_caller(int numOfArgs)
{
    if (numOfArgs == 4) {
        return start_dcs_single_scan(args.stringArgs[0], args.intArgs[1], args.stringArgs[2]);
    }
    return -1;
}

/**
 * caller function for get_dcs_scan_results
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_scan_results_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return get_dcs_scan_results(args.stringArgs[0], args.intArgs[1]);
    } else if (numOfArgs == 3) {
        bool single_scan = (args.intArgs[2] == 1);
        return get_dcs_scan_results(args.stringArgs[0], args.intArgs[1], single_scan);
    }
    return -1;
}

//
// Functions
//

int cli_bml::get_onboarding_status()
{
    onboard_status();
    return is_onboarding;
}

bool cli_bml::is_pending_response() { return pending_response; }

int cli_bml::onboard_status()
{
    is_onboarding = bml_onboard_status(ctx);
    std::cout << "onboard_status = " << is_onboarding << std::endl;
    return 0;
}

int cli_bml::ping()
{
    int ret = bml_ping(ctx);
    printBmlReturnVals("bml_ping", ret);
    return 0;
}

int cli_bml::nw_map_register_update_cb(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_nw_map_register_update_cb(ctx, NULL);
    } else {
        ret = bml_nw_map_register_update_cb(ctx, map_update_to_console_cb);
    }
    printBmlReturnVals("bml_nw_map_register_update_cb", ret);
    return 0;
}

int cli_bml::nw_map_query()
{
    // Register the query callback
    bml_nw_map_register_query_cb(ctx, cli_bml::map_query_to_console_cb);
    pending_response = true;
    int ret          = bml_nw_map_query(ctx);
    printBmlReturnVals("bml_nw_map_query", ret);
    return 0;
}

int cli_bml::connection_map()
{
    // Register the query callback
    bml_nw_map_register_query_cb(ctx, cli_bml::connection_map_to_console_cb);
    pending_response = true;
    int ret          = bml_nw_map_query(ctx);
    printBmlReturnVals("bml_nw_map_query", ret);
    return 0;
}

int cli_bml::stat_register_cb(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_stat_register_cb(ctx, NULL);
    } else {
        ret = bml_stat_register_cb(ctx, stats_update_to_console_cb);
    }
    printBmlReturnVals("bml_stat_register_cb", ret);
    return 0;
}

int cli_bml::events_register_cb(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_event_register_cb(ctx, NULL);
    } else {
        ret = bml_event_register_cb(ctx, events_update_to_console_cb);
    }
    printBmlReturnVals("bml_event_register_cb", ret);
    return 0;
}

int cli_bml::set_wifi_credentials(const std::string &ssid, const std::string &pass, int sec,
                                  int vap_id, int force)
{

    int ret = bml_set_wifi_credentials(ctx, ssid.c_str(), pass.c_str(), sec, vap_id, force);

    printBmlReturnVals("bml_set_wifi_credentials", ret);
    return 0;
}

int cli_bml::get_wifi_credentials(int vap_id)
{
    char ssid[BML_NODE_SSID_LEN];
    int sec;
    ssid[0] = 0;
    int ret = bml_get_wifi_credentials(ctx, vap_id, ssid, nullptr, &sec);
    std::string sec_str;

    switch (sec) {
    case BML_WLAN_SEC_NONE:
        sec_str = BML_WLAN_SEC_NONE_STR;
        break;

    case BML_WLAN_SEC_WEP64:
        sec_str = BML_WLAN_SEC_WEP64_STR;
        break;

    case BML_WLAN_SEC_WEP128:
        sec_str = BML_WLAN_SEC_WEP128_STR;
        break;

    case BML_WLAN_SEC_WPA_PSK:
        sec_str = BML_WLAN_SEC_WPA_PSK_STR;
        break;

    case BML_WLAN_SEC_WPA2_PSK:
        sec_str = BML_WLAN_SEC_WPA2_PSK_STR;
        break;

    case BML_WLAN_SEC_WPA_WPA2_PSK:
        sec_str = BML_WLAN_SEC_WPA_WPA2_PSK_STR;
        break;

    default:
        sec_str = "Unknown";
    }

    std::cout << "SSID='" << ssid << "', Security=" << sec_str << std::endl;

    printBmlReturnVals("bml_get_wifi_credentials", ret);
    return 0;
}

int cli_bml::set_onboarding_state(int enable)
{
    int ret = bml_set_onboarding_state(ctx, enable);
    printBmlReturnVals("bml_set_onboarding_state", ret);
    return 0;
}

int cli_bml::get_onboarding_state()
{
    int enable;
    int ret = bml_get_onboarding_state(ctx, &enable);
    std::cout << "Onboarding=" << enable << std::endl;
    printBmlReturnVals("bml_get_onboarding_state", ret);
    return 0;
}

int cli_bml::wps_onboarding(std::string iface)
{
    int ret = bml_wps_onboarding(ctx, iface.c_str());
    printBmlReturnVals("bml_wps_onboarding", ret);
    return 0;
}

int cli_bml::get_device_info()
{
    BML_DEVICE_INFO device_info;
    int ret = bml_get_device_info(ctx, &device_info);

    std::cout << "Manufacturer : " << device_info.manufacturer << std::endl;
    std::cout << "Model Name   : " << device_info.model_name << std::endl;
    std::cout << "Serial Number: " << device_info.serial_number << std::endl;

    std::cout << "LAN          : ";
    if (device_info.lan_iface_name[0]) {
        std::cout << device_info.lan_iface_name << " ("
                  << network_utils::ipv4_to_string(device_info.lan_ip_address) << "/"
                  << network_utils::ipv4_to_string(device_info.lan_network_mask) << ")";
    } else {
        std::cout << "N/A";
    }
    std::cout << std::endl;

    std::cout << "WAN          : ";
    if (device_info.wan_iface_name[0]) {
        std::cout << device_info.wan_iface_name << " ("
                  << network_utils::ipv4_to_string(device_info.wan_ip_address) << "/"
                  << network_utils::ipv4_to_string(device_info.wan_network_mask) << ")";
    } else {
        std::cout << "N/A";
    }
    std::cout << std::endl;

    printBmlReturnVals("bml_get_device_info", ret);
    return 0;
}

int cli_bml::get_bml_version()
{
    std::string ver = bml_get_bml_version();
    std::cout << "Bml version = " << ver << std::endl;
    return 0;
}

int cli_bml::get_master_slave_versions()
{
    beerocks_message::sVersions versions;
    versions.master_version[0] = 0;
    versions.slave_version[0]  = 0;
    int ret = bml_get_master_slave_versions(ctx, versions.master_version, versions.slave_version);
    std::cout << "Beerocks master version = " << versions.master_version
              << ", Beerocks slave version = " << versions.slave_version << std::endl;

    printBmlReturnVals("bml_get_master_slave_versions", ret);
    return 0;
}

int cli_bml::enable_client_roaming(int8_t isEnable)
{
    int result;
    int ret = (isEnable < 0) ? bml_get_client_roaming(ctx, &result)
                             : bml_set_client_roaming(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_roaming", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client_roaming mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_legacy_client_roaming(int8_t isEnable)
{
    int result;
    int ret = (isEnable < 0) ? bml_get_legacy_client_roaming(ctx, &result)
                             : bml_set_legacy_client_roaming(ctx, isEnable);
    printBmlReturnVals("bml_enable_legacy_client_roaming", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "legacy_client_roaming mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_client_roaming_prefer_signal_strength(int8_t isEnable)
{
    int result;
    int ret = (isEnable < 0) ? bml_get_client_roaming_prefer_signal_strength(ctx, &result)
                             : bml_set_client_roaming_prefer_signal_strength(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_roaming_prefer_signal_strength", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client roaming prefer signal strength mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_client_band_steering(int8_t isEnable)
{
    int result;
    int ret = (isEnable < 0) ? bml_get_client_band_steering(ctx, &result)
                             : bml_set_client_band_steering(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_band_steering", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client_band_steering mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_ire_roaming(int8_t isEnable)
{
    int result;
    int ret =
        (isEnable < 0) ? bml_get_ire_roaming(ctx, &result) : bml_set_ire_roaming(ctx, isEnable);
    printBmlReturnVals("bml_enable_ire_roaming", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "ire_roaming mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_load_balancer(int8_t isEnable)
{
    int result;
    int ret =
        (isEnable < 0) ? bml_get_load_balancer(ctx, &result) : bml_set_load_balancer(ctx, isEnable);
    printBmlReturnVals("bml_load_balancer", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "load_balancer mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_service_fairness(int8_t isEnable)
{
    int result;
    int ret = (isEnable < 0) ? bml_get_service_fairness(ctx, &result)
                             : bml_set_service_fairness(ctx, isEnable);
    printBmlReturnVals("bml_enable_service_fairness", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "service_fairness mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_dfs_reentry(int8_t isEnable)
{
    int result;
    int ret =
        (isEnable < 0) ? bml_get_dfs_reentry(ctx, &result) : bml_set_dfs_reentry(ctx, isEnable);
    printBmlReturnVals("bml_enable_dfs_reentry", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "service_dfs_reentry = " << result << std::endl;
    return 0;
}

int cli_bml::enable_certification_mode(int8_t isEnable)
{
    int result;
    int ret = (isEnable < 0) ? bml_get_certification_mode(ctx, &result)
                             : bml_set_certification_mode(ctx, isEnable);
    printBmlReturnVals("bml_enable_certification_mode", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "enable_certification_mode = " << result << std::endl;
    return 0;
}

int cli_bml::set_log_level(const std::string &module_name, const std::string &log_level, uint8_t on,
                           const std::string &mac)
{
    int ret = bml_set_log_level(ctx, module_name.c_str(), log_level.c_str(), on, mac.c_str());
    printBmlReturnVals("bml_set_log_level", ret);
    return 0;
}

int cli_bml::analyzer_init(std::string remote_pc_ip)
{
    //connect to master via bml
    int retries    = 36; //36 retries of 5 sec = 3 minutes
    bool connected = connect();
    while (!connected && retries > 0) {
        retries--;
        UTILS_SLEEP_MSEC(5000);
        connected = connect();
    }
    if (!connected) {
        std::cout << "failed connecting to master via bml, exiting... " << std::endl;
        return -1;
    }

    //Open tcp client socket to remote pc
    m_analyzer_socket = new SocketClient(remote_pc_ip, beerocks::ANALYZER_TCP_PORT);
    if (!m_analyzer_socket->getError().empty()) {
        LOG(ERROR) << "Failed connecting to remote pc: " << m_analyzer_socket->getError()
                   << ", exiting..." << std::endl;

        m_analyzer_socket->closeSocket();
        delete m_analyzer_socket;
        m_analyzer_socket = nullptr;

        return -1;
    }

    is_analyzer = true;
    select.addSocket(m_analyzer_socket);

    //register map_query_to_socket_cb and send a query request
    if (bml_nw_map_register_query_cb(ctx, cli_bml::map_query_to_socket_cb)) {
        std::cout << "failed to register nw_map_query to socket cb" << std::endl;
    }
    if (bml_nw_map_query(ctx)) {
        std::cout << "failed to send bml_nw_map_query request" << std::endl;
    }

    //register to stats and map updates
    if (bml_nw_map_register_update_cb(ctx, map_update_to_socket_cb)) {
        std::cout << "failed to register nw_map_update to socket cb" << std::endl;
    }
    if (bml_stat_register_cb(ctx, stats_update_to_socket_cb)) {
        std::cout << "failed to register statistics to socket cb" << std::endl;
    }
    if (bml_event_register_cb(ctx, events_update_to_socket_cb)) {
        std::cout << "failed to register events to socket cb" << std::endl;
    }

    int t = 0;
    while (is_analyzer) {
        UTILS_SLEEP_MSEC(1000);
        int sel_ret = select.selectSocket();
        if (sel_ret > 0) {
            m_analyzer_socket->readBytes(rx_buffer, message::MESSAGE_BUFFER_LENGTH,
                                         m_analyzer_socket->getBytesReady());
            if (bml_nw_map_query(ctx)) {
                std::cout << "failed to send bml_nw_map_query request" << std::endl;
                break;
            }
        }
        if (t < 19) {
            t++;
        } else {
            t = 0;
            if (bml_nw_map_query(ctx)) {
                std::cout << "failed to send bml_nw_map_query request" << std::endl;
                break;
            }
        }
    }

    return 0;
}

int cli_bml::set_global_restricted_channels(const std::string &restricted_channels)
{
    std::cout << "restricted_channels string = " << restricted_channels << std::endl;
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN] = {};
    auto vec_substr = string_utils::str_split(restricted_channels, ',');
    std::cout << "restricted_channels string = " << restricted_channels << std::endl;

    int count = 0;
    for (const auto &elment : vec_substr) {
        std::cout << elment << " count = " << int(count) << std::endl;
        restricted_channel[count] = uint8_t(string_utils::stoi(elment));
        count++;
    }

    for (const auto &channel : restricted_channel) {
        std::cout << "restricted_channels int = " << int(channel) << std::endl;
    }
    // std::for_each(std::begin(vec_substr) , std::end(vec_substr) , [&](std::string elm_substr) {
    //     std::cout << "elm_substr = " << elm_substr << std::endl;
    //     restricted_channel[count] = uint8_t(string_utils::stoi(elm_substr));
    //     count++;
    // });
    int ret = bml_set_global_restricted_channels(ctx, restricted_channel, uint8_t(count + 1));
    printBmlReturnVals("bml_set_global_restricted_channels", ret);
    return 0;
}

int cli_bml::get_global_restricted_channels()
{
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN];
    int ret = bml_get_global_restricted_channels(ctx, restricted_channel);
    std::cout << "Restricted_channels: = " << std::endl;
    for (auto &elem : restricted_channel) {
        std::cout << int(elem) << " ,";
    }
    printBmlReturnVals("bml_get_global_restricted_channels", ret);
    return 0;
}

int cli_bml::set_slave_restricted_channels(const std::string &restricted_channels,
                                           const std::string &hostap_mac)
{
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN];
    auto vec_substr = string_utils::str_split(restricted_channels, ',');
    int count       = 0;
    std::for_each(vec_substr.begin(), vec_substr.end(), [&](std::string substr) {
        restricted_channel[count] = uint8_t(string_utils::stoi(substr));
        count++;
    });
    std::cout << "hostap_mac = " << hostap_mac << std::endl;
    int ret = bml_set_slave_restricted_channels(ctx, restricted_channel, hostap_mac.c_str(),
                                                uint8_t(count));
    printBmlReturnVals("set_slave_restricted_channels", ret);
    return 0;
}

int cli_bml::get_slave_restricted_channels(const std::string &hostap_mac)
{
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN];
    int ret = bml_get_slave_restricted_channels(ctx, restricted_channel, hostap_mac.c_str());
    std::cout << "Restricted_channels for hostap_mac = " << hostap_mac << std::endl;
    for (auto &elem : restricted_channel) {
        std::cout << int(elem) << " ,";
    }
    printBmlReturnVals("get_slave_restricted_channels", ret);
    return 0;
}

int cli_bml::topology_discovery(const std::string &al_mac)
{
    int ret = bml_trigger_topology_discovery(ctx, al_mac.c_str());
    printBmlReturnVals("topology discovery", ret);
    return 0;
}

int cli_bml::channel_selection(const std::string &al_mac, const std::string &ruid)
{
    int ret = bml_channel_selection(ctx, al_mac.c_str(), ruid.c_str());
    printBmlReturnVals("channel_selection", ret);
    return 0;
}

#ifdef BEEROCKS_RDKB
int cli_bml::steering_set_group(uint32_t steeringGroupIndex, const std::string &str_cfg_2,
                                const std::string &str_cfg_5)
{

    BML_STEERING_AP_CONFIG cfg2 = {};
    BML_STEERING_AP_CONFIG cfg5 = {};
    std::cout << "set_ap_config entry" << std::endl;
    ;
    steering_set_group_string_to_struct(str_cfg_2, str_cfg_5, cfg2, cfg5);

    int ret = bml_rdkb_steering_set_group(ctx, steeringGroupIndex, &cfg2, &cfg5);
    printBmlReturnVals("bml_rdkb_steering_set_group", ret);
    return 0;
}

int cli_bml::steering_client_set(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                 const std::string &str_client_mac, const std::string &str_config)
{
    BML_MAC_ADDR client_mac;
    network_utils::mac_from_string(client_mac, str_client_mac);
    BML_MAC_ADDR bssid;
    network_utils::mac_from_string(bssid, str_bssid);
    int ret;
    if (str_config.empty()) {
        //client remove
        ret = bml_rdkb_steering_client_set(ctx, steeringGroupIndex, bssid, client_mac, nullptr);
    } else {
        // client add
        BML_STEERING_CLIENT_CONFIG config = {};
        steering_client_set_string_to_struct(str_config, config);
        ret = bml_rdkb_steering_client_set(ctx, steeringGroupIndex, bssid, client_mac, &config);
    }
    printBmlReturnVals("bml_rdkb_steering_client_set ", ret);
    return 0;
}

int cli_bml::steering_event_register(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_rdkb_steering_event_register(ctx, NULL);
    } else {
        ret = bml_rdkb_steering_event_register(ctx, events_update_to_console_cb);
    }

    printBmlReturnVals("bml_rdkb_steering_event_register", ret);
    return 0;
}

int cli_bml::steering_client_measure(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                     const std::string &str_client_mac)
{
    BML_MAC_ADDR client_mac;
    network_utils::mac_from_string(client_mac, str_client_mac);
    BML_MAC_ADDR bssid;
    network_utils::mac_from_string(bssid, str_bssid);
    int ret = bml_rdkb_steering_client_measure(ctx, steeringGroupIndex, bssid, client_mac);
    printBmlReturnVals("bml_rdkb_steering_client_measure", ret);
    return 0;
}
#endif //BEEROCKS_RDKB

/**
 * Enables or disables beerocks DCS continuous scans.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 * @param [in] enable Value of 1 to enable or 0 to disable.
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_enable(const std::string &radio_mac, int8_t enable)
{
    std::cout << "set_dcs_continuous_scan_enable" << std::endl;

    return 0;
}

/**
 * get DCS continuous scans param. Value is printed to the console.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_enable(const std::string &radio_mac)
{
    std::cout << __func__ << ", mac=" << radio_mac << std::endl;

    return 0;
}

/**
 * set DCS continuous scan params.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 * @param [in] dwell_time Set the dwell time in milliseconds.
 * @param [in] interval_time Set the interval time in seconds.
 * @param [in] channel_pool Set the channel pool for the DCS.
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_params(const std::string &radio_mac, int32_t dwell_time,
                                            int32_t interval_time, const std::string &channel_pool)
{
    std::cout << __func__ << ", mac=" << radio_mac << ", dwell_time=" << dwell_time
              << ", interval_time=" << interval_time << ", channel_pool=" << channel_pool
              << std::endl;

    return 0;
}

/**
 * get DCS continuous scan params. Values are printed to the console.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_params(const std::string &radio_mac)
{
    std::cout << __func__ << ", mac=" << radio_mac << std::endl;

    return 0;
}

/**
 * Start a single DCS scan with parameters.
 *
 * @param [in] radio_mac radio MAC of selected radio
 * @param [in] dwell_time Set the dwell time in milliseconds.
 * @param [in] channel_pool Set the channel pool for the DCS.
 *
 * @return 0 on success.
 */
int cli_bml::start_dcs_single_scan(const std::string &radio_mac, int32_t dwell_time,
                                   const std::string &channel_pool)
{
    std::cout << "start_dcs_single_scan, mac=" << radio_mac << ", dwell_time=" << dwell_time
              << ", channel_pool=" << channel_pool << std::endl;

    return 0;
}

/**
 * get DCS continuous scan params.
 *
 * @param [in] radio_mac radio MAC of selected radio
 * @param [in] max_results_size Max number of the returned results
 * @param [in] is_single_scan Flag indicating if the results belong to a single scan or not
 * 
 * @return 0 on success.
 */
int cli_bml::get_dcs_scan_results(const std::string &radio_mac, uint32_t max_results_size,
                                  bool is_single_scan)
{
    std::cout << __func__ << ", mac=" << radio_mac << ", max_results_size=" << max_results_size
              << ", is_single_scan=" << string_utils::bool_str(is_single_scan) << std::endl;

    return 0;
}
