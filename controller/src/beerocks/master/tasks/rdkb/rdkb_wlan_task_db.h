/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_string_utils.h>
#include <bcl/network/network_utils.h>
#include <beerocks/tlvf/beerocks_message_common.h>
#include <memory>
#include <unordered_map>

namespace son {
class rdkb_wlan_task_db {
    struct client_config {
        std::string mac;
        unsigned int snr_probe_req_hwm;
        unsigned int snr_probe_req_lwm;
        unsigned int snr_auth_frame_hwm;
        unsigned int snr_auth_frame_lwm;
        unsigned int snr_inactivity_crossing;
        unsigned int snr_high_crossing;
        unsigned int snr_low_crossing;
        unsigned int auth_reject_reason;

        client_config(const std::string &mac_,
                      const beerocks_message::sSteeringClientConfig &config_)
        {
            mac                     = mac_;
            snr_probe_req_hwm       = config_.snrProbeHWM;
            snr_probe_req_lwm       = config_.snrProbeLWM;
            snr_auth_frame_hwm      = config_.snrAuthHWM;
            snr_auth_frame_lwm      = config_.snrAuthLWM;
            snr_inactivity_crossing = config_.snrInactXing;
            snr_high_crossing       = config_.snrHighXing;
            snr_low_crossing        = config_.snrLowXing;
            auth_reject_reason      = config_.authRejectReason;
        }

        std::shared_ptr<beerocks_message::sSteeringClientConfig> get_client_config() const
        {
            auto config              = std::make_shared<beerocks_message::sSteeringClientConfig>();
            config->snrProbeHWM      = snr_probe_req_hwm;
            config->snrProbeLWM      = snr_probe_req_lwm;
            config->snrAuthHWM       = snr_auth_frame_hwm;
            config->snrAuthLWM       = snr_auth_frame_lwm;
            config->snrInactXing     = snr_inactivity_crossing;
            config->snrHighXing      = snr_high_crossing;
            config->snrLowXing       = snr_low_crossing;
            config->authRejectReason = auth_reject_reason;
            return config;
        }
    };

    struct ap_config {
        std::string bssid;
        unsigned int util_check_interval_sec;
        unsigned int util_avg_count;
        unsigned int inactivity_check_interval_sec;
        unsigned int inactivity_check_threshold_sec;
        std::unordered_map<std::string, std::shared_ptr<client_config>> client_config_list;

        ap_config(const beerocks_message::sSteeringApConfig &config_)
        {
            bssid                   = beerocks::net::network_utils::mac_to_string(config_.bssid);
            util_check_interval_sec = config_.utilCheckIntervalSec;
            util_avg_count          = config_.utilAvgCount;
            inactivity_check_interval_sec  = config_.inactCheckIntervalSec;
            inactivity_check_threshold_sec = config_.inactCheckThresholdSec;
        }

        beerocks_message::sSteeringApConfig get_ap_config() const
        {
            beerocks_message::sSteeringApConfig config;
            config.bssid                  = beerocks::net::network_utils::mac_from_string(bssid);
            config.utilCheckIntervalSec   = util_check_interval_sec;
            config.utilAvgCount           = util_avg_count;
            config.inactCheckIntervalSec  = inactivity_check_interval_sec;
            config.inactCheckThresholdSec = inactivity_check_threshold_sec;
            return config;
        }

        ap_config operator=(const beerocks_message::sSteeringApConfig &config_)
        {
            this->bssid = beerocks::net::network_utils::mac_to_string(config_.bssid);
            this->util_check_interval_sec        = config_.utilCheckIntervalSec;
            this->util_avg_count                 = config_.utilAvgCount;
            this->inactivity_check_interval_sec  = config_.inactCheckIntervalSec;
            this->inactivity_check_threshold_sec = config_.inactCheckThresholdSec;
            return *this;
        }

        const std::unordered_map<std::string, std::shared_ptr<client_config>> &
        get_client_config_list() const
        {
            return client_config_list;
        }
    };

    class steering_group_config {
        const int index;
        ap_config config_2ghz;
        ap_config config_5ghz;

    public:
        steering_group_config(int index_, const beerocks_message::sSteeringApConfig &config_2ghz_,
                              const beerocks_message::sSteeringApConfig &config_5ghz_)
            : index(index_), config_2ghz(config_2ghz_), config_5ghz(config_5ghz_)
        {
        }

        std::shared_ptr<beerocks_message::sSteeringApConfig>
        get_ap_config(const std::string &bssid);
        bool set_client_config(const std::string &mac, const std::string &bssid,
                               const beerocks_message::sSteeringClientConfig &config);
        bool clear_client_config(const std::string &mac, const std::string &bssid);
        bool update_group_config(const beerocks_message::sSteeringApConfig &config_2ghz,
                                 const beerocks_message::sSteeringApConfig &config_5ghz);
        const ap_config &get_config_2ghz() { return config_2ghz; }
        const ap_config &get_config_5ghz() { return config_5ghz; }
    };

    std::unordered_map<int, std::shared_ptr<steering_group_config>> steering_group_list;

public:
    bool set_client_config(const std::string &mac, const std::string &bssid,
                           int steering_group_index,
                           const beerocks_message::sSteeringClientConfig &config);

    bool clear_client_config(const std::string &mac, const std::string &bssid,
                             int steering_group_index);

    bool set_steering_group_config(int index,
                                   const beerocks_message::sSteeringApConfig &config_2ghz,
                                   const beerocks_message::sSteeringApConfig &config_5ghz);

    bool clear_steering_group_config(int index);

    std::pair<bool, beerocks_message::sSteeringApConfig> get_ap_config(const std::string &bssid);

    std::unordered_map<std::string, std::shared_ptr<beerocks_message::sSteeringClientConfig>>
    get_client_config_list(const std::string &bssid);

    const std::unordered_map<int, std::shared_ptr<steering_group_config>> &get_steering_group_list()
    {
        return steering_group_list;
    }

    int32_t get_group_index(const std::string &client_mac, std::string &bssid);

    void print_db();
};
} // namespace son
