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
import os
import grp
import shutil
import getpass
import sys
import json
from subprocess import Popen, PIPE


if not (sys.version_info.major == 3 and sys.version_info.minor >= 5):
    print("This script requires Python 3.5 or higher!")
    print("You are using Python {}.{}.".format(sys.version_info.major, sys.version_info.minor))
    sys.exit(1)


def check_docker_versions():
    DOCKER_MAJOR = 19
    DC_MAJOR = 1
    DC_MINOR = 25
    docker_version = os.popen('docker --version').read().split(' ')[2]
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
    def __init__(self, bid=None):
        self.scriptdir = os.path.dirname(os.path.realpath(__file__))
        os.chdir(self.scriptdir)
        self.rootdir = self.scriptdir

        if bid is not None:
            self.build_id = bid
            print('Using ID {}'.format(self.build_id))
            # return
        else:
            self.build_id = self.get_build_id()

        self.logdir = os.path.join(self.scriptdir, 'logs')
        if not os.path.exists(self.logdir):
            os.makedirs(self.logdir)
        for device in self._get_device_names():
            device_name = '{}-{}'.format(device, self.build_id)
            devicedir = os.path.join(self.logdir, device_name)
            if not os.path.exists(devicedir):
                print('Making {}'.format(devicedir))
                os.makedirs(devicedir)

    def _get_device_names(self):
        jspath = './tests/boardfarm_plugins/boardfarm_prplmesh/prplmesh_config_compose.json'
        js = json.loads(open(jspath, 'r').read())
        devices = []
        for device in js['prplmesh_compose']['devices']:
            devices.append(device['name'])
        return devices

    def get_build_id(self):
        ci_pipeline_id = os.getenv('CI_PIPELINE_ID')
        if ci_pipeline_id is not None:
            return ci_pipeline_id

        # Otherwise we are running on the local machine, just find last id
        # created and add one
        last_id = 0
        if not os.path.exists('logs'):
            return str(1)

        # Search if a directory exists with logs/<device>-<X> and use X+1 as
        # id. Get the first device from the json list
        search_prefix = self._get_device_names()[0] + '-'
        for d in os.listdir('logs'):
            if d.startswith(search_prefix):
                suffix = d[len(search_prefix):]
                isuffix = int(suffix)
                if isuffix > last_id:
                    last_id = isuffix
        if last_id == 0:
            new_id = 1
        else:
            new_id = last_id + 1
        return str(new_id)

    def copy_build_dir(self):
        new_id = self.build_id
        self.build_dir = 'build-{}'.format(new_id)
        shutil.copytree('build', 'build-{}'.format(self.build_dir))
        print('Copied build/ into {}'.format(self.build_dir))

    def dc(self, args, interactive=False):
        params = ['docker-compose', '-f',
                  'tools/docker/boardfarm-ci/docker-compose.yml']
        # params += ['-p', 'boardfarm-ci-{}'.format(self.build_id)]
        params += args
        local_env = os.environ
        local_env['ROOT_DIR'] = self.rootdir
        docker_gid = grp.getgrnam('docker')[2]
        local_env['CURRENT_UID'] = str(os.getuid()) + ':' + str(docker_gid)
        local_env['CURRENT_ID'] = str(os.getuid())
        local_env['RUN_ID'] = self.build_id

        if os.getenv('CI_PIPELINE_ID') is None:
            # Running locally
            local_env['CI_PIPELINE_ID'] = 'latest'
            local_env['FINAL_ROOT_DIR'] = self.rootdir
        else:
            # Running inside gitlab-ci
            local_env['FINAL_ROOT_DIR'] = '/builds/prpl-foundation/prplMesh'

        # local_env['CURRENT_UID']= str(os.getuid()) + ':' + str(os.getgid())
        if not interactive:
            proc = Popen(params, stdout=PIPE, stderr=PIPE)
            for line in proc.stdout:
                print(line.decode(), end='')
            proc.stdout.close()
        else:
            proc = Popen(params)
        return_code = proc.wait()
        return return_code


def vararg_callback(option, opt_str, value, parser):
    assert value is None
    value = []

    def floatable(str):
        try:
            float(str)
            return True
        except ValueError:
            return False

    for arg in parser.rargs:
        # stop on --foo like options
        if arg[:2] == "--" and len(arg) > 2:
            break
        # stop on -a, but not on -3 or -3.0
        if arg[:1] == "-" and len(arg) > 1 and not floatable(arg):
            break
        value.append(arg)

    del parser.rargs[:len(value)]
    setattr(parser.values, option.dest, value)


def cleanup(rc):
    if rc != 0:
        print('Return code !=0 -> {}'.format(rc))
    if getpass.getuser() == 'gitlab-runner':
        os.system('chown -R gitlab-runner:gitlab-runner .')
    sys.exit(rc)


if __name__ == '__main__':
    check_docker_versions()
    parser = argparse.ArgumentParser(description='Dockerized test launcher')
    parser.add_argument('--test', dest='test', type=str, help='Test to be run')
    parser.add_argument('--clean', dest='clean', action='store_true',
                        help='Clean containers images and networks')
    parser.add_argument('--build', dest='build', action='store_true',
                        help='Rebuild containers')
    parser.add_argument('--shell', dest='shell', action='store_true',
                        help='Run a shell on the bf container')
    parser.add_argument('--comp', dest='comp', action='store_true',
                        help='Pass the rest of arguments to docker-compose')
    parser.add_argument('--id', dest='bid', type=str,
                        help='Specify the id to use for build/shell/comp/clean')
    args, rest = parser.parse_known_args()

    if os.getenv('CI_PIPELINE_ID') is not None:
        args.bid == os.getenv('CI_PIPELINE_ID')

    if args.comp:
        if args.bid is None:
            print('Specify --id for the --comp parameter')
            sys.exit(0)
        services = Services(bid=args.bid)
        if len(rest) == 0:
            print('Usage: dctest --id <id> --comp <arguments to docker-compose>')
            sys.exit(1)
        sys.exit(services.dc(rest, interactive=True))
    else:
        if len(rest) > 0:
            print('Unknown parameters: {}'.format(rest))
            sys.exit(1)

    if args.clean:
        if args.bid is None:
            print('Specify --id for the --clean parameter')
            sys.exit(0)
        services = Services(bid=args.bid)
        rc = services.dc(['down', '--remove-orphans', '--rmi', 'all'])
        cleanup(rc)
    elif args.shell:
        if not args.bid:
            print('Specify --id for the shell parameter')
            sys.exit(0)
        services = Services(bid=args.bid)
        rc = services.dc(['run', '--rm', '--service-ports', '--entrypoint',
                          '/bin/bash', 'boardfarm'], interactive=True)
        cleanup(rc)
    elif args.build:
        if not args.bid:
            print('Specify --id for the build parameter')
            sys.exit(0)
        services = Services(bid=args.bid)
        rc = services.dc(['build'], interactive=True)
        cleanup(rc)
    else:
        if args.bid:
            services = Services(bid=args.bid)   # With new build id
        else:
            services = Services()   # With new build id
        # rc = services.dc(['up', 'boardfarm'])
        # rc = services.dc(['run', '--service-ports', '--entrypoint',
        #                  '/bin/bash', 'boardfarm'], interactive=True)
        rc = services.dc(['run', '--rm', '--service-ports', '--use-aliases',
                          'boardfarm'], interactive=True)
        cleanup(rc)
