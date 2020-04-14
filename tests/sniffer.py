###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import os
import json
import subprocess
from opts import debug, err, status


class Sniffer:
    '''Captures packets on an interface.'''
    def __init__(self, interface: str, tcpdump_log_dir: str):
        self.interface = interface
        self.tcpdump_log_dir = tcpdump_log_dir
        self.tcpdump_proc = None
        self.current_outputfile = None

    def start(self, outputfile_basename):
        '''Start tcpdump to outputfile.'''
        debug("Starting tcpdump, output file {}.pcap".format(outputfile_basename))
        os.makedirs(os.path.join(self.tcpdump_log_dir, 'logs'), exist_ok=True)
        self.current_outputfile = os.path.join(self.tcpdump_log_dir, outputfile_basename) + ".pcap"
        command = ["tcpdump", "-i", self.interface, '-U', '-w', self.current_outputfile]
        self.tcpdump_proc = subprocess.Popen(command, stderr=subprocess.PIPE)
        # tcpdump takes a while to start up. Wait for the appropriate output before continuing.
        # poll() so we exit the loop if tcpdump terminates for any reason.
        while not self.tcpdump_proc.poll():
            line = self.tcpdump_proc.stderr.readline()
            debug(line.decode()[:-1])  # strip off newline
            if line.startswith(b"tcpdump: listening on " + self.interface.encode()):
                # Make sure it doesn't block due to stderr buffering
                self.tcpdump_proc.stderr.close()
                break
        else:
            err("tcpdump terminated")
            self.tcpdump_proc = None
            self.current_outputfile = None

    def get_packet_capture(self):
        '''Get a list of packets from the last started tcpdump.'''
        if not self.current_outputfile:
            err("get_packet_capture but no capture file")
            return []
        tshark_command = ['tshark', '-r', self.current_outputfile, '-T', 'json']
        tshark_result = subprocess.run(tshark_command, stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
        if tshark_result.returncode != 0:
            err(tshark_result.stderr)
            err("tshark failed: {}".format(tshark_result.returncode))
            return []
        try:
            capture = json.loads(tshark_result.stdout)
            return capture
        except json.JSONDecodeError as error:
            err("capture JSON decoding failed: %s".format(error))
            return []

    def stop(self):
        '''Stop tcpdump if it is running.'''
        if self.tcpdump_proc:
            status("Terminating tcpdump")
            self.tcpdump_proc.terminate()
            self.tcpdump_proc = None
            self.current_outputfile = None
