import logging
import argparse
import os
import getpass
import re
import yaml

logger = logging.getLogger("config")

class owrtcfg(object):
    def __init__(self, path):
        self.path = path
        self.dotconfig = '{}/.config'.format(path)
        self.cfg = {'CONFIG_BUILD_SUFFIX' : None,
                    'CONFIG_TARGET_NAME' : 'mips_mips32_uClibc-0.9.33.2',
                    'CONFIG_TOOLCHAIN_ROOT' : '{}/staging_dir/toolchain-mips_mips32_gcc-4.8-linaro_uClibc-0.9.33.2'.format(path),
                    'CONFIG_TOOLCHAIN_PREFIX' : 'mips-openwrt-linux-uclibc-',
                    'CONFIG_TARGET_lantiq_xrx500_easy350_anywan_axepoint' : None
                    }

    def __get_entry_value(self, lines, name):
        key = name + '='
        return next((s.split('=')[1].replace('\"', '').replace('\n', '') for s in lines if key in s), None)

    def parse(self):
        lines = None
        with open(os.path.realpath(self.dotconfig), "r") as c:
                lines = c.readlines()
        for key in self.cfg:
            value = self.__get_entry_value(lines, key)
            if value: self.cfg[key] = value
            if not self.cfg[key]:
                logger.error("Failed to parse {} from {}".format(key, self.dotconfig))
        
        return self.cfg

    def __str__(self):
        return "path={}\nvalues={}".format(self.path, self.cfg)

class chdlabv3(object):
    def __init__(self, board, setup=None, user=None):
        from chdlab.commands.config import CHDLAB_config
        from chdlab.jira_wrappers.parent_jira import _ParentJira
        cfg = CHDLAB_config(cache=False, setup_number=setup, fetch=True)
        _ParentJira.set_config(cfg)
        self.cfg = cfg
        self.board_id = board

    def __str__(self):
        return self.cfg

    @property
    def jira(self):
        from chdlab.jira_wrappers.board_jira import BoardJira
        return BoardJira(name=self.board_id)

    def get_ssh_deploy_pc(self):
        vm = self.jira.linked_lan_vms[0]
        return "{}:{}@{}".format(vm.password, vm.username, vm.management_ip)

    def get_ssh_deploy_gw(self):
        return "{}:{}@{}".format(self.jira.password, self.jira.username, self.jira.ip)

    def get_target(self):
        return self.jira.platform.lower()

class chdlabv2(object):
    def __init__(cls, board, user=None):
        from chdlab_commands.jira_tools.jira_board import JiraBoard
        cls.jira = JiraBoard(board)

    def __str__(self):
        return self.jira

    def get_ssh_deploy_pc(cls):
        return "libit:libit@{}".format(cls.jira.get_lan_vms()[0].management_ip)

    def get_ssh_deploy_gw(cls):
        return "admin:admin@192.168.1.1"

    def get_target(cls):
        return cls.jira.type.lower()

class mapcfg(object):
    supported_targets = ["grx350", "axepoint", "grx750"]

    def __guess_target(self):
        guess = os.environ.get('rdkb_root')
        if guess:
            logger.info("guessed target=cgr since rdkb_root environment variable defined")
            return 'grx750'

    def __guess_setup_id(self):
        try:
            from chdlab.commands.config import CHDLAB_config
            return CHDLAB_config().setup_number
        except:
            logger.warning("chdlabv3 not supported, ignoring setup key")
            pass

    def __guess_toolchain_path(self):
        guess = os.environ.get('rdkb_root')
        if guess:
            logger.info("guessed external_toolchain_path from rdkb_root environment variable: {}/atom_rdkbos/build/tmp/sysroots".format(guess))
            return "{}/atom_rdkbos/build/tmp/sysroots".format(guess)

        guess = os.environ.get('UGW_CORE_DIR')
        if guess:
            logger.info("guessed external_toolchain_path from UGW_CORE_DIR environment variable: {}".format(guess))
            return guess

        guess = self.__get_external_toolchain_path(self.args.map_path)
        if guess:
            logger.info("guessed external_toolchain_path from previous config: {}".format(guess))
            return guess

    def __init__(self, args):
        self.args = args
        if self.args.guess:
            if not self.args.target:
                self.args.target = self.__guess_target()
            if not self.args.toolchain_path:
                self.args.toolchain_path = self.__guess_toolchain_path()
            if not self.args.setup_id:
                self.args.setup_id = self.__guess_setup_id()
        
        if self.args.gui:
            self.__gui_start()
        else:
            self.__generate()

    def __gui_start(self):
        try: import Tkinter
        except: import tkinter as Tkinter
        self.gui_master = Tkinter.Tk()
        self.gui_master.title("multiap configuration gui")
        
        self.gui_status = Tkinter.StringVar()
        self.gui_status.set("multiap configuration")
        self.gui_status_label = Tkinter.Label(self.gui_master, textvariable=self.gui_status)
        self.gui_status_label.pack()

        self.gui_user_label = Tkinter.Label(self.gui_master, text="User ID")
        self.gui_user_label.pack()
        self.gui_user_str = Tkinter.StringVar()
        if self.args.user: self.gui_user_str.set(self.args.user)
        else: user = self.gui_user_str.set(getpass.getuser())
        self.gui_user_ent = Tkinter.Entry(self.gui_master, textvariable=self.gui_user_str)
        self.gui_user_ent.pack()

        self.gui_setup_label = Tkinter.Label(self.gui_master, text="Setup ID")
        self.gui_setup_label.pack()
        self.gui_setup_str = Tkinter.StringVar()
        if self.args.setup_id: self.gui_setup_str.set(self.args.setup_id)
        self.gui_setup_ent = Tkinter.Entry(self.gui_master, textvariable=self.gui_setup_str)
        self.gui_setup_ent.pack()

        self.gui_board_label = Tkinter.Label(self.gui_master, text="Board ID")
        self.gui_board_label.pack()
        self.gui_board_str = Tkinter.StringVar()
        if self.args.board_id: self.gui_board_str.set(self.args.board_id)
        self.gui_board_ent = Tkinter.Entry(self.gui_master, textvariable=self.gui_board_str)
        self.gui_board_ent.pack()

        self.gui_ugw_path_label = Tkinter.Label(self.gui_master, text="path to openwrt/core")
        self.gui_ugw_path_label.pack()
        self.gui_ugw_path_str = Tkinter.StringVar()
        if self.args.toolchain_path and os.path.exists(self.args.toolchain_path): self.gui_ugw_path_str.set(os.path.realpath(self.args.toolchain_path))
        self.gui_ugw_path_ent = Tkinter.Entry(self.gui_master, textvariable=self.gui_ugw_path_str, width=55)
        self.gui_ugw_path_ent.pack()

        self.gui_no_overwrite_ent = Tkinter.IntVar()
        self.gui_no_overwrite_ent.set(self.args.no_overwrite)
        self.gui_no_overwrite_button = Tkinter.Checkbutton(self.gui_master, text="Don't override existing configuration", variable = self.gui_no_overwrite_ent)
        self.gui_no_overwrite_button.pack()

        self.gui_submit_button = Tkinter.Button(self.gui_master, text="Submit", command=self.__gui_submit)
        self.gui_submit_button.pack(side=Tkinter.LEFT)

        self.gui_cancel_button = Tkinter.Button(self.gui_master, text="Cancel", command=self.gui_master.quit)
        self.gui_cancel_button.pack(side=Tkinter.RIGHT)

        # start gui
        self.gui_master.mainloop()

    def __gui_submit(self):
        self.args.user = self.gui_user_ent.get()
        self.args.setup_id = self.gui_setup_ent.get()
        self.args.board_id = self.gui_board_ent.get()
        self.args.toolchain_path = self.gui_ugw_path_ent.get()
        self.args.no_overwrite = self.gui_no_overwrite_ent.get()
        self.gui_master.quit()
        self.__generate()

    def __get_external_toolchain_path(self, config_path):
        in_file = config_path+'/external_toolchain.cfg'
        if not os.path.isfile(in_file):
            return None

        with open(in_file, 'r') as f:
            for line in f.readlines():
                arg, value = line.strip().split("=")
                if arg == 'PLATFORM_BASE_DIR': return value

        return None

    def __gen_external_toolchain_conf(self, target, toolchain_prefix, base_dir, build_name):
        ''' Generate external_toolchain.cfg (for multiap repos compilation and deploy) '''
        
        out_file = self.args.map_path + '/external_toolchain.cfg'
        logger.info("Generate {}".format(out_file))
        if os.path.isfile(out_file):
            if self.args.no_overwrite:
                raise Exception("{} already exist and no_overwrite set".format(out_file))
            logger.info("Overriding {}".format(out_file))
        
        with open(out_file, 'w') as f:
            f.write('TARGET_PLATFORM={}\n'.format(target))
            f.write('PLATFORM_TOOLCHAIN_PREFIX={}\n'.format(toolchain_prefix))
            f.write('PLATFORM_BASE_DIR={}\n'.format(base_dir))
            f.write('PLATFORM_BUILD_NAME={}\n'.format(build_name))
            f.write('BWL_TYPE=DWPAL')

    def __gen_deploy_yaml(self, output_path, toolchain_path):
        ''' Generate deploy.yaml (for multiap deploy) '''

        template = os.path.dirname(os.path.realpath(__file__)) + '/deploy.template.yaml'
        out_file = output_path+'/deploy.yaml'
        logger.info("Generate {}".format(out_file))
        if os.path.isfile(out_file):
            if self.args.no_overwrite:
                raise Exception("{} already exist and no_overwrite set".format(out_file))
            logger.info("Overriding {}".format(out_file))
        
        proxy = {}
        proxy['pass'], proxy['user'], proxy['ip'] = re.split(':|@', self.args.ssh_deploy_pc)
        logger.info("proxy: {}".format(proxy))

        target = {}
        if not self.args.target or self.args.target not in mapcfg.supported_targets:
            raise Exception("target {} not supported for writing beerocks_dist.conf".format(self.args.target))
        target['type'] = self.args.target
        target['pass'], target['user'], target['ip'] = re.split(':|@', self.args.ssh_deploy_gw)
        logger.info("target: {}".format(target))

        with open(out_file, 'w') as f, open(template, 'r') as t:
            yaml.dump({'target' : target, 'proxy' : proxy}, f, default_flow_style=False)
            yaml.dump(yaml.load(t), f, default_flow_style=False)

    def __gen_beerocks_dist_conf(self, output_path, toolchain_path, cfg):
        ''' Generate beerocks_dist.conf (for beerocks compilation and deploy) '''

        out_file = output_path+'/beerocks_dist.conf'
        logger.info("Generate {}".format(out_file))
        if os.path.isfile(out_file):
            if self.args.no_overwrite:
                raise Exception("{} already exist and no_overwrite set".format(out_file))
            logger.info("Overriding {}".format(out_file))
        
        target = self.args.target
        if not target or target not in mapcfg.supported_targets:
            raise Exception("target {} not supported for writing beerocks_dist.conf".format(target))

        ssh_deploy_pc = self.args.ssh_deploy_pc
        if not ssh_deploy_pc: logger.warning("ssh_deploy_pc missing")
        ssh_deploy_gw = self.args.ssh_deploy_gw
        if not ssh_deploy_gw: logger.warning("ssh_deploy_gw missing")

        with open(out_file, 'w') as f:
            f.write('[global]\n')
            f.write('BEEROCKS_TARGET={}\n'.format(target))
            f.write('BEEROCKS_PACK=ON\n')
            f.write('\n')
            f.write('[target_{}]\n'.format(target))
            f.write('BEEROCKS_UGW_CORE={}\n'.format(toolchain_path))
            f.write('BEEROCKS_UGW_PLATFORM_BUILD_NAME=target-{}_{}\n'.format(cfg['CONFIG_TARGET_NAME'], cfg['CONFIG_BUILD_SUFFIX']))
            f.write('BEEROCKS_UGW_EXTERNAL_TOOLCHAIN_DIR={}\n'.format(cfg['CONFIG_TOOLCHAIN_ROOT']))
            f.write('\n')
            f.write('[build]\nssh_enabled=0\nssh_user=\nssh_host=\nssh_cmd=\nmake_args=-j28\n\n')
            f.write('[deploy]\n')
            f.write('ssh_deploy_pc={}\n'.format(ssh_deploy_pc))
            f.write('ssh_deploy_gw={}\n'.format(ssh_deploy_gw))

    def __generate(self):
        toolchain_path = self.args.toolchain_path
        if not toolchain_path or not os.path.exists(toolchain_path):
            raise Exception("Invalid toolchain_path={}".format(toolchain_path))
        
        if self.args.board_id:
            try:
                board = chdlabv2(self.args.board_id, self.args.user)
            except ImportError as e:
                board = chdlabv3(self.args.board_id, self.args.setup_id, self.args.user)
            except RuntimeError as e:
                logger.error("board jira failure (%s)" %e)
                raise
            self.args.ssh_deploy_gw = board.get_ssh_deploy_gw()
            self.args.ssh_deploy_pc = board.get_ssh_deploy_pc()
            self.args.target = board.get_target()

        if 'grx750' in self.args.target:
            target = 'rdkb'
            toolchain_prefix = '{}/x86_64-linux/usr/bin/i686-rdk-linux/i686-rdk-linux-'.format(toolchain_path)
            build_name = 'puma7-atom'
        else:
            cfg = owrtcfg(toolchain_path).parse()
            target = 'ugw'
            toolchain_prefix = 'PLATFORM_TOOLCHAIN_PREFIX={}/bin/{}\n'.format(cfg['CONFIG_TOOLCHAIN_ROOT'], cfg['CONFIG_TOOLCHAIN_PREFIX'])
            build_name = 'PLATFORM_BUILD_NAME=target-{}_{}\n'.format(cfg['CONFIG_TARGET_NAME'], cfg['CONFIG_BUILD_SUFFIX'])

        try: self.__gen_deploy_yaml(self.args.map_path, toolchain_path)
        except Exception as e: logger.error("failed to generate deploy.conf - {}".format(e))
        self.__gen_external_toolchain_conf(target, toolchain_prefix, toolchain_path, build_name)

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='config')):
        parser.help = "configure multiap standalone build"
        parser.add_argument("--toolchain_path", "-p", help="path to openwrt/core")
        parser.add_argument("--setup_id", "-s", help="chdlab setup id (requires chdlab python package)")
        parser.add_argument("--board_id", "-b", help="chdlab board id (requires chdlab python package)")
        parser.add_argument("--target", "-t", help="target platform")
        parser.add_argument("--ssh_deploy_pc", help="ssh deploy pc")
        parser.add_argument("--ssh_deploy_gw", default="admin:admin@192.168.1.1", help="ssh deploy gw")
        parser.add_argument("--no_overwrite", "-o", action="store_true", help="do not overwrite existing configuration")
        parser.add_argument("--gui", action="store_true", help="run conifuration helper gui")
        parser.add_argument("--guess", "-g", action="store_true", help="try to guess configuration parameters from environment variables")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)
