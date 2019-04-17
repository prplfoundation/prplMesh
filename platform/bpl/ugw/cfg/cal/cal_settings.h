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

#ifndef __CAL_SETTINGS_H__
#define __CAL_SETTINGS_H__

#include <map>
#include <memory>
#include <string>

#include <cal_interface.h>

namespace beerocks {
namespace bpl {

class cal_settings
{
  public:

    struct SDeviceInfo {
      std::string manufacturer;
      std::string model_name;
      std::string serial_number;
      std::string lan_interface_name;
      std::string lan_interface_ip;
      std::string lan_interface_netmask;
      std::string wan_interface_name;
      std::string wan_interface_ip;
      std::string wan_interface_netmask;
    };

    struct SAPSettings {
      bool enabled;
      bool acs;
      bool advertise_ssid;
      std::string ssid;
      std::string pass;
      std::string sec_mode;
    };

  public:
    cal_settings(cal_interface* cal = nullptr);
    virtual ~cal_settings();

    int beerocks_is_enabled();
    int beerocks_is_master();
    int beerocks_get_operating_mode();
    int beerocks_is_onboarding();
    int beerocks_set_onboarding(int enable);
    int beerocks_get_band_steering();
    int beerocks_get_client_roaming();
    int beerocks_get_device_info(SDeviceInfo& device_info);
    int beerocks_get_wifi_params(std::string iface_name, SAPSettings &ap_settings);
    int beerocks_get_backhaul_params(int& max_vaps, int& network_enabled, int& prefered_radio_band);
    int beerocks_get_backhaul_vaps(char* backhaul_vaps_buf, const int buf_len);
    int beerocks_get_wifi_credentials(const int radio_dir, std::string& ssid, std::string& pass, std::string& sec);
    int beerocks_set_beerocks_credentials(const int radio_dir, std::string ssid, std::string pass, std::string sec);
    int beerocks_set_wifi_credentials(std::string iface, std::string ssid, std::string pass, std::string sec);
    int beerocks_get_administrator_credentials(std::string &pass);
    int beerocks_is_wired_backhaul();

  private:

    int beerocks_read_bool_value(std::string param);

  private:
    std::unique_ptr<cal_interface> m_cal_owner;
    cal_interface* m_cal;
};

} // namespace bpl
} // namespace beerocks

#endif  // __CAL_SETTINGS_H__
