#!/usr/bin/env python

import logging
import logging.config
import argparse
import os
import sys
from commands.config import mapcfg
from commands.build import mapbuild
from commands.deploy import mapdeploy, mapcopy

THIS_SCRIPT_PATH = os.path.dirname(os.path.realpath(__file__))
logging.config.fileConfig(os.path.abspath(THIS_SCRIPT_PATH + "/logging.conf"))

def print_color(string, color):
	colors = {'red':'31', 'yellow':'33', 'blue':'34', 'magenta':'35', 'turquoise':'36', 'white':'37'}
	attr = []
	attr.append(colors[color])
	attr.append('1')
	print('\x1b[%sm%s\x1b[0m' % (';'.join(attr), string))

def ASSERT(string):
	print_color('ASSERT: %s' %(string), 'red')

def WARNING(string):
	print_color('%s' %(string), 'yellow')

class maptools(object):
    default_multiap_root = os.path.abspath(os.path.dirname(__file__)+'/../') #one dir above the tools repo

    def __init__(self):
        self.logger = logging.getLogger("maptools")
        self.parent_parser = argparse.ArgumentParser(description="MultiAP Tools")
        self.parent_parser.add_argument("--map-path", "-m", default=maptools.default_multiap_root, help="path to multiap sources tree")
        self.parent_parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")
        self.parent_parser.add_argument("--user", "-u", type=str, help="use custom username when connecting to jira")
        self.child_parser = self.parent_parser.add_subparsers(title="subcommand", help="subcommand help", dest="cmd")

        self.config_command = self.child_parser.add_parser('config')
        self.build_command = self.child_parser.add_parser('build')
        self.deploy_command = self.child_parser.add_parser('deploy')
        self.copy_command = self.child_parser.add_parser('copy')
        mapcfg.configure_parser(self.config_command)
        mapbuild.configure_parser(self.build_command)
        mapdeploy.configure_parser(self.deploy_command)
        mapcopy.configure_parser(self.copy_command)
        
        self.args = self.parent_parser.parse_args()
        if self.args.verbose: self.logger.setLevel(logging.DEBUG)

    def __str__(self):
        return str(self.args)

    def __config__(self):
        mapcfg(self.args)
    
    def __build__(self):
        mapbuild(self.args)

    def __deploy__(self):
        mapdeploy(self.args)

    def __copy__(self):
        mapcopy(self.args)

    def run(self):
        commands = {
            'config': self.__config__, 
            'build': self.__build__,
            'deploy': self.__deploy__,
            'copy': self.__copy__
        }

        try:
            self.logger.debug(self.args.cmd)
            commands[self.args.cmd]()
        except KeyError as err:
            if not self.args.cmd:
                self.parent_parser.print_help()
                sys.exit(0)
            raise Exception("Operation not supported ({}, err={})".format(self.args.cmd, err))

if __name__ == '__main__':
    try:
	    maptools().run()
    except Exception as e:
        ASSERT(e)
        raise