import logging
import argparse
import os
import subprocess

logger = logging.getLogger("test")
test_targets=['start', 'stop', 'restart', 'status', 'check']

class prpltester(object):
    def __init__(self, install_dir, verbose):
        self.script_path = os.path.join(install_dir, 'scripts/prplmesh_utils.sh')
        self.verbose = verbose

    def run_cmd(self, param):
        cmd = [self.script_path, param]
        subprocess.check_call(cmd)

class maptest(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)

        logger.info("{} prpltest".format(args.commands))

        install_dir = os.path.realpath(os.path.join(args.map_path, '..', 'build/install'))
        tester = prpltester(install_dir, args.verbose)

        self.run_command(tester, args.commands)

    def run_command(self, tester, commands):
        if 'start' in commands:
            tester.run_cmd('start')
        if 'stop' in commands:
            tester.run_cmd('stop')
        if 'check' in commands:
            tester.run_cmd('restart')
            tester.run_cmd('check')
            tester.run_cmd('stop')

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='test')):
        parser.help = "multiap_sw standalone test module"
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")
        parser.add_argument('-c', '--commands', choices=test_targets, nargs='+', default=['check'], help="test command (default is check)")

        return parser

    def __str__(self):
        return str(self.args)

