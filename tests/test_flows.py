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
import json

import send_CAPI_command

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
            bridge = inspect[0]['plugins'][0]['bridge']
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


if __name__ == '__main__':
    t = test_flows()
    t.init()
    if t.run_tests():
        sys.exit(1)
