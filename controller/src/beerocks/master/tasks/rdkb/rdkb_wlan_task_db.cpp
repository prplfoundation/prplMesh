/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "rdkb_wlan_task_db.h"
#include <algorithm>
#include <easylogging++.h>

using namespace beerocks;
using namespace son;

bool rdkb_wlan_task_db::steering_group_config::set_client_config(
    const std::string &mac, const std::string &bssid,
    const beerocks_message::sSteeringClientConfig &config)
{
    auto new_entry = std::make_shared<client_config>(mac, config);
    if (config_2ghz.bssid == bssid) {
        config_2ghz.client_config_list[mac] = new_entry;
    } else if (config_5ghz.bssid == bssid) {
        config_5ghz.client_config_list[mac] = new_entry;
    } else {
        LOG(ERROR) << "no bssid=" << bssid << " for steering_group_index=" << index;
        return false;
    }
    return true;
}

bool rdkb_wlan_task_db::steering_group_config::clear_client_config(const std::string &mac,
                                                                   const std::string &bssid)
{
    if (config_2ghz.bssid == bssid) {
        config_2ghz.client_config_list.erase(mac);
    } else if (config_5ghz.bssid == bssid) {
        config_5ghz.client_config_list.erase(mac);
    } else {
        LOG(ERROR) << "no bssid=" << bssid << " for steering_group_index=" << index;
        return false;
    }
    return true;
}

bool rdkb_wlan_task_db::steering_group_config::update_group_config(
    const beerocks_message::sSteeringApConfig &config_2ghz,
    const beerocks_message::sSteeringApConfig &config_5ghz)
{
    auto bssid_2ghz = beerocks::net::network_utils::mac_to_string(config_2ghz.bssid);
    auto bssid_5ghz = beerocks::net::network_utils::mac_to_string(config_5ghz.bssid);
    if ((bssid_2ghz != this->config_2ghz.bssid) || (bssid_5ghz != this->config_5ghz.bssid)) {
        LOG(ERROR) << "Can't change bssid in an existing steeringGroup";
        return false;
    }

    this->config_2ghz = config_2ghz;
    this->config_5ghz = config_5ghz;
    return true;
}

bool rdkb_wlan_task_db::set_client_config(const std::string &mac, const std::string &bssid,
                                          int steering_group_index,
                                          const beerocks_message::sSteeringClientConfig &config)
{
    auto it = steering_group_list.find(steering_group_index);
    if (it == steering_group_list.end()) {
        LOG(ERROR) << "can't find steering group " << steering_group_index;
        return false;
    }

    auto steering_group = it->second;
    return steering_group->set_client_config(mac, bssid, config);
}

bool rdkb_wlan_task_db::clear_client_config(const std::string &mac, const std::string &bssid,
                                            int steering_group_index)
{
    auto it = steering_group_list.find(steering_group_index);
    if (it == steering_group_list.end()) {
        LOG(ERROR) << "can't find steering group " << steering_group_index;
        return false;
    }

    auto steering_group = it->second;
    return steering_group->clear_client_config(mac, bssid);
}

bool rdkb_wlan_task_db::set_steering_group_config(
    int index, const beerocks_message::sSteeringApConfig &config_2ghz,
    const beerocks_message::sSteeringApConfig &config_5ghz)
{
    auto it = steering_group_list.find(index);
    if (it == steering_group_list.end()) {
        auto new_entry = std::make_shared<steering_group_config>(index, config_2ghz, config_5ghz);
        steering_group_list[index] = new_entry;
    } else {
        //otherwise update the parameters but don't overwrite the client config list
        auto steering_group = it->second;
        steering_group->update_group_config(config_2ghz, config_5ghz);
    }
    return true;
}

bool rdkb_wlan_task_db::clear_steering_group_config(int index)
{
    auto it = steering_group_list.find(index);
    if (it == steering_group_list.end()) {
        LOG(ERROR) << "can't find steering group index=" << index;
        return false;
    } else {
        steering_group_list.erase(it);
        return true;
    }
}

std::pair<bool, beerocks_message::sSteeringApConfig>
rdkb_wlan_task_db::get_ap_config(const std::string &bssid)
{
    for (auto group_entry : steering_group_list) {
        auto steering_group = group_entry.second;
        if (steering_group->get_config_2ghz().bssid == bssid) {
            return std::make_pair(true, steering_group->get_config_2ghz().get_ap_config());
        } else if (steering_group->get_config_5ghz().bssid == bssid) {
            return std::make_pair(true, steering_group->get_config_5ghz().get_ap_config());
        }
    }
    LOG(ERROR) << "can't find entry for bssid=" << bssid;
    return std::make_pair(false, {});
}

std::unordered_map<std::string, std::shared_ptr<beerocks_message::sSteeringClientConfig>>
rdkb_wlan_task_db::get_client_config_list(const std::string &bssid)
{
    std::unordered_map<std::string, std::shared_ptr<beerocks_message::sSteeringClientConfig>>
        result;
    for (auto group_entry : steering_group_list) {
        auto steering_group = group_entry.second;
        if (steering_group->get_config_2ghz().bssid == bssid) {
            auto client_config_list = steering_group->get_config_2ghz().get_client_config_list();
            for (auto client_config_entry : client_config_list) {
                result[client_config_entry.first] = client_config_entry.second->get_client_config();
            }
            break;
        } else if (steering_group->get_config_5ghz().bssid == bssid) {
            auto client_config_list = steering_group->get_config_5ghz().get_client_config_list();
            for (auto client_config_entry : client_config_list) {
                result[client_config_entry.first] = client_config_entry.second->get_client_config();
            }
            break;
        }
    }
    return result;
}

int32_t rdkb_wlan_task_db::get_group_index(const std::string &client_mac, std::string &bssid)
{
    //notification common data: group index
    for (auto group_entry : steering_group_list) {
        auto steering_group         = group_entry.second;
        auto client_config_map_2ghz = steering_group->get_config_2ghz().get_client_config_list();
        auto it_2ghz_clients        = client_config_map_2ghz.find(client_mac);
        auto client_config_map_5ghz = steering_group->get_config_5ghz().get_client_config_list();
        auto it_5ghz_clients        = client_config_map_5ghz.find(client_mac);
        if (it_2ghz_clients != client_config_map_2ghz.end() &&
            steering_group->get_config_2ghz().bssid == bssid) {
            return group_entry.first;
        } else if (it_5ghz_clients != client_config_map_5ghz.end() &&
                   steering_group->get_config_5ghz().bssid == bssid) {
            return group_entry.first;
        }
    }
    return -1;
}

void rdkb_wlan_task_db::print_db()
{
    //notification common data: group index
    std::for_each(
        steering_group_list.begin(), steering_group_list.end(),
        [&](std::pair<int, std::shared_ptr<steering_group_config>> const &group_entry) {
            LOG(DEBUG) << "***********************RDKB_DATABASE***********************";
            LOG(DEBUG) << "group_index = " << group_entry.first;
            LOG(DEBUG) << "*****2.4GHz ap config***** ";
            LOG(DEBUG) << "     bssid " << group_entry.second->get_config_2ghz().bssid;
            LOG(DEBUG)
                << "     utilCheckIntervalSec "
                << int(group_entry.second->get_config_2ghz().get_ap_config().utilCheckIntervalSec);
            LOG(DEBUG) << "     utilAvgCount "
                       << int(group_entry.second->get_config_2ghz().get_ap_config().utilAvgCount);
            LOG(DEBUG)
                << "     inactCheckIntervalSec "
                << group_entry.second->get_config_2ghz().get_ap_config().inactCheckIntervalSec;
            LOG(DEBUG)
                << "     inactCheckThresholdSec "
                << group_entry.second->get_config_2ghz().get_ap_config().inactCheckThresholdSec;
            LOG(DEBUG) << "*********client_list********* ";
            auto client_list_2ghz = group_entry.second->get_config_2ghz().client_config_list;
            std::for_each(
                client_list_2ghz.begin(), client_list_2ghz.end(),
                [&](std::pair<std::string, std::shared_ptr<client_config>> const &client_entry) {
                    LOG(DEBUG) << "             client_mac " << client_entry.first;
                    LOG(DEBUG) << "                 snrProbeHWM "
                               << int(client_entry.second->get_client_config()->snrProbeHWM);
                    LOG(DEBUG) << "                 snrProbeLWM "
                               << int(client_entry.second->get_client_config()->snrProbeLWM);
                    LOG(DEBUG) << "                 snrAuthHWM "
                               << int(client_entry.second->get_client_config()->snrAuthHWM);
                    LOG(DEBUG) << "                 snrAuthLWM "
                               << int(client_entry.second->get_client_config()->snrAuthLWM);
                    LOG(DEBUG) << "                 snrInactXing "
                               << int(client_entry.second->get_client_config()->snrInactXing);
                    LOG(DEBUG) << "                 snrHighXing "
                               << int(client_entry.second->get_client_config()->snrHighXing);
                    LOG(DEBUG) << "                 snrLowXing "
                               << int(client_entry.second->get_client_config()->snrLowXing);
                    LOG(DEBUG) << "                 authRejectReason "
                               << int(client_entry.second->get_client_config()->authRejectReason);
                });
            LOG(DEBUG) << "*****5GHz ap config***** ";
            LOG(DEBUG) << "     bssid " << group_entry.second->get_config_5ghz().bssid;
            LOG(DEBUG)
                << "     utilCheckIntervalSec "
                << group_entry.second->get_config_5ghz().get_ap_config().utilCheckIntervalSec;
            LOG(DEBUG) << "     utilAvgCount "
                       << group_entry.second->get_config_5ghz().get_ap_config().utilAvgCount;
            LOG(DEBUG)
                << "     inactCheckIntervalSec "
                << group_entry.second->get_config_5ghz().get_ap_config().inactCheckIntervalSec;
            LOG(DEBUG)
                << "     inactCheckThresholdSec "
                << group_entry.second->get_config_5ghz().get_ap_config().inactCheckThresholdSec;
            LOG(DEBUG) << "*********client_list********* ";
            auto client_list_5ghz = group_entry.second->get_config_5ghz().client_config_list;
            std::for_each(
                client_list_5ghz.begin(), client_list_5ghz.end(),
                [&](std::pair<std::string, std::shared_ptr<client_config>> const &client_entry) {
                    LOG(DEBUG) << "             client_mac " << client_entry.first;
                    LOG(DEBUG) << "                 snrProbeHWM "
                               << int(client_entry.second->get_client_config()->snrProbeHWM);
                    LOG(DEBUG) << "                 snrProbeLWM "
                               << int(client_entry.second->get_client_config()->snrProbeLWM);
                    LOG(DEBUG) << "                 snrAuthHWM "
                               << int(client_entry.second->get_client_config()->snrAuthHWM);
                    LOG(DEBUG) << "                 snrAuthLWM "
                               << int(client_entry.second->get_client_config()->snrAuthLWM);
                    LOG(DEBUG) << "                 snrInactXing "
                               << int(client_entry.second->get_client_config()->snrInactXing);
                    LOG(DEBUG) << "                 snrHighXing "
                               << int(client_entry.second->get_client_config()->snrHighXing);
                    LOG(DEBUG) << "                 snrLowXing "
                               << int(client_entry.second->get_client_config()->snrLowXing);
                    LOG(DEBUG) << "                 authRejectReason "
                               << int(client_entry.second->get_client_config()->authRejectReason);
                });
        });
}
