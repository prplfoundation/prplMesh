import logging
import argparse
import os
import getpass
import subprocess
import collections
import shutil

logger = logging.getLogger("build")
build_targets=['prepare', 'clean', 'distclean', 'make']
build_modules=['framework', 'common', 'controller', 'agent']

class cmakebuilder(object):
    def __init__(self, name, modules_dir, build_dir, install_dir, native_build=False, cmake_verbose=False, make_verbose=False, cmake_flags=[]):
        self.name = name
        self.native_build = native_build
        self.cmake_verbose = cmake_verbose
        self.make_verbose = make_verbose
        self.cmake_flags = cmake_flags
        self.src_path = "{}/{}".format(modules_dir, name)
        self.build_path = "{}/{}".format(build_dir, name)
        self.install_path = install_dir
        self.env = os.environ.copy()
        self.env["STAGING_DIR"] = ""

    def __str__(self):
        return "'{}' builder configuration:\n\tsrc_path: {}\n\tbuild_path: {}\n\tinstall_path: {}".format(self.name, self.src_path, self.build_path, self.install_path)

    def clean(self):
        if os.path.exists(self.build_path):
            logger.info("cleaning {}".format(self.name))
            os.system("xargs rm < {}/install_manifest.txt".format(self.build_path))
            shutil.rmtree(self.build_path)

    def prepare(self):
        if os.path.exists("{}/.prepared".format(self.build_path)):
            logger.info("{} already prepared, skip cmake {}".format(self.build_path, self.name))
            return

        cmd = ["cmake",
               "-H" + self.src_path,
               "-B" + self.build_path,
               "-DSTANDALONE=ON",
               "-DCMAKE_INSTALL_PREFIX=" + self.install_path]
        if not self.native_build:
            cmd.append("-DCMAKE_TOOLCHAIN_FILE=external_toolchain.cmake")
        cmd.extend(['-D%s' %f for f in self.cmake_flags])
        if self.cmake_verbose:
            cmd.append("--debug_output")
        logger.info("preparing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)
        open("{}/.prepared".format(self.build_path), 'a').close()
    
    def make(self):
        cmd = ["cmake",
               "--build", self.build_path,
               "--",
               "install",
               "-j" if not self.make_verbose else "VERBOSE=1"]
        logger.info("building & installing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)

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
            builder = cmakebuilder(name, modules_dir, build_dir, install_dir, args.native, args.cmake_verbose, args.make_verbose, args.cmake_flags)
            logger.debug(builder)
            if 'clean' in commands:
                builder.clean()
            if 'prepare' in commands:
                builder.prepare()
            if 'make' in commands:
                builder.prepare()
                builder.make()

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='build')):
        parser.help = "multiap_sw standalone build module"
        parser.add_argument('modules', choices=['all'] + build_modules, nargs='+', help='module[s] to build')
        parser.add_argument('-c', '--commands', choices=build_targets, nargs='+', default=['make'], help="build command (default is clean+make)")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")
        parser.add_argument("--native", "-n", action="store_true", help="Build native (not cross compile - ignore external_toolchain.cfg)")
        parser.add_argument('-f', '--cmake-flags', nargs='+', default=[], help="extra cmake flags")
        parser.add_argument("--cmake-verbose", action="store_true", help="cmake verbosity on (pass --debug-output to cmake command)")
        parser.add_argument("--make-verbose", action="store_true", help="make verbosity on (pass VERBOSE=1 to make)")

        return parser

    def __str__(self):
        return str(self.args)
