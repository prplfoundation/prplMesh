import logging
import argparse
import os
import getpass

class mapbuild(object):
    default_path = os.path.abspath(os.path.dirname(__file__)+'/../../') #one dir above the tools repo
    supported_targets = ["grx350"]

    def __init__(self, args):
        self.logger = logging.getLogger("build")
        self.args = args

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='build')):
        parser.help = "multiap_sw standalone build"
        parser.add_argument("--config_path", default=mapbuild.default_path, help="path to configuration files folder (external_toolchain.cfg and beerocks_dist.conf)")
        parser.add_argument("--build_path", default=mapbuild.default_path, help="path to multiap repos folder")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)
