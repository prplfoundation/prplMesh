/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "load_balancer_task.h"
#include "../db/db_algo.h"
#include "../son_actions.h"

#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace son;

#define ASSERT_NONZERO(a)                                                                          \
    if (a == 0) {                                                                                  \
        TASK_LOG(ERROR) << #a << " is zero! exiting";                                              \
        finish();                                                                                  \
        return;                                                                                    \
    }

load_balancer_task::load_balancer_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                       task_pool &tasks_, std::string ire_mac_,
                                       std::string task_name_)
    : task(task_name_), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_), ire_mac(ire_mac_)
{
    set_task_timeout(25000);
}

void load_balancer_task::work()
{
    /*
    if (!database.settings_load_balancing()) {
        TASK_LOG(INFO) << "load balancing disabled, killing task";
        finish();
        return;
    }
    */

    switch (state) {
    case START: {
        int prev_task_id = database.get_load_balancer_task_id(ire_mac);
        tasks.kill_task(prev_task_id);
        if (!database.assign_load_balancer_task_id(ire_mac, id)) {
            TASK_LOG(ERROR) << "can't assign this task to node " << ire_mac;
            finish();
            return;
        }

        state = REQUEST_LOAD_MEASUREMENTS;

        hostaps = database.get_node_children(ire_mac, beerocks::TYPE_SLAVE);
        break;
    }
    case REQUEST_LOAD_MEASUREMENTS: {
        bool load_info_valid = true;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST>(cmdu_tx, id);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }

        for (auto hostap : hostaps) {
            if (database.get_hostap_stats_info_timestamp(hostap) <= start_timestamp) {
                /*
                     * if the load info is not up-to-date, request a new report
                     */
                TASK_LOG(DEBUG) << "load info outdated, requestsing load measurement from hostap "
                                << hostap;
                son_actions::send_cmdu_to_agent(database.get_node_parent_ire(hostap), cmdu_tx,
                                                database, hostap);
                add_pending_mac(hostap,
                                beerocks_message::ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE);
                load_info_valid = false;
            }
        }

        if (load_info_valid) {
            state = BALANCE_STATIONS;
        } else {
            state = REQUEST_LOAD_MEASUREMENTS;
            set_responses_timeout(4000);
        }
        break;
    }

    case BALANCE_STATIONS: {
        /*
             * first we need to find the most loaded hostap to perform balancing on
             */
        std::string most_loaded_hostap;
        int max_load = 0;

        for (auto hostap : hostaps) {
            int hostap_channel_load = database.get_hostap_channel_load_percent(hostap);
            if (hostap_channel_load > max_load) {
                most_loaded_hostap = hostap;
                max_load           = hostap_channel_load;
            } else if (hostap_channel_load == max_load) {
                if (database.get_node_children(hostap).size() >
                    database.get_node_children(most_loaded_hostap).size()) {
                    /*
                         * TODO might need different sta count criteria for 2.4ghz and 5ghz hostaps
                         */
                    most_loaded_hostap = hostap;
                }
            }
        }

        /*
             * now that the hostap was found we need to find its least efficient sta in case of a 5ghz ap
             * or the most efficient sta in case of a 2.4ghz ap
             */
        bool current_ap_is_5ghz = database.is_node_5ghz(most_loaded_hostap);
        int ap_total_duration_ms =
            database.get_hostap_stats_measurement_duration(most_loaded_hostap);
        int ap_sta_load_percent =
            database.get_hostap_total_client_tx_load_percent(most_loaded_hostap) +
            database.get_hostap_total_client_rx_load_percent(most_loaded_hostap);
        int ap_tx_bytes = database.get_hostap_total_sta_tx_bytes(most_loaded_hostap);
        int ap_rx_bytes = database.get_hostap_total_sta_rx_bytes(most_loaded_hostap);

        ASSERT_NONZERO(ap_tx_bytes);
        ASSERT_NONZERO(ap_rx_bytes);

        LOG_CLI(DEBUG, "most loaded hostap is "
                           << most_loaded_hostap << " with " << max_load << " percent channel load"
                           << std::endl
                           << "ap_total_duration_ms=" << ap_total_duration_ms
                           << " ap_sta_load_percent=" << ap_sta_load_percent << std::endl
                           << "ap_tx_bytes=" << ap_tx_bytes << " ap_rx_bytes=" << ap_rx_bytes);

        /*
            LOG_CLI(DEBUG, "load_balancer_task: " << std::endl <<
                           "    most loaded hostap is " << most_loaded_hostap << " with " << max_load << " percent channel load" << std::endl);
                           */

        std::string chosen_client;
        int chosen_client_tx_bytes           = 0;
        int chosen_client_rx_bytes           = 0;
        int chosen_client_airtime_percentage = 0;
        float chosen_client_efficiency_ratio = 0;
        float min_efficiency_ratio           = std::numeric_limits<float>::max();
        float max_efficiency_ratio           = std::numeric_limits<float>::min();
        auto most_loaded_hostap_clients      = database.get_node_children(most_loaded_hostap);

        for (auto client : most_loaded_hostap_clients) {
            int client_tx_bytes           = database.get_node_tx_bytes(client);
            int client_rx_bytes           = database.get_node_rx_bytes(client);
            int client_airtime_percentage = database.get_node_tx_load_percent(client) +
                                            database.get_node_rx_load_percent(client);
            //int client_bytes_percentage = 100 * ((client_tx_bytes / ap_rx_bytes) + (client_rx_bytes / ap_tx_bytes));

            int client_bytes_percentage = 0;
            if (ap_rx_bytes > 0) {
                client_bytes_percentage += (client_tx_bytes / ap_rx_bytes);
            }
            if (ap_tx_bytes > 0) {
                client_bytes_percentage += (client_rx_bytes / ap_tx_bytes);
            }
            client_bytes_percentage = 100 * client_bytes_percentage;

            float client_efficiency_ratio = 0;
            if (client_airtime_percentage > 0) {
                //client_efficiency_ratio = (float)client_bytes_percentage / (float)client_airtime_percentage;
                double client_tx_bitrate          = database.get_node_tx_bitrate(client);
                double client_rx_bitrate          = database.get_node_rx_bitrate(client);
                uint16_t client_rx_phy_rate_100kb = database.get_load_rx_phy_rate_100kb(client);
                uint16_t client_tx_phy_rate_100kb = database.get_load_tx_phy_rate_100kb(client);

                if (client_tx_bitrate > 0) {
                    client_efficiency_ratio +=
                        ((double(client_tx_phy_rate_100kb) / 10) / client_tx_bitrate) *
                        (double(client_tx_bytes) / double(client_tx_bytes + client_rx_bytes));
                }

                if (client_rx_bitrate > 0) {
                    client_efficiency_ratio +=
                        ((double(client_rx_phy_rate_100kb) / 10) / client_rx_bitrate) *
                        (double(client_rx_bytes) / double(client_tx_bytes + client_rx_bytes));
                }

                client_efficiency_ratio =
                    (float)client_bytes_percentage / (float)client_airtime_percentage;
            }

            //TASK_LOG(DEBUG) << "client_bytes_percentage=" << client_bytes_percentage;
            //TASK_LOG(DEBUG) << "client_efficiency_ratio=" << client_efficiency_ratio;

            if (current_ap_is_5ghz) {
                if (client_efficiency_ratio > max_efficiency_ratio) {
                    max_efficiency_ratio             = client_efficiency_ratio;
                    chosen_client                    = client;
                    chosen_client_tx_bytes           = client_tx_bytes;
                    chosen_client_rx_bytes           = client_rx_bytes;
                    chosen_client_airtime_percentage = client_airtime_percentage;
                    chosen_client_efficiency_ratio   = client_efficiency_ratio;
                }
            } else {
                if (client_efficiency_ratio < min_efficiency_ratio) {
                    min_efficiency_ratio             = client_efficiency_ratio;
                    chosen_client                    = client;
                    chosen_client_tx_bytes           = client_tx_bytes;
                    chosen_client_rx_bytes           = client_rx_bytes;
                    chosen_client_airtime_percentage = client_airtime_percentage;
                    chosen_client_efficiency_ratio   = client_efficiency_ratio;
                }
            }
        }

        if (!chosen_client.empty()) {
            LOG_CLI(DEBUG,
                    "chosen client on hostap "
                        << most_loaded_hostap << " is " << chosen_client << std::endl
                        << "chosen_client_efficiency_ratio=" << chosen_client_efficiency_ratio
                        << " chosen_client_airtime_percentage=" << chosen_client_airtime_percentage
                        << std::endl
                        << "chosen_client_tx_bytes=" << chosen_client_tx_bytes
                        << " chosen_client_rx_bytes=" << chosen_client_rx_bytes);

            /*
                LOG_CLI(DEBUG, "load_balancer_task: " << std::endl <<
                               "    chosen client on hostap " << most_loaded_hostap << " is " << chosen_client << std::endl);
                               */
            /*
                 * assign task to chosen client as well
                 */
            database.assign_load_balancer_task_id(chosen_client, id);
        } else {
            TASK_LOG(ERROR) << "chosen client is empty! finishing task";

            LOG_CLI(DEBUG, "load_balancer_task: " << std::endl
                                                  << "    chosen client is empty! finishing task"
                                                  << std::endl);
            finish();
            return;
        }

/*
             * now that we know which client we want to move
             * we need to simulate the alternative APs that we can move it to
             * and see if the total throughput would be improved
             */

/*
             * calculate the extra throughput to be gained on the loaded hostap by moving the
             * chosen STA elsewhere and redistributing its airtime among the other STAs
             */
#if 0
            int other_clients_airtime_percentage = ap_sta_load_percent - chosen_client_airtime_percentage;
            ASSERT_NONZERO(other_clients_airtime_percentage);
            int extra_available_bytes = 0;
            for (auto client : clients) {
                float client_tx_share = (float)database.get_node_tx_load_percent(client) / other_clients_airtime_percentage;
                float client_rx_share = (float)database.get_node_rx_load_percent(client) / other_clients_airtime_percentage;
                int client_tx_bytes = database.get_node_tx_bytes(client);
                int client_rx_bytes = database.get_node_rx_bytes(client);
                extra_available_bytes = client_tx_bytes * (1 + client_tx_share) + client_rx_bytes * (1 + client_rx_share);
            }
            ASSERT_NONZERO(ap_total_duration_ms);
            int extra_available_bytes_per_second = extra_available_bytes / ap_total_duration_ms;
            TASK_LOG(DEBUG) << "removing client " << chosen_client << " from hostap " << most_loaded_hostap << " gains extra " << extra_available_bytes_per_second << "Bps for other clients";
            LOG_CLI(DEBUG, "load_balancer_task: " << std::endl <<
                           "    removing client " << chosen_client << " from hostap " << most_loaded_hostap << " gains extra " << extra_available_bytes_per_second << "Bps for other clients" << std::endl);
#else
//int extra_available_bytes = 0;
//int extra_available_bytes_per_second = 0;
#endif
        hostaps.erase(most_loaded_hostap);
        /*
             * now we check how each of the other hostaps' throughput would be affected
             * by transferring the STA to them
             */

        uint32_t chosen_client_bytes = chosen_client_tx_bytes + chosen_client_rx_bytes;

        std::string chosen_hostap;
        int chosen_hostap_predicted_bitrate       = INT32_MIN;
        int chosen_hostap_bytes_per_second_gained = INT32_MIN;
        bool sta_is_5ghz                          = database.is_node_5ghz(chosen_client);
        bool confine                              = false;

        auto sta_capabilities          = database.get_station_current_capabilities(chosen_client);
        uint16_t sta_phy_tx_rate_100kb = database.get_node_rx_phy_rate_100kb(chosen_client);

        for (auto hostap : hostaps) {
            son::wireless_utils::sPhyApParams hostap_params;

            hostap_params.is_5ghz = database.is_node_5ghz(hostap);

            if (hostap_params.is_5ghz && !database.get_node_5ghz_support(sta_mac)) {
                TASK_LOG(DEBUG) << "sta " << sta_mac
                                << " does not support 5ghz, skipping candidate " << hostap;
                continue;
            }

            hostap_params.bw       = database.get_node_bw(hostap);
            hostap_params.ant_num  = database.get_hostap_ant_num(hostap);
            hostap_params.ant_gain = database.get_hostap_ant_gain(hostap);
            hostap_params.tx_power = database.get_hostap_tx_power(hostap);

            int ul_rssi;
            //int estimated_ul_rssi, hostap_dl_rssi = beerocks::RSSI_INVALID;
            int8_t rx_rssi, rx_packets;
            if (database.get_node_cross_rx_rssi(sta_mac, chosen_hostap, rx_rssi, rx_packets)) {
                LOG(ERROR) << "can't get cross_rx_rssi for hostap " << chosen_hostap;
                continue;
            }

            ul_rssi = rx_rssi;

            // if(hostap_params.is_5ghz != current_ap_is_5ghz){ // cross band estimation
            //     if(current_ap_is_5ghz){
            //         estimated_ul_rssi = ul_rssi + database.config.roaming_band_pathloss_delta_db;
            //     }else{
            //         estimated_ul_rssi = ul_rssi - database.config.roaming_band_pathloss_delta_db;
            //     }
            // }else{
            //     estimated_ul_rssi = ul_rssi;
            // }

            son::wireless_utils::sPhyUlParams current_ul_params;
            current_ul_params = son::wireless_utils::estimate_ul_params(
                ul_rssi, sta_phy_tx_rate_100kb, sta_capabilities, hostap_params.bw,
                hostap_params.is_5ghz);
            //hostap_dl_rssi = son::wireless_utils::estimate_dl_rssi(estimated_ul_rssi, current_ul_params.tx_power, hostap_params);

            //double predicted_chosen_client_phy_rate = son::wireless_utils::estimate_ap_tx_phy_rate(hostap_dl_rssi, sta_capabilities, hostap_params.bw, hostap_params.is_5ghz);

            int hostap_duration_ms = database.get_hostap_stats_measurement_duration(hostap);

            ASSERT_NONZERO(ap_total_duration_ms);
            uint64_t normalized_chosen_client_bytes =
                chosen_client_bytes * hostap_duration_ms / ap_total_duration_ms;

            //int predicted_chosen_client_airtime_ms = 1000 * (double(normalized_chosen_client_bytes) / (1e+6*predicted_chosen_client_phy_rate));
            int predicted_chosen_client_bitrate =
                normalized_chosen_client_bytes / hostap_duration_ms;

            LOG_CLI(DEBUG, "load_balancer_task: "
                               << std::endl
                               << "   predicted bitrate for sta " << chosen_client << " on hostap "
                               << hostap << " is " << predicted_chosen_client_bitrate << " b/s");
            //int free_hostap_airtime_ms = hostap_duration_ms * float(100 - database.get_hostap_channel_load_percent(hostap)) / 100;

            if (predicted_chosen_client_bitrate > chosen_hostap_predicted_bitrate) {
                chosen_hostap                   = hostap;
                chosen_hostap_predicted_bitrate = predicted_chosen_client_bitrate;
                confine                         = ((!hostap_params.is_5ghz) && sta_is_5ghz);
            }

            /*
                auto clients = database.get_node_children(hostap);

                if(clients.size() < most_loaded_hostap_clients.size()){
                    chosen_hostap = hostap;
                    confine = ((!hostap_params.is_5ghz) && sta_is_5ghz);
                    break;
                }
                */

#if 0


                uint16_t predicted_chosen_client_phy_rate_100kb = database.get_load_rx_phy_rate_100kb(chosen_client) / 2;
                //FIXME
                    // son::wireless_utils::calculate_basic_phy_rate_100kb(database.get_node_max_supported_phy_rate_100kb(chosen_client),
                    //         database.get_hostap_ant_num(hostap),
                    //         database.get_hostap_ant_gain(hostap), 
                    //         database.get_hostap_tx_power(hostap), 
                    //         database.get_rssi_rx_measurement(chosen_client, hostap), 
                    //         database.get_node_rx_phy_rate_100kb(chosen_client),
                    //         sta_is_5ghz,
                    //         hostap_params.is_5ghz,
                    //         database.get_hostap_channel_bw(hostap));
                ASSERT_NONZERO(predicted_chosen_client_phy_rate_100kb);

                int hostap_duration_ms = database.get_hostap_stats_measurement_duration(hostap);
                ASSERT_NONZERO(hostap_duration_ms);
                uint64_t normalized_chosen_client_bytes = chosen_client_bytes * hostap_duration_ms / hostap_duration_ms;

                int predicted_chosen_client_airtime_ms = 1000 * (double(normalized_chosen_client_bytes) / (1e+5*predicted_chosen_client_phy_rate_100kb));

                int free_hostap_airtime_ms = hostap_duration_ms * float(100 - database.get_hostap_channel_load_percent(hostap)) / 100;

                int hostap_bytes_per_second_gained = extra_available_bytes_per_second;
                if (predicted_chosen_client_airtime_ms <= free_hostap_airtime_ms) {
                    /*
                     * new AP has enough free airtime to accommodate estimated STA traffic
                     */
                } else {

                    /*
                     * new AP has not enough free airtime to accommodate estimated STA traffic
                     * need to calculate how much throughput will be lost as a result
                     */
                    auto clients = database.get_node_children(hostap);
                    int excess_required_airtime_ms = predicted_chosen_client_airtime_ms - free_hostap_airtime_ms;
                    int total_predicted_airtime_ms = predicted_chosen_client_airtime_ms + (hostap_duration_ms - free_hostap_airtime_ms);
                    ASSERT_NONZERO(total_predicted_airtime_ms);

                    uint32_t predicted_lost_bytes = 0;

                    /*
                     * the extra airtime will be taken from each STA according to its total share
                     */
                    predicted_lost_bytes += normalized_chosen_client_bytes * float(excess_required_airtime_ms) / float(total_predicted_airtime_ms);

                    int hostap_total_client_tx_load_percent = database.get_hostap_total_client_tx_load_percent(hostap);
                    int hostap_total_client_rx_load_percent = database.get_hostap_total_client_rx_load_percent(hostap);
                    ASSERT_NONZERO(hostap_total_client_tx_load_percent);
                    ASSERT_NONZERO(hostap_total_client_rx_load_percent);

                    for (auto client : clients) {
                        float client_tx_share = (float)database.get_node_tx_load_percent(client) / hostap_total_client_tx_load_percent;
                        float client_rx_share = (float)database.get_node_rx_load_percent(client) / hostap_total_client_rx_load_percent;
                        int client_tx_bytes = database.get_node_tx_bytes(client);
                        int client_rx_bytes = database.get_node_rx_bytes(client);
                        predicted_lost_bytes += client_tx_bytes*client_tx_share + client_rx_bytes*client_rx_share;
                    }

                    int bytes_per_second_lost = predicted_lost_bytes / total_predicted_airtime_ms;
                    hostap_bytes_per_second_gained -= bytes_per_second_lost;
                }

                if (hostap_bytes_per_second_gained > chosen_hostap_bytes_per_second_gained) {
                    chosen_hostap = hostap;
                    chosen_hostap_bytes_per_second_gained = hostap_bytes_per_second_gained;
                    confine = ((!hostap_params.is_5ghz) && sta_is_5ghz);
                }
#endif
        }

        if (!chosen_hostap.empty()) {
            TASK_LOG(DEBUG) << "chosen hostap for sta " << chosen_client << " is " << chosen_hostap
                            << " providing a total gain of "
                            << chosen_hostap_bytes_per_second_gained << "Bps";

            //TASK_LOG(DEBUG) << "steering " << chosen_client << " to " << chosen_hostap;

            LOG_CLI(DEBUG, "load_balancer_task: "
                               << std::endl
                               << "    chosen hostap for sta " << chosen_client << " is "
                               << chosen_hostap << std::endl
                               << "    providing an estimated bitrate of "
                               << chosen_hostap_predicted_bitrate << "Bps" << std::endl
                               << "    --> steering " << chosen_client << " to " << chosen_hostap
                               << " confine=" << (confine ? "yes" : "no") << std::endl);

            /*
                LOG_CLI(DEBUG, "load_balancer_task: " << std::endl <<
                               "    chosen hostap for sta " << chosen_client << " is " << chosen_hostap << std::endl << 
                               "    providing a total gain of " << chosen_hostap_bytes_per_second_gained << "Bps" << std::endl <<
                               "    --> steering " << chosen_client << " to " << chosen_hostap << " confine=" << (confine?"yes":"no") << std::endl);
                               */

            database.set_node_confined_flag(chosen_client, confine);
            //TODO use steering_task_id
            //int steering_task_id = son_actions::steer_sta(database, tasks, chosen_client, chosen_hostap);
            sta_mac = chosen_client;
        } else {
            TASK_LOG(DEBUG) << "couldn't find a better hostap for sta " << chosen_client;
            LOG_CLI(DEBUG, "load_balancer_task: " << std::endl
                                                  << "   couldn't find a better hostap for sta "
                                                  << chosen_client << std::endl);
        }
        finish();
        break;
    }

    default:
        break;
    }
}

void load_balancer_task::handle_responses_timeout(
    std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
{
    for (auto entry : timed_out_macs) {
        std::string mac = entry.first;
        TASK_LOG(DEBUG) << "response from " << mac << " timed out, removing from list";
        hostaps.erase(mac);
    }
}
