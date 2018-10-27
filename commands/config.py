import logging
import argparse
import os
import getpass

class owrtcfg(object):
    def __init__(self, path):
        self.path = path
        self.cfg = {'CONFIG_BUILD_SUFFIX' : None,
                    'CONFIG_TARGET_NAME' : None,
                    'CONFIG_TOOLCHAIN_ROOT' : None,
                    'CONFIG_TOOLCHAIN_PREFIX' : None
                    }

    def __get_entry_value(self, lines, name):
        key = name + '='
        return next((s.split('=')[1].replace('\"', '').replace('\n', '') for s in lines if key in s), None)

    def parse(self):
        lines = None
        with open(os.path.realpath(self.path), "r") as c:
                lines = c.readlines()
        for key in self.cfg:
            self.cfg[key] = self.__get_entry_value(lines, key)
            if not self.cfg[key]:
                raise Exception("Failed to parse {} from {}".format(key, self.path))
        
        return self.cfg

    def __str__(self):
        return "path={}\nvalues={}".format(self.path, self.cfg)

class chdlab_board(object):
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
    default_out_path = os.path.abspath(os.path.dirname(__file__)+'/../../') #one dir above the tools repo
    supported_targets = ["grx350"]

    def __init__(self, args):
        self.logger = logging.getLogger("config")
        self.args = args
        if self.args.guess:
            if not self.args.toolchain_path: self.args.toolchain_path = os.environ.get('UGW_CORE_DIR')
        
        if self.args.gui:
            self.__gui_start()
        else:
            self.__generate()

    def __gui_start(self):
        #from Tkinter import Tk, StringVar, Label, Entry, IntVar, CheckButton, Button
        import Tkinter
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
        self.args.board_id = self.gui_board_ent.get()
        self.args.toolchain_path = self.gui_ugw_path_ent.get()
        self.args.no_overwrite = self.gui_no_overwrite_ent.get()
        self.gui_master.quit()
        self.__generate()

    def __gen_external_toolchain_conf(self, config_path, toolchain_path, cfg):
        ''' Generate external_toolchain.cfg (for multiap repos compilation and deploy) '''
        
        out_file = config_path+'/external_toolchain.cfg'
        self.logger.info("Generate {}".format(out_file))
        if os.path.isfile(out_file):
            if self.args.no_overwrite:
                raise Exception("{} already exist and no_overwrite set".format(out_file))
            self.logger.info("Overriding {}".format(out_file))
        
        with open(out_file, 'w') as f:
            f.write('TARGET_PLATFORM=ugw\n')
            f.write('PLATFORM_TOOLCHAIN_PREFIX={}/bin/{}\n'.format(cfg['CONFIG_TOOLCHAIN_ROOT'], cfg['CONFIG_TOOLCHAIN_PREFIX']))
            f.write('PLATFORM_BASE_DIR={}\n'.format(toolchain_path))
            f.write('PLATFORM_BUILD_NAME=target-{}_{}\n'.format(cfg['CONFIG_TARGET_NAME'], cfg['CONFIG_BUILD_SUFFIX']))

    def __gen_beerocks_dist_conf(self, output_path, toolchain_path, cfg):
        ''' Generate beerocks_dist.conf (for beerocks compilation and deploy) '''

        out_file = output_path+'/beerocks_dist.conf'
        self.logger.info("Generate {}".format(out_file))
        if os.path.isfile(out_file):
            if self.args.no_overwrite:
                raise Exception("{} already exist and no_overwrite set".format(out_file))
            self.logger.info("Overriding {}".format(out_file))
        
        target = self.args.target
        if not target or target not in mapcfg.supported_targets:
            raise Exception("target {} not supported for writing beerocks_dist.conf".format(target))

        ssh_deploy_pc = self.args.ssh_deploy_pc
        if not ssh_deploy_pc: self.logger.warning("ssh_deploy_pc missing")
        ssh_deploy_gw = self.args.ssh_deploy_gw
        if not ssh_deploy_gw: self.logger.warning("ssh_deploy_gw missing")

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
            self.logger.warning("Invalid toolchain_path={}".format(toolchain_path))
        
        config = owrtcfg(toolchain_path+'/.config').parse()
        if self.args.board_id:
            try: 
                board = chdlab_board(self.args.board_id, self.args.user)
            except RuntimeError as e:
                self.logger.error("board jira failure (%s)" %e)
                raise
            self.args.ssh_deploy_gw = board.get_ssh_deploy_gw()
            self.args.ssh_deploy_pc = board.get_ssh_deploy_pc()
            self.args.target = board.get_target()

        self.__gen_beerocks_dist_conf(self.args.output_path, toolchain_path, config)
        self.__gen_external_toolchain_conf(self.args.output_path, toolchain_path, config)
        

    @staticmethod
    def configure_parser(parser=argparse.ArgumentParser(prog='config')):
        parser.help = "configure multiap standalone build"
        parser.add_argument("--toolchain_path", "-p", help="path to openwrt/core")
        parser.add_argument("--output_path", "-O", default=mapcfg.default_out_path, help="path to save output config files")
        parser.add_argument("--board_id", "-b", help="chdlab board id (requires chdlab python package)")
        parser.add_argument("--target", "-t", default="grx350", help="target platform")
        parser.add_argument("--ssh_deploy_pc", help="ssh deploy pc")
        parser.add_argument("--ssh_deploy_gw", default="admin:admin@192.168.1.1", help="ssh deploy gw")
        parser.add_argument("--no_overwrite", "-o", action="store_true", help="do not overwrite existing configuration")
        parser.add_argument("--gui", "-g", action="store_true", help="run conifuration helper gui")
        parser.add_argument("--guess", default=True, help="try to guess configuration parameters from environment variables")
        parser.add_argument("--verbose", "-v", action="store_true", help="verbosity on")

        return parser

    def __str__(self):
        return str(self.args)
