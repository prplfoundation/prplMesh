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
from typing import Dict
import json
from threading import Thread
import send_CAPI_command
from send_CAPI_command import tlv
import signal
from datetime import datetime
import atexit

'''Regular expression to match a MAC address in a bytes string.'''
RE_MAC = rb"(?P<mac>([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})"

class map_vap:
    '''Represents a VAP in the connection map.'''
    def __init__(self, bssid: str, ssid: bytes):
        self.bssid = bssid
        self.ssid = ssid

class map_radio:
    '''Represents a radio in the connection map.'''
    def __init__(self, uid: str):
        self.uid = uid
        self.vaps = {}

    def add_vap(self, bssid: str, ssid: bytes):
        vap = map_vap(bssid, ssid)
        self.vaps[bssid] = vap
        return vap

class map_device:
    '''Represents a device in the connection map.'''
    def __init__(self, mac: str):
        self.mac = mac
        self.radios = {}

    def add_radio(self, uid: str):
        radio = map_radio(uid)
        self.radios[uid] = radio
        return radio

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

    def get_bridge_mac(self):
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
        return bridge

    def start_tshark(self, bridge, duration=None, reset=False):
        self.tshark_instances = {}
        self.tshark_capture = {}
        if reset:
            for t in self.tshark_instances:
                try:
                    t.captured_packets = []
                finally:
                    pass
        for test_index, test_name in enumerate(self.opts.tests):
            self.tshark_instances[test_name] = Thread(
                target=self.__capture, daemon=True, args=(test_index, test_name, bridge, duration,))

        for test_name, test_thread in self.tshark_instances.items():
            test_thread.start()

    def __capture(self, test_index, test_name, bridge, duration=None):
        
        # `test_index` currently unused, but might be useful for file output
        
        # for optional file output
        # self.outputfile = os.path.join(
        #     self.rootdir, 'logs', f'tshark_log_{test_name}.json')
        
        try:
            tshark_args = ['tshark', '-Q', '-i', bridge, '-T', 'json']
            if duration:
                self.tshark_instances[test_name].proc = subprocess.Popen(
                    tshark_args+['-a', f'duration:{duration}'], shell=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            else:
                self.tshark_instances[test_name].proc = subprocess.Popen(
                    tshark_args+['-a', 'duration:900'], shell=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        except subprocess.CalledProcessError as e:
            self.err(f'test {self.run_tests} failed, output: {e.output}')
    def check_tshark(self, target_thread_name=None, end=time.time(), start=None, file_output=False):
        '''
        ### Description
        Looks into the tshark captured data, updating the global capture data
        and returning the captured data from `start` to `end`

        ### Parameters
        `target_thread_name=None` (str) dictionary key (in self.tshark_instances) of the thread running tshark, if None, the method
        defaults to self.running (i.e. the current running test)

        `end=time.time()` (datetime) upper bound for captured packets timestamp

        `start=None` (datetime) lower bound for captured packets timestamp

        `file_output=False` flag to determine if the captured data would be 
        written to a file

        ### Returns

        a collection of captured `ieee1905` packets
        '''
        # if file_output:
        #     with open(self.outputfile, "w+") as f:
        #         f.write(f'\n{time.time()} - test {self.running}\n')
        if not target_thread_name:
            tshark_proc = self.tshark_instances[self.running].proc
        else:
            tshark_proc = self.tshark_instances[target_thread_name].proc
        os.kill(tshark_proc.pid, signal.SIGTERM)
        raw_stream = tshark_proc.communicate()
        stream = json.loads(raw_stream[0])
        tshark_dump = filter(lambda x: self.check_time_in_bounds(
            self.get_time_for_packet(x), end, start), stream)
        temp_capture = list(filter(
            lambda x: "ieee1905" in x["_source"]["layers"], tshark_dump))
        if target_thread_name:
            self.tshark_capture[target_thread_name] = temp_capture
        else:
            self.tshark_capture[self.running] = temp_capture
        return temp_capture

    def check_time_in_bounds(self, timestamp, end=time.time(), start=None):
        if start:
            return start < timestamp and timestamp < end
        return timestamp < end

    def get_time_for_packet(self, packet):
        return float(packet['_source']['layers']['frame']['frame.time_epoch'])
        # return datetime.fromtimestamp(float(packet_time))

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

    def beerocks_cli_command(self, command: str) -> bytes:
        '''Execute `command` beerocks_cli command on the controller and return its output.'''
        self.debug("Send CLI command " + command)
        res = self.docker_command(self.gateway,
                                   os.path.join(self.installdir, "bin", "beerocks_cli"),
                                   "-c",
                                   command)
        self.debug("  Response: " + res.decode('utf-8', errors='replace').strip())
        return res

    def get_conn_map(self) -> Dict[str, map_device]:
        '''Get the connection map from the controller.'''
        conn_map = {}
        for line in self.beerocks_cli_command("bml_conn_map").split(b'\n'):
            # TODO we need to parse indentation to get the exact topology.
            # For the time being, just parse the repeaters.
            bridge = re.search(rb' {8}IRE_BRIDGE: .* mac: ' + RE_MAC, line)
            radio = re.match(rb' {16}RADIO: .* mac: ' + RE_MAC, line)
            vap = re.match(rb' {20}fVAP.* bssid: ' + RE_MAC + rb', ssid: (?P<ssid>.*)$', line)
            if bridge:
                cur_agent = map_device(bridge.group('mac').decode('utf-8'))
                conn_map[cur_agent.mac] = cur_agent
            elif radio:
                cur_radio = cur_agent.add_radio(radio.group('mac').decode('utf-8'))
            elif vap:
                cur_radio.add_vap(vap.group('mac').decode('utf-8'), vap.group('ssid'))
        return conn_map

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
        self.start_tshark(self.get_bridge_mac(), reset=True)
        self.status('Starting tshark processes, wait 10s...')
        time.sleep(10)
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

    def _check_log_internal(self, device: str, program: str, regex: str) -> bool:
        '''Search for regex in logfile for program on device.'''
        logfilename = os.path.join(self.rootdir, 'logs', device, 'beerocks_{}.log'.format(program))
        with open(logfilename) as logfile:
            for line in logfile.readlines():
                if re.search(regex, line):
                    self.debug("Found '{}'\n\tin {}".format(regex, logfilename))
                    return True
        return False

    def check_log(self, device: str, program: str, regex: str) -> bool:
        '''Verify that on "device" the logfile for "program" matches "regex", fail if not.'''
        try:
            # HACK check_log is often used immediately after triggering a message on the other side.
            # That message needs some time to arrive on the receiver. Since our python script is pretty fast,
            # we tend to check it too quickly. As a simple workaround, add a small sleep here.
            # The good solution is to retry with a small timeout.
            time.sleep(.1)
            if self._check_log_internal(device, program, regex):
                return True
            else:
                return self.fail("'{}'\n\tin log of {} on {}".format(regex, program, device))
        except OSError:
            return self.fail("Can't read log of {} on {}".format(program, device))

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
        conn_map = self.get_conn_map()
        repeater1 = conn_map[self.mac_repeater1]
        repeater1_wlan0 = repeater1.radios[self.mac_repeater1_wlan0]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid not in (b'Multi-AP-24G-3', b'N/A'):
                self.fail('Wrong SSID: {vap.ssid} instead of Multi-AP-24G-3'.format(vap = vap))
        repeater1_wlan2 = repeater1.radios[self.mac_repeater1_wlan2]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap = vap))

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
        conn_map = self.get_conn_map()
        repeater1 = conn_map[self.mac_repeater1]
        repeater1_wlan0 = repeater1.radios[self.mac_repeater1_wlan0]
        for vap in repeater1_wlan0.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap = vap))
        repeater1_wlan2 = repeater1.radios[self.mac_repeater1_wlan2]
        for vap in repeater1_wlan2.vaps.values():
            if vap.ssid != b'N/A':
                self.fail('Wrong SSID: {vap.ssid} instead torn down'.format(vap = vap))

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

    def test_link_metric_query(self):
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0005,
                                       tlv(0x08,0x0002,"0x00 0x02"))
        time.sleep(1)

        self.debug("Confirming link metric query has been received on agent")
        self.check_log(self.repeater1, "agent", "Received LINK_METRIC_QUERY_MESSAGE")

        self.debug("Confirming link metric response has been received on controller")
        self.check_log(self.gateway, "controller", "Received LINK_METRIC_RESPONSE_MESSAGE")
        self.check_log(self.gateway, "controller", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.gateway, "controller", "Received TLV_RECEIVER_LINK_METRIC")

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

        self.debug("Send 1905 Link metric query to agent 1 (neighbor gateway)")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0005,
                                       tlv(0x08,0x0008,"0x01 {%s} 0x02" % self.mac_gateway))
        self.check_log(self.repeater1, "agent", "Received LINK_METRIC_QUERY_MESSAGE")
        self.check_log(self.gateway, "controller", "Received LINK_METRIC_RESPONSE_MESSAGE")
        self.check_log(self.gateway, "controller", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.gateway, "controller", "Received TLV_RECEIVER_LINK_METRIC")

        # Trigger combined infra metrics
        self.debug("Send Combined infrastructure metrics message to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8013)
        self.check_log(self.repeater1, "agent", "Received COMBINED_INFRASTRUCTURE_METRICS")
        self.check_log(self.repeater1, "agent", "Received TLV_TRANSMITTER_LINK_METRIC")
        self.check_log(self.repeater1, "agent", "Received TLV_RECEIVER_LINK_METRIC")

    def test_client_capability_query(self):
        sta_mac1 = '00:00:00:11:00:22'
        sta_mac2 = '00:00:00:11:00:33'

        self.debug("Send client capability query for unconnected STA")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8009,
                                       tlv(0x90,0x000C,
                                           '{mac_repeater1_wlan0} {sta_mac1}'
                                            .format(sta_mac1 = sta_mac1,
                                                    mac_repeater1_wlan0 = self.mac_repeater1_wlan0)))
        time.sleep(1)
        self.debug("Confirming client capability query has been received on agent")
        # check that both radio agents received it, in the future we'll add a check to verify which
        # radio the query was intended for.
        self.check_log(self.repeater1, "agent", r"CLIENT_CAPABILITY_QUERY_MESSAGE")

        self.debug("Confirming client capability report message has been received on controller")
        self.check_log(self.gateway, "controller", r"Received CLIENT_CAPABILITY_REPORT_MESSAGE")
        self.check_log(self.gateway, "controller",
                       r"Result Code= FAILURE, client MAC= {sta_mac1}, BSSID= {mac_repeater1_wlan0}"
                        .format(sta_mac1 = sta_mac1,
                                mac_repeater1_wlan0 = self.mac_repeater1_wlan0))

        self.debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0",
                            "EVENT AP-STA-CONNECTED {sta_mac2}".format(sta_mac2 = sta_mac2))

        self.debug("Send client capability query for connected STA")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8009,
                                       tlv(0x90,0x000C,
                                           '{mac_repeater1_wlan0} {sta_mac2}'
                                            .format(sta_mac2 = sta_mac2,
                                                    mac_repeater1_wlan0 = self.mac_repeater1_wlan0)))
        time.sleep(1)

        self.debug("Confirming client capability report message has been received on controller")
        self.check_log(self.gateway, "controller", r"Received CLIENT_CAPABILITY_REPORT_MESSAGE")
        self.check_log(self.gateway, "controller",
                       r"Result Code= SUCCESS, client MAC= {sta_mac2}, BSSID= {mac_repeater1_wlan0}"
                        .format(sta_mac2 = sta_mac2,
                                mac_repeater1_wlan0 = self.mac_repeater1_wlan0))

    def test_client_association_dummy(self):
        sta_mac = "11:11:33:44:55:66"

        self.debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED {}".format(sta_mac))
        self.debug("Send client association control request to the chosen BSSID to steer the client (UNBLOCK) ")
        self.beerocks_cli_command('client_allow {} {}'.format(sta_mac, self.mac_repeater1_wlan2))
        time.sleep(1)

        self.debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater1, "agent_wlan2",
                       r"Got client allow request for {}".format(sta_mac))

        self.debug("Send client association control request to all other (BLOCK) ")
        self.beerocks_cli_command('client_disallow {} {}'.format(sta_mac, self.mac_repeater1_wlan0))
        time.sleep(1)

        self.debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater1, "agent_wlan0",
                       r"Got client disallow request for {}".format(sta_mac))

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

    # Disabled because it sometimes fails (cfr. #711) and it has to be executed before any
    # autoconfig test
    def _test_optimal_path_dummy(self):
        # TODO make sure the same SSID is configured on each radio
        self.debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED 11:22:33:44:55:66")
        self.debug("Pre-prepare RRM Beacon Response for association handling task")
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10")
        self.debug("Confirming 11k request is done by association handling task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:10 channel 1", 2)

        self.debug("Update Stats")
        self.send_bwl_event(self.repeater1, "wlan0", "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-38,-39,-40,-41 snr=38,39,40,41 uplink=1000 downlink=800")
        self.debug("Pre-prepare RRM Beacon Responses for optimal path task")
        #Response for IRE1, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:10")
        #Response for IRE1, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:20")
        #Response for IRE2, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10")
        #Response for IRE2, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:00:00:20")
        #Response for GW, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:10")
        #Response for GW, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:20")
        self.debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:20 channel 149", 20)

        self.debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:20 channel 149", 20)

        self.debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:10 channel 1", 20)

        self.debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:20 channel 149", 20)

        self.debug("Confirming 11k request is done by optimal path task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:10 channel 1", 20)

        self.debug("Confirming no steer is done")
        self.wait_for_log(self.gateway, "controller", r"could not find a better path for sta 11:22:33:44:55:66", 20)

        # Steer scenario
        self.debug("Update Stats")
        self.send_bwl_event(self.repeater1, "wlan0", "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-58,-59,-60,-61 snr=18,19,20,21 uplink=100 downlink=80")
        self.debug("Pre-prepare RRM Beacon Responses for optimal path task")
        #Response for IRE1, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:10")
        #Response for IRE1, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:20")
        #Response for IRE2, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-60 rsni=20 bssid=aa:bb:cc:00:00:10")
        #Response for IRE2, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:00:00:20")
        #Response for GW, BSSID of wlan0.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:10")
        #Response for GW, BSSID of wlan2.0
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:20")
        self.debug("Confirming steering is requested by optimal path task")
        self.wait_for_log(self.gateway, "controller", r"optimal_path_task: steering", 20)

        # Error scenario, sta doesn't support 11k
        self.debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED 11:22:33:44:55:77")
        self.debug("Pre-prepare RRM Beacon Response with error for association handling task")
        self.send_bwl_event(self.repeater1, "wlan0", "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:77 channel=0 dialog_token=0 measurement_rep_mode=4 op_class=0 duration=0 rcpi=0 rsni=0 bssid=aa:bb:cc:00:00:10")
        self.debug("Confirming 11k request is done by association handling task")
        self.wait_for_log(self.repeater1, "monitor_wlan0", r"Beacon 11k request to sta 11:22:33:44:55:77 on bssid aa:bb:cc:00:00:10 channel 1", 20)

        self.debug("Confirming STA doesn't support beacon measurement")
        self.wait_for_log(self.gateway, "controller", r"setting sta 11:22:33:44:55:77 as beacon measurement unsupported", 20)

        self.debug("Confirming optimal path falls back to RSSI measurements")
        self.wait_for_log(self.gateway, "controller", r"requesting rssi measurements for 11:22:33:44:55:77", 20)

    def test_client_steering_dummy(self):
        sta_mac = "11:22:33:44:55:66"

        self.debug("Connect dummy STA to wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-CONNECTED {}".format(sta_mac))
        self.debug("Send steer request ")
        self.beerocks_cli_command("steer_client {} {}".format(sta_mac, self.mac_repeater1_wlan2))
        time.sleep(1)

        self.debug("Confirming Client Association Control Request message was received (UNBLOCK)")
        self.check_log(self.repeater1, "agent_wlan2", r"Got client allow request")

        self.debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater1, "agent_wlan0", r"Got client disallow request")

        self.debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater2, "agent_wlan0", r"Got client disallow request")

        self.debug("Confirming Client Association Control Request message was received (BLOCK)")
        self.check_log(self.repeater2, "agent_wlan2", r"Got client disallow request")

        self.debug("Confirming Client Steering Request message was received - mandate")
        self.check_log(self.repeater1, "agent_wlan0", r"Got steer request")

        self.debug("Confirming BTM Report message was received")
        self.check_log(self.gateway, "controller", r"CLIENT_STEERING_BTM_REPORT_MESSAGE")

        self.debug("Confirming ACK message was received")
        self.check_log(self.repeater1, "agent_wlan0", r"ACK_MESSAGE")

        self.debug("Disconnect dummy STA from wlan0")
        self.send_bwl_event(self.repeater1, "wlan0", "EVENT AP-STA-DISCONNECTED {}".format(sta_mac))
        # Make sure that controller sees disconnect before connect by waiting a little
        time.sleep(1)

        self.debug("Connect dummy STA to wlan2")
        self.send_bwl_event(self.repeater1, "wlan2", "EVENT AP-STA-CONNECTED {}".format(sta_mac))
        self.debug("Confirm steering success by client connected")
        self.check_log(self.gateway, "controller", r"steering successful for sta {}".format(sta_mac))
        self.check_log(self.gateway, "controller", r"sta {} disconnected due to steering request".format(sta_mac))

    def test_client_steering_policy(self):
        self.debug("Send client steering policy to agent 1")
        mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8003,
            tlv(0x89, 0x000C, "{0x00 0x00 0x01 {0x112233445566 0x01 0xFF 0x14}}"))
        time.sleep(1)
        self.debug("Confirming client steering policy has been received on agent")

        self.check_log(self.repeater1, "agent_wlan0", r"MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE")
        time.sleep(1)
        self.debug("Confirming client steering policy ack message has been received on the controller")
        self.check_log(self.gateway, "controller", r"ACK_MESSAGE, mid=0x{:04x}".format(mid))

    def test_client_association(self):
        self.debug("Send topology request to agent 1")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0002)
        self.debug("Confirming topology query was received")
        self.check_log(self.repeater1, "agent", r"TOPOLOGY_QUERY_MESSAGE")

        self.debug("Send client association control message")
        self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8016,
            tlv(0x9D, 0x000F, "{%s 0x00 0x1E 0x01 {0x000000110022}}" % self.mac_repeater1_wlan0))

        self.debug("Confirming client association control message has been received on agent")
        # check that both radio agents received it,in the future we'll add a check to verify which
        # radio the query was intended for.
        self.check_log(self.repeater1, "agent_wlan0", r"CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE")
        self.check_log(self.repeater1, "agent_wlan2", r"CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE")

        self.debug("Confirming ACK message was received on controller")
        self.check_log(self.gateway, "controller", r"ACK_MESSAGE")

    def test_higher_layer_data_payload_trigger(self):
        mac_gateway_hex = '0x' + self.mac_gateway.replace(':', '')
        self.debug("mac_gateway_hex = " + mac_gateway_hex)
        payload = 199 * (mac_gateway_hex + " ") + mac_gateway_hex

        self.debug("Send Higher Layer Data message")
        # MCUT sends Higher Layer Data message to CTT Agent1 by providing:
        # Higher layer protocol = "0x00"
        # Higher layer payload = 200 concatenated copies of the ALID of the MCUT (1200 octets)
        mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x8018,
                                             tlv(0xA0, 0x04b1, "{0x00 %s}" % payload))

        self.debug("Confirming higher layer data message was received in the agent" )

        self.check_log(self.repeater1, "agent", r"HIGHER_LAYER_DATA_MESSAGE")

        self.debug("Confirming matching protocol and payload length")

        self.check_log(self.repeater1, "agent", r"protocol: 0")
        self.check_log(self.repeater1, "agent", r"payload_length: 0x4b0")

        self.debug("Confirming ACK message was received in the controller")
        self.check_log(self.gateway, "controller", r"ACK_MESSAGE, mid=0x{:04x}".format(mid))

    def test_topology(self):
        mid = self.gateway_ucc.dev_send_1905(self.mac_repeater1, 0x0002)
        self.debug("Confirming topology query was received")
        self.check_log(self.repeater1, "agent", r"TOPOLOGY_QUERY_MESSAGE")

    def on_exit(self):
        for name, thr in self.tshark_instances.items():
            try:
                os.kill(thr.proc.pid, signal.SIGTERM)
            finally:
                pass


if __name__ == '__main__':
    t = test_flows()
    atexit.register(t.on_exit)
    t.init()
    if t.run_tests():
        sys.exit(1)
