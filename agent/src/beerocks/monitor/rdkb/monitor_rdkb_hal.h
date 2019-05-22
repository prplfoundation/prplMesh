/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MONITOR_RDKB_HAL_H
#define MONITOR_RDKB_HAL_H

#include "../monitor_db.h"

#include <beerocks/bcl/beerocks_message_structs.h>
#include <beerocks/bcl/network/socket.h>

#include <tlvf/CmduMessageTx.h>

namespace son {
class rdkb_hal_ap_config {
public:
    rdkb_hal_ap_config(const int8_t apIndex_) { apIndex = apIndex_; }

    void setInactCheckIntervalSec(unsigned interval) { inactCheckIntervalSec = interval; }
    void setInactCheckThresholdSec(unsigned interval) { inactCheckThresholdSec = interval; }
    void setInactCheckThresholdPackets(unsigned inactThresholdPackets_)
    {
        inactCheckThresholdPackets = inactThresholdPackets_;
    }

    unsigned getInactCheckIntervalSec() const { return inactCheckIntervalSec; }
    unsigned getInactCheckThresholdSec() const { return inactCheckThresholdSec; }
    unsigned getInactCheckThresholdPackets() const { return inactCheckThresholdPackets; }
    int8_t getApIndex() const { return apIndex; }

private:
    int8_t apIndex;
    unsigned inactCheckIntervalSec      = 0; /*  Client inactive check interval      */
    unsigned inactCheckThresholdSec     = 0; /*  Client inactive threshold           */
    unsigned inactCheckThresholdPackets = 0;
};

class rdkb_hal_sta_config {
public:
    typedef enum state_change {
        HAL_CLIENT_START_STATE,
        HAL_CLIENT_INACTIVE_STATE,
        HAL_CLIENT_ACTIVE_STATE,
    } client_state_t;

    struct client_stats_t {
        client_stats_t() : total_packets(0), sample_packets(0) {}
        uint32_t total_packets;  // total number of packets.
        uint32_t sample_packets; // number of packets in the last poll cycle.
    };

    rdkb_hal_sta_config(const std::string mac_) { mac = mac_; }

    void setSnrInactXing(int8_t threshold) { snrInactXing = threshold; }
    void setSnrHighXing(int8_t threshold) { snrHighXing = threshold; }
    void setSnrLowXing(int8_t threshold) { snrLowXing = threshold; }
    void setStartTime(std::chrono::steady_clock::time_point time) { startTime = time; }
    void setLastSampleTime(std::chrono::steady_clock::time_point time) { lastSampleTime = time; }
    void setVapIndex(int8_t vapIndex_) { vapIndex = vapIndex_; }
    void setRxPrevSnr(int8_t rx_snr_) { rx_snr = rx_snr_; }
    void setAccumulatedPackets(uint32_t tot_packets) { stats.total_packets = tot_packets; }
    void setSamplePackets(uint32_t pakets) { stats.sample_packets = pakets; }
    void setSample(int nsample) { sample = nsample; }

    int8_t getSnrInactXing() { return snrInactXing; }
    int8_t getSnrHighXing() { return snrHighXing; }
    int8_t getSnrLowXing() { return snrLowXing; }
    int8_t getVapIndex() const { return vapIndex; }
    int8_t getRxPrevSnr() { return rx_snr; }
    const std::string &getMac() { return mac; }
    int getSample() { return sample; }
    void clearData();

    void setState(client_state_t new_state);
    client_state_t getState() { return rdkb_hal_client_state; }

    std::chrono::steady_clock::time_point getStartTime() { return startTime; }
    std::chrono::steady_clock::time_point getLastSampleTime() { return lastSampleTime; }
    uint32_t getAccumulatedPackets() { return stats.total_packets; }
    uint32_t getSamplePackets() { return stats.sample_packets; }

    friend std::ostream &operator<<(std::ostream &ost, rdkb_hal_sta_config &sta);
    std::string printStats();
    std::string getStateAbbreviation();

private:
    std::string getStateString(client_state_t state);
    std::string mac;
    int8_t vapIndex;
    int8_t snrInactXing; /* Inactive SNR crossing threshold */
    int8_t snrHighXing;  /* High SNR crossing threshold     */
    int8_t snrLowXing;   /* Low SNR crossing threshold      */

    std::chrono::steady_clock::time_point startTime      = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastSampleTime = std::chrono::steady_clock::now();
    client_state_t rdkb_hal_client_state                 = HAL_CLIENT_START_STATE;
    client_stats_t stats;
    int8_t rx_snr = beerocks::SNR_INVALID;
    int sample    = -1;
};

class monitor_rdkb_hal {

public:
    typedef struct mon_rdkb_debug_info {
        std::string sta_mac;
        int8_t prev_snr;
        int8_t cur_snr;
        int8_t snrInactXing;
        int8_t snrLowXing;
        int8_t snrHighXing;
        std::string state;
        int32_t tx_packets;
        int32_t rx_packets;
        int accumulatedPackets;
        int sample;
    } mon_rdkb_debug_info_t;

    typedef enum snr_change {
        WIFI_STEERING_SNR_UNCHANGED = 0, /* SNR hasn't crossed */
        WIFI_STEERING_SNR_HIGHER    = 1, /* SNR went higher    */
        WIFI_STEERING_SNR_LOWER     = 2  /* SNR went lower     */
    } snr_change_t;

    typedef struct thresholds {
        snr_change_t high;
        snr_change_t low;
        snr_change_t inactive;
    } crossing_status_t;

    monitor_rdkb_hal(ieee1905_1::CmduMessageTx &cmdu_tx_);
    ~monitor_rdkb_hal() {}
    bool start(monitor_db *mon_db_, Socket *slave_socket_);
    void stop();

    void process();

    std::shared_ptr<rdkb_hal_sta_config> conf_add_client(const std::string &sta_mac);
    std::shared_ptr<rdkb_hal_sta_config> conf_get_client(const std::string &sta_mac);
    std::shared_ptr<rdkb_hal_ap_config> conf_add_ap(const int8_t vap_id);

    bool conf_erase_ap(const int8_t vap_id);
    bool conf_erase_client(const std::string &sta_mac);

private:
    snr_change_t get_snr_change_type(int8_t prev, int8_t cur, int8_t threshold);
    void send_activity_event(const std::string &sta_mac, bool active, int8_t vap_id);
    void send_snr_crossing_event(const std::string &sta_mac, monitor_sta_node::SStaStats &sta_stats,
                                 crossing_status_t threshs, int8_t vap_id);
    template <typename T, typename K> bool conf_erase(T &conf, K k);
    std::shared_ptr<rdkb_hal_sta_config> conf_find_client(const std::string sta_mac);
    std::shared_ptr<rdkb_hal_ap_config> conf_find_ap(const int8_t vap_id);
    void print_debug_info(mon_rdkb_debug_info_t &mdi);

    monitor_db *mon_db   = nullptr;
    Socket *slave_socket = nullptr;
    ieee1905_1::CmduMessageTx &cmdu_tx;

    void sta_erase_all();
    std::unordered_map<std::string, std::shared_ptr<son::rdkb_hal_sta_config>> conf_stas;
    std::unordered_map<int8_t, std::shared_ptr<son::rdkb_hal_ap_config>> conf_aps;
};
} //namespace son
#endif // MONITOR_RDKB_HALL_H
