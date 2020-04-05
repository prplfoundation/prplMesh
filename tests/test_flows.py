#! /usr/bin/env python3
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Arnout Vandecappelle (Essensium/Mind)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import argparse
import os
import re
import subprocess
import sys
import platform
import time
from typing import Dict

import environment as env
from capi import tlv, UCCSocket
from opts import debug, err, message, opts, status

'''Regular expression to match a MAC address in a bytes string.'''
RE_MAC = rb"(?P<mac>([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})"


class MapVap:
    '''Represents a VAP in the connection map.'''

    def __init__(self, bssid: str, ssid: bytes):
        self.bssid = bssid
        self.ssid = ssid


class MapRadio:
    '''Represents a radio in the connection map.'''

    def __init__(self, uid: str):
        self.uid = uid
        self.vaps = {}

    def add_vap(self, bssid: str, ssid: bytes):
        vap = MapVap(bssid, ssid)
        self.vaps[bssid] = vap
        return vap


class MapDevice:
    '''Represents a device in the connection map.'''

    def __init__(self, mac: str):
        self.mac = mac
        self.radios = {}

    def add_radio(self, uid: str):
        radio = MapRadio(uid)
        self.radios[uid] = radio
        return radio


class TestFlows:
    def __init__(self):
        self.tests = [attr[len('test_'):] for attr in dir(self) if attr.startswith('test_')]

        self.rootdir = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..'))
        self.installdir = os.path.join(self.rootdir, 'build', 'install')
        self.running = ''
        self.tcpdump_proc = None
        self.bridge_name = 'br-lan'

    def fail(self, msg: str) -> bool:
        '''Print a red error message, increment failure count and return False.'''
        self.check_error += 1
        err('FAIL: {}'.format(msg))
        if opts.stop_on_failure:
            sys.exit(1)
        return False

    def start_test(self, test: str):
        '''Call this at the beginning of a test.'''
        self.running = test
        status(test + " starting")

    def docker_command(self, device: str, *command: str) -> bytes:
        '''Execute `command` in docker container `device` and return its output.'''
        return subprocess.check_output(("docker", "exec", device) + command)

    def beerocks_cli_command(self, command: str) -> bytes:
        '''Execute `command` beerocks_cli command on the controller and return its output.'''
        debug("Send CLI command " + command)
        res = self.docker_command(self.gateway,
                                  os.path.join(self.installdir, "bin", "beerocks_cli"),
                                  "-c",
                                  command)
        debug("  Response: " + res.decode('utf-8', errors='replace').strip())
        return res

    def get_conn_map(self) -> Dict[str, MapDevice]:
        '''Get the connection map from the controller.'''
        conn_map = {}
        for line in self.beerocks_cli_command("bml_conn_map").split(b'\n'):
            # TODO we need to parse indentation to get the exact topology.
            # For the time being, just parse the repeaters.
            bridge = re.search(rb' {8}IRE_BRIDGE: .* mac: ' + RE_MAC, line)
            radio = re.match(rb' {16}RADIO: .* mac: ' + RE_MAC, line)
            vap = re.match(rb' {20}fVAP.* bssid: ' + RE_MAC + rb', ssid: (?P<ssid>.*)$', line)
            if bridge:
                cur_agent = MapDevice(bridge.group('mac').decode('utf-8'))
                conn_map[cur_agent.mac] = cur_agent
            elif radio:
                cur_radio = cur_agent.add_radio(radio.group('mac').decode('utf-8'))
            elif vap:
                cur_radio.add_vap(vap.group('mac').decode('utf-8'), vap.group('ssid'))
        return conn_map

    def open_CAPI_socket(self, device: str, controller: bool = False) -> UCCSocket:
        '''Open a CAPI socket to the agent (or controller, if set) on "device".'''
        # First, get the UCC port from the config file
        if controller:
            config_file_name = 'beerocks_controller.conf'
        else:
            config_file_name = 'beerocks_agent.conf'
        with open(os.path.join(self.installdir, 'config', config_file_name)) as config_file:
            ucc_port = re.search(
                r'ucc_listener_port=(?P<port>[0-9]+)', config_file.read()).group('port')

        # On WSL, connect to the locally exposed container port
        if self.on_wsl:
            published_port_output = subprocess.check_output(
                ["docker", "port", device, ucc_port]).decode('utf-8').split(":")
            device_ip = published_port_output[0]
            ucc_port = int(published_port_output[1])
        else:
            device_ip_output = self.docker_command(
                device, 'ip', '-f', 'inet', 'addr', 'show', self.bridge_name)
            device_ip = re.search(
                r'inet (?P<ip>[0-9.]+)', device_ip_output.decode('utf-8')).group('ip')

        return UCCSocket(device_ip, ucc_port)

    def init(self, unique_id: str, skip_init: bool):
        '''Initialize the tests.'''
        self.start_test('init')
        self.gateway = 'gateway-' + unique_id
        self.repeater1 = 'repeater1-' + unique_id
        self.repeater2 = 'repeater2-' + unique_id
        self.on_wsl = "microsoft" in platform.uname()[3].lower()

        env.launch_environment_docker(unique_id, skip_init)

        self.gateway_ucc = self.open_CAPI_socket(self.gateway, True)
        self.repeater1_ucc = self.open_CAPI_socket(self.repeater1)
        self.repeater2_ucc = self.open_CAPI_socket(self.repeater2)

        self.mac_gateway = self.gateway_ucc.dev_get_parameter('ALid')
        debug('mac_gateway: {}'.format(self.mac_gateway))
        self.mac_repeater1 = self.repeater1_ucc.dev_get_parameter('ALid')
        debug('mac_repeater1: {}'.format(self.mac_repeater1))
        self.mac_repeater2 = self.repeater2_ucc.dev_get_parameter('ALid')
        debug('mac_repeater2: {}'.format(self.mac_repeater2))

        mac_repeater1_wlan0_output = self.docker_command(
            self.repeater1, "ip", "-o", "link", "list", "dev", "wlan0")
        self.mac_repeater1_wlan0 = re.search(
            rb"link/ether " + RE_MAC, mac_repeater1_wlan0_output).group('mac').decode()
        debug("Repeater1 wl0: {}".format(self.mac_repeater1_wlan0))
        mac_repeater1_wlan2_output = self.docker_command(
            self.repeater1, "ip", "-o", "link", "list", "dev", "wlan2")
        self.mac_repeater1_wlan2 = re.search(
            rb"link/ether " + RE_MAC, mac_repeater1_wlan2_output).group('mac').decode()
        debug("Repeater1 wl2: {}".format(self.mac_repeater1_wlan2))

        mac_repeater2_wlan0_output = self.docker_command(
            self.repeater2, "ip", "-o", "link", "list", "dev", "wlan0")
        self.mac_repeater2_wlan0 = re.search(
            rb"link/ether " + RE_MAC, mac_repeater2_wlan0_output).group('mac').decode()
        debug("Repeater2 wl0: {}".format(self.mac_repeater2_wlan0))
        mac_repeater2_wlan2_output = self.docker_command(
            self.repeater2, "ip", "-o", "link", "list", "dev", "wlan2")
        self.mac_repeater2_wlan2 = re.search(
            rb"link/ether " + RE_MAC, mac_repeater2_wlan2_output).group('mac').decode()
        debug("Repeater2 wl2: {}".format(self.mac_repeater2_wlan2))

    def _check_log_internal(self, device: str, program: str, regex: str, start_line: int):
        '''Search for regex in logfile for program on device.'''
        logfilename = os.path.join(self.rootdir, 'logs', device, 'beerocks_{}.log'.format(program))

        # WSL doesn't support symlinks on NTFS, so resolve the symlink manually
        if self.on_wsl:
            logfilename = os.path.join(
                self.rootdir, 'logs', device,
                subprocess.check_output(["tail", "-2", logfilename]).
                decode('utf-8').rstrip(' \t\r\n\0'))

        with open(logfilename) as logfile:
            for (i, v) in enumerate(logfile.readlines()):
                if i <= start_line:
                    continue
                search = re.search(regex, v)
                if search:
                    debug("Found '{}'\n\tin {}".format(regex, logfilename))
                    return (True, i, search.groups())
        return (False, start_line, None)

    def check_log(self, device: str, program: str, regex: str, start_line=0):
        '''Verify that on "device" the logfile for "program" matches "regex", fail if not.'''
        try:
            # HACK check_log is often used immediately after triggering a message on the other side.
            # That message needs some time to arrive on the receiver. Since our python script is
            # pretty fast, we tend to check it too quickly. As a simple workaround, add a small
            # sleep here. The good solution is to retry with a small timeout.
            time.sleep(.1)
            result, line, value = self._check_log_internal(device, program, regex, start_line)
            if result:
                return True, line, value
            else:
                return \
                    self.fail("'{}'\n\tin log of {} on {}".format(
                        regex, program, device)), line, None
        except OSError:
            return self.fail("Can't read log of {} on {}".format(program, device)), line, None

    def wait_for_log(self, device: str, program: str, regex: str, timeout: float) -> bool:
        deadline = time.monotonic() + timeout
        try:
            while time.monotonic() < deadline:
                if self._check_log_internal(device, program, regex):
                    return True
                time.sleep(1)
        except OSError:
            return self.fail("Can't read log of {} on {}".format(program, device))
        return self.fail("'{}'\n\tin log of {} on {} after {}s".format(regex, program,
                                                                       device, timeout))

    def send_bwl_event(self, device: str, radio: str, event: str) -> None:
        """Send a bwl event `event` to `radio` on `device`."""
        # The file is only available within the docker container so we need to use an echo command.
        # Inside the container, $USER is set to the username that was used for starting it.
        command = "echo \"{event}\" > /tmp/$USER/beerocks/{radio}/EVENT".format(**locals())
        self.docker_command(device, 'sh', '-c', command)

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
        self.check_log(self.repeater1, "agent_wlan0", r"WSC Global authentication success")
        self.check_log(self.repeater1, "agent_wlan2", r"WSC Global authentication success")
        self.check_log(self.repeater1, "agent_wlan0", r"KWA \(Key Wrap Auth\) success")
        self.check_log(self.repeater1, "agent_wlan2", r"KWA \(Key Wrap Auth\) success")
        self.check_log(self.repeater1, "agent_wlan0",
                       r".* Controller configuration \(WSC M2 Encrypted Settings\)")
        self.check_log(self.repeater1, "agent_wlan2",
                       r".* Controller configuration \(WSC M2 Encrypted Settings\)")

    def test_ap_config_renew(self):
        # Regression test: MAC address should be case insensitive
        mac_repeater1_upper = self.mac_repeater1.upper()
        # Configure the controller and send renew
        self.gateway_ucc.cmd_reply("DEV_RESET_DEFAULT")
        self.gateway_ucc.cmd_reply(
            "DEV_SET_CONFIG,"
            "bss_info1,{} 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,"
            "bss_info2,{} 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0"
            .format(mac_repeater1_upper, self.mac_repeater1))
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x000A,
                                       tlv(0x01, 0x0006, "{" + self.mac_gateway + "}"),
                                       tlv(0x0F, 0x0001, "{0x00}"),
                                       tlv(0x10, 0x0001, "{0x00}"))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(self.repeater1, "agent_wlan0",
                       r"Received credentials for ssid: Multi-AP-24G-1 .* bss_type: 2")
        self.check_log(self.repeater1, "agent_wlan0",
                       r"Received credentials for ssid: Multi-AP-24G-2 .* bss_type: 1")
        self.check_log(self.repeater1, "agent_wlan2", r".* tear down radio")

        bssid1 = self.repeater1_ucc.dev_get_parameter('macaddr',
                                                      ruid='0x' +
                                                      self.mac_repeater1_wlan0.replace(':', ''),
                                                      ssid='Multi-AP-24G-1')
        if not bssid1:
            self.fail("repeater1 didn't configure Multi-AP-24G-1")

    def test_ap_config_bss_tear_down(self):
        # Configure the controller and send renew
        self.gateway_ucc.cmd_reply("DEV_RESET_DEFAULT")
        self.gateway_ucc.cmd_reply(
            "DEV_SET_CONFIG,bss_info1,"
            "{self.mac_repeater1} 8x Multi-AP-24G-3 0x0020 0x0008 maprocks1 0 1".format(self=self))
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x000A,
                                       tlv(0x01, 0x0006, "{" + self.mac_gateway + "}"),
                                       tlv(0x0F, 0x0001, "{0x00}"),
                                       tlv(0x10, 0x0001, "{0x00}"))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(self.repeater1, "agent_wlan0",
                       r"Received credentials for ssid: Multi-AP-24G-3 .* bss_type: 2")
        self.check_log(self.repeater1, "agent_wlan2", r".* tear down radio")
        conn_map = self.get_conn_map()
        repeater1 = conn_map[self.mac_repeater1]
        repeater1_wlan0 = repeater1.radios[self.mac_repeater1_wlan0]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid not in (b'Multi-AP-24G-3', b'N/A'):
                self.fail('Wrong SSID: {vap.ssid} instead of Multi-AP-24G-3'.format(vap=vap))
        repeater1_wlan2 = repeater1.radios[self.mac_repeater1_wlan2]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))

        # SSIDs have been removed for the CTT Agent1's front radio
        self.gateway_ucc.cmd_reply(
            "DEV_SET_CONFIG,bss_info1,{self.mac_repeater1} 8x".format(self=self))
        # Send renew message
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x000A,
                                       tlv(0x01, 0x0006, "{" + self.mac_gateway + "}"),
                                       tlv(0x0F, 0x0001, "{0x00}"),
                                       tlv(0x10, 0x0001, "{0x00}"))

        time.sleep(3)
        self.check_log(self.repeater1, "agent_wlan0", r".* tear down radio")
        conn_map = self.get_conn_map()
        repeater1 = conn_map[self.mac_repeater1]
        repeater1_wlan0 = repeater1.radios[self.mac_repeater1_wlan0]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))
        repeater1_wlan2 = repeater1.radios[self.mac_repeater1_wlan2]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap=vap))

    def test_channel_selection(self):
        debug("Send channel preference query")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8004)
        time.sleep(1)
        debug("Confirming channel preference query has been received on agent")
        self.check_log(self.repeater1, "agent_wlan0", "CHANNEL_PREFERENCE_QUERY_MESSAGE")
        self.check_log(self.repeater1, "agent_wlan2", "CHANNEL_PREFERENCE_QUERY_MESSAGE")

        debug("Send empty channel selection request")
        cs_req_mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1,
                                                    0x8006, tlv(0x00, 0x0000, "{}"))
        time.sleep(1)

        debug("Confirming channel selection request has been received on controller")
        self.check_log(
            self.gateway,
            "controller",
            r"CHANNEL_SELECTION_RESPONSE_MESSAGE, mid={}".format(cs_req_mid)
        )

        debug("Confirming empty channel selection request has been received on agent")
        self.check_log(self.repeater1, "agent_wlan0", "CHANNEL_SELECTION_REQUEST_MESSAGE")
        self.check_log(self.repeater1, "agent_wlan2", "CHANNEL_SELECTION_REQUEST_MESSAGE")

        debug("Confirming OPERATING_CHANNEL_REPORT_MESSAGE message has been received on \
            controller with mid")

        result, ocrm_line, mid_match = self.check_log(
            self.gateway, "controller",
            r'.+OPERATING_CHANNEL_REPORT_MESSAGE,\smid=(\d+)'
        )
        if (mid_match):
            debug("Confirming ACK_MESSAGE from the controller \
                with same mid as OPERATING_CHANNEL_REPORT_MESSAGE")
            self.check_log(
                self.repeater1,
                "agent_wlan0",
                "ACK_MESSAGE, mid={}".format(mid_match[0])
            )

            self.check_log(
                self.repeater1,
                "agent_wlan2",
                "ACK_MESSAGE, mid={}".format(mid_match[0])
            )

        tp20dBm = 0x14
        tp21dBm = 0x15

        for payload_transmit_power in (tp20dBm, tp21dBm):
            debug("Send empty channel selection request with changing tx_power_limit")
            cs_req_mid = self.gateway_ucc.dev_send_1905(
                self.mac_repeater1,
                0x8006,
                tlv(0x8D, 0x0007, '{} 0x{:02x}'.format(self.mac_repeater1_wlan0,
                                                       payload_transmit_power)),
                tlv(0x8D, 0x0007, '{} 0x{:02x}'.format(self.mac_repeater1_wlan2,
                                                       payload_transmit_power))
            )
            time.sleep(1)

            self.check_log(
                self.gateway,
                "controller",
                r"CHANNEL_SELECTION_RESPONSE_MESSAGE, mid={}".format(cs_req_mid)
            )

            self.check_log(self.repeater1, "agent_wlan0", "CHANNEL_SELECTION_REQUEST_MESSAGE")
            self.check_log(self.repeater1, "agent_wlan2", "CHANNEL_SELECTION_REQUEST_MESSAGE")

            self.check_log(self.repeater1, "agent_wlan0",
                           "tlvTransmitPowerLimit {}".format(payload_transmit_power))
            self.check_log(self.repeater1, "agent_wlan2",
                           "tlvTransmitPowerLimit {}".format(payload_transmit_power))

            self.check_log(self.gateway, "controller", "tx_power={}".format(payload_transmit_power))

            result, ocrm_line, mid_match = self.check_log(
                self.gateway, "controller",
                r'.+OPERATING_CHANNEL_REPORT_MESSAGE,\smid=(\d+)',
                ocrm_line
            )
            if (result):
                self.check_log(
                    self.repeater1,
                    "agent_wlan0",
                    "ACK_MESSAGE, mid={}".format(mid_match[0])
                )

                self.check_log(
                    self.repeater1,
                    "agent_wlan2",
                    "ACK_MESSAGE, mid={}".format(mid_match[0])
                )

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
            mid = self.gateway_ucc.dev_send_1905(
                self.mac_repeater1,
                0x8006,
                tlv(0x8B, 0x005F, '{} {}'.format(self.mac_repeater1_wlan0, payload_wlan0)),
                tlv(0x8D, 0x0007, '{} 0x{:2x}'.format(self.mac_repeater1_wlan0, tp20dBm)),
                tlv(0x8B, 0x004C, '{} {}'.format(self.mac_repeater1_wlan0, payload_wlan2)),
                tlv(0x8D, 0x0007, '{} 0x{:2x}'.format(self.mac_repeater1_wlan2, tp20dBm))
            )
            time.sleep(1)

            debug(
                "Confirming channel selection request has been received on controller,\
                    step {}".format(i))
            self.check_log(
                self.gateway,
                "controller",
                r"CHANNEL_SELECTION_RESPONSE_MESSAGE, mid={}".format(mid)
            )

            debug(
                "Confirming channel selection request has been received on agent,step {}".format(i))

            self.check_log(self.repeater1, "agent_wlan0", "CHANNEL_SELECTION_REQUEST_MESSAGE")
            self.check_log(self.repeater1, "agent_wlan2", "CHANNEL_SELECTION_REQUEST_MESSAGE")

            debug(
                "Confirming tlvTransmitPowerLimit has been received with correct value on \
                    agent, step {}".format(i))

            self.check_log(
                self.repeater1, "agent_wlan0", "tlvTransmitPowerLimit {}".format(tp20dBm))

            self.check_log(self.repeater1, "agent_wlan0",
                           "ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START")

            self.check_log(
                self.repeater1, "agent_wlan2", "tlvTransmitPowerLimit {}".format(tp20dBm))

            self.check_log(self.repeater1, "agent_wlan2",
                           "ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START")

            debug("Confirming CHANNEL_SELECTION_RESPONSE_MESSAGE has been received, \
                step {}".format(i))

            self.check_log(self.gateway, "controller", "CHANNEL_SELECTION_RESPONSE_MESSAGE")

            debug("Confirming OPERATING_CHANNEL_REPORT_MESSAGE has been received on \
                controller step {}".format(i))

            result, ocrm_line, mid_match = self.check_log(
                self.gateway, "controller",
                r'.+OPERATING_CHANNEL_REPORT_MESSAGE,\smid=(\d+)',
                ocrm_line)

            if (mid_match):
                self.check_log(
                    self.repeater1,
                    "agent_wlan0",
                    "ACK_MESSAGE, mid={}".format(mid_match[0]))

                self.check_log(
                    self.repeater1,
                    "agent_wlan2",
                    "ACK_MESSAGE, mid={}".format(mid_match[0]))

            debug("Confirming tx_power has been received with correct value on controller, \
                step {}".format(i))
            self.check_log(self.gateway, "controller", "tx_power={}".format(tp20dBm))

    def test_ap_capability_query(self):
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8001)
        time.sleep(1)

        debug("Confirming ap capability query has been received on agent")
        self.check_log(self.repeater1, "agent", "AP_CAPABILITY_QUERY_MESSAGE")

        debug("Confirming ap capability report has been received on controller")
        self.check_log(self.gateway, "controller", "AP_CAPABILITY_REPORT_MESSAGE")

    def test_link_metric_query(self):
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0005,
                                       tlv(0x08, 0x0002, "0x00 0x02"))
        time.sleep(1)

        debug("Confirming link metric query has been received on agent")
        self.check_log(self.repeater1, "agent", "Received LINK_METRIC_QUERY_MESSAGE")

        debug("Confirming link metric response has been received on controller")
        self.check_log(self.gateway, "controller", "Received LINK_METRIC_RESPONSE_MESSAGE")
        self.check_log(self.gateway, "controller", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.gateway, "controller", "Received TLV_RECEIVER_LINK_METRIC")

    def test_combined_infra_metrics(self):
        debug("Send AP Metrics query message to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x800B,
                                       tlv(0x93, 0x0007, "0x01 {%s}" % (self.mac_repeater1_wlan0)))
        self.check_log(self.repeater1, "agent_wlan0", "Received AP_METRICS_QUERY_MESSAGE")
        # TODO agent should send response autonomously, with same MID.
        # AP metrics TLV
        tlv1 = tlv(0x94, 0x000d, "{%s} 0x01 0x0002 0x01 0x1f2f3f" % (self.mac_repeater1_wlan0))
        # STA metrics TLV with no metrics
        tlv2 = tlv(0x96, 0x0007, "{55:44:33:22:11:00} 0x00")
        # STA metrics TLV for STA connected to this BSS
        tlv3 = tlv(0x96, 0x001a,
                   "{66:44:33:22:11:00} 0x01 {%s} 0x11223344 0x1a2a3a4a 0x1b2b3b4b 0x55" % self.mac_repeater1_wlan0)  # noqa E501
        # STA traffic stats TLV for same STA
        tlv4 = tlv(0xa2, 0x0022,
                   "{55:44:33:22:11:00} 0x10203040 0x11213141 0x12223242 0x13233343 0x14243444 0x15253545 0x16263646")  # noqa E501
        self.repeater1_ucc.dev_send_1905(self.mac_gateway, 0x800C, tlv1, tlv2, tlv3, tlv4)
        self.check_log(self.gateway, "controller", "Received AP_METRICS_RESPONSE_MESSAGE")

        debug("Send AP Metrics query message to agent 2")
        self.gateway_ucc.dev_send_1905(self.mac_repeater2, 0x800B,
                                       tlv(0x93, 0x0007, "0x01 {%s}" % self.mac_repeater2_wlan2))
        self.check_log(self.repeater2, "agent_wlan2", "Received AP_METRICS_QUERY_MESSAGE")
        # TODO agent should send response autonomously
        # Same as above but with different STA MAC addresses, different values and
        # skipping the empty one
        tlv1 = tlv(0x94, 0x000d, "{%s} 0x01 0x0002 0x01 0x1f2f3f" % (self.mac_repeater2_wlan2))
        tlv3 = tlv(0x96, 0x001a,
                   "{77:44:33:22:11:00} 0x01 {%s} 0x19293949 0x10203040 0x11213141 0x99" % self.mac_repeater2_wlan2)  # noqa E501
        tlv4 = tlv(0xa2, 0x0022,
                   "{77:44:33:22:11:00} 0xa0203040 0xa1213141 0xa2223242 0xa3233343 0xa4243444 0xa5253545 0xa6263646")  # noqa E501
        self.repeater2_ucc.dev_send_1905(self.mac_gateway, 0x800C, tlv1, tlv3, tlv4)
        self.check_log(self.gateway, "controller", "Received AP_METRICS_RESPONSE_MESSAGE")

        debug("Send 1905 Link metric query to agent 1 (neighbor gateway)")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0005,
                                       tlv(0x08, 0x0008, "0x01 {%s} 0x02" % self.mac_gateway))
        self.check_log(self.repeater1, "agent", "Received LINK_METRIC_QUERY_MESSAGE")
        self.check_log(self.gateway, "controller", "Received LINK_METRIC_RESPONSE_MESSAGE")
        self.check_log(self.gateway, "controller", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.gateway, "controller", "Received TLV_RECEIVER_LINK_METRIC")

        # Trigger combined infra metrics
        debug("Send Combined infrastructure metrics message to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8013)
        self.check_log(self.repeater1, "agent", "Received COMBINED_INFRASTRUCTURE_METRICS")
        self.check_log(self.repeater1, "agent", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.repeater1, "agent", "Received TLV_RECEIVER_LINK_METRIC")

    def test_client_capability_query(self):
        sta_mac1 = '00:00:00:11:00:22'
        sta_mac2 = '00:00:00:11:00:33'
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
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8009,
                                       tlv(0x90, 0x000C,
                                           '{mac_repeater1_wlan0} {sta_mac1}'
                                           .format(sta_mac1=sta_mac1,
                                                   mac_repeater1_wlan0=self.mac_repeater1_wlan0)))
        time.sleep(1)
        debug("Confirming client capability query has been received on agent")
        # check that both radio agents received it, in the future we'll add a check to verify which
        # radio the query was intended for.
        self.check_log(self.repeater1, "agent", r"CLIENT_CAPABILITY_QUERY_MESSAGE")

        debug("Confirming client capability report message has been received on controller")
        self.check_log(self.gateway, "controller", r"Received CLIENT_CAPABILITY_REPORT_MESSAGE")
        self.check_log(self.gateway, "controller",
                       r"Result Code= FAILURE, client MAC= {sta_mac1}, BSSID= {mac_repeater1_wlan0}"
                       .format(sta_mac1=sta_mac1,
                               mac_repeater1_wlan0=self.mac_repeater1_wlan0))

        debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0",
                            "EVENT AP-STA-CONNECTED {sta_mac2}".format(sta_mac2=sta_mac2))

        debug("Send client capability query for connected STA")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8009,
                                       tlv(0x90, 0x000C,
                                           '{mac_repeater1_wlan0} {sta_mac2}'
                                           .format(sta_mac2=sta_mac2,
                                                   mac_repeater1_wlan0=self.mac_repeater1_wlan0)))
        time.sleep(1)

        debug("Confirming client capability report message has been received on controller")
        self.check_log(self.gateway, "controller", r"Received CLIENT_CAPABILITY_REPORT_MESSAGE")
        self.check_log(self.gateway, "controller",
                       r"Result Code= SUCCESS, client MAC= {sta_mac2}, BSSID= {mac_repeater1_wlan0}"
                        .format(sta_mac2 = sta_mac2,
                                mac_repeater1_wlan0 = self.mac_repeater1_wlan0))        
        
        for line in association_frame.splitlines( ):
            self.check_log(self.gateway, "controller",
                       r"{line}"
                        .format(line = line))

    def test_client_association_dummy(self):
        sta_mac = "11:11:33:44:55:66"

        debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED {}".format(sta_mac))
        debug("Send client association control request to the chosen BSSID (UNBLOCK)")
        self.beerocks_cli_command('client_allow {} {}'.format(sta_mac, self.mac_repeater1_wlan2))
        time.sleep(1)

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater1, "agent_wlan2",
                       r"Got client allow request for {}".format(sta_mac))

        debug("Send client association control request to all other (BLOCK) ")
        self.beerocks_cli_command('client_disallow {} {}'.format(sta_mac, self.mac_repeater1_wlan0))
        time.sleep(1)

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater1, "agent_wlan0",
                       r"Got client disallow request for {}".format(sta_mac))

    def test_client_steering_mandate(self):
        debug("Send topology request to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0002)
        time.sleep(1)
        debug("Confirming topology query was received")
        self.check_log(self.repeater1, "agent", "TOPOLOGY_QUERY_MESSAGE")

        debug("Send topology request to agent 2")
        self.gateway_ucc.dev_send_1905(self.mac_repeater2, 0x0002)
        time.sleep(1)
        debug("Confirming topology query was received")
        self.check_log(self.repeater2, "agent", "TOPOLOGY_QUERY_MESSAGE")

        debug("Send Client Steering Request message for Steering Mandate to CTT Agent1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8014,
                                       tlv(0x9B, 0x001b,
                                           "{%s 0xe0 0x0000 0x1388 0x01 {0x000000110022} 0x01 {%s 0x73 0x24}}" % (self.mac_repeater1_wlan0, self.mac_repeater2_wlan0)))  # noqa E501
        time.sleep(1)
        debug("Confirming Client Steering Request message was received - mandate")
        self.check_log(self.repeater1, "agent_wlan0", "Got steer request")

        debug("Confirming BTM Report message was received")
        self.check_log(self.gateway, "controller", "CLIENT_STEERING_BTM_REPORT_MESSAGE")

        debug("Checking BTM Report source bssid")
        self.check_log(self.gateway, "controller", "BTM_REPORT from source bssid %s" %
                       self.mac_repeater1_wlan0)

        debug("Confirming ACK message was received")
        self.check_log(self.repeater1, "agent_wlan0", "ACK_MESSAGE")

        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8014,
                                       tlv(0x9B, 0x000C,
                                           "{%s 0x00 0x000A 0x0000 0x00}" % self.mac_repeater1_wlan0))  # noqa E501
        time.sleep(1)
        debug("Confirming Client Steering Request message was received - Opportunity")
        self.check_log(self.repeater1, "agent_wlan0", "CLIENT_STEERING_REQUEST_MESSAGE")

        debug("Confirming ACK message was received")
        self.check_log(self.gateway, "controller", "ACK_MESSAGE")

        debug("Confirming steering completed message was received")
        self.check_log(self.gateway, "controller", "STEERING_COMPLETED_MESSAGE")

        debug("Confirming ACK message was received")
        self.check_log(self.repeater1, "agent_wlan0", "ACK_MESSAGE")

    # Disabled because it sometimes fails (cfr. #711) and it has to be executed before any
    # autoconfig test
    def _test_optimal_path_dummy(self):
        # TODO make sure the same SSID is configured on each radio
        debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED 11:22:33:44:55:66")
        debug("Pre-prepare RRM Beacon Response for association handling task")
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10")  # noqa E501
        debug("Confirming 11k request is done by association handling task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:10 channel 1", 2)  # noqa E501

        debug("Update Stats")
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-38,-39,-40,-41 snr=38,39,40,41 uplink=1000 downlink=800")  # noqa E501
        debug("Pre-prepare RRM Beacon Responses for optimal path task")
        # Response for IRE1, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:10")  # noqa E501
        # Response for IRE1, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:20")  # noqa E501
        # Response for IRE2, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10")  # noqa E501
        # Response for IRE2, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:00:00:20")  # noqa E501
        # Response for GW, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:10")  # noqa E501
        # Response for GW, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:20")  # noqa E501
        debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:20 channel 149", 20)  # noqa E501

        debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:20 channel 149", 20)  # noqa E501

        debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:10 channel 1", 20)  # noqa E501

        debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:20 channel 149", 20)  # noqa E501

        debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:10 channel 1", 20)  # noqa E501

        debug("Confirming no steer is done")
        self.wait_for_log(self.gateway, "controller",
                          r"could not find a better path for sta 11:22:33:44:55:66", 20)

        # Steer scenario
        debug("Update Stats")
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-58,-59,-60,-61 snr=18,19,20,21 uplink=100 downlink=80")  # noqa E501
        debug("Pre-prepare RRM Beacon Responses for optimal path task")
        # Response for IRE1, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:10")  # noqa E501
        # Response for IRE1, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:20")  # noqa E501
        # Response for IRE2, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-60 rsni=20 bssid=aa:bb:cc:00:00:10")  # noqa E501
        # Response for IRE2, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:00:00:20")  # noqa E501
        # Response for GW, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:10")  # noqa E501
        # Response for GW, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:20")  # noqa E501
        debug("Confirming steering is requested by optimal path task")
        self.wait_for_log(self.gateway, "controller", r"optimal_path_task: steering", 20)

        # Error scenario, sta doesn't support 11k
        debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED 11:22:33:44:55:77")
        debug("Pre-prepare RRM Beacon Response with error for association handling task")
        self.send_bwl_event(self.repeater1, "wlan0",
                            "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:77 channel=0 dialog_token=0 measurement_rep_mode=4 op_class=0 duration=0 rcpi=0 rsni=0 bssid=aa:bb:cc:00:00:10")  # noqa E501
        debug("Confirming 11k request is done by association handling task")
        self.wait_for_log(self.repeater1, "monitor_wlan0",
                          r"Beacon 11k request to sta 11:22:33:44:55:77 on bssid aa:bb:cc:00:00:10 channel 1", 20)  # noqa E501

        debug("Confirming STA doesn't support beacon measurement")
        self.wait_for_log(self.gateway, "controller",
                          r"setting sta 11:22:33:44:55:77 as beacon measurement unsupported", 20)

        debug("Confirming optimal path falls back to RSSI measurements")
        self.wait_for_log(self.gateway, "controller",
                          r"requesting rssi measurements for 11:22:33:44:55:77", 20)

    def test_client_steering_dummy(self):
        sta_mac = "11:22:33:44:55:66"

        debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED {}".format(sta_mac))
        debug("Send steer request ")
        self.beerocks_cli_command("steer_client {} {}".format(sta_mac, self.mac_repeater1_wlan2))
        time.sleep(1)

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater1, "agent_wlan2", r"Got client allow request")

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater1, "agent_wlan0", r"Got client disallow request")

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater2, "agent_wlan0", r"Got client disallow request")

        debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater2, "agent_wlan2", r"Got client disallow request")

        debug("Confirming Client Steering Request message was received - mandate")
        self.check_log(self.repeater1, "agent_wlan0", r"Got steer request")

        debug("Confirming BTM Report message was received")
        self.check_log(self.gateway, "controller", r"CLIENT_STEERING_BTM_REPORT_MESSAGE")

        debug("Confirming ACK message was received")
        self.check_log(self.repeater1, "agent_wlan0", r"ACK_MESSAGE")

        debug("Disconnect dummy STA from wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-DISCONNECTED {}".format(sta_mac))
        # Make sure that controller sees disconnect before connect by waiting a little
        time.sleep(1)

        debug("Connect dummy STA to wlan2")
        self.send_bwl_event(self.repeater1, "wlan2", "EVENT AP-STA-CONNECTED {}".format(sta_mac))
        debug("Confirm steering success by client connected")
        self.check_log(self.gateway, "controller",
                       r"steering successful for sta {}".format(sta_mac))
        self.check_log(self.gateway, "controller",
                       r"sta {} disconnected due to steering request".format(sta_mac))

        # Make sure that all blocked agents send UNBLOCK messages at the end of
        # disallow period (default 25 sec)
        time.sleep(25)

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater1, "agent_wlan0", r"Got client allow request")

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater2, "agent_wlan0", r"Got client allow request")

        debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater2, "agent_wlan2", r"Got client allow request")

    def test_client_steering_policy(self):
        debug("Send client steering policy to agent 1")
        mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8003,
                                             tlv(0x89, 0x000C, "{0x00 0x00 0x01 {0x112233445566 0x01 0xFF 0x14}}"))  # noqa E501
        time.sleep(1)
        debug("Confirming client steering policy has been received on agent")

        self.check_log(self.repeater1, "agent_wlan0", r"MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE")
        time.sleep(1)
        debug("Confirming client steering policy ack message has been received on the controller")
        self.check_log(self.gateway, "controller", r"ACK_MESSAGE, mid=0x{:04x}".format(mid))

    def test_client_association(self):
        debug("Send topology request to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0002)
        debug("Confirming topology query was received")
        self.check_log(self.repeater1, "agent", r"TOPOLOGY_QUERY_MESSAGE")

        debug("Send client association control message")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8016,
                                       tlv(0x9D, 0x000F,
                                           "{%s 0x00 0x1E 0x01 {0x000000110022}}" % self.mac_repeater1_wlan0))  # noqa E501

        debug("Confirming client association control message has been received on agent")
        # check that both radio agents received it,in the future we'll add a check to verify which
        # radio the query was intended for.
        self.check_log(self.repeater1, "agent_wlan0", r"CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE")
        self.check_log(self.repeater1, "agent_wlan2", r"CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE")

        debug("Confirming ACK message was received on controller")
        self.check_log(self.gateway, "controller", r"ACK_MESSAGE")

    def test_higher_layer_data_payload_trigger(self):
        mac_gateway_hex = '0x' + self.mac_gateway.replace(':', '')
        debug("mac_gateway_hex = " + mac_gateway_hex)
        payload = 199 * (mac_gateway_hex + " ") + mac_gateway_hex

        debug("Send Higher Layer Data message")
        # MCUT sends Higher Layer Data message to CTT Agent1 by providing:
        # Higher layer protocol = "0x00"
        # Higher layer payload = 200 concatenated copies of the ALID of the MCUT (1200 octets)
        mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8018,
                                             tlv(0xA0, 0x04b1, "{0x00 %s}" % payload))

        debug("Confirming higher layer data message was received in the agent")

        self.check_log(self.repeater1, "agent", r"HIGHER_LAYER_DATA_MESSAGE")

        debug("Confirming matching protocol and payload length")

        self.check_log(self.repeater1, "agent", r"protocol: 0")
        self.check_log(self.repeater1, "agent", r"payload_length: 0x4b0")

        debug("Confirming ACK message was received in the controller")
        self.check_log(self.gateway, "controller", r"ACK_MESSAGE, mid=0x{:04x}".format(mid))

    def test_topology(self):
        mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0002)
        debug("Confirming topology query was received")
        self.check_log(self.repeater1, "agent", r"TOPOLOGY_QUERY_MESSAGE.*mid={:d}".format(mid))


if __name__ == '__main__':
    t = TestFlows()

    parser = argparse.ArgumentParser()
    parser.add_argument("--tcpdump", "-t", action='store_true', default=False,
                        help="capture the packets during each test")
    parser.add_argument("--verbose", "-v", action='store_true', default=False,
                        help="report each action")
    parser.add_argument("--stop-on-failure", "-s", action='store_true', default=False,
                        help="exit on the first failure")
    user = os.getenv("SUDO_USER", os.getenv("USER", ""))
    parser.add_argument("--unique-id", "-u", type=str, default=user,
                        help="append UNIQUE_ID to all container names, e.g. gateway-<UNIQUE_ID>; "
                             "defaults to {}".format(user))
    parser.add_argument("--skip-init", action='store_true', default=False,
                        help="don't start up the containers")
    parser.add_argument("tests", nargs='*',
                        help="tests to run; if not specified, run all tests: " + ", ".join(t.tests))
    options = parser.parse_args()

    unknown_tests = [test for test in options.tests if test not in t.tests]
    if unknown_tests:
        parser.error("Unknown tests: {}".format(', '.join(unknown_tests)))

    opts.verbose = options.verbose
    opts.tcpdump = options.tcpdump
    opts.tcpdump_dir = os.path.join(t.rootdir, 'logs')
    opts.stop_on_failure = options.stop_on_failure

    t.init(options.unique_id, options.skip_init)
    if t.run_tests(options.tests):
        sys.exit(1)
