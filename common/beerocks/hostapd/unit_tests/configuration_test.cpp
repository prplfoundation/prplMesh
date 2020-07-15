/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "configuration.h"
#include <fstream>

#include <gtest/gtest.h>

namespace {

const std::string configuration_path("/tmp/");
const std::string configuration_file_name("hostapd.conf");
const std::string configuration_content(
    "driver=nl80211\n"
    "logger_syslog=127\n"
    "logger_syslog_level=2\n"
    "logger_stdout=127\n"
    "logger_stdout_level=2\n"
    "sDisableMasterVap=1\n"
    "atf_config_file=/var/run/hostapd-phy0-atf.conf\n"
    "country_code=US\n"
    "ieee80211d=1\n"
    "hw_mode=g\n"
    "beacon_int=100\n"
    "channel=acs_smart\n"
    "ieee80211n=1\n"
    "ht_capab=[LDPC][SHORT-GI-20][SHORT-GI-40][TX-STBC][MAX-AMSDU-7935][DSSS_CCK-40]\n"
    "vht_capab=[RXLDPC][SHORT-GI-80][SHORT-GI-160][TX-STBC-2BY1][SU-BEAMFORMER][SU-BEAMFORMEE][MU-"
    "BEAMFORMER][VHT-TXOP-PS][VHT160][MAX-MPDU-11454][MAX-A-MPDU-LEN-EXP7][BF-ANTENNA-4][SOUNDING-"
    "DIMENSION-2]\n"
    "ieee80211ax=1\n"
    "acs_num_scans=1\n"
    "acs_smart_info_file=/var/run/acs_smart_info_wlan0.txt\n"
    "acs_history_file=/var/run/acs_history_wlan0.txt\n"
    "interface=wlan0\n"
    "ctrl_interface=/var/run/hostapd\n"
    "ap_isolate=1\n"
    "rrm_neighbor_report=1\n"
    "bss_transition=1\n"
    "interworking=1\n"
    "disassoc_low_ack=1\n"
    "preamble=1\n"
    "wmm_enabled=1\n"
    "ignore_broadcast_ssid=1\n"
    "uapsd_advertisement_enabled=1\n"
    "mbo=1\n"
    "vendor_elements=dd050009860100\n"
    "vendor_vht=1\n"
    "auth_algs=1\n"
    "wpa=0\n"
    "ssid=dummy_ssid_0\n"
    "bridge=br-lan\n"
    "bssid=02:9A:96:FB:59:0F\n"

    "bss=wlan0.0\n"
    "ctrl_interface=/var/run/hostapd\n"
    "ap_isolate=1\n"
    "ap_max_inactivity=60\n"
    "rrm_neighbor_report=1\n"
    "bss_transition=1\n"
    "interworking=1\n"
    "disassoc_low_ack=1\n"
    "preamble=1\n"
    "wmm_enabled=1\n"
    "mode=ap\n"
    "ignore_broadcast_ssid=0\n"
    "uapsd_advertisement_enabled=1\n"
    "mbo=1\n"
    "vendor_elements=dd050009860100\n"
    "vendor_vht=1\n"
    "auth_algs=1\n"
    "eap_server=1\n"
    "device_type=6-0050F204-1\n"
    "device_name=WLAN-ROUTER\n"
    "manufacturer=Intel Corporation\n"
    "config_methods=push_button\n"
    "wps_independent=1\n"
    "wps_cred_processing=1\n"
    "os_version=01020300\n"
    "manufacturer_url=http://www.intel.com\n"
    "model_description=TR069 Gateway\n"
    "wps_rf_bands=a\n"
    "bridge=br-lan\n"
    "bssid=02:9A:96:FB:59:11\n"
    "ssid=Multi-AP-24G-1\n"
    "wpa=2\n"
    "okc=0\n"
    "wpa_key_mgmt=WPA-PSK\n"
    "wpa_pairwise=CCMP\n"
    "ieee80211w=0\n"
    "wpa_passphrase=maprocks1\n"
    "disable_pmksa_caching=1\n"
    "wpa_disable_eapol_key_retries=0\n"
    "wps_state=2\n"
    "mesh_mode=fAP\n"
    "multi_ap_backhaul_ssid=\"Multi-AP-24G-2\"\n"
    "multi_ap_backhaul_wpa_passphrase=maprocks2\n"

    "bss=wlan0.1\n"
    "mode=non-ap\n"
    "ctrl_interface=/var/run/hostapd\n"
    "ap_isolate=1\n"
    "ap_max_inactivity=60\n"
    "rrm_neighbor_report=1\n"
    "bss_transition=1\n"
    "interworking=1\n"
    "disassoc_low_ack=1\n"
    "preamble=1\n"
    "wmm_enabled=1\n"
    "ignore_broadcast_ssid=0\n"
    "uapsd_advertisement_enabled=1\n"
    "mbo=1\n"
    "vendor_elements=dd050009860100\n"
    "vendor_vht=1\n"
    "auth_algs=1\n"
    "bridge=br-lan\n"
    "bssid=02:9A:96:FB:59:12\n"
    "ssid=Multi-AP-24G-2\n"
    "wpa=2\n"
    "okc=0\n"
    "wpa_key_mgmt=WPA-PSK\n"
    "wpa_pairwise=CCMP\n"
    "ieee80211w=0\n"
    "wpa_passphrase=maprocks2\n"
    "disable_pmksa_caching=1\n"
    "wpa_disable_eapol_key_retries=0\n"
    "mesh_mode=bAP\n"
    "sFourAddrMode=1\n"
    "max_num_sta=1\n"

    "#bss=wlan0.2\n"
    "#ctrl_interface=/var/run/hostapd\n"
    "#ap_isolate=1\n"
    "#ap_max_inactivity=60\n"
    "#rrm_neighbor_report=1\n"
    "#mode=ap\n"
    "#bss_transition=1\n"
    "#interworking=1\n"
    "#disassoc_low_ack=1\n"
    "#preamble=1\n"
    "#wmm_enabled=1\n"
    "#ignore_broadcast_ssid=0\n"
    "#uapsd_advertisement_enabled=1\n"
    "#mbo=1\n"
    "#vendor_elements=dd050009860100\n"
    "#vendor_vht=1\n"
    "#auth_algs=1\n"
    "#wpa=0\n"
    "#bridge=br-lan\n"
    "#bssid=02:9A:96:FB:59:13\n"
    "#start_disabled=1\n"

    "bss=wlan0.3\n"
    "ctrl_interface=/var/run/hostapd\n"
    "ap_isolate=1\n"
    "ap_max_inactivity=60\n"
    "rrm_neighbor_report=1\n"
    "bss_transition=1\n"
    "interworking=1\n"
    "disassoc_low_ack=1\n"
    "preamble=1\n"
    "wmm_enabled=1\n"
    "ignore_broadcast_ssid=0\n"
    "uapsd_advertisement_enabled=1\n"
    "mbo=1\n"
    "vendor_elements=dd050009860100\n"
    "vendor_vht=1\n"
    "auth_algs=1\n"
    "wpa=0\n"
    "bridge=br-lan\n"
    "bssid=02:9A:96:FB:59:14\n"
    "start_disabled=1\n"
    "mode=ap\n");

void clean_start()
{
    // save the content of the string (start clean)
    std::ofstream tmp(configuration_path + configuration_file_name);
    tmp << configuration_content;
    tmp.flush();
}

TEST(configuration_test, load)
{
    //// start prerequsite ////

    clean_start();

    //// end prerequsite ////

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    EXPECT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, store)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    // clean_start();

    // load the dummy configuration file
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    EXPECT_FALSE(conf) << conf;

    // construct a configuration
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    // add a value to vap
    conf.set_create_vap_value("wlan0.3", "was_i_stroed", "yes_you_were");
    EXPECT_TRUE(conf) << conf;

    // store
    conf.store();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, set_string_head_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    // replace existing key
    conf.set_create_head_value("vht_capab", "this is shorter");
    EXPECT_TRUE(conf) << conf;

    // remove exiting key
    conf.set_create_head_value("ht_capab", "");
    EXPECT_TRUE(conf) << conf;

    // add a new key/value
    conf.set_create_head_value("new_head", "{pnew->next=phead; phead=pnew;}");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, set_int_head_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    // replace existing key
    conf.set_create_head_value("bss_transition", 451);
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, set_string_vap_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    //   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    // replace existing value for existing key for existing vap
    conf.set_create_vap_value("wlan0.2", "disassoc_low_ack", "734");
    EXPECT_TRUE(conf) << conf;

    // add a key/value to exising vap
    conf.set_create_vap_value("wlan0.3", "unit_test_ok", "true");
    EXPECT_TRUE(conf) << conf;

    // remove key/value from existing vap
    conf.set_create_vap_value("wlan0.0", "vendor_elements", "");
    EXPECT_TRUE(conf) << conf;

    // set key/value for NON existing vap
    conf.set_create_vap_value("no_such", "how_do_you_do", "i_am_doing_fine");
    EXPECT_FALSE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

TEST(configuration_test, set_int_vap_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    // replace existing value for existing key for existing vap
    conf.set_create_vap_value("wlan0.1", "ignore_broadcast_ssid", 42);
    EXPECT_TRUE(conf) << conf;

    // add a key/value to exising vap
    conf.set_create_vap_value("wlan0.3", "i_am_negative", -24);
    EXPECT_TRUE(conf) << conf;

    // try to replace existing value for existing key for existing vap
    // with NON-int value. we expect the value to be trancated here
    // at the caller site
    conf.set_create_vap_value("wlan0.2", "wmm_enabled", 333.444);
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

TEST(configuration_test, get_vap_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    std::string value;

    // get existing value for existing vap
    value = conf.get_vap_value("wlan0.1", "wpa_passphrase");
    EXPECT_EQ(value, "maprocks2") << conf;

    // another check - existing value for existing vap
    value = conf.get_vap_value("wlan0.1", "bssid");
    EXPECT_EQ(value, "02:9A:96:FB:59:12");

    // get NON existing value for existing vap
    value = conf.get_vap_value("wlan0.1", "does_not_exist");
    EXPECT_EQ(value, "") << conf;

    // try to get for NON existing vap
    value = conf.get_vap_value("no_vap", "key");
    EXPECT_EQ(value, "") << conf;

    //// end test ////
}

TEST(configuration_test, disable_all_ap)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    // disable by adding a key/value
    auto disable_func = [&conf](const std::string vap) {
        conf.set_create_vap_value(vap, "start_disabled", 1);
    };

    conf.for_all_ap_vaps(disable_func);
    EXPECT_TRUE(conf) << conf;


    // disable by commenting
    auto comment_func = [&conf](const std::string vap) { conf.comment_vap(vap); };

    conf.for_all_ap_vaps(comment_func);
    EXPECT_TRUE(conf) << conf;

    // store
    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end prerequsite ////
}

TEST(configuration_test, enable_all_ap)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    // enable by removing key/value
    auto enable_func = [&conf](const std::string vap) {
        conf.set_create_vap_value(vap, "start_disabled", "");
    };

    conf.for_all_ap_vaps(enable_func);
    EXPECT_TRUE(conf) << conf;

    // enable by uncommenting
    auto uncomment_func = [&conf](const std::string vap) { conf.uncomment_vap(vap); };

    conf.for_all_ap_vaps(uncomment_func);
    EXPECT_TRUE(conf) << conf;

    // store
    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end prerequsite ////
}

TEST(configuration_test, comment_vap)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    conf.comment_vap("wlan0.1");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

TEST(configuration_test, uncomment_vap)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
   // clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    // comment twice!
    conf.comment_vap("wlan0.1");
    conf.comment_vap("wlan0.1");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    conf.uncomment_vap("wlan0.1");
    EXPECT_TRUE(conf) << conf;

    conf.uncomment_vap("wlan0.2");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    // replace existing value for existing key for existing vap
    conf.set_create_vap_value("wlan0.2", "disassoc_low_ack", "734");
    EXPECT_TRUE(conf) << conf;

    // add a key/value to exising vap
    conf.set_create_vap_value("wlan0.3", "unit_test_ok", "true");
    EXPECT_TRUE(conf) << conf;

    // remove key/value from existing vap
    conf.set_create_vap_value("wlan0.0", "vendor_elements", "");
    EXPECT_TRUE(conf) << conf;

    // set key/value for NON existing vap
    conf.set_create_vap_value("no_such", "how_do_you_do", "i_am_doing_fine");
    EXPECT_FALSE(conf) << conf;

    //// end test ////
}

TEST(configuration_test, itererate_both_containers)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    //clean_start();

    // construct a configuration
    prplmesh::hostapd::config::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load();
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    std::vector<std::string> ssid = {"00:11:22:33:44:55", "ab:cd:ef:01:02:03"};

    auto set_ssid = [&conf, &ssid](const std::string vap, std::vector<std::string>::iterator it) {
        if (it != ssid.end()) {
            // as long as we didn't finish our containr
            // we set the given's vap ssid
            conf.set_create_vap_value(vap, "ssid", *it);
        } else {
            // when we done with our container, we simply
            // comment the rest of the vaps
            conf.comment_vap(vap);
        }
    };

    conf.for_all_ap_vaps(set_ssid, ssid.begin(), ssid.end());
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

} // namespace
