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
import time
import json

import send_CAPI_command
from send_CAPI_command import tlv

'''Regular expression to match a MAC address in a bytes string.'''
RE_MAC = rb"(?P<mac>([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})"

class test_flows:
    def __init__(self):
        self.tests = [attr[len('test_'):] for attr in dir(self) if attr.startswith('test_')]

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
                            help="tests to run; if not specified, run all tests: " + ", ".join(self.tests))
        self.opts = parser.parse_args()

        if not self.opts.tests:
            self.opts.tests = self.tests

        unknown_tests = [test for test in self.opts.tests if test not in self.tests]
        if unknown_tests:
            parser.error("Unknown tests: {}".format(', '.join(unknown_tests)))

        self.rootdir = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..'))
        self.installdir = os.path.join(self.rootdir, 'build', 'install')
        self.running = ''
        self.tcpdump_proc = None
        self.bridge_name = 'br-lan'


    def message(self, message: str, color: int = 0):
        '''Print a message, optionally in a color, preceded by the currently running test.'''
        full_message = '{:20} {}'.format(self.running, message)
        if color:
            print('\x1b[1;{}m{}\x1b[0m'.format(color, full_message))
        else:
            print(full_message)

    def debug(self, message: str):
        '''Print a debug message if verbose is enabled.'''
        if self.opts.verbose:
            self.message(message)

    def status(self, message: str):
        '''Print a purple status message.'''
        self.message(message, 35)

    def err(self, message: str):
        '''Print a red error message.'''
        self.message(message, 31)

    def ok(self):
        '''Print a green OK message.'''
        self.message("OK", 32)

    def fail(self, message: str) -> bool:
        '''Print a red error message, increment failure count and return False.'''
        self.check_error += 1
        self.err('FAIL: {}'.format(message))
        if self.opts.stop_on_failure:
            sys.exit(1)
        return False

    def start_test(self, test: str):
        '''Call this at the beginning of a test.'''
        self.running = test
        self.status("starting")

    def tcpdump_start(self):
        '''Start tcpdump if enabled by config.'''
        if self.opts.tcpdump:
            outputfile = os.path.join(self.rootdir, 'logs', 'test_{}.pcap'.format(self.running))
            self.debug("Starting tcpdump, output file {}".format(outputfile))
            inspect = json.loads(subprocess.check_output(('docker', 'network', 'inspect',
                                                          'prplMesh-net-{}'.format(self.opts.unique_id))))
            prplmesh_net = inspect[0]
            # podman adds a 'plugins' indirection that docker doesn't have.
            if 'plugins' in prplmesh_net:
                bridge = prplmesh_net['plugins'][0]['bridge']
            else:
                # docker doesn't report the interface name of the bridge. So format it based on the
                # ID.
                bridge_id = prplmesh_net['Id']
                bridge = 'br-' + bridge_id[:12]
            self.tcpdump_proc = subprocess.Popen(["tcpdump", "-i", bridge, "-w", outputfile], stderr=subprocess.PIPE)
            # tcpdump takes a while to start up. Wait for the appropriate output before continuing.
            # poll() so we exit the loop if tcpdump terminates for any reason.
            while not self.tcpdump_proc.poll():
                line = self.tcpdump_proc.stderr.readline()
                self.debug(line.decode()[:-1]) # strip off newline
                if line.startswith(b"tcpdump: listening on " + bridge.encode()):
                    self.tcpdump_proc.stderr.close() # Make sure it doesn't block due to stderr buffering
                    break
            else:
                self.err("tcpdump terminated")
                self.tcpdump_proc = None

    def tcpdump_kill(self):
        '''Stop tcpdump if it is running.'''
        if self.tcpdump_proc:
            self.status("Terminating tcpdump")
            self.tcpdump_proc.terminate()
            self.tcpdump_proc = None

    def docker_command(self, device: str, *command: str) -> bytes:
        '''Execute `command` in docker container `device` and return its output.'''
        return subprocess.check_output(("docker", "exec", device) + command)

    def open_CAPI_socket(self, device: str, controller: bool = False) -> send_CAPI_command.UCCSocket:
        '''Open a CAPI socket to the agent (or controller, if set) on "device".'''
        # First, get the UCC port from the config file
        if controller:
            config_file_name = 'beerocks_controller.conf'
        else:
            config_file_name = 'beerocks_agent.conf'
        with open(os.path.join(self.installdir, 'config', config_file_name)) as config_file:
            ucc_port = re.search(r'ucc_listener_port=(?P<port>[0-9]+)', config_file.read()).group('port')

        device_ip_output = self.docker_command(device, 'ip', '-f', 'inet', 'addr', 'show', self.bridge_name)
        device_ip = re.search(r'inet (?P<ip>[0-9.]+)', device_ip_output.decode('utf-8')).group('ip')
        return send_CAPI_command.UCCSocket(device_ip, ucc_port)

    def init(self):
        '''Initialize the tests.'''
        self.start_test('init')
        self.gateway = 'gateway-' + self.opts.unique_id
        self.repeater1 = 'repeater1-' + self.opts.unique_id
        self.repeater2 = 'repeater2-' + self.opts.unique_id
        if not self.opts.skip_init:
            self.tcpdump_start()
            try:
                subprocess.check_call((os.path.join(self.rootdir, "tests", "test_gw_repeater.sh"),
                                       "-f", "-u", self.opts.unique_id, "-g", self.gateway,
                                       "-r", self.repeater1, "-r", self.repeater2, "-d", "7"))
            finally:
                self.tcpdump_kill()

        self.gateway_ucc = self.open_CAPI_socket(self.gateway, True)
        self.repeater1_ucc = self.open_CAPI_socket(self.repeater1)
        self.repeater2_ucc = self.open_CAPI_socket(self.repeater2)

        self.mac_gateway = self.gateway_ucc.dev_get_parameter('ALid')
        self.debug('mac_gateway: {}'.format(self.mac_gateway))
        self.mac_repeater1 = self.repeater1_ucc.dev_get_parameter('ALid')
        self.debug('mac_repeater1: {}'.format(self.mac_repeater1))
        self.mac_repeater2 = self.repeater2_ucc.dev_get_parameter('ALid')
        self.debug('mac_repeater2: {}'.format(self.mac_repeater2))

        mac_repeater1_wlan0_output = self.docker_command(self.repeater1, "ip", "-o",  "link", "list", "dev", "wlan0")
        self.mac_repeater1_wlan0 = re.search(rb"link/ether " + RE_MAC, mac_repeater1_wlan0_output).group('mac').decode()
        self.debug("Repeater1 wl0: {}".format(self.mac_repeater1_wlan0))
        mac_repeater1_wlan2_output = self.docker_command(self.repeater1, "ip", "-o",  "link", "list", "dev", "wlan2")
        self.mac_repeater1_wlan2 = re.search(rb"link/ether " + RE_MAC, mac_repeater1_wlan2_output).group('mac').decode()
        self.debug("Repeater1 wl2: {}".format(self.mac_repeater1_wlan2))

        mac_repeater2_wlan0_output = self.docker_command(self.repeater2, "ip", "-o",  "link", "list", "dev", "wlan0")
        self.mac_repeater2_wlan0 = re.search(rb"link/ether " + RE_MAC, mac_repeater2_wlan0_output).group('mac').decode()
        self.debug("Repeater2 wl0: {}".format(self.mac_repeater2_wlan0))
        mac_repeater2_wlan2_output = self.docker_command(self.repeater2, "ip", "-o",  "link", "list", "dev", "wlan2")
        self.mac_repeater2_wlan2 = re.search(rb"link/ether " + RE_MAC, mac_repeater2_wlan2_output).group('mac').decode()
        self.debug("Repeater2 wl2: {}".format(self.mac_repeater2_wlan2))

    def check_log(self, device: str, program: str, regex: str) -> bool:
        '''Verify that on "device" the logfile for "program" matches "regex", fail if not.'''
        logfilename = os.path.join(self.rootdir, 'logs', device, 'beerocks_{}.log'.format(program))
        try:
            # HACK check_log is often used immediately after triggering a message on the other side.
            # That message needs some time to arrive on the receiver. Since our python script is pretty fast,
            # we tend to check it too quickly. As a simple workaround, add a small sleep here.
            # The good solution is to retry with a small timeout.
            time.sleep(.1)
            with open(logfilename) as logfile:
                for line in logfile.readlines():
                    if re.search(regex, line):
                        self.debug("Found '{}'\n\tin {}".format(regex, logfilename))
                        return True
        except OSError:
            return self.fail("Can't read {}".format(logfilename))
        return self.fail("'{}'\n\tin log of {} on {}".format(regex, program, device))

    def run_tests(self):
        '''Run all tests as specified on the command line.'''
        total_errors = 0
        for test in self.opts.tests:
            self.start_test(test)
            self.tcpdump_start()
            self.check_error = 0
            try:
                getattr(self, 'test_' + test)()
            finally:
                self.tcpdump_kill()
            if self.check_error != 0:
                self.err("failed")
            else:
                self.ok()
            total_errors += self.check_error
        return total_errors

    # TEST DEFINITIONS #

    def test_initial_ap_config(self):
        '''Check initial configuration on repeater1.'''
        self.check_log(self.repeater1, "agent_wlan0", r"WSC Global authentication success")
        self.check_log(self.repeater1, "agent_wlan2", r"WSC Global authentication success")
        self.check_log(self.repeater1, "agent_wlan0", r"KWA \(Key Wrap Auth\) success")
        self.check_log(self.repeater1, "agent_wlan2", r"KWA \(Key Wrap Auth\) success")
        self.check_log(self.repeater1, "agent_wlan0", r".* Controller configuration \(WSC M2 Encrypted Settings\)")
        self.check_log(self.repeater1, "agent_wlan2", r".* Controller configuration \(WSC M2 Encrypted Settings\)")

    def test_ap_config_renew(self):
        # Regression test: MAC address should be case insensitive
        mac_repeater1_upper = self.mac_repeater1.upper()
        # Configure the controller and send renew
        self.gateway_ucc.cmd_reply("DEV_RESET_DEFAULT")
        self.gateway_ucc.cmd_reply(
            "DEV_SET_CONFIG,"
                "bss_info1,{mac_repeater1_upper} 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,"
                "bss_info2,{self.mac_repeater1} 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0".format(**locals()))
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x000A,
            tlv(0x01, 0x0006, "{" + self.mac_gateway + "}"),
            tlv(0x0F, 0x0001, "{0x00}"),
            tlv(0x10, 0x0001, "{0x00}"))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(self.repeater1, "agent_wlan0", r"Received credentials for ssid: Multi-AP-24G-1 .* bss_type: 2")
        self.check_log(self.repeater1, "agent_wlan0", r"Received credentials for ssid: Multi-AP-24G-2 .* bss_type: 1")
        self.check_log(self.repeater1, "agent_wlan2", r".* tear down radio")

        bssid1 = self.repeater1_ucc.dev_get_parameter('macaddr',
                                                      ruid = '0x' + self.mac_repeater1_wlan0.replace(':', ''),
                                                      ssid = 'Multi-AP-24G-1')
        if not bssid1:
            self.fail("repeater1 didn't configure Multi-AP-24G-1")

    def test_ap_config_bss_tear_down(self):
        # Configure the controller and send renew
        self.gateway_ucc.cmd_reply("DEV_RESET_DEFAULT")
        self.gateway_ucc.cmd_reply(
            "DEV_SET_CONFIG,bss_info1,"
                "{self.mac_repeater1} 8x Multi-AP-24G-3 0x0020 0x0008 maprocks1 0 1".format(self = self))
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x000A,
            tlv(0x01, 0x0006, "{" + self.mac_gateway + "}"),
            tlv(0x0F, 0x0001, "{0x00}"),
            tlv(0x10, 0x0001, "{0x00}"))

        # Wait a bit for the renew to complete
        time.sleep(3)

        self.check_log(self.repeater1, "agent_wlan0", r"Received credentials for ssid: Multi-AP-24G-3 .* bss_type: 2")
        self.check_log(self.repeater1, "agent_wlan2", r".* tear down radio")
        # TODO check that conn map was updated on controller

        # SSIDs have been removed for the CTT Agent1's front radio
        self.gateway_ucc.cmd_reply(
            "DEV_SET_CONFIG,bss_info1,{self.mac_repeater1} 8x".format(self = self))
        # Send renew message
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x000A,
            tlv(0x01, 0x0006, "{" + self.mac_gateway + "}"),
            tlv(0x0F, 0x0001, "{0x00}"),
            tlv(0x10, 0x0001, "{0x00}"))

        time.sleep(3)
        self.check_log(self.repeater1, "agent_wlan0", r".* tear down radio")
        # TODO check that conn map was updated on controller

    def test_channel_selection(self):
        self.debug("Send channel preference query")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8004)
        time.sleep(1)
        self.debug("Confirming channel preference query has been received on agent")
        self.check_log(self.repeater1, "agent_wlan0", "CHANNEL_PREFERENCE_QUERY_MESSAGE")
        self.check_log(self.repeater1, "agent_wlan2", "CHANNEL_PREFERENCE_QUERY_MESSAGE")

        self.debug("Send channel selection request")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8006)
        time.sleep(1)
        self.debug("Confirming channel selection request has been received on agent")
        self.check_log(self.repeater1, "agent_wlan0", "CHANNEL_SELECTION_REQUEST_MESSAGE")
        self.check_log(self.repeater1, "agent_wlan2", "CHANNEL_SELECTION_REQUEST_MESSAGE")

        #self.debug("Confirming 1905.1 Ack Message request was received on agent")
        # TODO: When creating handler for the ACK message on the agent, replace lookup of this string
        # TODO: currently controller sends empty channel selection request, so no switch is performed
        # self.check_log(self.repeater1, "agent_wlan0", "ACK_MESSAGE")
        # self.check_log(self.repeater1, "agent_wlan2", "ACK_MESSAGE")

    def test_ap_capability_query(self):
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8001)
        time.sleep(1)

        self.debug("Confirming ap capability query has been received on agent")
        self.check_log(self.repeater1, "agent", "AP_CAPABILITY_QUERY_MESSAGE")

        self.debug("Confirming ap capability report has been received on controller")
        self.check_log(self.gateway, "controller", "AP_CAPABILITY_REPORT_MESSAGE")

    def test_combined_infra_metrics(self):
        self.debug("Send AP Metrics query message to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x800B,
                                       tlv(0x93,0x0007,"0x01 {%s}" % (self.mac_repeater1_wlan0)))
        self.check_log(self.repeater1, "agent_wlan0", "Received AP_METRICS_QUERY_MESSAGE")
        # TODO agent should send response autonomously, with same MID.
        # tlv1 == AP metrics TLV
        # tlv2 == STA metrics TLV with no metrics
        # tlv3 == STA metrics TLV for STA connected to this BSS
        # tlv4 == STA traffic stats TLV for same STA
        self.repeater1_ucc.dev_send_1905(self.mac_gateway, 0x800C,
            tlv(0x94,0x000d,"{%s} 0x01 0x0002 0x01 0x1f2f3f" % (self.mac_repeater1_wlan0)),
            tlv(0x96,0x0007,"{55:44:33:22:11:00} 0x00"),
            tlv(0x96,0x001a,"{66:44:33:22:11:00} 0x01 {%s} 0x11223344 0x1a2a3a4a 0x1b2b3b4b 0x55" %
                self.mac_repeater1_wlan0),
            tlv(0xa2,0x0022,"{55:44:33:22:11:00} 0x10203040 0x11213141 0x12223242 0x13233343 "
                "0x14243444 0x15253545 0x16263646"))
        self.check_log(self.gateway, "controller", "Received AP_METRICS_RESPONSE_MESSAGE")

        self.debug("Send AP Metrics query message to agent 2")
        self.gateway_ucc.dev_send_1905(self.mac_repeater2, 0x800B,
                                       tlv(0x93,0x0007,"0x01 {%s}" % self.mac_repeater2_wlan2))
        self.check_log(self.repeater2, "agent_wlan2", "Received AP_METRICS_QUERY_MESSAGE")
        # TODO agent should send response autonomously
        # Same as above but with different STA MAC addresses, different values and skipping the empty one
        self.repeater2_ucc.dev_send_1905(self.mac_gateway, 0x800C,
            tlv(0x94,0x0010,"{%s} 0x11 0x1002 0x90 0x1c2c3c 0x1d2d3d" % self.mac_repeater2_wlan2),
            tlv(0x96,0x001a,"{77:44:33:22:11:00} 0x01 {%s} 0x19293949 0x10203040 0x11213141 0x99" % self.mac_repeater2_wlan2),
            tlv(0xa2,0x0022,"{77:44:33:22:11:00} 0xa0203040 0xa1213141 0xa2223242 0xa3233343 "
                         "0xa4243444 0xa5253545 0xa6263646"))
        self.check_log(self.gateway, "controller", "Received AP_METRICS_RESPONSE_MESSAGE")

        self.debug("Send 1905 Link metric query to agent 1 (neighbor agent 2)")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0005,
                                       tlv(0x08,0x0008,"0x01 {%s} 0x02" % self.mac_repeater2))
        self.check_log(self.repeater1, "agent_wlan0", "Received LINK_METRIC_QUERY_MESSAGE")
        # TODO agent should send response autonomously
        self.repeater1_ucc.dev_send_1905(self.mac_gateway, 0x6,
            tlv(0x09,0x0029,"{%s} {%s} {%s} {%s} 0x0100 0x01 0x00000000 0x0000e300 0x4230 0x0064 0x0300" %
                (self.mac_repeater1, self.mac_repeater2, self.mac_repeater1_wlan0, self.mac_repeater2_wlan2)),
            tlv(0x0a,0x0023,"{%s} {%s} {%s} {%s} 0x0100 0x00000007 0x00020000 0x31" %
                (self.mac_repeater1, self.mac_repeater2, self.mac_repeater1, self.mac_repeater2)))
        self.check_log(self.gateway, "controller", "Received LINK_METRIC_RESPONSE_MESSAGE")

        # Trigger combined infra metrics
        self.debug("Send Combined infrastructure metrics message to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8013)
        self.check_log(self.repeater1, "agent", "Received COMBINED_INFRASTRUCTURE_METRICS")
        self.check_log(self.repeater1, "agent", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.repeater1, "agent", "Received TLV_RECEIVER_LINK_METRIC")

    def test_client_steering_mandate(self):
        self.debug("Send topology request to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0002)
        time.sleep(1)
        self.debug("Confirming topology query was received")
        self.check_log(self.repeater1, "agent", "TOPOLOGY_QUERY_MESSAGE")

        self.debug("Send topology request to agent 2")
        self.gateway_ucc.dev_send_1905(self.mac_repeater2, 0x0002)
        time.sleep(1)
        self.debug("Confirming topology query was received")
        self.check_log(self.repeater2, "agent", "TOPOLOGY_QUERY_MESSAGE")

        self.debug("Send Client Steering Request message for Steering Mandate to CTT Agent1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8014,
            tlv(0x9B, 0x001b, "{%s 0xe0 0x0000 0x1388 0x01 {0x000000110022} 0x01 {%s 0x73 0x24}}" %
                (self.mac_repeater1_wlan0, self.mac_repeater2_wlan0)))
        time.sleep(1)
        self.debug("Confirming Client Steering Request message was received - mandate")
        self.check_log(self.repeater1, "agent_wlan0", "Got steer request")

        self.debug("Confirming BTM Report message was received")
        self.check_log(self.gateway, "controller", "CLIENT_STEERING_BTM_REPORT_MESSAGE")

        self.debug("Checking BTM Report source bssid")
        self.check_log(self.gateway, "controller", "BTM_REPORT from source bssid %s" % self.mac_repeater1_wlan0)

        self.debug("Confirming ACK message was received")
        self.check_log(self.repeater1, "agent_wlan0", "ACK_MESSAGE")

        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8014,
            tlv(0x9B, 0x000C, "{%s 0x00 0x000A 0x0000 0x00}" % self.mac_repeater1_wlan0))
        time.sleep(1)
        self.debug("Confirming Client Steering Request message was received - Opportunity")
        self.check_log(self.repeater1, "agent_wlan0", "CLIENT_STEERING_REQUEST_MESSAGE")

        self.debug("Confirming ACK message was received")
        self.check_log(self.gateway, "controller", "ACK_MESSAGE")

        self.debug("Confirming steering completed message was received")
        self.check_log(self.gateway, "controller", "STEERING_COMPLETED_MESSAGE")

        self.debug("Confirming ACK message was received")
        self.check_log(self.repeater1, "agent_wlan0", "ACK_MESSAGE")

if __name__ == '__main__':
    t = test_flows()
    t.init()
    if t.run_tests():
        sys.exit(1)
