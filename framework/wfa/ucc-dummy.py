#!/usr/bin/env python3
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import socket
import argparse
import threading

def handle(host, port, respDict):
    id = threading.get_ident()
    print("{}: Listening on {}:{}".format(id, host, port))
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        with s.accept() as (conn, addr):
            print("{}: {}@{} Connected by {}".format(id, host, port, addr))
            while True:
                try:
                    data = conn.recv(1024)
                except Exception as e:
                    print("Exception {}".format(e))
                    break
                print("{}@{} Received: {}".format(host, port, data))
                resp = [b'Status,RUNNING', b'Status,COMPLETE'] # default response
                for key in respDict:
                    if key in str(data):
                        resp = respDict[key] # specific resonse
                for r in resp:
                    print("{}@{} Sending: {}".format(host, port, r))
                    conn.sendall(r)
    print("{}: {}@{} Exiting!".format(id, host, port))

parser = argparse.ArgumentParser()
parser.add_argument("--ports", nargs='+', type=int,
                    default=[7000, 7001, 7002, 7003, 7004, 7005, 7006,
                             7007, 7008, 7009, 7010, 7011, 7012, 8888])
parser.add_argument("--host", default='127.0.0.1')
parser.add_argument("--dst-alid", default="11:22:33:44:55:66")
args = parser.parse_args()

respDict = {
    'dev_reset_default' : [b'Status,RUNNING', b'Status,COMPLETE'],
    'ca_get_version' : [b'Status,RUNNING', b'Status,COMPLETE,vendor,dummy,model,dummy,version,1.0'],
    'dev_get_parameter' : [b'Status,RUNNING', 'Status,COMPLETE,aLid,{}'.format(args.dst_alid).encode()],
}

print("Starting threads on host {}, ports {}".format(args.host, args.ports))
threads = list()

for port in args.ports:
    print("Creating thread {}@{}".format(args.host, port))
    t = threading.Thread(target=handle, args=(args.host, port, respDict,))
    threads.append(t)
    print("Starting thread {}@{}".format(args.host, port))
    t.start()

for t in threads:
    print("Before joining thread {}@{}".format(args.host, port))
    t.join()
    print("After joining thread {}@{}".format(args.host, port))
