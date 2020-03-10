import logging
import argparse
import os
import shutil
import yaml
import glob
import hashlib
import errno
import tarfile

logging.getLogger("paramiko").setLevel(logging.WARNING)
logger = logging.getLogger("deploy")
deploy_modules = ['framework', 'common', 'controller', 'agent']


class mapconnect(object):
    SSHOPTIONS = "-oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no"  # default ssh options

    def __init__(self, conf):
        self.proxy = conf['proxy']
        self.target = conf['target']
        self.__connect__()

    def __connect__(self):
        ''' connect sftp and ssh clients '''

        import paramiko
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
        self.client.connect(self.proxy['ip'], port=22,
                            username=self.proxy['user'], password=self.proxy['pass'])

    def run(self, commands=[]):
        ''' Run commands on target via proxy '''
        for c in commands:
            logger.debug("@{} --> {}".format(self.proxy['ip'], c))
            stdin, stdout, stderr = self.client.exec_command(c)
            if stdout.channel.recv_exit_status():
                logger.error("Command failed:\n{}\nexit code - {}, stderr - {}".format(
                    c, stdout.channel.recv_exit_status(), stderr.read()))

    def upload(self, files, path):
        ''' Uploads files to the target via proxy
            files - local files
            path - path in target
        '''
        # update list - (local_path, proxy_path, perm, md5sum)
        update_list = [(os.path.abspath(f), '/tmp/%s' % (os.path.basename(f)),
                        format(os.stat(f).st_mode & 0o0777, 'o'),
                        hashlib.md5(open(f, 'rb').read()).hexdigest()) for f in files]

        # copy to proxy
        for lpath, ppath, perm, md5sum in update_list:
            logger.info("Upload {} (md5sum {}) to {}:{} via {}".format(
                lpath, md5sum, self.target['ip'], path, self.proxy['ip']))
            self.sftp.put(lpath, ppath)

        ssh_cmd_template = 'sshpass -p {} ssh {} {}@{}'.format(
            self.target['pass'], mapconnect.SSHOPTIONS, self.target['user'], self.target['ip'])
        mkdir_cmd = '{} "mkdir -p {}"'.format(ssh_cmd_template, path)
        scp_cmd = 'sshpass -p {} scp -r {} {} {}@{}:{}'.format(
            self.target['pass'],
            mapconnect.SSHOPTIONS,
            ' '.join(['{}'.format(ppath) for lpath, ppath, perm, md5sum in update_list]),
            self.target['user'],
            self.target['ip'],
            path)
        chmod_cmd = '{} "{}"'.format(
            ssh_cmd_template,
            ';'.join(['chmod {} {}'.format(
                perm,
                os.path.join(path, os.path.basename(lpath)))
                for lpath, ppath, perm, md5sum in update_list]))

        # run commands at proxy
        self.run([mkdir_cmd, scp_cmd, chmod_cmd])

        # delete from proxy
        for lpath, ppath, perm, md5sum in update_list:
            self.sftp.remove(ppath)


class mapcopy(object):
    def __init__(self, args):
        if args.verbose:
            logger.setLevel(logging.DEBUG)
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


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def reset(tarinfo):
    tarinfo.uid = tarinfo.gid = 0
    tarinfo.uname = tarinfo.gname = "root"
    return tarinfo


class mapdeploy(object):
    def __init__(self, args):
        if args.verbose:
            logger.setLevel(logging.DEBUG)
        modules = deploy_modules if 'all' in args.modules else [
            m for m in deploy_modules if m in args.modules]
        self.modules_dir = os.path.realpath(args.map_path)
        self.build_dir = os.path.realpath(self.modules_dir + '/build')
        self.pack_dir = os.path.realpath(self.build_dir + '/pack')

        conf_file = args.conf if args.conf else os.path.realpath(self.modules_dir + '/deploy.yaml')
        logger.debug("modules_dir={}, build_dir={}, conf={}".format(
            self.modules_dir, self.build_dir, conf_file))
        with open(conf_file, 'r') as f:
            self.conf = yaml.load(f)
            if not args.pack_only:
                self.connect = mapconnect(self.conf)
            self.os = self.conf['target']['type']

        logger.debug("deploy configuration: {}".format(self.conf))
        logger.info("{} deploy {}".format(self.os, modules))

        if os.path.exists(self.pack_dir):
            logger.info("Delete previous packing {}".format(self.pack_dir))
            shutil.rmtree(self.pack_dir)

        for m in modules:
            logger.debug("pack {}".format(m))
            self.pack(m)

        # create multiap_deploy.tar.gz
        archive = os.path.join(self.pack_dir, "multiap_deploy.tar.gz")
        with tarfile.open(archive, "w:gz") as tar:
            tar.add(self.pack_dir, arcname='/', filter=reset)
        deploy_sh = os.path.dirname(os.path.realpath(__file__)) + '/deploy_%s.sh' % self.os
        shutil.copy(deploy_sh, os.path.join(self.pack_dir, os.path.basename(deploy_sh)))

        # upload to target
        if not args.pack_only:
            self.connect.upload([archive, deploy_sh], args.path)

    def pack(self, name):
        pack_dir = self.pack_dir
        out_dir = os.path.join(self.build_dir, name, 'out')
        pack_dirs = [(d, self.conf['deploy'][self.os][name][d])
                     for d in os.listdir(out_dir) if os.path.isdir(os.path.join(out_dir, d))]

        for src, dst in pack_dirs:
            src_path = os.path.join(out_dir, src)
            dst_path = os.path.join(pack_dir, os.path.relpath(dst, '/'))
            mkdir_p(dst_path)
            files = glob.glob('{}/*'.format(src_path))
            for f in files:
                md5sum = hashlib.md5(open(f, 'rb').read()).hexdigest()
                logger.debug("packing {} (md5sum {})".format(f, md5sum))
                if (os.path.islink(f)):
                    os.symlink(os.readlink(f), os.path.join(dst_path, os.path.basename(f)))
                else:
                    shutil.copy(os.path.abspath(f), os.path.join(dst_path, os.path.basename(f)))

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='deploy')):
        parser.help = "multiap_sw standalone deploy module"
        parser.add_argument('modules', choices=['all'] +
                            deploy_modules, nargs='+', help='module[s] to deploy')
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")
        parser.add_argument("--pack-only", action="store_true",
                            help="only pack multiap for later deployment "
                                 "(multiap_deploy.tar.gz and deploy.sh)")
        parser.add_argument("--path", "-p", default="/tmp/multiap/deploy/",
                            help="path to deploy on in target")
        parser.add_argument("--conf", "-c", help="path to deploy.yaml")

        return parser

    def __str__(self):
        return str(self.args)
