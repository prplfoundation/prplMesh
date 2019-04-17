import logging
import argparse
import os
import getpass
import subprocess
import collections
import shutil
import tarfile
import time

logger = logging.getLogger("package")
package_modules=['framework', 'common', 'controller', 'agent']

class mappackage(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)
        modules = package_modules if 'all' in args.modules else [m for m in package_modules if m in args.modules]
        modules_dir = os.path.realpath(args.map_path)
        package_dir = os.path.realpath(modules_dir + '/package')
        h = {'framework': None, 'common': None, 'controller': None, 'agent': None}

        logger.info("Packaging {}".format(modules))
        logger.debug("modules_dir={}, package_dir={}".format(modules_dir, package_dir))

        if os.path.exists(package_dir):
            logger.info("deleting {}".format(package_dir))
            shutil.rmtree(package_dir)

        for name in modules:
            src_path = "{}/{}".format(modules_dir, name)
            dst_path = "{}/{}".format(package_dir, name)
            stdout, stderr = subprocess.Popen(['git', 'rev-parse', '--short', 'HEAD'], cwd=src_path, stdout=subprocess.PIPE).communicate()
            try: h[name] = stdout.decode("utf-8").strip()
            except: pass
            logger.debug("Copy {} to {}".format(src_path, dst_path))
            shutil.copytree(src_path, dst_path, symlinks=True, ignore=lambda directory, contents: [".git"])
            with open("{}/VERSION".format(dst_path), 'w') as f: f.write("intel_multiap_{}_{}_{}".format(name, time.strftime("%Y%m%d"), h[name]))
            for root, dirs, files in os.walk(dst_path):
                for name in files: self.remove_patents(os.path.join(root, name))
        
        with tarfile.open("{}/intel_multiap_{}.tar.gz".format(package_dir, time.strftime("%Y%m%d")), "w:gz") as tar:
            tar.add(package_dir, arcname=os.path.basename(package_dir))

    def remove_patents(self, file):
        if file.endswith(('.h', '.hpp', '.c', '.cpp')) and 'PATENT' in open(file).read():
            logger.warning("===> {} has patents - deleting patents code sections <===".format(file))
            with open(file, "r") as f:
                lines = f.readlines()
            with open(file, "w") as f:
                skipline=0
                for line in lines:
                    if '#ifdef' in line and 'PATENT' in line: skipline+=1
                    elif skipline and '#ifdef' in line: skipline+=1
                    if not skipline: f.write(line)
                    if skipline and '#endif' in line: skipline-=1

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='package')):
        parser.help = "multiap_sw standalone packaging module"
        parser.add_argument('modules', choices=['all'] + package_modules, nargs='+', help='module[s] to package')
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)
