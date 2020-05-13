#! /usr/bin/env python3
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import argparse
import os
import sys
import time
from typing import Callable, Union

import connmap
import environment as env
from capi import tlv
from opts import debug, err, message, opts, status
import sniffer


class TestFlows:
    def __init__(self):
        self.tests = [attr[len('test_'):] for attr in dir(self) if attr.startswith('test_')]
        self.running = ''

    def __fail_no_message(self) -> bool:
        '''Increment failure count and return False.'''
        self.check_error += 1
        if opts.stop_on_failure:
            sys.exit(1)
        return False

    def fail(self, msg: str) -> bool:
        '''Print a red error message, increment failure count and return False.'''
        err('FAIL: {}'.format(msg))
        return self.__fail_no_message()

    def start_test(self, test: str):
        '''Call this at the beginning of a test.'''
        self.running = test
        status(test + " starting")

    def check_log(self, entity_or_radio: Union[env.ALEntity, env.Radio], regex: str,
                  start_line: int = 0) -> bool:
        '''Verify that the logfile for "entity_or_radio" matches "regex", fail if not.'''
        return self.wait_for_log(entity_or_radio, regex, start_line, 0.3)

    def wait_for_log(self, entity_or_radio: Union[env.ALEntity, env.Radio], regex: str,
                     start_line: int, timeout: float) -> bool:
        result, line, match = entity_or_radio.wait_for_log(regex, start_line, timeout)
        if not result:
            self.__fail_no_message()
        return result, line, match

    def check_cmdu(
        self, msg: str, match_function: Callable[[sniffer.Packet], bool]
    ) -> [sniffer.Packet]:
        """Verify that the wired_sniffer has captured a CMDU that satisfies match_function.

        Mark failure if no satisfying packet is found.

        Parameters
        ----------
        msg: str
            Message to show in case of failure. It is formatted in a context like
            "No CMDU <msg> found".

        match_function: Callable[[sniffer.Packet], bool]
            A function that returns True if it is the expected packet. It is called on every
            packet returned by Sniffer.get_packet_capture until it returns True.

        Returns
        -------
        [sniffer.Packet]
            The matching packets.
        """
        debug("Checking for CMDU {}".format(msg))
        capture = env.wired_sniffer.get_packet_capture()
        result = [packet for packet in capture if packet.ieee1905 and match_function(packet)]
        if not result:
            self.fail("No CMDU {} found".format(msg))
        return result

    def check_cmdu_type(
        self, msg: str, msg_type: int, eth_src: str, eth_dst: str = None, mid: int = None
    ) -> [sniffer.Packet]:
        """Verify that the wired sniffer has captured a CMDU.

        Mark failure if the CMDU is not found.

        Parameters
        ----------
        msg: str
            Message to show in case of failure. It is formatted in a context like
            "No CMDU <msg> found".

        msg_type: int
            CMDU message type that is expected.

        eth_src: str
            MAC address of the sender that is expected.

        eth_dst: str
            MAC address of the destination that is expected. If omitted, the IEEE1905.1 multicast
            MAC address is used.

        mid: int
            Message Identifier that is expected. If omitted, the MID is not checked.

        Returns
        -------
        [sniffer.Packet]
            The matching packets.
        """
        if eth_dst is None:
            eth_dst = "01:80:c2:00:00:13"
        return self.check_cmdu(msg, lambda packet:
                               packet.eth_src == eth_src and
                               packet.eth_dst == eth_dst and
                               packet.ieee1905_message_type == msg_type and
                               (mid is None or packet.ieee1905_mid == mid))

    def check_cmdu_type_single(
        self, msg: str, msg_type: int, eth_src: str, eth_dst: str = None, mid: int = None
    ) -> Union[sniffer.Packet, None]:
        '''Like check_cmdu_type, but also check that only a single CMDU is found.'''
        cmdus = self.check_cmdu_type(msg, msg_type, eth_src, eth_dst, mid)
        if not cmdus:
            return None  # Failure already reported by check_cmdu
        if len(cmdus) > 1:
            self.fail("Multiple CMDUs {} found".format(msg))
            return None
        return cmdus[0]

    def check_cmdu_has_tlvs(
        self, packet: Union[sniffer.Packet, None], tlv_type: int
    ) -> [sniffer.Tlv]:
        '''Check that the packet has at least one TLV of the given type.

        Mark failure if no TLV of that type is found.

        Parameters
        ----------
        packet: Union[sniffer.Packet, None]
            The packet to verify. It may also be None, to make it easy to let it follow
            check_cmdu_type_single. If it is None, no failure is raised. If it is not an IEEE1905
            packet, a failure *is* raised.

        tlv_type: int
            The type of TLV to look for.

        Returns
        -------
        [sniffer.Tlv]
            List of TLVs of the requested type. Empty list if the check fails.
        '''
        if not packet:
            return []  # No additional failure, assumed to already have been raised
        if not packet.ieee1905:
            self.fail("Packet is not IEEE1905: {}".format(packet))
            return []
        tlvs = [tlv for tlv in packet.ieee1905_tlvs if tlv.tlv_type == tlv_type]
        if not tlvs:
            self.fail("No TLV of type 0x{:02x} found in packet".format(tlv_type))
            debug("  {}".format(packet))
        return tlvs

    def check_cmdu_has_tlv_single(
        self, packet: Union[sniffer.Packet, None], tlv_type: int
    ) -> Union[sniffer.Tlv, None]:
        '''Like check_cmdu_has_tlvs, but also check that only one TLV of that type is found.'''
        tlvs = self.check_cmdu_has_tlvs(packet, tlv_type)
        if not tlvs:
            return None
        if len(tlvs) > 1:
            self.fail("More than one ({}) TLVs of type 0x{:02x} found".format(len(tlvs), tlv_type))
            debug("  {}".format(packet))
            return None
        return tlvs[0]

    def run_tests(self, tests):
        '''Run all tests as specified on the command line.'''
        total_errors = 0
        if not tests:
            tests = self.tests
        for test in tests:
            test_full = 'test_' + test
            self.start_test(test)
            env.wired_sniffer.start(test_full)
            self.check_error = 0
            try:
                getattr(self, test_full)()
            finally:
                env.wired_sniffer.stop()
            if self.check_error != 0:
                err(test + " failed")
            else:
                message(test + " OK", 32)
            total_errors += self.check_error
        return total_errors

    # TEST DEFINITIONS #

    def test_initial_ap_config(self):
        '''Check initial configuration on repeater1.'''
        self.check_log(env.agents[0].radios[0], r"WSC Global authentication success")
        self.check_log(env.agents[0].radios[1], r"WSC Global authentication success")
        self.check_log(env.agents[0].radios[0], r"KWA \(Key Wrap Auth\) success")
        self.check_log(env.agents[0].radios[1], r"KWA \(Key Wrap Auth\) success")
        self.check_log(env.agents[0].radios[0],
                       r".* Controller configuration \(WSC M2 Encrypted Settings\)")
        self.check_log(env.agents[0].radios[1],
                       r".* Controller configuration \(WSC M2 Encrypted Settings\)")

    def test_ap_config_renew(self):
        # Regression test: MAC address should be case insensitive
        mac_repeater1_upper = env.agents[0].mac.upper()
        # Configure the controller and send renew
        env.controller.cmd_reply("DEV_RESET_DEFAULT")
        env.controller.cmd_reply(
            "DEV_SET_CONFIG,"
            "bss_info1,{} 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,"
            "bss_info2,{} 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0"
            .format(mac_repeater1_upper, env.agents[0].mac))
        env.controller.dev_send_1905(env.agents[0].mac, 0x000A,
                                     tlv(0x01, 0x0006, "{" + env.controller.mac + "}"),
                                     tlv(0x0F, 0x0001, "{0x00}"),
                                     tlv(0x10, 0x0001, "{0x00}"))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(env.agents[0].radios[0],
                       r"Received credentials for ssid: Multi-AP-24G-1 .* "
                       r"fronthaul: true backhaul: false")
        self.check_log(env.agents[0].radios[0],
                       r"Received credentials for ssid: Multi-AP-24G-2 .*"
                       r"fronthaul: false backhaul: true")
        self.check_log(env.agents[0].radios[1], r".* tear down radio")

        bssid1 = env.agents[0].dev_get_parameter('macaddr',
                                                 ruid='0x' +
                                                 env.agents[0].radios[0].mac.replace(':', ''),
                                                 ssid='Multi-AP-24G-1')
        if not bssid1:
            self.fail("repeater1 didn't configure Multi-AP-24G-1")

        # simulate wps onboarding to the backhaul vap
        env.agents[0].start_wps_registration("24G")
        self.check_log(env.agents[0].radios[0], r"Start WPS PBC")

    def test_ap_config_bss_tear_down(self):
        # Configure the controller and send renew
        env.controller.cmd_reply("DEV_RESET_DEFAULT")
        env.controller.cmd_reply(
            "DEV_SET_CONFIG,bss_info1,"
            "{} 8x Multi-AP-24G-3 0x0020 0x0008 maprocks1 0 1".format(env.agents[0].mac))
        env.controller.dev_send_1905(env.agents[0].mac, 0x000A,
                                     tlv(0x01, 0x0006, "{" + env.controller.mac + "}"),
                                     tlv(0x0F, 0x0001, "{0x00}"),
                                     tlv(0x10, 0x0001, "{0x00}"))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(env.agents[0].radios[0],
                       r"Received credentials for ssid: Multi-AP-24G-3 .*"
                       r"fronthaul: true backhaul: false")
        self.check_log(env.agents[0].radios[1], r".* tear down radio")
        conn_map = connmap.get_conn_map()
        repeater1 = conn_map[env.agents[0].mac]
        repeater1_wlan0 = repeater1.radios[env.agents[0].radios[0].mac]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid not in (b'Multi-AP-24G-3', b'N/A'):
                self.fail('Wrong SSID: {vap.ssid} instead of Multi-AP-24G-3'.format(vap=vap))
        repeater1_wlan2 = repeater1.radios[env.agents[0].radios[1].mac]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))

        # SSIDs have been removed for the CTT Agent1's front radio
        env.controller.cmd_reply(
            "DEV_SET_CONFIG,bss_info1,{} 8x".format(env.agents[0].mac))
        # Send renew message
        env.controller.dev_send_1905(env.agents[0].mac, 0x000A,
                                     tlv(0x01, 0x0006, "{" + env.controller.mac + "}"),
                                     tlv(0x0F, 0x0001, "{0x00}"),
                                     tlv(0x10, 0x0001, "{0x00}"))

        time.sleep(3)
        self.check_log(env.agents[0].radios[0], r".* tear down radio")
        conn_map = connmap.get_conn_map()
        repeater1 = conn_map[env.agents[0].mac]
        repeater1_wlan0 = repeater1.radios[env.agents[0].radios[0].mac]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))
        repeater1_wlan2 = repeater1.radios[env.agents[0].radios[1].mac]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))

    def test_ap_config_bss_tear_down_cli(self):
        # Same test as the previous one but using CLI instead of dev_send_1905

        env.beerocks_cli_command('bml_clear_wifi_credentials {}'.format(env.agents[0].mac))
        env.beerocks_cli_command('bml_set_wifi_credentials {} {} {} {} {}'.format(env.agents[0].mac,
                                 "Multi-AP-24G-3-cli", "maprocks1", "24g", "fronthaul"))
        env.beerocks_cli_command('bml_update_wifi_credentials {}'.format(env.agents[0].mac))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(env.agents[0].radios[0],
                       r"Received credentials for ssid: Multi-AP-24G-3-cli .*"
                       r"fronthaul: true backhaul: false")
        self.check_log(env.agents[0].radios[1], r".* tear down radio")
        conn_map = connmap.get_conn_map()
        repeater1 = conn_map[env.agents[0].mac]
        repeater1_wlan0 = repeater1.radios[env.agents[0].radios[0].mac]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid not in (b'Multi-AP-24G-3-cli', b'N/A'):
                self.fail('Wrong SSID: {vap.ssid} instead of Multi-AP-24G-3-cli'.format(vap=vap))
        repeater1_wlan2 = repeater1.radios[env.agents[0].radios[1].mac]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))

        env.beerocks_cli_command('bml_clear_wifi_credentials {}'.format(env.agents[0].mac))
        env.beerocks_cli_command('bml_update_wifi_credentials {}'.format(env.agents[0].mac))

        time.sleep(3)
        self.check_log(env.agents[0].radios[0], r".* tear down radio")
        conn_map = connmap.get_conn_map()
        repeater1 = conn_map[env.agents[0].mac]
        repeater1_wlan0 = repeater1.radios[env.agents[0].radios[0].mac]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))
        repeater1_wlan2 = repeater1.radios[env.agents[0].radios[1].mac]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))

    def test_channel_selection(self):
        debug("Send channel preference query")
        ch_pref_query_mid = env.controller.dev_send_1905(env.agents[0].mac, 0x8004)
        time.sleep(1)
        debug("Confirming channel preference query has been received on agent")
        self.check_log(env.agents[0].radios[0], "CHANNEL_PREFERENCE_QUERY_MESSAGE")
        self.check_log(env.agents[0].radios[1], "CHANNEL_PREFERENCE_QUERY_MESSAGE")

        # TODO should be a single response (currently two are sent)
        self.check_cmdu_type("channel preferency response", 0x8005, env.agents[0].mac,
                             env.controller.mac, ch_pref_query_mid)

        debug("Send empty channel selection request")
        cs_req_mid = env.controller.dev_send_1905(env.agents[0].mac,
                                                  0x8006, tlv(0x00, 0x0000, "{}"))
        time.sleep(1)

        debug("Confirming empty channel selection request has been received on agent")
        self.check_log(env.agents[0].radios[0], "CHANNEL_SELECTION_REQUEST_MESSAGE")
        self.check_log(env.agents[0].radios[1], "CHANNEL_SELECTION_REQUEST_MESSAGE")

        # TODO should be a single response (currently two are sent)
        self.check_cmdu_type("channel selection response", 0x8007, env.agents[0].mac,
                             env.controller.mac, cs_req_mid)
        oper_channel_reports = self.check_cmdu_type("operating channel report", 0x8008,
                                                    env.agents[0].mac, env.controller.mac)
        for report in oper_channel_reports:
            self.check_cmdu_type_single("ACK", 0x8000, env.controller.mac, env.agents[0].mac,
                                        report.ieee1905_mid)

        env.checkpoint()

        tp20dBm = 0x14
        tp21dBm = 0x15

        for payload_transmit_power in (tp20dBm, tp21dBm):
            debug("Send empty channel selection request with changing tx_power_limit")
            cs_req_mid = env.controller.dev_send_1905(
                env.agents[0].mac,
                0x8006,
                tlv(0x8D, 0x0007, '{} 0x{:02x}'.format(env.agents[0].radios[0].mac,
                                                       payload_transmit_power)),
                tlv(0x8D, 0x0007, '{} 0x{:02x}'.format(env.agents[0].radios[1].mac,
                                                       payload_transmit_power))
            )
            time.sleep(1)

            self.check_log(env.agents[0].radios[0], "CHANNEL_SELECTION_REQUEST_MESSAGE")
            self.check_log(env.agents[0].radios[1], "CHANNEL_SELECTION_REQUEST_MESSAGE")

            self.check_log(env.agents[0].radios[0],
                           "tlvTransmitPowerLimit {}".format(payload_transmit_power))
            self.check_log(env.agents[0].radios[1],
                           "tlvTransmitPowerLimit {}".format(payload_transmit_power))

            # TODO should be a single response (currently two are sent)
            self.check_cmdu_type("channel selection response", 0x8007, env.agents[0].mac,
                                 env.controller.mac, cs_req_mid)

            oper_channel_reports = self.check_cmdu_type("operating channel report", 0x8008,
                                                        env.agents[0].mac, env.controller.mac)
            for report in oper_channel_reports:
                for ocr in report.ieee1905_tlvs:
                    if ocr.tlv_type != 0x8F:
                        self.fail("Unexpected TLV in operating channel report: {}".format(ocr))
                        continue
                    if int(ocr.operating_channel_eirp) != payload_transmit_power:
                        self.fail("Unexpected transmit power {} instead of {} for {}".format(
                            ocr.operating_channel_eirp, payload_transmit_power,
                            ocr.operating_channel_radio_id))
                self.check_cmdu_type_single("ACK", 0x8000, env.controller.mac, env.agents[0].mac,
                                            report.ieee1905_mid)

            env.checkpoint()

        # payload_wlan0 - request for change channel on 6
        payload_wlan0 = (
            "0x14 "
            "{0x51 {0x0C {0x01 0x02 0x03 0x04 0x05 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D} 0x00}} "
            "{0x52 {0x00 0x00}} "
            "{0x53 {0x08 {0x01 0x02 0x03 0x04 0x05 0x07 0x08 0x09} 0x00}} "
            "{0x54 {0x08 {0x05 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D} 0x00}} "
            "{0x73 {0x00 0x00}} "
            "{0x74 {0x00 0x00}} "
            "{0x75 {0x00 0x00}} "
            "{0x76 {0x00 0x00}} "
            "{0x77 {0x00 0x00}} "
            "{0x78 {0x00 0x00}} "
            "{0x79 {0x00 0x00}} "
            "{0x7A {0x00 0x00}} "
            "{0x7B {0x00 0x00}} "
            "{0x7C {0x00 0x00}} "
            "{0x7D {0x00 0x00}} "
            "{0x7E {0x00 0x00}} "
            "{0x7F {0x00 0x00}} "
            "{0x80 {0x00 0x00}} "
            "{0x81 {0x00 0x00}} "
            "{0x82 {0x00 0x00}} "
        )

        # payload_wlan2  - request for change channel on 36
        payload_wlan2 = (
            "0x14 "
            "{0x51 {0x00 0x00}} "
            "{0x52 {0x00 0x00}} "
            "{0x53 {0x00 0x00}} "
            "{0x54 {0x00 0x00}} "
            "{0x73 0x03 {0x28 0x2C 0x30} 0x00} "
            "{0x74 0x01 {0x2C} 0x00} "
            "{0x75 {0x00 0x00}} "
            "{0x76 {0x00 0x00}} "
            "{0x77 {0x00 0x00}} "
            "{0x78 {0x00 0x00}} "
            "{0x79 {0x00 0x00}} "
            "{0x7A {0x00 0x00}} "
            "{0x7B {0x00 0x00}} "
            "{0x7C {0x00 0x00}} "
            "{0x7D {0x00 0x00}} "
            "{0x7E {0x00 0x00}} "
            "{0x7F {0x00 0x00}} "
            "{0x80 0x05 {0x3A 0x6A 0x7A 0x8A 0x9B} 0x00} "
            "{0x81 {0x00 0x00}} "
            "{0x82 {0x00 0x00}}"
        )

        """
        Step 1: Trigger channel selection to channel 6 and 36. Check that
                operating channel report was sent.

        Step 2: Trigger channel selection to channel 6 and 36 again - check that
                operating channel report is sent again. This is to catch bugs
                when we don't send channel report when there is no need to
                switch channel
        """
        for i in range(1, 3):
            debug("Send channel selection request, step {}".format(i))
            cs_req_mid = env.controller.dev_send_1905(
                env.agents[0].mac,
                0x8006,
                tlv(0x8B, 0x005F, '{} {}'.format(env.agents[0].radios[0].mac, payload_wlan0)),
                tlv(0x8D, 0x0007, '{} 0x{:2x}'.format(env.agents[0].radios[0].mac, tp20dBm)),
                tlv(0x8B, 0x004C, '{} {}'.format(env.agents[0].radios[1].mac, payload_wlan2)),
                tlv(0x8D, 0x0007, '{} 0x{:2x}'.format(env.agents[0].radios[1].mac, tp20dBm))
            )
            time.sleep(1)

            debug(
                "Confirming channel selection request has been received on agent,step {}".format(i))

            self.check_log(env.agents[0].radios[0], "CHANNEL_SELECTION_REQUEST_MESSAGE")
            self.check_log(env.agents[0].radios[1], "CHANNEL_SELECTION_REQUEST_MESSAGE")

            debug(
                "Confirming tlvTransmitPowerLimit has been received with correct value on \
                    agent, step {}".format(i))

            self.check_log(env.agents[0].radios[0], "tlvTransmitPowerLimit {}".format(tp20dBm))

            self.check_log(env.agents[0].radios[0],
                           "ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START")

            self.check_log(env.agents[0].radios[1], "tlvTransmitPowerLimit {}".format(tp20dBm))

            self.check_log(env.agents[0].radios[0],
                           "ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START")

            # TODO should be a single response (currently two are sent)
            self.check_cmdu_type("channel selection response", 0x8007, env.agents[0].mac,
                                 env.controller.mac, cs_req_mid)

            oper_channel_reports = self.check_cmdu_type("operating channel report", 0x8008,
                                                        env.agents[0].mac, env.controller.mac)
            for report in oper_channel_reports:
                for ocr in report.ieee1905_tlvs:
                    if ocr.tlv_type != 0x8F:
                        self.fail("Unexpected TLV in operating channel report: {}".format(ocr))
                        continue
                    if int(ocr.operating_channel_eirp) != tp20dBm:
                        self.fail("Unexpected transmit power {} instead of {} for {}".format(
                            ocr.operating_channel_eirp, payload_transmit_power,
                            ocr.operating_channel_radio_id))
                self.check_cmdu_type_single("ACK", 0x8000, env.controller.mac, env.agents[0].mac,
                                            report.ieee1905_mid)

            env.checkpoint()

    def test_ap_capability_query(self):
        env.controller.dev_send_1905(env.agents[0].mac, 0x8001)
        time.sleep(1)

        debug("Confirming ap capability query has been received on agent")
        self.check_log(env.agents[0], "AP_CAPABILITY_QUERY_MESSAGE")

        debug("Confirming ap capability report has been received on controller")
        self.check_log(env.controller, "AP_CAPABILITY_REPORT_MESSAGE")

    def test_link_metric_query(self):
        env.controller.dev_send_1905(env.agents[0].mac, 0x0005,
                                     tlv(0x08, 0x0002, "0x00 0x02"))
        time.sleep(1)

        debug("Confirming link metric query has been received on agent")
        self.check_log(env.agents[0], "Received LINK_METRIC_QUERY_MESSAGE")

        debug("Confirming link metric response has been received on controller")
        self.check_log(env.controller, "Received LINK_METRIC_RESPONSE_MESSAGE")
        self.check_log(env.controller, "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(env.controller, "Received TLV_RECEIVER_LINK_METRIC")

    def test_combined_infra_metrics(self):
        debug("Send AP Metrics query message to agent 1")
        vap1 = env.agents[0].radios[0].vaps[0]
        vap2 = env.agents[1].radios[1].vaps[0]
        env.controller.dev_send_1905(env.agents[0].mac, 0x800B,
                                     tlv(0x93, 0x0007, "0x01 {%s}" % (vap1.bssid)))
        self.check_log(env.controller, "Received AP_METRICS_RESPONSE_MESSAGE")

        debug("Send AP Metrics query message to agent 2")
        env.controller.dev_send_1905(env.agents[1].mac, 0x800B,
                                     tlv(0x93, 0x0007, "0x01 {%s}" % vap2.bssid))
        self.check_log(env.controller, "Received AP_METRICS_RESPONSE_MESSAGE")

        debug("Send 1905 Link metric query to agent 1 (neighbor gateway)")
        env.controller.dev_send_1905(env.agents[0].mac, 0x0005,
                                     tlv(0x08, 0x0008, "0x01 {%s} 0x02" % env.controller.mac))
        self.check_log(env.agents[0], "Received LINK_METRIC_QUERY_MESSAGE")
        self.check_log(env.controller, "Received LINK_METRIC_RESPONSE_MESSAGE")
        self.check_log(env.controller, "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(env.controller, "Received TLV_RECEIVER_LINK_METRIC")

        # Trigger combined infra metrics
        debug("Send Combined infrastructure metrics message to agent 1")
        env.controller.dev_send_1905(env.agents[0].mac, 0x8013)
        self.check_log(env.agents[0], "Received COMBINED_INFRASTRUCTURE_METRICS")
        self.check_log(env.agents[0], "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(env.agents[0], "Received TLV_RECEIVER_LINK_METRIC")

    def test_client_capability_query(self):
        sta1 = env.Station.create()
        sta2 = env.Station.create()

        association_frame = """00 0e 4d 75 6c 74 69 2d 41 50 2d 32 34 47 2d 31
01 08 02 04 0b 0c 12 16 18 24 21 02 00 14 30 14
01 00 00 0f ac 04 01 00 00 0f ac 04 01 00 00 0f
ac 02 00 00 32 04 30 48 60 6c 3b 10 51 51 53 54
73 74 75 76 77 78 7c 7d 7e 7f 80 82 3b 16 0c 01
02 03 04 05 0c 16 17 18 19 1a 1b 1c 1d 1e 1f 20
21 80 81 82 46 05 70 00 00 00 00 46 05 71 50 50
00 04 7f 0a 04 00 0a 82 21 40 00 40 80 00 dd 07
00 50 f2 02 00 01 00 2d 1a 2d 11 03 ff ff 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 18 e6
e1 09 00 bf 0c b0 79 d1 33 fa ff 0c 03 fa ff 0c
03 c7 01 10 """

        debug("Send client capability query for unconnected STA")
        env.controller.dev_send_1905(env.agents[0].mac, 0x8009,
                                     tlv(0x90, 0x000C,
                                         '{} {}'.format(env.agents[0].radios[0].mac, sta1.mac)))
        time.sleep(1)
        debug("Confirming client capability query has been received on agent")
        # check that both radio agents received it, in the future we'll add a check to verify which
        # radio the query was intended for.
        self.check_log(env.agents[0], r"CLIENT_CAPABILITY_QUERY_MESSAGE")

        debug("Confirming client capability report message has been received on controller")
        self.check_log(env.controller, r"Received CLIENT_CAPABILITY_REPORT_MESSAGE")
        self.check_log(env.controller,
                       r"Result Code= FAILURE, client MAC= {}, BSSID= {}"
                       .format(sta1.mac, env.agents[0].radios[0].mac))

        debug("Connect dummy STA to wlan0")
        env.agents[0].radios[0].vaps[0].associate(sta2)

        debug("Send client capability query for connected STA")
        env.controller.dev_send_1905(env.agents[0].mac, 0x8009,
                                     tlv(0x90, 0x000C,
                                         '{} {}'.format(env.agents[0].radios[0].mac, sta2.mac)))
        time.sleep(1)

        debug("Confirming client capability report message has been received on controller")
        self.check_log(env.controller, r"Received CLIENT_CAPABILITY_REPORT_MESSAGE")
        self.check_log(env.controller,
                       r"Result Code= SUCCESS, client MAC= {}, BSSID= {}"
                       .format(sta2.mac, env.agents[0].radios[0].mac))

        for line in association_frame.splitlines():
            self.check_log(env.controller, r"{}".format(line))

    def test_client_association_dummy(self):
        sta = env.Station.create()

        debug("Connect dummy STA to wlan0")
        env.agents[0].radios[0].vaps[0].associate(sta)
        debug("Send client association control request to the chosen BSSID (UNBLOCK)")
        env.beerocks_cli_command('client_allow {} {}'.format(sta.mac, env.agents[0].radios[1].mac))
        time.sleep(1)

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(env.agents[0].radios[1], r"Got client allow request for {}".format(sta.mac))

        debug("Send client association control request to all other (BLOCK) ")
        env.beerocks_cli_command('client_disallow {} {}'.format(sta.mac,
                                                                env.agents[0].radios[0].mac))
        time.sleep(1)

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(env.agents[0].radios[0],
                       r"Got client disallow request for {}".format(sta.mac))

        # TODO client blocking not implemented in dummy bwl

        # Check in connection map
        conn_map = connmap.get_conn_map()
        map_radio = conn_map[env.agents[0].mac].radios[env.agents[0].radios[0].mac]
        map_vap = map_radio.vaps[env.agents[0].radios[0].vaps[0].bssid]
        if sta.mac not in map_vap.clients:
            self.fail("client {} not in conn_map, clients: {}".format(sta.mac, map_vap.clients))

        # Associate with other radio, check that conn_map is updated
        env.agents[0].radios[0].vaps[0].disassociate(sta)
        env.agents[0].radios[1].vaps[0].associate(sta)
        time.sleep(1)  # Wait for conn_map to be updated
        conn_map = connmap.get_conn_map()
        map_agent = conn_map[env.agents[0].mac]
        map_radio1 = map_agent.radios[env.agents[0].radios[1].mac]
        map_vap1 = map_radio1.vaps[env.agents[0].radios[1].vaps[0].bssid]
        if sta.mac not in map_vap1.clients:
            self.fail("client {} not in conn_map, clients: {}".format(sta.mac, map_vap1.clients))
        map_radio0 = map_agent.radios[env.agents[0].radios[0].mac]
        map_vap0 = map_radio0.vaps[env.agents[0].radios[0].vaps[0].bssid]
        if sta.mac in map_vap0.clients:
            self.fail("client {} still in conn_map, clients: {}".format(sta.mac, map_vap0.clients))

        # Associate with other radio implies disassociate from first
        env.agents[0].radios[0].vaps[0].associate(sta)
        time.sleep(1)  # Wait for conn_map to be updated
        conn_map = connmap.get_conn_map()
        map_agent = conn_map[env.agents[0].mac]
        map_radio1 = map_agent.radios[env.agents[0].radios[1].mac]
        map_vap1 = map_radio1.vaps[env.agents[0].radios[1].vaps[0].bssid]
        if sta.mac in map_vap1.clients:
            self.fail("client {} still in conn_map, clients: {}".format(sta.mac, map_vap1.clients))
        map_radio0 = map_agent.radios[env.agents[0].radios[0].mac]
        map_vap0 = map_radio0.vaps[env.agents[0].radios[0].vaps[0].bssid]
        if sta.mac not in map_vap0.clients:
            self.fail("client {} not in conn_map, clients: {}".format(sta.mac, map_vap0.clients))

    def test_client_association_link_metrics(self):
        ''' This test verifies that a MAUT with an associated STA responds to
        an Associated STA Link Metrics Query message with an Associated STA Link Metrics
        Response message containing an Associated STA Link Metrics TLV for the associated STA.'''

        sta_mac = "11:11:33:44:55:66"
        debug("Send link metrics query for unconnected STA")
        env.controller.dev_send_1905(env.agents[0].mac, 0x800D,
                                     tlv(0x95, 0x0006, '{sta_mac}'.format(sta_mac=sta_mac)))
        self.check_log(env.agents[0],
                       "client with mac address {sta_mac} not found".format(sta_mac=sta_mac))
        time.sleep(1)

        sta = env.Station.create()
        debug('sta: {}'.format(sta.mac))

        env.agents[0].radios[0].vaps[0].associate(sta)

        time.sleep(1)

        mid = env.controller.dev_send_1905(env.agents[0].mac, 0x800D,
                                           tlv(0x95, 0x0006, '{sta_mac}'.format(sta_mac=sta.mac)))
        time.sleep(1)
        self.check_log(env.agents[0],
                       "Send AssociatedStaLinkMetrics to controller, mid = {}".format(mid))

    def test_client_steering_mandate(self):
        debug("Send topology request to agent 1")
        env.controller.dev_send_1905(env.agents[0].mac, 0x0002)
        time.sleep(1)
        debug("Confirming topology query was received")
        self.check_log(env.agents[0], "TOPOLOGY_QUERY_MESSAGE")

        debug("Send topology request to agent 2")
        env.controller.dev_send_1905(env.agents[1].mac, 0x0002)
        time.sleep(1)
        debug("Confirming topology query was received")
        self.check_log(env.agents[1], "TOPOLOGY_QUERY_MESSAGE")

        debug("Send Client Steering Request message for Steering Mandate to CTT Agent1")
        env.controller.dev_send_1905(env.agents[0].mac, 0x8014,
                                       tlv(0x9B, 0x001b,
                                           "{%s 0xe0 0x0000 0x1388 0x01 {0x000000110022} 0x01 {%s 0x73 0x24}}" % (env.agents[0].radios[0].mac, env.agents[1].radios[0].mac)))  # noqa E501
        time.sleep(1)
        debug("Confirming Client Steering Request message was received - mandate")
        self.check_log(env.agents[0].radios[0], "Got steer request")

        debug("Confirming BTM Report message was received")
        self.check_log(env.controller, "CLIENT_STEERING_BTM_REPORT_MESSAGE")

        debug("Checking BTM Report source bssid")
        self.check_log(env.controller, "BTM_REPORT from source bssid %s" %
                       env.agents[0].radios[0].mac)

        debug("Confirming ACK message was received")
        self.check_log(env.agents[0].radios[0], "ACK_MESSAGE")

        env.controller.dev_send_1905(env.agents[0].mac, 0x8014,
                                       tlv(0x9B, 0x000C,
                                           "{%s 0x00 0x000A 0x0000 0x00}" % env.agents[0].radios[0].mac))  # noqa E501
        time.sleep(1)
        debug("Confirming Client Steering Request message was received - Opportunity")
        self.check_log(env.agents[0].radios[0], "CLIENT_STEERING_REQUEST_MESSAGE")

        debug("Confirming ACK message was received")
        self.check_log(env.controller, "ACK_MESSAGE")

        debug("Confirming steering completed message was received")
        self.check_log(env.controller, "STEERING_COMPLETED_MESSAGE")

        debug("Confirming ACK message was received")
        self.check_log(env.agents[0].radios[0], "ACK_MESSAGE")

    def test_client_steering_dummy(self):
        sta = env.Station.create()

        debug("Connect dummy STA to wlan0")
        env.agents[0].radios[0].vaps[0].associate(sta)
        debug("Send steer request ")
        env.beerocks_cli_command("steer_client {} {}".format(sta.mac, env.agents[0].radios[1].mac))
        time.sleep(1)

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(env.agents[0].radios[1], r"Got client allow request")

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(env.agents[0].radios[0], r"Got client disallow request")

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(env.agents[1].radios[0], r"Got client disallow request")

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(env.agents[1].radios[1], r"Got client disallow request")

        debug("Confirming Client Steering Request message was received - mandate")
        self.check_log(env.agents[0].radios[0], r"Got steer request")

        debug("Confirming BTM Report message was received")
        self.check_log(env.controller, r"CLIENT_STEERING_BTM_REPORT_MESSAGE")

        debug("Confirming ACK message was received")
        self.check_log(env.agents[0].radios[0], r"ACK_MESSAGE")

        debug("Disconnect dummy STA from wlan0")
        env.agents[0].radios[0].vaps[0].disassociate(sta)
        # Make sure that controller sees disconnect before connect by waiting a little
        time.sleep(1)

        debug("Connect dummy STA to wlan2")
        env.agents[0].radios[1].vaps[0].associate(sta)
        debug("Confirm steering success by client connected")
        self.check_log(env.controller, r"steering successful for sta {}".format(sta.mac))
        self.check_log(env.controller,
                       r"sta {} disconnected due to steering request".format(sta.mac))

        # Make sure that all blocked agents send UNBLOCK messages at the end of
        # disallow period (default 25 sec)
        time.sleep(25)

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(env.agents[0].radios[0], r"Got client allow request")

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(env.agents[1].radios[0], r"Got client allow request")

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(env.agents[1].radios[1], r"Got client allow request")

    def test_multi_ap_policy_config_w_steering_policy(self):
        debug("Send multi-ap policy config request with steering policy to agent 1")
        mid = env.controller.dev_send_1905(env.agents[0].mac, 0x8003,
                                             tlv(0x89, 0x000C, "{0x00 0x00 0x01 {%s 0x01 0xFF 0x14}}" % env.agents[0].radios[0].mac))  # noqa E501
        time.sleep(1)
        debug("Confirming multi-ap policy config request has been received on agent")

        self.check_log(env.agents[0], r"MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE")
        time.sleep(1)
        debug("Confirming multi-ap policy config ack message has been received on the controller")
        self.check_log(env.controller, r"ACK_MESSAGE, mid=0x{:04x}".format(mid))

    def send_and_check_policy_config_metric_reporting(self, agent, include_sta_traffic_stats=True,
                                                      include_sta_link_metrics=True):
        debug("Send multi-ap policy config request with metric reporting policy to agent")
        reporting_value = 0
        if include_sta_traffic_stats:
            reporting_value |= 0x80
        if include_sta_link_metrics:
            reporting_value |= 0x40
        radio_policies = ["{%s 0x00 0x00 0x01 0x%02x}" % (radio.mac, reporting_value)
                          for radio in agent.radios]
        metric_reporting_tlv = tlv(0x8a, 2 + 10 * len(radio_policies),
                                   "{0x00 0x%02x %s}" % (len(radio_policies),
                                                         " ".join(radio_policies)))
        mid = env.controller.dev_send_1905(agent.mac, 0x8003, metric_reporting_tlv)
        time.sleep(1)
        debug("Confirming multi-ap policy config request was acked by agent")
        self.check_cmdu_type_single("ACK", 0x8000, agent.mac, env.controller.mac, mid)

    def test_multi_ap_policy_config_w_metric_reporting_policy(self):
        self.send_and_check_policy_config_metric_reporting(env.agents[0], True, True)

    def test_client_association(self):
        debug("Send topology request to agent 1")
        env.controller.dev_send_1905(env.agents[0].mac, 0x0002)
        debug("Confirming topology query was received")
        self.check_log(env.agents[0], r"TOPOLOGY_QUERY_MESSAGE")

        debug("Send client association control message")
        env.controller.dev_send_1905(env.agents[0].mac, 0x8016,
                                       tlv(0x9D, 0x000F,
                                           "{%s 0x00 0x1E 0x01 {0x000000110022}}" % env.agents[0].radios[0].mac))  # noqa E501

        debug("Confirming client association control message has been received on agent")
        # check that both radio agents received it,in the future we'll add a check to verify which
        # radio the query was intended for.
        self.check_log(env.agents[0].radios[0], r"CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE")
        self.check_log(env.agents[0].radios[1], r"CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE")

        debug("Confirming ACK message was received on controller")
        self.check_log(env.controller, r"ACK_MESSAGE")

    def test_higher_layer_data_payload_trigger(self):
        mac_gateway_hex = '0x' + env.controller.mac.replace(':', '')
        debug("mac_gateway_hex = " + mac_gateway_hex)
        payload = 199 * (mac_gateway_hex + " ") + mac_gateway_hex

        debug("Send Higher Layer Data message")
        # MCUT sends Higher Layer Data message to CTT Agent1 by providing:
        # Higher layer protocol = "0x00"
        # Higher layer payload = 200 concatenated copies of the ALID of the MCUT (1200 octets)
        mid = env.controller.dev_send_1905(env.agents[0].mac, 0x8018,
                                           tlv(0xA0, 0x04b1, "{0x00 %s}" % payload))

        debug("Confirming higher layer data message was received in the agent")

        self.check_log(env.agents[0], r"HIGHER_LAYER_DATA_MESSAGE")

        debug("Confirming matching protocol and payload length")

        self.check_log(env.agents[0], r"protocol: 0")
        self.check_log(env.agents[0], r"payload_length: 0x4b0")

        debug("Confirming ACK message was received in the controller")
        self.check_log(env.controller, r"ACK_MESSAGE, mid=0x{:04x}".format(mid))

    def test_topology(self):
        mid = env.controller.dev_send_1905(env.agents[0].mac, 0x0002)
        debug("Confirming topology query was received")
        self.check_log(env.agents[0], r"TOPOLOGY_QUERY_MESSAGE.*mid={:d}".format(mid))


if __name__ == '__main__':
    t = TestFlows()

    parser = argparse.ArgumentParser()
    parser.add_argument("--verbose", "-v", action='store_true', default=False,
                        help="report each action")
    parser.add_argument("--stop-on-failure", "-s", action='store_true', default=False,
                        help="exit on the first failure")
    user = os.getenv("SUDO_USER", os.getenv("USER", ""))
    parser.add_argument("--unique-id", "-u", type=str, default=user,
                        help="append UNIQUE_ID to all container names, e.g. gateway-<UNIQUE_ID>; "
                             "defaults to {}".format(user))
    parser.add_argument("--tag", "-t", type=str,
                        help="use runner image with tag TAG instead of 'latest'")
    parser.add_argument("--skip-init", action='store_true', default=False,
                        help="don't start up the containers")
    parser.add_argument("tests", nargs='*',
                        help="tests to run; if not specified, run all tests: " + ", ".join(t.tests))
    options = parser.parse_args()

    unknown_tests = [test for test in options.tests if test not in t.tests]
    if unknown_tests:
        parser.error("Unknown tests: {}".format(', '.join(unknown_tests)))

    opts.verbose = options.verbose

    opts.tcpdump_dir = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', 'logs'))
    opts.stop_on_failure = options.stop_on_failure

    t.start_test('init')
    env.launch_environment_docker(options.unique_id, options.skip_init, options.tag)

    if t.run_tests(options.tests):
        sys.exit(1)
