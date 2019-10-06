import logging
import argparse
import os
import getpass
import subprocess
import collections
import shutil
import multiprocessing

logger = logging.getLogger("build")
build_targets=['prepare', 'clean', 'distclean', 'make']
map_modules=['framework', 'common', 'controller', 'agent']
dep_modules=['nng', 'safeclib', 'dwpal', 'hostapd', 'wpa_supplicant']

# base builder class
class builder(object):
    def __init__(self, name, modules_dir, build_dir, install_dir):
        logger.debug("modules_dir={}, build_dir={}, install_dir={}".format(modules_dir, build_dir, install_dir))

        self.name = name
        self.src_path = "{}/{}".format(modules_dir, name)
        self.build_path = "{}/{}".format(build_dir, name)
        self.install_path = install_dir
        self.env = os.environ.copy()

        # If the STAGING_DIR environment variable is not set, 
        # use an empty variable for suppressing compiler warnings
        if not "STAGING_DIR" in os.environ:
            self.env["STAGING_DIR"] = ""

    def __str__(self):
        return "'{}' builder configuration:\n\tsrc_path: {}\n\tbuild_path: {}\n\tinstall_path: {}".format(self.name, self.src_path, self.build_path, self.install_path)

    def clean(self):
        if os.path.exists(self.build_path):
            logger.info("cleaning {}".format(self.name))
            shutil.rmtree(self.build_path)

    def distclean(self):
        self.clean()
        if os.path.exists(self.install_path):
            shutil.rmtree(self.install_path)

    def prepare(self):
        raise NotImplementedError('prepare() function must be overrided')

    def make(self):
        raise NotImplementedError('make() function must be overrided')

# cmake-based builder class
class cmakebuilder(builder):
    def __init__(self, name, modules_dir, build_dir, install_dir, cmake_verbose=False, make_verbose=False, cmake_flags=[], generator=None):
        self.cmake_verbose = cmake_verbose
        self.make_verbose = make_verbose
        self.cmake_flags = cmake_flags
        self.generator = generator

        super(cmakebuilder, self).__init__(name, modules_dir, build_dir, install_dir)

    def clean(self):
        if os.path.exists(self.build_path):
            os.system("xargs rm < {}/install_manifest.txt".format(self.build_path))
        super(cmakebuilder, self).clean()

    def prepare(self):
        if os.path.exists("{}/.prepared".format(self.build_path)):
            logger.info("{} already prepared, skip cmake {}".format(self.build_path, self.name))
            return

        cmd = ["cmake",
               "-H" + self.src_path,
               "-B" + self.build_path,
               "-DBUILD_SHARED_LIBS=ON",
               "-DCMAKE_INSTALL_PREFIX=" + self.install_path]
        cmd.extend(['-D%s' %f for f in self.cmake_flags])
        if self.cmake_verbose:
            cmd.append("--debug_output")
        if self.generator:
            cmd.extend(["-G", self.generator])
        logger.info("preparing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)
        open("{}/.prepared".format(self.build_path), 'a').close()

    def make(self):
        cmd = ["cmake",
               "--build", self.build_path,
               "--target", "install"]
        if self.make_verbose:
            cmd.extend(["--", "VERBOSE=1"])
        else:
            cmd.extend(["--", "-j", str(multiprocessing.cpu_count() + 1)])
        logger.info("building & installing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)

# automake-based builder class
class acbuilder(builder):
    def __init__(self, name, modules_dir, build_dir, install_dir, make_verbose=False, extra_config_args=()):
        self.make_verbose = make_verbose
        self.extra_config_args = extra_config_args
        super(acbuilder, self).__init__(name, modules_dir, build_dir, install_dir)

    def clean(self):
        if os.path.exists(self.build_path):
            os.system("make -C {} uninstall".format(self.build_path))
        super(acbuilder, self).clean()

    def prepare(self):
        tools_dir = os.getcwd()
        os.chdir(self.src_path)
        cmd = ["./build-tools/autogen.sh", "-f"]
        logger.info("preparing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)

        self.configure()
        os.chdir(tools_dir)

    def configure(self):
        try:
            os.mkdir(self.build_path)
        except OSError:
            pass
        os.chdir(self.build_path)

        cmd = [self.src_path + "/configure",
               "--prefix=" + self.install_path]
        cmd.extend(self.extra_config_args)
        logger.info("configuring {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)

    def make(self):
        cmd = ["make", '-C', self.build_path, "install"]
        if self.make_verbose:
            cmd.extend(["V=s"])
        else:
            cmd.extend(["-j", str(multiprocessing.cpu_count() + 1)])
        logger.info("building & installing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)

# hostap builder class
class hostapbuilder(builder):
    def __init__(self, name, config_dir, modules_dir, install_dir, make_verbose=False):
        self.make_verbose = make_verbose
        super(hostapbuilder, self).__init__(name, modules_dir, modules_dir, install_dir)
        self.config_src_fpath = os.path.abspath(os.path.join(config_dir, name + ".config"))
        self.config_dst_fpath = self.build_path + "/.config"

    def clean(self):
        os.system("make -C {} clean".format(self.build_path))
        if os.path.exists(self.config_dst_fpath):
            os.remove(self.config_dst_fpath)

    def distclean(self):
        self.clean()

    def prepare(self):
        logger.info("preparing {}".format(self.name))
        shutil.copy2(self.config_src_fpath, self.config_dst_fpath)

    def make(self):
        bin_dir = "BINDIR=" + self.install_path + "/bin"
        cmd = ["make", bin_dir, '-C', self.build_path, "install"]
        if self.make_verbose:
            cmd.extend(["V=s"])
        else:
            cmd.extend(["-j", str(multiprocessing.cpu_count() + 1)])
        logger.info("building & installing {}: {}".format(self.name, " ".join(cmd)))
        subprocess.check_call(cmd, env=self.env)

        # it requires to do a clean because the another builder instance with different config
        # may use pre-built object files while building
        self.clean()

class mapbuild(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)

        commands = args.commands
        _map_modules = map_modules if 'all' in args.modules or 'map' in args.modules else [m for m in map_modules if m in args.modules]
        _dep_modules = dep_modules if 'all' in args.modules or 'dep' in args.modules else [m for m in dep_modules if m in args.modules]

        logger.info("{} {}".format(commands, _map_modules + _dep_modules))

        build_dir = os.path.realpath(os.path.join(args.map_path, '..', 'build'))
        install_dir = os.path.join(build_dir, 'install')

        # build dep modules
        modules_dir = os.path.join(os.path.realpath(args.map_path), "../3rdparty")
        for name in _dep_modules:
            if name == 'dwpal' or name == 'hostapd' or name == 'wpa_supplicant':
                modules_dir = os.path.join(os.path.realpath(args.map_path), "../hostap")

            if name == 'nng' or name == 'dwpal':
                builder = cmakebuilder(name, modules_dir, build_dir, install_dir, args.cmake_verbose, args.make_verbose,
                    args.cmake_flags, args.generator)

            if name == 'safeclib':
                builder = acbuilder('safeclib', modules_dir, build_dir, install_dir, args.make_verbose,
                                    ['--enable-strmax=65536'])

            if name == 'hostapd' or name == 'wpa_supplicant':
                builder = hostapbuilder(name, os.path.join(args.map_path, "tools", "config"),
                                        modules_dir, install_dir, args.make_verbose)

            self.run_command(builder, commands)

        # build map modules
        modules_dir = os.path.realpath(args.map_path)

        map_cmake_flags = ["STANDALONE=ON"] + args.cmake_flags
        if not args.native: 
            if "CMAKE_TOOLCHAIN_FILE" in os.environ:
                map_cmake_flags += ["CMAKE_TOOLCHAIN_FILE=" + os.path.abspath(os.environ.get('CMAKE_TOOLCHAIN_FILE'))]
            else:
                map_cmake_flags += ["CMAKE_TOOLCHAIN_FILE=external_toolchain.cmake"]
        for name in _map_modules:
            builder = cmakebuilder(name, modules_dir, build_dir, install_dir, args.cmake_verbose, args.make_verbose,
                map_cmake_flags, args.generator)

            self.run_command(builder, commands)

    def run_command(self, builder, commands):
        logger.debug(builder)
        if 'distclean' in commands:
            builder.distclean()
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
        parser.add_argument('modules', choices=['all', 'map', 'dep'] + map_modules + dep_modules, nargs='+', help='module[s] to build')
        parser.add_argument('-c', '--commands', choices=build_targets, nargs='+', default=['make'], help="build command (default is clean+make)")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")
        parser.add_argument("--native", "-n", action="store_true", help="Build native (not cross compile - ignore external_toolchain.cfg)")
        parser.add_argument('-f', '--cmake-flags', nargs='+', default=[], help="extra cmake flags")
        parser.add_argument("--cmake-verbose", action="store_true", help="cmake verbosity on (pass --debug-output to cmake command)")
        parser.add_argument("--make-verbose", action="store_true", help="make verbosity on (pass VERBOSE=1 to make)")
        parser.add_argument("--generator", "-G", help="Specify a build system generator (cfr. cmake -G)")

        return parser

    def __str__(self):
        return str(self.args)
