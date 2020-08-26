/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <fstream>
#include <hostapd/configuration.h>

#include <gtest/gtest.h>

namespace {

const std::string vap_indication_1("bss=");
const std::string vap_indication_2("interface=");
const std::string configuration_path("/tmp/");
const std::string configuration_file_name("omnia.conf");
const std::string configuration_content(
    "driver=nl80211\n"
    "logger_syslog=127\n"
    "logger_syslog_level=1\n"
    "logger_stdout=127\n"
    "logger_stdout_level=7\n"
    "hw_mode=a\n"
    "beacon_int=100\n"
    "channel=44\n"
    "tx_queue_data2_burst=2.0\n"
    "ieee80211n=1\n"
    "ht_coex=0\n"
    "ht_capab=[HT40+][LDPC][SHORT-GI-20][SHORT-GI-40][TX-STBC][RX-STBC1][MAX-AMSDU-7935][DSSS_CCK-40]\n"
    "vht_oper_chwidth=1\n"
    "vht_oper_centr_freq_seg0_idx=42\n"
    "ieee80211ac=1\n"
    "vht_capab=[RXLDPC][SHORT-GI-80][TX-STBC-2BY1][RX-ANTENNA-PATTERN][TX-ANTENNA-PATTERN][RX-STBC-1][MAX-MPDU-11454][MAX-A-MPDU-LEN-EXP7]\n"

    "interface=wlan0\n"
    "ctrl_interface=/var/run/hostapd\n"
    "ap_isolate=1\n"
    "bss_load_update_period=60\n"
    "chan_util_avg_period=600\n"
    "disassoc_low_ack=1\n"
    "preamble=1\n"
    "wmm_enabled=1\n"
    "ignore_broadcast_ssid=0\n"
    "uapsd_advertisement_enabled=1\n"
    "utf8_ssid=1\n"
    "multi_ap=0\n"
    "wpa_passphrase=prplBEE$\n"
    "wpa_psk_file=/var/run/hostapd-wlan0.psk\n"
    "auth_algs=1\n"
    "wpa=2\n"
    "wpa_pairwise=CCMP\n"
    "ssid=prplmesh-front\n"
    "bridge=br-lan\n"
    "wpa_disable_eapol_key_retries=0\n"
    "wpa_key_mgmt=WPA-PSK\n"
    "okc=0\n"
    "disable_pmksa_caching=1\n"
    "dynamic_vlan=0\n"
    "vlan_naming=1\n"
    "vlan_file=/var/run/hostapd-wlan0.vlan\n"
    "bssid=04:f0:21:24:24:17\n"

    "bss=wlan0-1\n"
    "ctrl_interface=/var/run/hostapd\n"
    "ap_isolate=1\n"
    "bss_load_update_period=60\n"
    "chan_util_avg_period=600\n"
    "disassoc_low_ack=1\n"
    "preamble=1\n"
    "wmm_enabled=1\n"
    "ignore_broadcast_ssid=0\n"
    "uapsd_advertisement_enabled=1\n"
    "utf8_ssid=1\n"
    "multi_ap=0\n"
    "wpa_passphrase=prplBEE$\n"
    "wpa_psk_file=/var/run/hostapd-wlan0-1.psk\n"
    "auth_algs=1\n"
    "wpa=2\n"
    "wpa_pairwise=CCMP\n"
    "ssid=prplmesh-back\n"
    "bridge=br-lan\n"
    "wpa_disable_eapol_key_retries=0\n"
    "wpa_key_mgmt=WPA-PSK\n"
    "okc=0\n"
    "disable_pmksa_caching=1\n"
    "dynamic_vlan=0\n"
    "vlan_naming=1\n"
    "vlan_file=/var/run/hostapd-wlan0-1.vlan\n"
    "wds_sta=1\n"
    "bssid=06:f0:21:24:24:17\n"
);

void clean_start()
{
    // save the content of the string (start clean)
    std::ofstream tmp(configuration_path + configuration_file_name);
    tmp << configuration_content;
    tmp.flush();
}

// Suppress cppcheck syntax error for gtest TEST macro
// cppcheck-suppress syntaxError
TEST(configuration_test, load)
{
    //// start prerequsite ////

    clean_start();

    //// end prerequsite ////

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    EXPECT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, store)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // load the dummy configuration file
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);

    // construct a configuration
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    // add a value to vap
    conf.set_create_vap_value("wlan0-1", "was_i_stroed", "yes_you_were");
    EXPECT_TRUE(conf) << conf;

    // store
    conf.store();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, set_string_head_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
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
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    // replace existing key
    conf.set_create_head_value("bss_transition", 451);
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, get_head_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    // get existing key
    auto val = conf.get_head_value("logger_syslog");
    EXPECT_EQ(val, "127");

    // get non existing key
    val = conf.get_head_value("out_of_office");
    EXPECT_EQ(val, "");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;
}

TEST(configuration_test, set_string_vap_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    conf.set_create_vap_value("wlan0", "ssid", "ran_home");
    EXPECT_TRUE(conf) << conf;

    // replace existing value for existing key for existing vap
    conf.set_create_vap_value("wlan0", "disassoc_low_ack", "734");
    EXPECT_TRUE(conf) << conf;

    // add a key/value to exising vap
    conf.set_create_vap_value("wlan0", "unit_test_ok", "true");
    EXPECT_TRUE(conf) << conf;

    // remove key/value from existing vap
    conf.set_create_vap_value("wlan0-1", "vendor_elements", "");
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
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    // replace existing value for existing key for existing vap
    conf.set_create_vap_value("wlan0-1", "ignore_broadcast_ssid", 42);
    EXPECT_TRUE(conf) << conf;

    // add a key/value to exising vap
    conf.set_create_vap_value("wlan0", "i_am_negative", -24);
    EXPECT_TRUE(conf) << conf;

    // try to replace existing value for existing key for existing vap
    // with NON-int value. we expect the value to be trancated here
    // at the caller site
    conf.set_create_vap_value("wlan0", "wmm_enabled", 333.444);
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

TEST(configuration_test, get_vap_values)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    std::string value;

    // get existing value for existing vap
    value = conf.get_vap_value("wlan0", "wpa_passphrase");
    EXPECT_EQ(value, "prplBEE$") << conf;

    // another check - existing value for existing vap
    value = conf.get_vap_value("wlan0", "bssid");
    EXPECT_EQ(value, "04:f0:21:24:24:17");

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
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    // identify vap by  mode=ap
    /*
    auto mode_predicate = [&conf](const std::string &vap) {
        return conf.get_vap_value(vap, "mode") == "ap";
    };
    */

    // all vaps are ap vaps
    auto all_predicate = [&conf](const std::string &vap) { return true; };

    // disable by adding a key/value
    auto disable_func = [&conf](const std::string vap) {
        conf.set_create_vap_value(vap, "start_disabled", 1);
    };

    conf.for_all_ap_vaps(disable_func, all_predicate);
    EXPECT_TRUE(conf) << conf;

    // disable by commenting
    auto comment_func = [&conf](const std::string vap) { conf.comment_vap(vap); };

    conf.for_all_ap_vaps(comment_func, all_predicate);
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
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    // enable by removing key/value
    auto enable_func = [&conf](const std::string vap) {
        conf.set_create_vap_value(vap, "start_disabled", "");
    };

    auto ap_predicate = [&conf](const std::string &vap) {
        return conf.get_vap_value(vap, "mode") == "ap";
    };

    conf.for_all_ap_vaps(enable_func, ap_predicate);
    EXPECT_TRUE(conf) << conf;

    // enable by uncommenting
    auto uncomment_func = [&conf](const std::string &vap) { conf.uncomment_vap(vap); };

    conf.for_all_ap_vaps(uncomment_func, ap_predicate);
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
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    conf.comment_vap("wlan0");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

TEST(configuration_test, uncomment_vap)
{
    //// start prerequsite ////

    // save the content of the string (start clean)
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    // comment twice!
    conf.comment_vap("wlan0");
    conf.comment_vap("wlan0");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);
    ;
    ASSERT_TRUE(conf) << conf;

    //// end prerequsite ////

    //// start test ////

    conf.uncomment_vap("wlan0");
    EXPECT_TRUE(conf) << conf;

    conf.uncomment_vap("wlan0");
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    // replace existing value for existing key for existing vap
    conf.set_create_vap_value("wlan0", "disassoc_low_ack", "734");
    EXPECT_TRUE(conf) << conf;

    // add a key/value to exising vap
    conf.set_create_vap_value("wlan0", "unit_test_ok", "true");
    EXPECT_TRUE(conf) << conf;

    // remove key/value from existing vap
    conf.set_create_vap_value("wlan0-1", "preamble", "");
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
    clean_start();

    // construct a configuration
    prplmesh::hostapd::Configuration conf(configuration_path + configuration_file_name);
    ASSERT_FALSE(conf) << conf;

    // load the dummy configuration file
    conf.load(vap_indication_1,vap_indication_2);

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

    auto ap_predicate = [&conf](const std::string &vap) {
        return conf.get_vap_value(vap, "mode") == "ap";
    };

    conf.for_all_ap_vaps(set_ssid, ssid.begin(), ssid.end(), ap_predicate);
    EXPECT_TRUE(conf) << conf;

    conf.store();
    EXPECT_TRUE(conf) << conf;

    //// end test ////
}

} // namespace
