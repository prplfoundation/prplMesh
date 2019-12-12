/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_AP_WLAN_HAL_H_
#define _BWL_AP_WLAN_HAL_H_

#include "base_wlan_hal.h"

namespace bwl {

/*!
 * Hardware abstraction layer for WLAN Access Point.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class ap_wlan_hal : public virtual base_wlan_hal {

    // Public definitions
public:
    /*!
     * Supported 4 address (WDS) modes
     */
    enum class WDSMode {
        Disabled = 0, /*!< 4 address mode not supported */
        Static   = 1, /*!< Single MAC */
        Dynamic  = 2, /*!< Automatic learning */
        List     = 3  /*!< Manually managed list of MACs */
    };

    enum class Event {
        Invalid = 0,

        AP_Attached,
        AP_Enabled,
        AP_Disabled,

        STA_Connected,
        STA_Disconnected,
        STA_Unassoc_RSSI,
        STA_Softblock_Drop,
        STA_Steering_Probe_Req,
        STA_Steering_Auth_Fail,

        Interface_Enabled,
        Interface_Disabled,

        ACS_Started,
        ACS_Completed,
        ACS_Failed,

        CSA_Finished,

        BSS_TM_Response,

        DFS_CAC_Completed,
        DFS_NOP_Finished,
        DFS_RADAR_Detected
    };

    // Public methods
public:
    virtual ~ap_wlan_hal() = default;

    /*!
     * Enable the radio interface
     *
     * @return true on success or false on error.
     */
    virtual bool enable() = 0;

    /*!
     * Disable the radio interface
     *
     * @return true on success or false on error.
     */
    virtual bool disable() = 0;

    /*!
     * Set start_disabled flag
     * 
     * @param [in] enable The start_disabled flag
     * @param [in] vap_id vap_id to set
     * 
     * @return true on success or false on error.
     */
    virtual bool set_start_disabled(bool enable, int vap_id = beerocks::IFACE_RADIO_ID) = 0;

    /*!
     * Set the AP channel
     * 
     * @param [in] chan The channel to switch to.
     * @param [in] bw The bandwidth (in Mhz) of the target channel.
     * @param [in] center_channel VHT center frequency.
     * 
     * @return true on success or false on error.
     */
    virtual bool set_channel(int chan, int bw, int center_channel) = 0;

    /*!
     * Allow the station with the given MAC address to connect.
     *
     * @param [in] mac The MAC address of the station.
     * @param [in] bssid The BSSID to which the operation is applicable.
     *
     * @return true on success or false on error.
     */
    virtual bool sta_allow(const std::string &mac, const std::string &bssid) = 0;

    /*!
     * Deny the station with the given MAC address from connecting to the AP.
     *
     * @param [in] mac The MAC address of the station.        
     * @param [in] bssid The BSSID to which the operation is applicable.
     * 
     * @return true on success or false on error.
     */
    virtual bool sta_deny(const std::string &mac, const std::string &bssid) = 0;

    /*!
     * Disassociate the station with the given MAC address.
     *
     * @param [in] vap_id
     * @param [in] mac The MAC address of the station.
     * @param [in] reason The reason for the disassociation
     *
     * @return true on success or false on error.
     */
    virtual bool sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason = 0) = 0;

    /*!
     * Deautanticate the station with the given MAC address.
     *
     * @param [in] vap_id
     * @param [in] mac The MAC address of the station.
     * @param [in] reason The reason for the deathentication
     *
     * @return true on success or false on error.
     */
    virtual bool sta_deauth(int8_t vap_id, const std::string &mac, uint32_t reason = 0) = 0;

    /*!
     * Send a 802.11v steer request (BSS Transition) to a connected station.
     *
     * @param [in] mac The MAC address of the station.
     * @param [in] bssid The MAC address of the target AP.
     * @param [in] chan The channel of the target AP.
     * @param [in] disassoc_timer Time in TU (time units, 1 TU = 1.024ms) before the AP should 
     *             forcefully disconnect the client. Setting a non-ZERO value
     *             should enable the "disassociation imminent" function and
     *             arm the internal AP timer (usually performed by the hardware).
     * @param [in] valid_int number of beacons for which the neighbor list is valid
     *
     * @return true on success or false on error.
     */
    virtual bool sta_bss_steer(const std::string &mac, const std::string &bssid, int chan,
                               int disassoc_timer, int valid_int) = 0;

    /*!
     * Update wifi credentials.
     *
     * @param [in] List of wifi credentials.
     *
     * @return true on success or false on error.
     */

    virtual bool
    update_vap_credentials(std::list<son::wireless_utils::sBssInfoConf> &bss_info_conf_list) = 0;

    /*!
     * TODO: Move to the base class?
     * 
     * Measure the RSSI of an unassociated station.
     * The result of the measurement should be sent as an internal event.
     * 
     * @param [in] mac The MAC address of the station.
     * @param [in] chan The channel of the target AP.
     * @param [in] bw The bandwidth (in Mhz) of the target channel.
     * @param [in] vht_center_frequency VHT center frequency.
     * @param [in] delay Delay in milliseconds before the beginning of the measurement window.
     * @param [in] window_size Measurement window size (in milliseconds).
     *
     * @return true on success or false on error.
     */
    virtual bool sta_unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                              int vht_center_frequency, int delay,
                                              int window_size) = 0;

    /*!
     * Add a station to softblock list
     * 
     * @param [in] vap_name The of the vap
     * @param [in] client_mac The MAC address of the station.
     * @param [in] reject_error_code The reject error code that will be send to the client when it try to connect if it not between the thresholds.
     * @param [in] probe_snr_threshold_hi Probe response SNR high threshold.
     * @param [in] probe_snr_threshold_lo Probe response SNR low threshold
     * @param [in] authetication_snr_threshold_hi Authetication response SNR high threshold.
     * @param [in] authetication_snr_threshold_lo Authetication response SNR low threshold.
     *
     * @return true on success or false on error.
     */
    virtual bool sta_softblock_add(const std::string &vap_name, const std::string &client_mac,
                                   uint8_t reject_error_code, uint8_t probe_snr_threshold_hi,
                                   uint8_t probe_snr_threshold_lo,
                                   uint8_t authetication_snr_threshold_hi,
                                   uint8_t authetication_snr_threshold_lo) = 0;

    // TODO: To be removed?  since hostapd doesn't support removeing STA from softblock list.
    /*!
     * Remove a station from softblock list
     * 
     * @param [in] vap_name The of the vap
     * @param [in] client_mac The MAC address of the station.
     *
     * @return true on success or false on error.
     */
    virtual bool sta_softblock_remove(const std::string &vap_name,
                                      const std::string &client_mac) = 0;
    /*!
     * Switch the AP to the given channel.
     *
     * @param [in] chan The channel to switch to.
     * @param [in] bw The bandwidth (in Mhz) of the target channel.
     * @param [in] vht_center_frequency VHT center frequency.
     *
     * @return true on success or false on error.
     */
    virtual bool switch_channel(int chan, int bw, int vht_center_frequency) = 0;

    /*!
     * Update the Radio antenna configuration.
     * 
     * @param [in] mode Radio antenna configuration.
     *
     * @return true on success or false on error.
     */
    virtual bool set_antenna_mode(AntMode mode) = 0;

    /*!
     * Set the WDS (4 address) mode for the Radio.
     * 
     * @param [in] mode WDS mode.
     *
     * @return true on success or false on error.
     */
    virtual bool wds_set_mode(WDSMode mode) = 0;

    /*!
     * Add a STA to the WDS list.
     * 
     * @param [in] mac The MAC address of the STA.
     *
     * @return true on success or false on error.
     */
    virtual bool wds_add_sta(const std::string &mac) = 0;

    /*!
     * Delete a STA from the WDS list.
     * 
     * @param [in] mac The MAC address of the STA.
     *
     * @return true on success or false on error.
     */
    virtual bool wds_del_sta(const std::string &mac) = 0;

    /*!
     * Clear the WDS list.
     * 
     * @return true on success or false on error.
     */
    virtual bool wds_clear_list() = 0;

    /*!
     * Set failsafe channel.
     * 
     * @param [in] chan Channel number.
     * @param [in] bw Channel bandwidth.
     * @param [in] vht_center_frequency VHT center frequency.
     * 
     * @return true on success or false on error.
     */
    virtual bool failsafe_channel_set(int chan, int bw, int vht_center_frequency) = 0;

    /*!
     * Get the failsafe channel.
     * 
     * @param [out] chan Channel number.
     * @param [out] bw Channel bandwidth.
     * 
     * @return true on success or false on error.
     */
    virtual bool failsafe_channel_get(int &chan, int &bw) = 0;

    // TODO: UPDATE AFTER THE MERGE WITH CHANNEL SELECTION
    virtual bool restricted_channels_set(char *channel_list) = 0;
    virtual bool restricted_channels_get(char *channel_list) = 0;
    //virtual bool restricted_channels_set(...);
    //virtual bool restricted_channels_get(...);

    /*!
     * Read the ACS (Automatic Channel Selection) report from the hardware.
     * On successful completion the infromation can be retrieved 
     * using the get_acs_report() method.
     *
     * @return true on success or false on error.
     */
    virtual bool read_acs_report() = 0;

    /*!
     * Read the supported channls from the hardware
     * On successful completion the information can be retrieved 
     *
     * @return true on success or false on error.
     */
    virtual bool read_supported_channels() = 0;

    /*!
     * Returns a string representation of the WLAN driver version.
     */
    virtual std::string get_radio_driver_version() = 0;

    /*!
     * Set/Get enable vap beacon transmittion.
     * 
     * @return true on success or false on error.
     */
    virtual bool set_vap_enable(const std::string &iface_name, const bool enable) = 0;
    virtual bool get_vap_enable(const std::string &iface_name, bool &enable)      = 0;

    /*!
     * Generate "AP-STA-CONNECTED" events on connected clients on AP
     * 
     * @return true on success or false on error.
     */
    virtual bool generate_connected_clients_events() = 0;
};

// AP HAL factory types
bwl::ap_wlan_hal *ap_wlan_hal_create(std::string iface_name, bwl::hal_conf_t hal_conf,
                                     bwl::base_wlan_hal::hal_event_cb_t cb);
void ap_wlan_hal_destroy(bwl::ap_wlan_hal *obj);

} // namespace bwl

#endif // _BWL_AP_WLAN_HAL_H_
