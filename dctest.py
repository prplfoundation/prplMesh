#!/usr/bin/env python3
#
# Launch the test suite using docker and docker-compose. This script wraps
# the creation of the bridge(s) to be able to connect external devices with
# the docker network, launching the service for boardfarm.
#
# As this script is run outside containers, it does not use anything apart
# from Python 3.5 (will work on later versions but only uses 3.5 features)
#
# The best way to make sure no Python 3.5+ features are used is running the
# script with a Python 3.5.0 interpreter. Compile it from:
#
# https://www.python.org/ftp/python/3.5.0/Python-3.5.0.tgz
#
# Also, when calling a function look for 'New in version 3.X' where X > 5
#
from __future__ import print_function  # To check for python2 or < 3.5 execution
import argparse
import fcntl
import os
import sys
from subprocess import Popen, PIPE


if not (sys.version_info.major == 3 and sys.version_info.minor >= 5):
    print("This script requires Python 3.5 or higher!")
    print("You are using Python {}.{}.".format(sys.version_info.major, sys.version_info.minor))
    sys.exit(1)


def check_docker_versions():
    DOCKER_MAJOR = 19
    DC_MAJOR = 1
    DC_MINOR = 25
    docker_version  = os.popen('docker --version').read().split(' ')[2]
    docker_major = int(docker_version.split('.')[0])
    if docker_major < DOCKER_MAJOR:
        fmt = "This script requires docker {}.0 or higher"
        print(fmt.format(DOCKER_MAJOR))
        print("You are usng version {}".format(docker_version))
        sys.exit(1)
    dc_version = os.popen('docker-compose --version').read().split(' ')[2]
    dc_major = int(dc_version.split('.')[0])
    dc_minor = int(dc_version.split('.')[1])
    if dc_major < DC_MAJOR:
        fmt = "This script requires docker-compose {}.{} or higher"
        print(fmt.format(DC_MAJOR, DC_MINOR))
        print("You are usng version {}".format(dc_version))
        sys.exit(1)
    if dc_minor < DC_MINOR:
        fmt = "This script requires docker-compose {}.{} or higher"
        print(fmt.format(DC_MAJOR, DC_MINOR))
        print("You are usng version {}".format(dc_version))
        sys.exit(1)


class Services:
    def __init__(self):
        scriptdir = os.path.dirname(os.path.realpath(__file__))
        os.chdir(scriptdir)
        self.rootdir = os.path.dirname(scriptdir)
        print(self.rootdir)

    def _setNonBlocking(fd):
        """
        Set the file description of the given file descriptor to non-blocking.
        """
        flags = fcntl.fcntl(fd, fcntl.F_GETFL)
        flags = flags | os.O_NONBLOCK
        fcntl.fcntl(fd, fcntl.F_SETFL, flags)

    def dc(self, args, interactive=False):
        params = ['docker-compose', '-f',
                  'tools/docker/boardfarm-ci/docker-compose.yml']
        params += args
        local_env = os.environ
        local_env['ROOT_DIR'] = self.rootdir
        if not interactive:
            proc = Popen(params, stdout=PIPE, stderr=PIPE)
            for line in proc.stdout:
                print(line.decode(), end='')
            proc.stdout.close()
        else:
            proc = Popen(params)
        return_code = proc.wait()
        return return_code


if __name__ == '__main__':
    check_docker_versions()
    parser = argparse.ArgumentParser(description='Dockerized test launcher')
    parser.add_argument('--test', dest='test', type=str, help='Test to be run')
    parser.add_argument('--clean', dest='clean', action='store_true',
                        help='Clean containers images and networks')
    parser.add_argument('--shell', dest='shell', action='store_true',
                        help='Run a shell on the bf container')
    args = parser.parse_args()
    services = Services()
    if args.clean:
        services.dc(['down', '--remove-orphans', '--rmi', 'all'])
    elif args.shell:
        rc = services.dc(['run', '--service-ports', '--entrypoint',
                          '/bin/bash', 'boardfarm'], interactive=True)
        if rc != 0:
            print('Return code !=0 -> {}'.format(rc))
    else:
        rc = services.dc(['up', 'boardfarm'])
        if rc != 0:
            print('Return code !=0 -> {}'.format(rc))

