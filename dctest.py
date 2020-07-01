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
import argparse
import os
from subprocess import Popen, PIPE


class Services:
    def __init__(self):
        scriptdir = os.path.dirname(os.path.realpath(__file__))
        os.chdir(scriptdir)
        self.rootdir = os.path.dirname(scriptdir)
        print(self.rootdir)

    def dc(self, args):
        params = ['docker-compose', '-f',
                  'tools/docker/boardfarm-ci/docker-compose.yml']
        params += args
        local_env = os.environ
        local_env['ROOT_DIR'] = self.rootdir
        proc = Popen(params, stdout=PIPE, stderr=PIPE)
        for line in proc.stdout:
            print(line.decode(), end='')
        proc.stdout.close()
        return_code = proc.wait()
        return return_code


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Dockerized test launcher')
    parser.add_argument('--test', dest='test', type=str, help='Test to be run')
    parser.add_argument('--clean', dest='clean', help='Test to be run')
    args = parser.parse_args()
    services = Services()
    if args.clean:
        services.dc(['down', '--remove-orphans', '--rmi', 'all'])
    else:
        rc = services.dc(['up', 'boardfarm'])
        if rc != 0:
            print('Return code !=0 -> {}'.format(rc))

