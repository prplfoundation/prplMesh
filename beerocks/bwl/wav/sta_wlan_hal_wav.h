/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#ifndef _BWL_STA_WLAN_HAL_WAV_H_
#define _BWL_STA_WLAN_HAL_WAV_H_

#include "base_wlan_hal_wav.h"
#include "../common/sta_wlan_hal.h"

namespace bwl {
namespace wav {

    /*!
     * Hardware abstraction layer for WLAN Station/Client.
     */
    class sta_wlan_hal_wav : public base_wlan_hal_wav, public sta_wlan_hal {

        // Public methods
        public:

            /*!
             * Constructor.
             *
             * @param [in] iface_name STA/Client interface name.
             * @param [in] callback Callback for handling internal events.
             */
            sta_wlan_hal_wav(std::string iface_name, hal_event_cb_t callback);
            virtual ~sta_wlan_hal_wav();

            virtual bool detach() override;

            virtual bool initiate_scan() override;
            virtual int  get_scan_results(const std::string& ssid, SScanResult* list, int size) override;

            virtual bool connect(const std::string& ssid, const std::string& pass, 
                    WiFiSec sec, const std::string& bssid, uint8_t channel, bool hidden_ssid) override;

            virtual bool disconnect() override;

            virtual bool roam(const std::string& bssid, uint8_t channel) override;

            virtual bool get_4addr_mode() override;
            virtual bool set_4addr_mode(bool enable) override;

            virtual bool unassoc_rssi_measurement(const std::string& mac, int chan, int bw, 
                    int vht_center_frequency, int delay, int window_size) override;

            virtual bool is_connected() override;
            virtual int  get_rssi() override;
            virtual int  get_channel() override;

            std::string  get_ssid() override;
            std::string  get_bssid() override;

        protected:

            virtual bool process_wav_event(parsed_obj_map_t& parsed_obj) override;

            // Overload for Monitor events
            bool event_queue_push(sta_wlan_hal::Event event, std::shared_ptr<void> data = {})
            { return base_wlan_hal::event_queue_push(int(event), data); }

        private:

            // Active network parameters
            std::string             m_active_ssid;
            std::string             m_active_bssid;
            std::string             m_active_pass;
            WiFiSec                 m_active_security;
            uint8_t                 m_active_channel    = 0;
            int                     m_active_network_id = -1;

            // Unassociated measurement state variables
            std::chrono::steady_clock::time_point   m_unassoc_measure_start;
            int                                     m_unassoc_measure_window_size   = 0;
            int                                     m_unassoc_measure_delay         = 0;
    };

} // namespace wav
} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_WAV_H_
