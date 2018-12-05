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
        # update list - (local_path, proxy_path, perm, md5sum)
        update_list = [(os.path.abspath(f), '/tmp/%s' % (os.path.basename(f)), oct(os.stat(f).st_mode & 0777), hashlib.md5(open(f, 'rb').read()).hexdigest()) for f in files]

        # copy to proxy
        for lpath, ppath, perm, md5sum in update_list:
             logger.info("Upload {} (md5sum {}) to {}:{} via {}".format(lpath, md5sum, self.target['ip'], path, self.proxy['ip']))
             self.sftp.put(lpath, ppath)

        ssh_cmd_template = 'sshpass -p {} ssh {} {}@{}'.format(self.target['pass'], mapconnect.SSHOPTIONS, self.target['user'], self.target['ip'])
        mkdir_cmd = '{} "mkdir -p {}"'.format(ssh_cmd_template, path)
        scp_cmd = 'sshpass -p {} scp -r {} {} {}@{}:{}'.format(self.target['pass'], mapconnect.SSHOPTIONS, ' '.join(['{}'.format(ppath) for lpath, ppath, perm, md5sum in update_list]), self.target['user'], self.target['ip'], path)
        chmod_cmd = '{} "{}"'.format(ssh_cmd_template, ';'.join(['chmod {} {}'.format(perm, os.path.join(path, os.path.basename(lpath))) for lpath, ppath, perm, md5sum in update_list]))

        # run commands at proxy
        self.run([mkdir_cmd, scp_cmd, chmod_cmd])

        # delete from proxy
        for lpath, ppath, perm, md5sum in update_list: self.sftp.remove(ppath)

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
            self.os = 'ugw' if self.conf['target']['type'] in ['grx350', 'axepoint'] else 'rdkb'
    
        logger.debug("deploy configuration: {}".format(self.conf))
        logger.info("{} deploy {}".format(self.os, modules))

        for m in modules:
            logger.debug("deploy {}".format(m))
            self.deploy(m)

    def deploy(self, name):
        if name in ['agent', 'controller']:
            archive = "{}/{}/beerocks_v1.4_{}.tar.gz".format(self.build_dir, name, name)
            softlink = "/opt/beerocks/update_beerocks_{}".format(name)
            path = self.conf['deploy'][self.os][name]['bin']
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
                logger.info("{} deploy binaries: {}-->{}".format(name, [os.path.basename(b) for b in bins], self.conf['deploy'][self.os][name]['bin']))
                self.connect.upload(bins, self.conf['deploy'][self.os][name]['bin'])
            if libs:
                logger.info("{} deploy libraries: {}-->{}".format(name, [os.path.basename(l) for l in libs], self.conf['deploy'][self.os][name]['lib']))
                self.connect.upload(libs, self.conf['deploy'][self.os][name]['lib'])

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