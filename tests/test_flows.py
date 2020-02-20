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
import sys


class test_flows:
    def __init__(self):
        self.tests = [attr[len('test_'):] for attr in dir(self) if attr.startswith('test_')]

        parser = argparse.ArgumentParser()
        parser.add_argument("--tcpdump", "-t", action='store_true', default=False,
                            help="capture the packets during each test - UNINMPLEMENTED")
        parser.add_argument("--verbose", "-v", action='store_true', default=False,
                            help="report each action")
        parser.add_argument("--stop-on-failure", "-s", action='store_true', default=False,
                            help="exit on the first failure")
        user = os.getenv("SUDO_USER", os.getenv("USER", ""))
        parser.add_argument("--unique-id", "-u", type=str, default=user,
                            help="append UNIQUE_ID to all container names, e.g. gateway-<UNIQUE_ID>; "
                                 "defaults to {}".format(user))
        parser.add_argument("--skip-init", action='store_true', default=False,
                            help="don't start up the containers - UNINMPLEMENTED")
        parser.add_argument("tests", nargs='*',
                            help="tests to run; if not specified, run all tests: " + ", ".join(self.tests))
        self.opts = parser.parse_args()

        if not self.opts.tests:
            self.opts.tests = self.tests

        unknown_tests = [test for test in self.opts.tests if test not in self.tests]
        if unknown_tests:
            parser.error("Unknown tests: {}".format(', '.join(unknown_tests)))

        self.rootdir = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..'))
        self.running = ''

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

    def init(self):
        '''Initialize the tests.'''
        self.start_test('init')
        self.gateway = 'gateway-' + self.opts.unique_id
        self.repeater1 = 'repeater1-' + self.opts.unique_id
        self.repeater2 = 'repeater2-' + self.opts.unique_id

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
            self.check_error = 0
            getattr(self, 'test_' + test)()
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
