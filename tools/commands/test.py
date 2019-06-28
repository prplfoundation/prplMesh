import logging
import argparse
import os
import subprocess

logger = logging.getLogger("test")
test_targets=['start', 'stop', 'restart', 'status']

class maptest(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)

        logger.info("{} prpltest".format(args.commands))

        install_dir = os.path.realpath(os.path.join(args.map_path, '..', 'build/install'))
        self.script_path = os.path.join(install_dir, 'scripts/prplmesh_utils.sh')

        if 'start' in args.commands:
            self.run_command('start')
        if 'stop' in args.commands:
            self.run_command('stop')
        if 'restart' in args.commands:
            self.run_command('restart')
        if 'status' in args.commands:
            self.run_command('status')

    def run_command(self, param):
        cmd = [self.script_path, param]
        subprocess.check_call(cmd)

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='test')):
        parser.help = "multiap_sw standalone test module"
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")
        parser.add_argument('-c', '--commands', choices=test_targets, nargs='+', default=['start'], help="test command (default is start)")

        return parser

    def __str__(self):
        return str(self.args)

