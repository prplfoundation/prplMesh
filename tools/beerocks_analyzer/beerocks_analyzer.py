#!/usr/bin/env python3
import logging
import subprocess
import sys
import os
import ctypes
import threading
import socket
import time
from ipaddress import ip_address, IPv4Address
from pathlib import Path
from typing import Union

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *

from os import waitpid, execv, read, write

from random import randint
import paramiko
import imp
import signal
from multiprocessing import Process
from threading import Thread
import struct

import logger_setup

VERSION="3.3"

t_list=[]
g_run_flag=True
g_ssh=None
g_ext_log_file=False
g_marker_update=False
LOG_FILE="beerocks_analyzer.log"

bra = None
app = None

logger_setup.setup_logger()
logger = logging.getLogger(__name__)

def printUsage():
    logger.info("usage: BeeRocksAnalyzer [options]")
    logger.info("    -f=<input log file>                                         - input log file")
    logger.info("    -map                                                  - start connectivity map")
    logger.info("    -map_win                                              - start connectivity map on a separare window")
    logger.info("    -conf=<conf_file_name>                                - run configuration from ./conf/conf_file_name")
    logger.info("                                                          - can execute with several -conf=<conf_file_name1> -conf=<conf_file_name2> ...")
    logger.info("    -gw_ip=<IP of GW>                                     - GW ip to get updates from. If empty, the GW is assumed to be running locally")
    logger.info("    -bin_path=<path_to_beerocks_cli>                      - Path to the beerocks_cli binary. If empty, use build from <top_level>/../")
    logger.info("    -my_ip=[IP of PC]                                     - PC ip to send updates, if not set will automaticly set")
    logger.info("    -ssh_port=[ssh port of GW]                            - SSH port used to connect to GW")

def signal_handler(signal, frame):
    global t_list, g_run_flag

    logger.info("Signal received: " + str(signal))
    for t in t_list:
        try:
            t.terminate()
        except Exception as e:
            logger.warning("Exception when trying to terminate thread: " + str(e))
            pass
    g_run_flag=False

    global g_ssh
    if g_ssh != None:
        g_ssh.close()

class UpdateSig(QObject):
    sig = Signal(float)

class BeeRocksAnalyzer(QMainWindow):
    def __init__(self, argv, parent=None):
        super(BeeRocksAnalyzer, self).__init__(parent)
        self.argv=argv
        self.isPlay=False
        self.restart=False
        self.restartDone=True

        self.log_start_pos=0
        self.wa_widget = None
        self.wa_widget_mod = None
        self.cm_widget = None
        self.cm_widget_mod = None
        self.log_widget = None
        self.log_widget_mod = None

        self.ap_mac2num = {}
        self.sta_mac2num = {}

        self.fdLog = None
        self.fname="beerocks_analyzer.log"
        self.isMap = False
        self.isGraphs = False
        self.isMapSepWin = False
        self.isExtLogFile = False
        self.isOnline = False
        self.wait_for_mark = False
        for i in range(0,len(argv)):
            arg_i = argv[i].strip()
            if arg_i.startswith("-f="):
                self.fname = arg_i.split("=")[1]
            if arg_i.startswith("-map"):
                self.isMap = True
                if "win" in arg_i:
                    self.isMapSepWin = True
            if arg_i.startswith("-graphs"):
                self.isGraphs = True
            if arg_i.startswith("-ext_log_file"):
                self.isExtLogFile = True
        if self.isExtLogFile:
            self.argv.remove("-ext_log_file")
        else:
            self.isOnline = True

        self.timer = QTimer(self)
        self.timer.setSingleShot(True)
        self.connect(self.timer, SIGNAL("timeout()"), self.timerEvent)

        self.timeUpdateSig = UpdateSig()
        self.timeUpdateSig.sig.connect(self.timeUpdateSlot)

        self.fileTime = 0.0
        self.runTime = 0.0

        self.threadExit = False
        self.thread = None

        self.main_widget = QSplitter(Qt.Horizontal)
        self.main_vsplitter = QSplitter(Qt.Vertical)
        self.main_vsplitter.addWidget(self.main_widget)

        self.createMainFrame()
        self.createMenu()

        self.setCentralWidget(self.main_vsplitter)

        self.createLoggerWidget(2)

        if self.isGraphs:
            self.createAnalyzerWidget(0)
        if self.isMap:
            self.createConnMapWidget(1)

        if not self.isExtLogFile:
            self.playPauseButtonTrig()

    def closeEvent(self, event):
        self.stopReadSampleThread()
        self.closeLogFiles()
        try: self.log_widget.close()
        except: pass
        if self.isGraphs:
            try: self.wa_widget.close()
            except: pass
        if self.isMap:
            try: self.cm_widget.close()
            except: pass
        QMainWindow.closeEvent(self, event)

    def getLogFilePos(self):
        return self.fdLog.tell()

    def closeLogFiles(self):
        if self.fdLog != None:
            self.fdLog.close()
            self.fdLog=None

    def openLogFiles(self, log_start_pos=0):
        self.closeLogFiles()
        if self.fname != None:
            try: # open log file
                self.fdLog = open(self.fname,"rt")
                if log_start_pos > 0:
                    logger.debug("openLogFiles: seek to {:d}", log_start_pos)
                    self.fdLog.seek(log_start_pos, 0)
                elif log_start_pos < 0:
                    logger.debug("openLogFiles: seek to end of file", log_start_pos)
                    self.fdLog.seek(0, 2)
            except:
                self.fdLog = None
            if self.fdLog == None:
                logger.error("Error(1): can't open file {}".format(self.fname))
                sys.exit(0)

    def startReadSampleThread(self, update_start_time=False):
        self.start_time = time.time() - self.fileTime
        self.stopReadSampleThread()
        self.thread = threading.Thread(target=self.readSampleThread, args=(update_start_time,))
        self.thread.start()

    def stopReadSampleThread(self):
        if self.thread != None:
            if self.thread.isAlive():
                self.threadExit=True
                try:
                    self.thread.join(2)
                except: pass
            del self.thread
            self.thread = None
            return True
        else:
            return True

    def readSample(self, line):
        param_n = []
        param_v = []
        param_t = -1
        is_start=False
        is_stop=False
        is_map_update=False
        res=""

        if line.startswith("#") or len(line) < 4 : 
            return [param_t, is_start, is_stop, is_map_update, res]

        i1 = line.find("|")
        if(i1 == -1) : 
            return [param_t, is_start, is_stop, is_map_update, res]

        param_t = float(line[0:i1])

        i2=line.find(",")
        if i2 == -1: # START or STOP or error
            try:
                param_m = (str(line[i1+1:])).strip()
                if param_m=="MARK" and self.wait_for_mark:
                    self.wait_for_mark=False
                if param_m=="START" or  param_m=="MARK":
                    is_start=True
                elif param_m=="STOP":
                    is_stop = True
                else:
                    logger.error("readSample() 1 line --> {}\n", line)
            except Exception as e : # TODO: too broad exception
                logger.error("readSample() 1 line --> {}\n{}", line, str(e))

            return [param_t, is_start, is_stop, is_map_update, res]
        else:
            try:
                param_m = str(line[i1+1:i2])
                # read line's key,value: param_n<-key, param_v<-value
                line_args = line[i2+1:].split(',')
                param_n = []
                for arg in line_args:
                    arg_val = arg.split(':')
                    key = arg_val[0].lower().strip()
                    param_n.append(key)
                    if ("mac" in key) or ("bssid" in key) or ("backhaul" in key):
                        param_v.append(arg.replace(":","=",1).split("=")[1].strip())
                    else:
                        param_v.append(arg_val[1].strip())
            except Exception as e:
                logger.error("readSample() 1 line --> {}".format(line))
                logger.exception(e)
                return [param_t, is_start, is_stop, is_map_update, res]

        if param_v == None: 
            return [param_t, is_start, is_stop, is_map_update, res]
        if not self.wait_for_mark:
            param_m_val = param_m.split(':')
            param_m_n = param_m_val[0].strip()

            if param_m_n != "type" and param_m_n != "Type":
                is_map_update = True

            if param_m_n == "Name": #nw_map_update
                try:
                    i_state=param_n.index('state')
                    i_mac=param_n.index('mac')
                    i_type=param_n.index('type')
                    if not ("1" in param_v[i_type]):
                        i_ap_mac=param_n.index('parent bssid')
                    
                except Exception as e: # TODO: too broad exception
                    logger.error("readSample()  --> {}, "
                                             "nw_map_update line does not contain state "
                                             "or mac or type or parent bssid".format(line))
                    logger.exception(e)
                    return [param_t, is_start, is_stop, is_map_update, res]

                state = (param_v[i_state].split())[0]
                mac = param_v[i_mac]
                line_type = param_v[i_type]
                if not ("1" in line_type):
                    ap_mac = param_v[i_ap_mac]
                
                if "2" in line_type: #IRE
                    try:
                        i_backhaul=param_n.index('backhaul')
                        mac = param_v[i_backhaul] #for IRE, the backhaul mac is the "client" mac
                    except Exception as e: # TODO: too broad exception
                        logger.error("readSample()  --> {}, "
                                                 "nw_map_update IRE line does not contain "
                                                 "a backhaul mac address\n{}".format(line, str(e)))
                        return [param_t, is_start, is_stop, is_map_update, res]
                if ("2" in line_type) or ("3" in line_type): #IRE or client
                    if state == "Connected":
                        if not(self.sta_mac2num.has_key(mac)): # new sta mac addr
                            self.sta_mac2num[mac] = len(self.sta_mac2num)
                        res += "sta_id: %d,"%self.sta_mac2num[mac]
                        
                        if not(self.ap_mac2num.has_key(ap_mac)): # new ap mac addr
                            self.ap_mac2num[ap_mac] = len(self.ap_mac2num)
                        res += "ap_id: %d,"%self.ap_mac2num[ap_mac]
                        if res.endswith(','):
                            res = res[:-1]

            elif param_m_n == "type" or param_m_n == "Type":
                param_m_v = int(param_m_val[1].strip())
                try:
                    i1=param_n.index('mac')
                except Exception as e: # TODO: too broad exception
                    logger.error("readSample() --> {}, 'stats_update' line not contain mac address.\n{}".format(line, e))
                    return [param_t, is_start, is_stop, is_map_update, res]

                mac = param_v[i1]
                i1+=1
                if param_m_v == 1: #AP stats update
                    if not(self.ap_mac2num.has_key(mac)): # new ap mac addr
                        self.ap_mac2num[mac] = len(self.ap_mac2num)
                    res += "ap_id: %d"%self.ap_mac2num[mac]

                elif param_m_v == 3: #Client stats update
                    if not(self.sta_mac2num.has_key(mac)): # new sta mac addr
                        self.sta_mac2num[mac] = len(self.sta_mac2num)
                    res += "sta_id: %d"%self.sta_mac2num[mac]

        # End of readSample()
        return [param_t, is_start, is_stop, is_map_update, res]

    def readSampleThread(self, update_start_time = False):
        if self.isGraphs:
            self.wa_widget.update_start_time=True
        self.threadExit=False
        while not self.threadExit:
            if self.fdLog != None:
                update_widgets = True
                line = self.fdLog.readline()
                if not line:
                    time.sleep(1)
                    continue
                [param_t1, is_start, is_stop, is_map_update, sta_ap_ids] = self.readSample(line)
                if len(sta_ap_ids) > 0:
                    line += ", " + sta_ap_ids
                if is_stop:
                    logger.info("Read stop flag, stopping")
                    self.threadExit=True

                if param_t1 == -1:
                    time.sleep(0.1)
                elif param_t1 == 0.000:
                    logger.debug("readSampleThread() --> param_t1=0.000, updating self.start_time")
                    self.start_time = time.time()
                else:
                    if update_start_time:
                        logger.debug("updating start_time")
                        self.start_time = time.time() - float(param_t1)
                        update_start_time = False
                    self.runTime = round(time.time() - self.start_time, 3)
                    delta_t = self.fileTime - self.runTime
                    if delta_t > 0.1:
                        #if delta_t > 5:
                        logger.debug("readSampleThread() --> sleeping for {:.3f} seconds".format(delta_t))
                        time.sleep(delta_t)
                    elif (delta_t < -3) and (not is_map_update):
                        logger.debug("readSampleThread() --> (delta_t < -3) "
                                   "and (not is_map_update) -> update_widgets = False")
                        update_widgets = False
                    self.restartRuntime = False
                    self.timeUpdateSig.sig.emit(param_t1)
                    if update_widgets and not self.wait_for_mark:
                        if "BML_EVENT" in line:
                            self.log_widget.readSampleAndUpdateLogger(line, param_t1, is_start, is_stop)
                        else:
                            if self.isMap:
                                logger.debug("self.isMap, calling readSample")
                                self.cm_widget.readSample(line, param_t1, is_start, is_stop)
                            if self.isGraphs:
                                self.wa_widget.readSampleAndUpdateGraphs(line, param_t1, is_start, is_stop)

    def createAnalyzerWidget(self, widget_index=0):
        self.wa_widget_mod = __import__("beerocks_analyzer_widget")
        self.wa_widget = self.wa_widget_mod.BeeRocksAnalyzerWidget(self.argv)
        self.wa_widget.restartSig.sig.connect(self.resetFromLog)
        self.openLogFiles(self.log_start_pos)
        self.main_widget.insertWidget(widget_index, self.wa_widget)

    def createConnMapWidget(self, widget_index=0):
        self.cm_widget_mod = __import__("connectivity_map_widget")
        self.cm_widget = self.cm_widget_mod.ConnectivityMapWidget()
        self.cm_widget.restartSig.sig.connect(self.resetFromLog)
        self.openLogFiles(self.log_start_pos)
        if self.isMapSepWin:
            self.cm_widget.setWindowTitle("BeeRocks Analyzer V%s" % VERSION) 
            self.cm_widget.setMinimumSize(800,600)
            self.cm_widget.move(0,0)
            self.cm_widget.show()
        else:
            self.main_widget.insertWidget(widget_index, self.cm_widget)

    def createLoggerWidget(self, widget_index=0):
        self.log_widget_mod = __import__("logger_widget")
        self.log_widget = self.log_widget_mod.LoggerWidget()
        self.log_widget.restartSig.sig.connect(self.resetFromLog)
        self.openLogFiles(self.log_start_pos)
        self.main_vsplitter.insertWidget(widget_index, self.log_widget)

    def resetAnalyzerWidget(self):
        self.wa_widget.setParent(None)
        self.wa_widget.close()
        self.wa_widget.deleteLater()
    
    def resetConnMapWidget(self):
        self.cm_widget.setParent(None)
        self.cm_widget.close()
        self.cm_widget.deleteLater()

    def resetLoggerWidget(self):
        self.log_widget.setParent(None)
        self.log_widget.close()
        self.log_widget.deleteLater()

    def timerEvent(self):
        if self.restart:
            self.restart=False

            del self.log_widget_mod
            sys.modules.pop("logger_widget")
            self.createLoggerWidget(2)

            if self.isGraphs:
                del self.wa_widget_mod
                sys.modules.pop("beerocks_analyzer_widget")
                self.createAnalyzerWidget(0)

            if self.isMap:
                del self.cm_widget_mod
                sys.modules.pop("connectivity_map_widget")
                self.createConnMapWidget(1)

            if self.isPlay:
                self.startReadSampleThread()
            self.restartDone=True

    def playPauseButtonTrig(self):
        if self.isPlay:
            self.stopReadSampleThread()
            if self.isExtLogFile: self.timeSlider.setEnabled(True)
        else:
            self.startReadSampleThread()
            if self.isExtLogFile: self.timeSlider.setEnabled(False)

        self.isPlay = not self.isPlay
        self.playPauseAction.setIcon(self.pauseIcon if self.isPlay else self.playIcon)

    def onlineOfflineButtonTrig(self):
        global g_marker_update
        self.stopReadSampleThread()
        self.isOnline = not self.isOnline
        
        self.createMainFrame()
        self.timeUpdateSig.sig.emit(self.fileTime)
        if self.isOnline:
            self.log_start_pos = -1 #end of file
            self.openLogFiles(self.log_start_pos)
            # ask for markers and only then start read sample thread
            g_marker_update = True
            self.wait_for_mark = True
            self.startReadSampleThread(True)
        elif self.isPlay:
            self.playPauseButtonTrig()

        self.onlineOfflineAction.setIcon(self.onlineIcon if self.isOnline else self.offlineIcon)
    
    def resetFromLog(self):
        self.log_start_pos = self.getLogFilePos()
        self.reset()

    def reset(self):
        if self.restartDone:
            self.restartDone=False
            self.resetLoggerWidget()
            if self.isGraphs:
                self.resetAnalyzerWidget()
            if self.isMap:
                self.resetConnMapWidget()
            self.ap_mac2num = {}
            self.sta_mac2num = {}
            self.restart=True
            self.timer.start(100)

    def createMenu(self):
        self.file_menu = self.menuBar().addMenu("&File")
        quit_action = self.createAction("&Quit", slot=self.close,  shortcut="Ctrl+Q", tip="Close the application")
        self.file_menu.addAction(quit_action)

    def createAction(self, text, slot=None, shortcut=None, icon=None, tip=None, checkable=False,  signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon(":/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action

    def createMainFrame(self):
        self.script_name = os.path.basename(__file__)
        self.scriptPath = os.path.abspath(__file__)
        self.scriptPath = self.scriptPath.replace(self.script_name,"")
        if self.scriptPath.find("\\"): self.scriptPath = self.scriptPath.replace("\\","/")
        
        try: self.removeToolBar(self.toolbar)
        except: pass
        self.toolbar = self.addToolBar('Tools')
        self.toolbarWidgetLayout = QHBoxLayout()

        self.playIcon = QIcon(self.scriptPath + "/icons/play.png")
        self.pauseIcon = QIcon(self.scriptPath + "/icons/pause.png")
        self.playPauseAction = QAction(self.playIcon, 'Play/Pause', self)
        self.playPauseAction.triggered.connect(self.playPauseButtonTrig)

        self.onlineIcon = QIcon(self.scriptPath + "/icons/online_button.png")
        self.offlineIcon = QIcon(self.scriptPath + "/icons/offline_button.png")
        self.onlineOfflineAction = QAction(self.onlineIcon, 'Online/Offline', self)
        self.onlineOfflineAction.triggered.connect(self.onlineOfflineButtonTrig)

        if not self.isExtLogFile:
            self.toolbar.addAction(self.onlineOfflineAction)

        if self.isExtLogFile or (not self.isOnline):
            #Play/Pause button
            self.toolbar.addAction(self.playPauseAction)

            #Slider
            self.getFileMarkers()
     
            self.timeSlider = QSlider()
            self.lastTimerValue = 0.0
            self.lastTimerReleasedValue = 0.0
            self.lastTimerValueChanged = False
            self.timeSlider.valueChanged.connect(self.timeSliderChanged)
            self.timeSlider.sliderReleased.connect(self.timeSliderReleased)
            self.timeSlider.setOrientation(Qt.Horizontal)
            self.timeSlider.setMinimum(0)
            self.timeSlider.setMaximum(len(self.file_marks)-1)
            self.timeSlider.setTickInterval(1)
            self.timeSlider.setMaximumHeight(24)
            self.timeSlider.setMinimumWidth(220)
            self.timeSlider.setToolTip("Timeline slider")
            
            self.toolbarWidgetLayout.addWidget(self.timeSlider)

        #File Time
        self.fileTimeLabel = QLabel()
        self.fileTimeLabel.setText("File time:")
        self.fileTimeText = QTextEdit()
        self.fileTimeText.setEnabled(False)
        self.fileTimeText.setMaximumHeight(24)
        self.fileTimeText.setMaximumWidth(60)
        self.fileTimeText.setText("0.0")
        self.fileTimeText.setToolTip("Current Time")

        self.start_time = None

        self.toolbarWidgetLayout.addWidget(self.fileTimeLabel)
        self.toolbarWidgetLayout.addWidget(self.fileTimeText)     
        self.toolbarWidgetLayout.addStretch(1)

        self.toolbarWidget = QWidget()
        self.toolbarWidget.setLayout(self.toolbarWidgetLayout)
        self.toolbar.addWidget(self.toolbarWidget)

        self.setWindowTitle("BeeRocks Analyzer V%s" % VERSION) 
        self.setWindowIcon(QIcon(self.scriptPath + '/icons/wifi.ico'))
        self.setMinimumSize(800,600)
        self.move(0,0)
        self.showMaximized()

    def getFileMarkers(self):
        logger.info("Scanning log markers/time line...")
        fd= open(self.fname,"rt")
        line=" "
        self.file_marks=[]
        p=0
        t="0.000"
        self.file_marks.append( (p, t) )
        while True:
            p = fd.tell()
            line = fd.readline()
            if line == "":
                break
            i = line.find("|")
            t = line[0:i]
            if line.find("|MARK") != -1:
                self.file_marks.append( (p, t) )

        fd.close()
        logger.info("Done.")

    def timeSliderChanged(self, value):
        self.lastTimerValueChanged = (self.lastTimerValue != value)
        self.lastTimerValue = value
        self.fileTimeText.setText(self.file_marks[self.lastTimerValue][1])

    def timeSliderReleased(self):
        if (self.lastTimerReleasedValue != self.lastTimerValue) or (self.lastTimerValueChanged):
            self.lastTimerValueChanged = False
            self.lastTimerReleasedValue = self.lastTimerValue
            self.log_start_pos = self.file_marks[self.lastTimerValue][0]
            self.fileTime = float(self.file_marks[self.lastTimerValue][1]) 
            self.reset()

    def timeUpdateSlot(self, t):
        self.fileTime = float(t)
        if self.restartRuntime or self.start_time==None:
            self.runTime = self.fileTime
        else:
            self.runTime = round(time.time() - self.start_time, 3)

        self.fileTimeText.setText(str(t))

def init(argv):
    global app, bra
    app = QApplication(argv)
    myappid = 'intel.BeeRocksAnalyzer' # arbitrary string
    if sys.platform.startswith("win"): ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
    bra = BeeRocksAnalyzer(argv)

def show():
    global app, bra
    bra.show()
    app.exec_()

def socket_server(log_file):
    global g_run_flag, g_marker_update
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind the socket to the port
    server_address = ('', 10000)
    try:
        logger.info("starting up on {a[0]} port {a[1]}".format(a=server_address))
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(server_address)
    except Exception as e: # TODO: too broad exception
        logger.error("could not start up on {a[0]} port {a[1]}:\n{e}".format(a=server_address, e=e))
        return

    # Listen for incoming connections
    sock.listen(1)

    #open log file
    file = open(log_file,'w') 

    run_local=True
    while g_run_flag and run_local:
        # Wait for a connection
        logger.info('waiting for a connection...')
        connection, client_address = sock.accept()
        connection.settimeout(5)
        startTime = time.time()

        logger.info("connection from {}".format(client_address))
        file.write( '%.3f|START\n' % (time.time() - startTime))
        file.flush()
        # Receive the data in chunks
        data = ""
        while g_run_flag:
            if g_marker_update:
                g_marker_update = False
                sent_bytes = connection.send("marker")
            try:
                data += connection.recv(256).decode("utf-8")
                logger.debug("Trying to get data from the socket")
            except socket.timeout:
                logger.debug("Socket timed out")
                pass
            except KeyboardInterrupt:
                logger.info("Keyboard interrupt, stopping")
                run_local=False
                break
            except Exception as e:
                logger.error("Error when handling data from the socket_server:\n"
                             "{}\n"
                             "Stopping".format(str(e)))
                run_local=False
                break
            while g_run_flag:
                try:
                    i=data.index('\n')
                except Exception as e:
                    logger.debug("data.index failed: {}".format(data))
                    break
                send_data = data.split('\n')[0]
                data = data[i+1:]
                if send_data:
                    file.write(('%.3f|' % (time.time() - startTime)) + send_data + '\n')
                    file.flush()

    # Clean up the connection
    connection.close()
    file.write('%.3f|STOP\n' % (time.time() - startTime))
    file.flush()

    file.close()


def local_start_beerocks_cli(beerocks_cli_path: Union[Path, None], target_ip: IPv4Address) -> int:
    """Start beerocks_cli locally.

    Parameters
    ----------
    beerocks_cli_path: Union[Path, None]
        The path to beerocks_cli's binary.
        If None, the following path will be tried: <prplMesh_top_level>/../build/install/bin/beerocks_cli
    target_ip: IPv4Address
        The IP address to send the data to. Defaults to 127.0.0.1.

    Returns
    -------
    int
        The return status of beerocks_cli.
    """
    if not beerocks_cli_path:
        script_folder = Path(os.path.dirname(os.path.realpath(__file__)))
        beerocks_cli_path = script_folder.parent.parent.parent \
                            / "build" / "install" / "bin" / "beerocks_cli"
    if not beerocks_cli_path.is_file():
        raise ValueError("Path to beerocks_cli not found: {}".format(beerocks_cli_path))
    if not target_ip:
        target_ip = ip_address("127.0.0.1")

    command = [beerocks_cli_path, "-a", str(target_ip)]
    logger.debug("Starting beerock_cli -a")
    exit_status = subprocess.call(command)
    logger.debug("beerock_cli -a exited")

    return exit_status


def ssh_start_beerocks_cli(host="192.168.1.1",my_ip="",ssh_port=22, user='admin', password='admin'):
    global g_ssh
    g_ssh = paramiko.SSHClient()
    g_ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    g_ssh.connect(host, port=ssh_port, username=user, password=password, allow_agent = False)
    
    if my_ip == "":
        my_ip = g_ssh.get_transport().sock.getsockname()[0]
        
    command = '/opt/beerocks/beerocks_cli -a ' + my_ip

    #create ssh shell
    ssh_shell = g_ssh.invoke_shell()

    #establish connection
    in_buff = ''
    while not in_buff.endswith(':~# '):
        in_buff += ssh_shell.recv(9999)
        if in_buff.endswith('password: '): 
            time.sleep(3)
            logger.info("ssh_command()  --> got 'password:', sending password...")
            ssh_shell.send(password + '\n')
            in_buff = ''
        elif 'are you sure you want to continue connecting' in in_buff:
            time.sleep(3)
            logger.info("ssh_command()  --> got 'are you sure you want to continue connecting', sending 'yes'...")
            ssh_shell.send('yes\n')
            in_buff = ''
    
    #execute command
    i, o, e = g_ssh.exec_command(command)
    return o.channel.recv_exit_status()
    
def main(argv):
    global t_list, g_run_flag, g_ext_log_file
    signal.signal(signal.SIGINT, signal_handler)
    try:
        log_file=LOG_FILE
        gw_ip=""
        my_ip=None
        ssh_port=22
        bin_path = None
        g_ext_log_file = False
        is_conn_map = False
        is_conn_map_sep_win = False
        conf_list=[]
        #check python version
        if (sys.version_info.major != 3) or (sys.version_info.minor < 5):
            logger.critical("Error: python version must be 3.5+")
            return
        #get command line args
        argv_len = len(sys.argv)
        if argv_len == 1:
            printUsage()
            return
        else:
            for i in range(argv_len):
                arg_i = sys.argv[i].strip()
                if arg_i.startswith("-f="):
                    g_ext_log_file = True
                    log_file = arg_i.split("=")[1]
                elif arg_i.startswith("-map"):
                    is_conn_map = True
                    if "win" in arg_i:
                        is_conn_map_sep_win = True
                elif arg_i.startswith("-conf="):
                    conf_file_name = arg_i.split("=")[1]
                    conf_i = imp.load_source('module.name', "./conf/" + conf_file_name)
                    conf_i.CMD.append("-graphs")
                    conf_list.append(conf_i.CMD)
                elif arg_i.startswith("-gw_ip="):
                    gw_ip = arg_i.split("=")[1]
                elif arg_i.startswith("-bin_path="):
                    bin_path = Path(arg_i.split("=")[1])
                elif arg_i.startswith("-my_ip="):
                    my_ip = arg_i.split("=")[1]
                elif arg_i.startswith("-ssh_port="):
                    ssh_port = int(arg_i.split("=")[1])

        #add conn map
        if is_conn_map:
            map_arg = "-map"
            if is_conn_map_sep_win:
                map_arg += "_win"
            if len(conf_list) == 0:
                conf_list.append([map_arg])
            else:
                for conf in conf_list:
                    conf.append(map_arg)
        
        #Update CMD in conf_list
        CMD=["-f=%s" % log_file]
        CMD.append("-realtime=1.0,60")
        if g_ext_log_file:
            CMD.append("-ext_log_file")
        for i in range (0,len(conf_list)):
            if len(conf_list[i]) > 1:
                conf_list[i] = [conf_list[i][0]] + CMD + conf_list[i][1:]
            else:
                conf_list[i] = [conf_list[i][0]] + CMD
        
        #Server socket & beerocks_cli updates
        if not g_ext_log_file: 
            #start server_socket
            t = Thread(target=socket_server, args=(log_file,))
            t.start()
            t_list.append(t)
            # beerocks_cli updates (either via ssh or locally)
            time.sleep(3)
            if not gw_ip:
                # running locally
                t = Thread(target=local_start_beerocks_cli, args=(bin_path, my_ip))
            else:
                t = Thread(target=ssh_start_beerocks_cli, args=(gw_ip, my_ip, ssh_port))
            t.start()
            t_list.append(t)
        
        command = conf_list[0]
        time.sleep(randint(1,10)/10.0)

        init(command[:])
        show()

        g_run_flag=False

        for t in t_list:
            logger.debug("Joining thread {}" + str(t))
            t.join()

    except KeyboardInterrupt:
        logger.info("Keyboard interrupt, terminating...")
        for t in t_list:
            try:
                t.terminate()
            except Exception as e:
                logger.warning("Exception when trying to terminate thread: " + str(e))
        g_run_flag=False
        sys.exit(0)

if __name__ == "__main__":
    main(sys.argv)
