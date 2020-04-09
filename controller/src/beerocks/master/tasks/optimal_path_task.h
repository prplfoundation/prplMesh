/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _OPTIMAL_PATH_TASK_H_
#define _OPTIMAL_PATH_TASK_H_

#include "../db/db.h"
#include "task.h"
#include "task_pool.h"

#define MEAS_MAX_DELAY_ALLOWED 10

namespace son {

class optimal_path_task : public task {
public:
    optimal_path_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_,
                      std::string sta_mac_, int starting_delay_ms_ = 0,
                      std::string task_name_ = std::string("optimal_path"));
    virtual ~optimal_path_task() {}

    /////////////// FOR DEBUG ONLY ////////////////
    static int cli_beacon_request_duration;
    static int cli_beacon_request_rand_ival;
    static beerocks::eMeasurementMode11K cli_beacon_request_mode;
    ///////////////////////////////////////////////

protected:
    virtual void work() override;
    virtual void
    handle_response(std::string slave_mac,
                    std::shared_ptr<beerocks::beerocks_header> beerocks_header) override;
    virtual void handle_responses_timeout(
        std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
        override;

private:
    bool check_if_sta_can_steer_to_ap(std::string ap);
    void send_rssi_measurement_request(const std::string &agent_mac, std::string client_mac,
                                       int channel, std::string hostap, int id);
    bool assert_original_parent();
    bool calculate_measurement_delay(std::set<std::string> temp_cross_hostaps, std::string sta_ap,
                                     std::string sta_mac);
    bool ready_to_pick_optimal_path(std::set<std::string> temp_cross_hostaps, std::string sta_ap,
                                    std::string sta_mac);
    bool is_measurement_valid(std::set<std::string> temp_cross_hostaps, std::string sta_ap,
                              std::string sta_mac);
    bool all_measurement_succeed(std::set<std::string> temp_cross_hostaps, std::string sta_ap,
                                 std::string sta_mac);
    bool is_delay_match_window(std::set<std::string> temp_cross_hostaps, std::string sta_ap);
    void change_measurement_window_size(std::string sta_ap, bool inc);
    bool get_station_default_capabilities(bool is_bandtype_5ghz,
                                          beerocks::message::sRadioCapabilities &default_sta_cap);

    double calculate_weighted_phy_rate(std::string client_mac, std::string hostap_mac);
    double calculate_weighted_phy_rate(std::string hostap_mac, int &hops);
    bool is_hostap_on_cs_process(std::string hostap_mac);

    db &database;
    ieee1905_1::CmduMessageTx &cmdu_tx;
    task_pool &tasks;
    std::string sta_mac;
    int starting_delay_ms   = 0;
    bool sta_support_11k    = false;
    bool started_as_client  = false;
    int sticky_roaming_rssi = 0;
    std::string current_hostap_vap;
    std::string current_hostap;
    std::string current_hostap_ssid;
    std::string chosen_hostap;

    enum states {
        //common states:
        START = 0,

        //11k states:
        FILL_POTENTIAL_AP_LIST_11K,
        REQUEST_11K_MEASUREMENTS_BY_BSSID,
        FIND_AND_PICK_HOSTAP_11K,

        // cross states:
        FILL_POTENTIAL_AP_LIST_CROSS,
        REQUEST_CROSS_RSSI_MEASUREMENTS,
        FIND_AND_PICK_HOSTAP_CROSS,
        SEND_STEER_ACTION,
        WAIT_FOR_HANDOVER,
    };
    int state = START;

    //11K
    std::unordered_map<std::string, bool>
        potential_11k_aps; // key = ap_mac, value = is_valid_measurement
    std::unordered_map<std::string, bool>::iterator potential_ap_iter =
        std::unordered_map<std::string, bool>::iterator();
    std::string current_agent_mac;
    uint8_t valid_beacon_measurement_report_count  = 0;
    uint8_t beacon_measurement_request_cycle_count = 0;
    uint8_t iterator_element_counter               = 0;

    /////////////// FOR DEBUG ONLY ////////////////
    bool use_cli_value = false;
    ///////////////////////////////////////////////

    beerocks_message::sBeaconRequest11k measurement_request = {};

    //CROSS
    std::set<std::string> hostaps;
    int calculate_measurement_delay_count = 0;
};

} // namespace son

#endif
