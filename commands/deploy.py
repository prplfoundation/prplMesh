import logging
import argparse
import os
import getpass
import subprocess
import collections
import shutil
import yaml
import paramiko
import glob
import hashlib

logging.getLogger("paramiko").setLevel(logging.WARNING)
logger = logging.getLogger("deploy")
deploy_modules=['framework', 'common', 'controller', 'agent']
deploy_paths = {"framework" : {"bin" : "/opt/multiap/framework", "lib" : "/usr/lib"},
                "common" : {"bin" : "/opt/multiap/common", "lib" : "/usr/lib"},
                "agent" : {"bin" : "/opt/beerocks", "lib" : "/usr/lib"},
                "controller" : {"bin" : "/opt/beerocks", "lib" : "/usr/lib"},
                }

class mapconnect(object):
    SSHOPTIONS = "-oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no" #default ssh options

    def __init__(self, conf):
        self.proxy = conf['proxy']
        self.target = conf['target']
        self.__connect__()
    
    def __connect__(self):
        ''' connect sftp and ssh clients '''
       
        # start sftp connection
        t = paramiko.Transport((self.proxy['ip'], 22))
        t.connect(username=self.proxy['user'], password=self.proxy['pass'])
        logger.info("connect SFTP @{}".format(self.proxy['ip']))
        self.sftp = paramiko.SFTPClient.from_transport(t)
        # start ssh connection
        self.client = paramiko.SSHClient()
        self.client.load_system_host_keys()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        logger.info("connect SSH {}@{}".format(self.proxy['user'], self.proxy['ip']))
        self.client.connect(self.proxy['ip'], port=22, username=self.proxy['user'], password=self.proxy['pass'])

    def run(self, commands=[]):
        ''' Run commands on target via proxy '''
        for c in commands:
            logger.debug("@{} --> {}".format(self.proxy['ip'], c))
            stdin, stdout, stderr = self.client.exec_command(c)
            if stdout.channel.recv_exit_status():
                logger.error("Command failed:\n{}\nexit code - {}, stderr - {}".format(c, stdout.channel.recv_exit_status(), stderr.read()))

    def upload(self, files, path):
        ''' Uploads files to the target via proxy
            files - local files
            path - path in target
        '''
        proxy_file_paths = ['/tmp/%s' % (os.path.basename(f)) for f in files]

        for file in files:
            md5sum = hashlib.md5(open(file, 'rb').read()).hexdigest()
            logger.info("Upload {} (md5sum {}) to {}:{} via {}".format(os.path.basename(file), md5sum, self.target['ip'], path, self.proxy['ip']))
            # copy to proxy via sftp
            proxy_file_path = '/tmp/%s' % (os.path.basename(file))
            self.sftp.put(file, proxy_file_path)

        # copy from proxy to target via ssh
        commands = ['sshpass -p {} ssh {} {}@{} "mkdir -p {}"'.format(self.target['pass'], mapconnect.SSHOPTIONS, self.target['user'], self.target['ip'], path),
                    'sshpass -p admin scp -r {} {} admin@192.168.1.1:{}'.format(mapconnect.SSHOPTIONS, ' '.join(proxy_file_paths), path)]

        self.run(commands)

        # remove copied files from proxy
        for p in proxy_file_paths: self.sftp.remove(p)

class mapcopy(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)
        self.modules_dir = os.path.realpath(args.map_path)
        conf_file = args.conf if args.conf else os.path.realpath(self.modules_dir + '/deploy.yaml')
        with open(conf_file, 'r') as f:
            self.conf = yaml.load(f)
            self.connect = mapconnect(self.conf)
        logger.debug("copy configuration: {}".format(self.conf))
        logger.info("copy {} to target @{}".format(args.file, args.path))
        self.connect.upload(args.file, args.path)

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='copy')):
        parser.help = "multiap_sw standalone deploy module"
        parser.add_argument('file', nargs="+", help="only upload a file to target")
        parser.add_argument("--path", "-p", default="/tmp/", help="path to copy on in target")
        parser.add_argument("--conf", "-c", help="path to deploy.yaml")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)

class mapdeploy(object):
    def __init__(self, args):
        if args.verbose: logger.setLevel(logging.DEBUG)
        modules = deploy_modules if 'all' in args.modules else [m for m in deploy_modules if m in args.modules]
        self.modules_dir = os.path.realpath(args.map_path)
        self.build_dir = os.path.realpath(self.modules_dir + '/build')
        conf_file = args.conf if args.conf else os.path.realpath(self.modules_dir + '/deploy.yaml')
        logger.debug("modules_dir={}, build_dir={}, conf={}".format(self.modules_dir, self.build_dir, conf_file))
        with open(conf_file, 'r') as f:
            self.conf = yaml.load(f)
            self.connect = mapconnect(self.conf)
    
        logger.debug("deploy configuration: {}".format(self.conf))
        logger.info("deploy {}".format(modules))

        for m in modules:
            logger.debug("deploy {}".format(m))
            self.deploy(m)

    def deploy(self, name):
        if name in ['agent', 'controller']:
            archive = "{}/{}/beerocks_v1.4_{}.tar.gz".format(self.build_dir, name, name)
            softlink = "/opt/beerocks/update_beerocks_{}".format(name)
            path = deploy_paths[name]['bin']
            # copy archive to target
            self.connect.upload([archive], path)
            # delete old soft link if exists and create a new soft link on target
            target = self.conf['target']
            ssh_command = 'sshpass -p {} ssh {} {}@{} '.format(target['pass'], mapconnect.SSHOPTIONS, target['user'], target['ip'])
            command = '{} "rm {}; ln -s {} {}"'.format(ssh_command, softlink, os.path.join(path, os.path.basename(archive)), softlink)
            self.connect.run([command])
        else:
            # deploy out/bin and out/lib in all other modules
            bin_dir = os.path.join(self.build_dir, name, 'out/bin')
            lib_dir = os.path.join(self.build_dir, name, 'out/lib')
            bins = glob.glob('{}/*'.format(bin_dir))
            libs = glob.glob('{}/*'.format(lib_dir))
            logger.info("{} bin_dir: {}".format(name, bin_dir))
            logger.info("{} lib_dir: {}".format(name, lib_dir))
            if bins:
                logger.info("{} deploy binaries: {}-->{}".format(name, [os.path.basename(b) for b in bins], deploy_paths[name]['bin']))
                self.upload(bins, deploy_paths[name]['bin'])
            if libs:
                logger.info("{} deploy libraries: {}-->{}".format(name, [os.path.basename(l) for l in libs], deploy_paths[name]['lib']))
                self.upload(libs, deploy_paths[name]['lib'])

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='deploy')):
        parser.help = "multiap_sw standalone deploy module"
        parser.add_argument('modules', choices=['all'] + deploy_modules, nargs='+', help='module[s] to deploy')
        parser.add_argument("--path", "-p", default="/tmp/multiap/deploy/", help="path to deploy on in target")
        parser.add_argument("--conf", "-c", help="path to deploy.yaml")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)