###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Arnout Vandecappelle (Essensium/Mind)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import os
import subprocess

from opts import debug, err, opts, status


class Sniffer:
    '''Captures packets on an interface.'''
    def __init__(self, interface: str):
        self.interface = interface
        self.tcpdump_proc = None

    def start(self, outputfile_basename):
        '''Start tcpdump if enabled by config.'''
        if opts.tcpdump:
            debug("Starting tcpdump, output file {}.pcap".format(outputfile_basename))
            os.makedirs(os.path.join(opts.tcpdump_dir, 'logs'), exist_ok=True)
            outputfile = os.path.join(opts.tcpdump_dir, outputfile_basename) + ".pcap"
            command = ["tcpdump", "-i", self.interface, "-w", outputfile]
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

    def stop(self):
        '''Stop tcpdump if it is running.'''
        if self.tcpdump_proc:
            status("Terminating tcpdump")
            self.tcpdump_proc.terminate()
            self.tcpdump_proc = None
