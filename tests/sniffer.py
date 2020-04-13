###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import os
import subprocess
import json
from opts import debug, err, opts, status


class Sniffer:
    '''Captures packets on an interface.'''
    def __init__(self, interface: str):
        self.interface = interface
        self.tcpdump_proc = None
        self.outputfiles = {}
        self.captures = {}

    def start(self, test_name):
        '''Start tcpdump.'''
        outputfile_basename = "test_" + test_name
        debug("Starting tcpdump, output file {}.pcap".format(outputfile_basename))
        os.makedirs(os.path.join(opts.tcpdump_dir, 'logs'), exist_ok=True)
        outputfile = os.path.join(opts.tcpdump_dir, outputfile_basename) + ".pcap"
        self.outputfiles[test_name] = outputfile
        command = ["tcpdump", "-i", self.interface, '-U', '-w', outputfile]
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

    def get_packet_capture(self, test_name):
        tshark_command = ['tshark', '-r', self.outputfiles[test_name], '-T', 'json']
        self.tshark_proc = subprocess.run(
            tshark_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        try:
            capture = json.loads(self.tshark_proc.stdout)
            def ieee_filter(x): return 'ieee1905' in x['_source']['layers']
            result = self._simplify_packets(filter(ieee_filter, capture))
            if not result or len(result) == 0:
                return None
            try:
                self.captures[test_name] = self.captures[test_name] + result
            except:
                self.captures[test_name] = result
            return self.captures[test_name]
        except BaseException as ex:
            return None

    def _simplify_packets(self, packets):
        def _simplify(x):
            x['src_mac'] = x['_source']['layers']['eth']['eth.src']
            x['dst_mac'] = x['_source']['layers']['eth']['eth.dst']
            x['time_epoch'] = float(x['_source']['layers']['frame']['frame.time_epoch'])
            x['time'] = x['_source']['layers']['frame']['frame.time']
            x['ieee1905_layer'] = x['_source']['layers']['ieee1905']
            x['msg_type'] = int(x['ieee1905_layer']['ieee1905.message_type'])
            x['mid'] = int(x['ieee1905_layer']['ieee1905.message_id'],16)
            x['tlvs'] = {entry[0]:entry[1] for entry in filter(lambda a: a[0].startswith('ieee1905.') == False ,x['ieee1905_layer'].items())}
            return x
        return list(map(_simplify,packets))

    def get_packets_for_cmdu_type(self, test_name, msg_type: int):
        def filter_func(x): return x['msg_type'] == msg_type
        try:
            return list(filter(filter_func, self.get_packet_capture(test_name)))
        except:
            err("No packets captured for specified cmdu type: {}".format(msg_type))
            return None

    def filter_packets_by_time(self, packets, start_time=None, end_time=time()):
        def _get_time_epoch_for_packet(packet):
            t = packet['time_epoch']
            if start_time:
                return t >= start_time and t <= end_time
            return t <= end_time
        return list(filter(_get_time_epoch_for_packet, packets))

    def stop(self):
        '''Stop tcpdump if it is running.'''
        if self.tcpdump_proc:
            status("Terminating tcpdump")
            self.tcpdump_proc.terminate()
            self.tcpdump_proc = None
