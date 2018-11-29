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
        modules_dir = os.path.realpath(args.map_path)
        build_dir = os.path.realpath(modules_dir + '/build')
        conf_file = args.conf if args.conf else os.path.realpath(modules_dir + '/deploy.yaml')
        logger.debug("modules_dir={}, build_dir={}, conf={}".format(modules_dir, build_dir, conf_file))
        with open(conf_file, 'r') as f: self.conf = yaml.load(f)
        logger.debug("deploy configuration: {}".format(self.conf))
        logger.info("deploy {}".format(modules))

        for name in modules:
            logger.debug("deploy {}".format(name))

        self.upload("{}/controller/beerocks_v1.4_controller.tar.gz".format(build_dir), "/tmp/multiap/deploy/")

    def upload(self, file, path):
        ''' Uploads a file to the target vi proxy
            file - local file
            path - path in target
        '''
        proxy = self.conf['proxy']
        target = self.conf['target']
        logger.info("Upload {} to {}:{} via {}".format(os.path.basename(file), target['ip'], path, proxy['ip']))

        # start sftp connection
        t = paramiko.Transport((proxy['ip'], 22))
        t.connect(username=proxy['user'], password=proxy['pass'])
        sftp = paramiko.SFTPClient.from_transport(t)

        # start ssh connection
        client = paramiko.SSHClient()
        client.load_system_host_keys()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect(self.conf['proxy']['ip'], port=22, username=proxy['user'], password=proxy['pass'])
    
        # copy to proxy via sftp
        proxy_file_path = '/tmp/%s' % (os.path.basename(file))
        sftp.put(file, proxy_file_path)

        # copy from proxy to target via ssh
        sshoptions = "-oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no"
        commands = ['sshpass -p {} ssh {} {}@{} "mkdir -p {}"'.format(target['pass'], sshoptions, target['user'], target['ip'], path),
                    'sshpass -p admin scp -r {} {} admin@192.168.1.1:{}'.format(sshoptions, proxy_file_path, path)]
        
        for c in commands:
            logger.debug("@{} --> {}".format(proxy['ip'], c))
            stdin, stdout, stderr = client.exec_command(c)
            if stdout.channel.recv_exit_status():
                logger.error("Failed executing command: exit code - {}, stderr - {}".format(stdout.channel.recv_exit_status(), stderr.read()))

        # remove copied file from proxy
        sftp.remove(proxy_file_path)

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='deploy')):
        parser.help = "multiap_sw standalone deploy module"
        parser.add_argument('modules', choices=['all'] + deploy_modules, nargs='+', help='module[s] to deploy')
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
