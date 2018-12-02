import logging
import argparse
import os
import getpass
import subprocess
import collections
import shutil
import yaml
import paramiko

logging.getLogger("paramiko").setLevel(logging.WARNING)
logger = logging.getLogger("deploy")
deploy_modules=['framework', 'common', 'controller', 'agent']

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
            self.__connect__()
        logger.debug("deploy configuration: {}".format(self.conf))
        logger.info("deploy {}".format(modules))

        if args.file:
            # HACK to copy single file
            logger.info("copy single file {} to target @{}".format(args.file, args.path))
            self.upload(args.file, args.path)
        else:
            for m in modules:
                logger.debug("deploy {}".format(m))
                self.deploy(m)

    def deploy(self, name):
        if name in ['agent', 'controller']:
            archive = "{}/{}/beerocks_v1.4_{}.tar.gz".format(self.build_dir, name, name)
            softlink = "/opt/beerocks/update_beerocks_{}".format(name)
            path = "/opt/beerocks/"
            # copy archive to target
            self.upload(archive, path)
            # create soft link on target
            target = self.conf['target']
            command = 'sshpass -p {} ssh {} {}@{} "ln -s {} {}"'.format(target['pass'], self.sshoptions,
                    target['user'], target['ip'], '{}/{}'.format(path, os.path.basename(archive)), softlink)
            self.run([command])
        else:
            # deploy out/bin and out/lib in all other modules
            logger.error("deploy {} not supported yet...".format(name))

    def __connect__(self):
        ''' connect sftp and ssh clients '''
        proxy = self.conf['proxy']
        target = self.conf['target']
        
        #default ssh options
        self.sshoptions = "-oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no"
        # start sftp connection
        t = paramiko.Transport((proxy['ip'], 22))
        t.connect(username=proxy['user'], password=proxy['pass'])
        self.sftp = paramiko.SFTPClient.from_transport(t)
        # start ssh connection
        self.client = paramiko.SSHClient()
        self.client.load_system_host_keys()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.client.connect(proxy['ip'], port=22, username=proxy['user'], password=proxy['pass'])

    def run(self, commands=[]):
        ''' Run commands on target via proxy '''
        proxy = self.conf['proxy']
        target = self.conf['target']
        for c in commands:
            logger.debug("@{} --> {}".format(proxy['ip'], c))
            stdin, stdout, stderr = self.client.exec_command(c)
            if stdout.channel.recv_exit_status():
                logger.error("Failed executing command: exit code - {}, stderr - {}".format(stdout.channel.recv_exit_status(), stderr.read()))
        
    def upload(self, file, path):
        ''' Uploads a file to the target via proxy
            file - local file
            path - path in target
        '''
        proxy = self.conf['proxy']
        target = self.conf['target']
        logger.info("Upload {} to {}:{} via {}".format(os.path.basename(file), target['ip'], path, proxy['ip']))

        # copy to proxy via sftp
        proxy_file_path = '/tmp/%s' % (os.path.basename(file))
        self.sftp.put(file, proxy_file_path)

        # copy from proxy to target via ssh
        commands = ['sshpass -p {} ssh {} {}@{} "mkdir -p {}"'.format(target['pass'], self.sshoptions, target['user'], target['ip'], path),
                    'sshpass -p admin scp -r {} {} admin@192.168.1.1:{}'.format(self.sshoptions, proxy_file_path, path)]

        self.run(commands)
        # remove copied file from proxy
        self.sftp.remove(proxy_file_path)

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='deploy')):
        parser.help = "multiap_sw standalone deploy module"
        parser.add_argument('modules', choices=['all'] + deploy_modules, nargs='+', help='module[s] to deploy')
        parser.add_argument("--file", "-f", help="only upload a file to target")
        parser.add_argument("--path", "-p", default="/tmp/multiap/deploy/", help="path to deploy on in target")
        parser.add_argument("--conf", "-c", help="path to beerocks_dist.conf")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)

# Not used - only for reference if needed in the future (we always tar and send a single tar file, no need to copy like this!)
# def __put_dir__(self, sftp, source, target):
#     ''' Uploads the contents of the source directory to the target path. The
#         target directory needs to exists. All subdirectories in source are 
#         created under target.
#     '''
#     for item in os.listdir(source):
#         if os.path.isfile(os.path.join(source, item)):
#             sftp.put(os.path.join(source, item), '%s/%s' % (target, item))
#         else:
#             try:
#                 sftp.mkdir('%s/%s' % (target, item))
#             except:
#                 pass
#             __put_dir__(sftp, os.path.join(source, item), '%s/%s' % (target, item))
