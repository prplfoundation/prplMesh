import logging
import argparse
import os
import getpass
import subprocess
import collections
import shutil

logger = logging.getLogger("build")
build_targets=['clean', 'distclean', 'make']
build_modules=['framework', 'common', 'beerocks'] # TODO change beerocks to 'controller', 'agent' once split

class cmakebuilder(object):
    def __init__(self, name, modules_dir, build_dir, install_dir):
        self.name = name
        self.src_path = "{}/{}".format(modules_dir, name)
        self.build_path = "{}/{}".format(build_dir, name)
        '''
        Use relative path for self.install_path, as it is used for CMAKE_INSTALL_PREFIX,
        and external_toolchain.cfg (in source repos) expects relative paths otherwise it treats
        the build as out of tree.
        '''
        self.install_path = os.path.relpath(install_dir, self.src_path)
        self.env = os.environ.copy()
        self.env["STAGING_DIR"] = ""

    def __str__(self):
        return "'{}' builder configuration:\n\tsrc_path: {}\n\tbuild_path: {}\n\tinstall_path: {}".format(self.name, self.src_path, self.build_path, self.install_path)

    def clean(self):
        if os.path.exists(self.build_path):
            logger.info("cleaning {}".format(self.name))
            shutil.rmtree(self.build_path)

    def prepare(self):
        if os.path.exists("{}/.prepared".format(self.build_path)):
            logger.info("{} already prepared, skip cmake {}".format(self.build_path, self.name))
            return

        logger.info("preparing {}".format(self.name))
        cmd = "cmake -H{} -B{} -DCMAKE_TOOLCHAIN_FILE=external_toolchain.cmake -DCMAKE_INSTALL_PREFIX={}".format(
            self.src_path, self.build_path, self.install_path)
        subprocess.check_call(cmd, shell=True, env=self.env)
        open("{}/.prepared".format(self.build_path), 'a').close()
    
    def make(self):
        logger.info("building & installing {}".format(self.name))
        cmd = "cmake --build {} -- install -j".format(self.build_path)
        subprocess.check_call(cmd, shell=True, env=self.env)

class mapbuild(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)
        commands = args.commands
        modules = build_modules if 'all' in args.modules else [m for m in build_modules if m in args.modules]
        modules_dir = os.path.realpath(args.map_path)
        build_dir = os.path.realpath(modules_dir + '/build')
        install_dir = os.path.realpath(build_dir + '/install')
       
        if 'distclean' in commands and os.path.exists(build_dir):
             logger.info("distclean - deleting {}".format(build_dir))
             shutil.rmtree(build_dir)

        logger.info("{} {}".format(commands, modules))
        logger.debug("modules_dir={}, build_dir={}, install_dir={}".format(modules_dir, build_dir, install_dir))

        for name in modules:
            builder = cmakebuilder(name, modules_dir, build_dir, install_dir)
            logger.debug(builder)
            if 'clean' in commands:
                builder.clean()
            if 'make' in commands:
                builder.prepare()
                builder.make()

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='build')):
        parser.help = "multiap_sw standalone build module"
        parser.add_argument('modules', choices=['all'] + build_modules, nargs='+', help='module[s] to build')
        parser.add_argument('-c', '--commands', choices=build_targets, nargs='+', default=['make'], help="build command (default is clean+make)")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)
