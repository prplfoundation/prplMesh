#!/usr/bin/python
import logging
import sys
import os

import time
import numpy as np
import math
import re
from datetime import datetime
from datetime import timedelta

import threading

from PySide2.QtCore import *
from PySide2.QtGui import *

import matplotlib
from PySide2.QtCore import QObject, Signal
from PySide2.QtWidgets import QWidget, QVBoxLayout, QTabWidget, QHBoxLayout

matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas

try:
    from matplotlib.backends.backend_qt4 import NavigationToolbar2QT as NavigationToolbar
except ImportError:
    from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar

from matplotlib.ticker import ScalarFormatter, FormatStrFormatter

BAR_CODES = (matplotlib.path.Path.MOVETO,
         matplotlib.path.Path.LINETO,
         matplotlib.path.Path.LINETO,
         matplotlib.path.Path.LINETO,
         matplotlib.path.Path.CLOSEPOLY,)

BARS_COLORS = np.array([i for i in range(10,100, 10)])

def makeBarCollection(x, y, w=0.2):
    yb=0
    patch_a=[]
    for i in range(len(y)):
        verts = (
            (x-w, yb), # left, bottom
            (x-w, yb+y[i]), # left, top
            (x+w, yb+y[i]), # right, top
            (x+w, yb), # right, bottom
            (0., 0.), # ignored
            )
        yb+=y[i]
        barpath = matplotlib.path.Path(verts, BAR_CODES)
        patch = matplotlib.patches.PathPatch(barpath)
        patch_a.append(patch)

    collection = matplotlib.collections.PatchCollection(patch_a, cmap=matplotlib.cm.jet, alpha=0.2)
    collection.set_array(BARS_COLORS[0:len(y)])
    return collection

class FixedOrderFormatter(ScalarFormatter):
    def __init__(self, order_of_mag=0, useOffset=True, useMathText=False):
        self._order_of_mag = order_of_mag
        ScalarFormatter.__init__(self, useOffset=useOffset,
                                 useMathText=useMathText)
    def _set_orderOfMagnitude(self, range):
        self.orderOfMagnitude = self._order_of_mag

class UpdateSig(QObject):
    sig = Signal(int)

class BeeRocksAnalyzerWidget(QWidget):
    def __init__(self, argv, parent=None):
        super(BeeRocksAnalyzerWidget, self).__init__(parent)
        self.logger = logging.getLogger(__name__)
        self.wait_for_start=True
        self.figsInfo = []
        self.figsTitle = []
        self.subplots = []
        self.fig_ax = []
        self.figCanvas = []
        self.thread = None
        self.threadEvent = threading.Event()
        self.updateSig = UpdateSig()
        self.restartSig = UpdateSig()
        self.inCsv=None
        self.realtimeInterval = 1.0
        self.realtimeWindow = 60.0
        self.print_vals = False
        self.print_labels = False
        self.outCsv=None
        self.updatePlotRate=100
        self.realtimeWindow_start=-1.0
        self.realtimeWindow_end=-1.0
        self.update_start_time=False
        self.marker = ('*', 'D', 's', 'x', 'o', '^', 'p')
        self.color  = ('b', 'g', 'c', 'm', 'k', 'y', 'r')
        self.linestyle = '-'
        self.next_color_idx = 0
        self.general_label_color_idx = 0

        ### Log Data containers ###
        self.ap_mac2num = {}
        self.ap_mac2sta_mac = {}
        self.sta_mac2num = {}
        self.sta_num2color = {}  # key = device_num : val = color_idx
        self.ap_num2color =  {}  # key = device_num : val = color_idx
        self.fig_subplot_nums2marker = {(0,0): 0} # key = (fig_num , subplot_num) : val = next_marker_idx

        ###########################
        self.getCommandLineArgs(argv)
    
        self.figs_tab_widget = QTabWidget(self)
        vbox = QVBoxLayout()
        vbox.addWidget(self.figs_tab_widget)
        self.setLayout(vbox)
        self.addPlots()
        self.updateSig.sig.connect(self.updatePlotData)
        ############################

    def closeEvent(self, event):
        try: self.updateSig.sig.disconnect()
        except: pass
        try: self.restartSig.sig.disconnect()
        except: pass
        self.figs_tab_widget.clear()
        self.figsInfo = []
        self.figsTitle = []
        self.subplots = []
        self.fig_subplot_nums2marker = []
        self.fig_ax = []
        self.figCanvas = []
        try:plt.close('all')
        except: pass
        try: QWidget.closeEvent(self, event)
        except: pass

    def getCommandLineArgs(self, argv):
        for i in range(0,len(argv)):
            arg_i = argv[i].strip()
            try:
                if arg_i.startswith("-f="):
                    continue
                elif arg_i.startswith("-fig_title"):
                    s = arg_i.split("=")
                    fig      = int(s[0][-2:-1])
                    sub_plot = int(s[0][-1:])
                    while len(self.figsTitle)-1 < fig: self.figsTitle.append([])
                    while len(self.figsTitle[fig])-1 < sub_plot: self.figsTitle[fig].append([])
                    self.figsTitle[fig][sub_plot] = s[1]
                elif arg_i.startswith("-plot"):
                    s = arg_i.split("=")
                    fig      = int(s[0][-2:-1])
                    sub_plot = int(s[0][-1:])
                    labels = s[1].split(",")
                    while len(self.figsInfo)-1 < fig: 
                        self.figsInfo.append([])
                        self.subplots.append([])
                        self.fig_ax.append([])
                    while len(self.figsInfo[fig])-1 < sub_plot:
                        self.figsInfo[fig].append([])
                        self.subplots[fig].append([])
                        self.fig_ax[fig].append([])
                    for i in range(len(self.fig_ax)):
                        self.fig_ax[i][0] = None
                    for l in labels:
                        self.figsInfo[fig][sub_plot].append(l)
                elif arg_i.startswith("-realtime="):
                    s = arg_i.split("=")
                    ss = s[1].split(",")
                    self.realtimeInterval = float(ss[0])
                    self.realtimeWindow = float(ss[1])
                elif arg_i.startswith("-print_vals"):
                    self.print_vals = True
                elif arg_i.startswith("-print_labels"):
                    self.print_labels = True
                elif arg_i.startswith("-autoplay"):
                    pass
                elif arg_i.startswith("-output_csv"):
                    self.outCsv = open('output.csv', 'w')
                elif arg_i.startswith("-map") or arg_i.startswith("-graphs"):
                    pass
                else:
                    self.logger.error("Error, unknown argument ({:d}) --> {}".format(i, arg_i))
                    self.printUsage()
                    sys.exit(0)
            except Exception as e: # TODO: too broad exception
                self.logger.error("Error(2), in argument ({:d}) --> {}".format(i, arg_i))
                self.printUsage()
                sys.exit(0)

    def printUsage(self):
        self.logger.info("usage: BeeRocksAnalyzer [options]\n")
        self.logger.info("    -f=<log file>                                         - input log file\n")
        self.logger.info("    -plot<fig num><sub plot>=<Label 1>,<Label 1>,..       - enable and configure plots\n")
        self.logger.info("    -realtime=<update interval sec>,<samples>             - enable real time plot\n")
        self.logger.info("    -print_vals                                           - print log values to console\n")
        self.logger.info("    -print_labels                                         - print log labels to console\n")
        self.logger.info("    -output_csv                                           - output params to csv file\n")

    def printLabels(self):
        self.logger.info("printLabels:")
        for a_name in dir(self):
            if a_name.endswith("_v"):
                self.logger.info(" {}".format(a_name))

    def printVals(self):
        self.logger.info("printVals:\n")
        for a_name in dir(self):
            if a_name.endswith("_v") or a_name.endswith("_t"):
                self.logger.info(" {} : {}\n".format(a_name, str(getattr(BeeRocksAnalyzerWidget, a_name))))

    def readSampleAndUpdateGraphs(self, line, param_t1, is_start, is_stop):
        if is_start:
            self.wait_for_start=False
            return
        elif is_stop:
            self.restartSig.sig.emit(1)
            return

        self.readSample(line, param_t1)
        #update start time
        if self.update_start_time:
            self.realtimeWindow_start = param_t1
            self.realtimeWindow_end = self.realtimeWindow_start + self.realtimeInterval
            self.update_start_time=False

        if not self.update_start_time:
            if param_t1 > self.realtimeWindow_end:
                self.updateSig.sig.emit(0)
                self.threadEvent.wait(2.0)
                self.threadEvent.clear()
                self.update_start_time=True

    def readSample(self, line, param_t):
        param_n = []
        param_v = []
        if line.startswith("#") or len(line) < 4 : return None
        i1 = line.find("|")
        if(i1 == -1) : return

        i2=line.find(",")
        try:
            param_m = str(line[i1+1:i2])
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

        except Exception as e: # TODO: too broad exception
            self.logger.error("Error, readSample() 1 line --> {}".format(line))
            self.logger.exception(e)
            return

        if param_v == None: return

        param_m_val = param_m.split(':')
        param_m_n = param_m_val[0].strip()

        if param_m_n == "Name": #nw_map_update
            self.logger.debug("Updating network map")
            try:
                i_state=param_n.index('state')
                i_mac=param_n.index('mac')
                i_type=param_n.index('type')
                if not ("1" in param_v[i_type]):
                    i_ap_mac=param_n.index('parent bssid')
                
            except Exception as e: # TODO: too broad exception
                self.logger.error("readSample()  --> {}, "
                                  "nw_map_update line does not contain state or mac"
                                  " or type or parent bssid".format(line))
                self.logger.exception(e)
                return

            state = (param_v[i_state].split())[0]
            mac = param_v[i_mac]
            line_type = param_v[i_type]
            if not ("1" in line_type):
                ap_mac = param_v[i_ap_mac]
            
            if "2" in line_type: #IRE
                try:
                    i_backhaul=param_n.index('backhaul')
                    mac = param_v[i_backhaul] #for IRE, the backhaul mac is the "client" mac
                except Exception as e:  # TODO: too broad exception
                    self.logger.error("readSample()  --> %s, nw_map_update IRE line does not contain"
                                      " a backhaul mac address".format(line))
                    self.logger.exception(e)
                    return
            if ("2" in line_type) or ("3" in line_type): #IRE or client
                if state == "Disconnected":
                    for mac_t in self.ap_mac2sta_mac: #remove sta from the previous mac addr
                        if mac in self.ap_mac2sta_mac[mac_t]:
                            self.ap_mac2sta_mac[mac_t].remove(mac)

                elif state == "Connected":
                    if not(self.sta_mac2num.has_key(mac)): # new sta mac addr
                        sta_id=-1
                        try: 
                            i_sta_id=param_n.index('sta_id')
                            sta_id = int(param_v[i_sta_id])
                        except Exception as e:  # TODO: too broad exception
                            self.logger.error("readSample()  --> {}, nw_map_update - "
                                              "new STA line does not contain sta_id".format(line))
                            self.logger.exception(e)
                            return
                        self.sta_mac2num[mac] = sta_id
                        self.defineLineColor('sta', self.sta_mac2num[mac])
                    
                    if not(self.ap_mac2num.has_key(ap_mac)): # new ap mac addr
                        #ap_id = len(self.ap_mac2num)
                        ap_id=-1
                        try: 
                            i_ap_id=param_n.index('ap_id')
                            ap_id = int(param_v[i_ap_id])
                        except Exception as e:  # TODO: too broad exception
                            self.logger.error("readSample()  --> %s, nw_map_update - "
                                              "new AP line does not contain ap_id\n{}".format(line, e))
                            return
                        self.ap_mac2num[ap_mac] = ap_id
                        self.defineLineColor('ap', ap_id)
                        self.ap_mac2sta_mac[ap_mac] = []

                    for mac_t in self.ap_mac2sta_mac: #remove sta from the previous mac addr
                        if mac in self.ap_mac2sta_mac[mac_t] and (mac_t != ap_mac):
                            self.ap_mac2sta_mac[mac_t].remove(mac)
                    if not(mac in self.ap_mac2sta_mac[ap_mac]):
                        self.ap_mac2sta_mac[ap_mac].append(mac)

        elif param_m_n == "type" or param_m_n == "Type":
            param_m_v = int(param_m_val[1].strip())
            try:
                i1=param_n.index('mac')
            except Exception as e:  # TODO: too broad exception
                self.logger.error("readSample() --> {}, 'stats_update' line not contain mac address".format(line))
                self.logger.exception(e)
                return

            mac = param_v[i1]
            i1+=1
            if param_m_v == 1: #AP stats update
                if not(self.ap_mac2num.has_key(mac)): # new ap mac addr
                    ap_id=-1
                    try: 
                        i_ap_id=param_n.index('ap_id')
                        ap_id = int(param_v[i_ap_id])
                    except Exception as e:  # TODO: too broad exception
                        self.logger.error("readSample()  --> %s, nw_map_update"
                                          " - new AP line does not contain ap_id".format(line))
                        self.logger.exception(e)
                        return
                    self.ap_mac2num[mac] = ap_id
                    self.defineLineColor('ap', ap_id)
                    self.ap_mac2sta_mac[mac] = []
                ap_id = self.ap_mac2num[mac]

                for j in range(i1, len(param_n)):  # fill atrribute
                    name = 'ap%d_' % self.ap_mac2num[mac] + param_n[j]
                    val = int(param_v[j])
                    self.addAttr(param_t, name, val, 'ap',ap_id)

            elif param_m_v == 3: #Client stats update
                if not(self.sta_mac2num.has_key(mac)): # new sta mac addr
                    sta_id=-1
                    try: 
                        i_sta_id=param_n.index('sta_id')
                        sta_id = int(param_v[i_sta_id])
                    except Exception as e:  # TODO: too broad exception
                        self.logger.error("readSample()  --> %s, nw_map_update"
                                          " - new STA line does not contain sta_id".format(line))
                        self.logger.exception(e)
                        return
                    self.sta_mac2num[mac] = sta_id
                    self.defineLineColor('sta', self.sta_mac2num[mac])
                sta_num = self.sta_mac2num[mac]

                ap_num=None
                for ap_mac in self.ap_mac2sta_mac: # find ap connected to sta
                    if mac in self.ap_mac2sta_mac[ap_mac]:
                        ap_num = self.ap_mac2num[ap_mac] # extract match ap_num connected to specific sta_mac
                        break
                
                if ap_num==None:
                    self.logger.error("Error, readSample() --> {}, 'client_stats_update'"
                                      " did not find sta_mac={} in self.ap_mac2sta_mac".format(line, mac))
                    return
                
                for j in range(i1, len(param_n)):
                    name_pattern = 'ap%d_sta%d_'+ param_n[j]
                    name = name_pattern % (ap_num ,sta_num)
                    val = int(param_v[j])
                    self.addAttr(param_t, name, val, 'sta', sta_num)  # fill right atrribute with val
        # End of readSample()

    def getAttrVal(self, name):
        return [getattr(BeeRocksAnalyzerWidget, name + "_v"), getattr(BeeRocksAnalyzerWidget, name + "_t")]

    def getValByName(self, name):
        try:
            ar_v = getattr(BeeRocksAnalyzerWidget, name + "_v")
        except:
            ar_v = []
        return ar_v

    def getTimeByName(self, name):
        try:
            ar_t = getattr(BeeRocksAnalyzerWidget, name + "_t")
        except:
            ar_t = []
        return ar_t

    def getTimeDiffSec(self, startTime):
        dt  = datetime.now() - startTime
        dt_sec = dt.seconds + (dt.microseconds/1000000.0)
        return dt_sec

    def addAttr(self, param_t, param_n, param_v, entity, entity_num):
        is_new_attr=False
        try:
            ar_v = getattr(BeeRocksAnalyzerWidget, param_n + "_v")
            ar_t = getattr(BeeRocksAnalyzerWidget, param_n + "_t")
        except:
            ar_v = []
            ar_t = []
            setattr(BeeRocksAnalyzerWidget, param_n + "_v", ar_v)
            setattr(BeeRocksAnalyzerWidget, param_n + "_t", ar_t)
            is_new_attr=True
        
        if ar_t != []:
            if (param_t - ar_t[-1]) > 3:
                ar_v.append(None)
                ar_t.append(param_t-0.1)
            
        ar_v.append(param_v)
        ar_t.append(param_t)

        if is_new_attr:
            self.addRemoveNewFigSubplots(ar_t, ar_v, param_n, True, entity, entity_num)

    def defineLineColor(self, entity, num):
        if entity=='ap':
            self.ap_num2color[num] = self.next_color_idx
        elif entity=='sta':
            self.sta_num2color[num] = self.next_color_idx
        else:
            self.logger.error("error defining color , entity={}", entity)
            return
        self.next_color_idx += 1
        if self.next_color_idx == len(self.color):
            self.next_color_idx = 0

    def addRemoveNewFigSubplots(self, ar_t, ar_v, target_lable, is_add, entity, entity_num):
        # print "addRemoveNewFigSubplots is_add=",is_add," entity=",entity," entity_num=",entity_num
        asterisk_signed = False
        for f in range(len(self.figsInfo)):
            for p in range(len(self.figsInfo[f])):
                ar_lables = self.figsInfo[f][p]  
                for param_n in ar_lables:
                    if param_n != target_lable:
                        param_n_s = param_n.split("*")
                        if len(param_n_s) == 1: continue
                        num_of_asterisk = param_n.count('*')
                        re_pattern = '[0-9]+'
                        re_str = str()
                        for param_n_s_element in param_n_s:
                            re_str+=(param_n_s_element+re_pattern)
                        re_str = re_str[:-len(re_pattern)]
                        str_match = re.match(re_str,target_lable)
                        if str_match!=None: 
                            target_lable_hit = (str_match.group() == target_lable)
                        else: target_lable_hit = False
                        if not target_lable_hit: continue
                        if is_add:
                            self.figsInfo[f][p].append(target_lable)
                            asterisk_signed=True
                        else:
                            self.figsInfo[f][p].remove(target_lable)
                            for c in range(len(self.subplots[f][p])):
                                if self.subplots[f][p][c][2] == target_lable:
                                    ax = self.subplots[f][p][c][0]
                                    handles, labels = ax.get_legend_handles_labels()
                                    lable_idx = labels.index(target_lable)
                                    del handles[lable_idx]; del labels[lable_idx]
                                    ax.legend(handles, labels,loc='center left', prop={'size':8}, bbox_to_anchor=(1, 0.5))
                                    del self.subplots[f][p][c]
                                    plt.subplots_adjust(left=0.07, right=0.87, top=0.96, bottom=0.08)
                                    return
                    
                    elif is_add and (asterisk_signed or param_n == target_lable):
                        # add plot_line
                        ax = self.fig_ax[f][p]
                        if entity_num == None:
                            self.general_label_color_idx+=1
                            if self.general_label_color_idx >= len(self.color): self.general_label_color_idx=0
                            color =  self.color[self.general_label_color_idx]
                            marker = self.marker[-1]
                        else:
                            color = self.getLineColor(entity, entity_num)
                            if color == None:
                                self.logger.error("Error, color == None, param_n={}".format(param_n))
                                return
                            marker = self.marker[0]
                            if self.fig_subplot_nums2marker.has_key((f,p)):
                                marker=self.marker[self.fig_subplot_nums2marker[(f,p)]]
                                self.fig_subplot_nums2marker[(f,p)] += 1
                                if self.fig_subplot_nums2marker[(f,p)] == len(self.marker):
                                    self.fig_subplot_nums2marker[(f,p)] = 0
                            else:
                                marker=self.marker[0]
                                self.fig_subplot_nums2marker[(f,p)] = 1

                        plot_line, = ax.plot(ar_t, ar_v, color=color, marker=marker,markersize=5,linewidth=1.2, linestyle=self.linestyle, label=target_lable)
                        ax.ticklabel_format(axis='y',style='plain',scilimits=(1,4))

                        ax.set_xlim([0,1]) 
                        self.subplots[f][p].append((ax, plot_line, param_n))

                        if target_lable.find("_event") != -1:
                            ax.set_ylabel("[Event 0/1/..]")
                        elif target_lable.endswith("_bps"):
                            ax.set_ylabel("[bps]")
                        elif target_lable.endswith("_load"):
                            ax.set_ylabel("[%]")
                        elif target_lable.endswith("bw"):
                            ax.set_ylabel("[MHz]")
                        elif target_lable.endswith("ch"):
                            ax.set_ylabel("[Channel Number]")
                        elif target_lable.endswith("bytes"):
                            ax.set_ylabel("[Bytes/Sec]")
                        elif target_lable.endswith("packets"):
                            ax.set_ylabel("[Packets/Sec]")
                        elif target_lable.endswith("retries"):
                            ax.set_ylabel("[#Retries]")
                        elif target_lable.endswith("rssi"):
                            ax.set_ylabel("[dbm]")

                        handles, labels = ax.get_legend_handles_labels()
                        ax.legend(handles, labels, loc='center left', prop={'size':8}, bbox_to_anchor=(1, 0.5))
                        plt.subplots_adjust(left=0.07, right=0.82, top=0.96, bottom=0.08)
                        return

    def updatePlotData(self):
        if self.threadEvent.isSet(): return
        fig_num=0
        t_start = self.realtimeWindow_start - self.realtimeWindow
        if t_start <0: t_start=0
        xlim = [t_start, self.realtimeWindow_start + self.realtimeInterval + 1.0]
        for f in range(len(self.figsInfo)):
            if self.figsInfo[f] == None: continue
            self.deleteOldSamples(f) # clear unused values
            update_fig=False
            for p in range(len(self.figsInfo[f])):
                ar_lables = self.figsInfo[f][p]
                ymin=[]; ymax=[]
                update_subplot=False
                ax = None
                for param_n in ar_lables:
                    if param_n.find("*") == -1:
                        try:
                            ar_t = getattr(BeeRocksAnalyzerWidget, param_n + "_t")
                            ar_v = getattr(BeeRocksAnalyzerWidget, param_n + "_v")
                        except:
                            continue

                        for s in range(len( self.subplots[f][p])):
                            (ax, plot_line, param_n_tmp) = self.subplots[f][p][s]
                            if param_n_tmp == param_n:
                                update_subplot=True
                                update_fig = True
                                plot_line.set_data(ar_t, ar_v)
                                min, max = self.getMinMax(ar_v)
                                ymin.append(min)
                                ymax.append(max)
                                break

                if update_subplot: # update fig min max
                    if len(ymin) > 0 and len(ymax) > 0:
                        ax.set_xlim(xlim)
                        ylim = [np.min(ymin)-1, np.max(ymax)+1]
                        dy = (ylim[1] - ylim[0])/10.0
                        ylim[0]-=dy; ylim[1]+=dy
                        ax.set_ylim(ylim)
                        self.setYticks(ax, ylim[1])

            if update_fig:
                (fig,canvas) = self.figCanvas[fig_num]
                canvas.draw()
                update_fig = False

            fig_num+=1
        self.threadEvent.set()

    def getMinMax(self, vec):
        if(len(vec)==0): return (0, 0)
        vmax = vmin = 0
        for i in range(len(vec)):
            if vec[i] == None: continue
            if vmin > vec[i]: vmin = vec[i]
            if vmax < vec[i]: vmax = vec[i]
        return (vmin, vmax)

    def deleteOldSamples(self, fig):
        t_start = self.realtimeWindow_start - self.realtimeWindow
        if t_start < 0: t_start=0
        for f in range(len(self.figsInfo)):
            if f != fig: continue
            if self.figsInfo[f] == None: continue
            for p in range(len(self.figsInfo[f])):
                ar_lables = self.figsInfo[f][p]
                for param_n in ar_lables:
                    try:
                        ar_v = getattr(BeeRocksAnalyzerWidget, param_n + "_v")
                        ar_t = getattr(BeeRocksAnalyzerWidget, param_n + "_t")
                        idx = 0
                        t_start_idx=-1
                        while(idx < len(ar_t)):
                            if ar_t[idx] >=  t_start:
                                t_start_idx = idx
                                break
                            idx+=1
                        #delete old elements
                        if t_start_idx == -1:
                            delattr(BeeRocksAnalyzerWidget,ar_v)
                            delattr(BeeRocksAnalyzerWidget,ar_t)
                            self.addRemoveNewFigSubplots(None, None, param_n, False, None, None)                            
                        else:
                            del ar_v[ : t_start_idx]
                            del ar_t[ : t_start_idx]
                    except: pass

    def setYticks(self, ax, vmax):
        if vmax != 0: exp = math.log(abs(vmax),10)
        else: exp=1
        math.copysign(exp, vmax)
        if exp >= 9: exp = 9
        elif exp >= 6: exp = 6
        elif exp >= 3: exp = 3
        elif exp > -3: exp = 0
        elif exp >= -3: exp = -3
        elif exp >= -6: exp = -6
        elif exp >= -9: exp = -9
        ax.yaxis.set_major_formatter(FixedOrderFormatter(exp))

    def getLineColor(self, entity, entity_num):
        if(entity=='ap'):
            return self.color[self.ap_num2color[entity_num]]
        elif(entity=='sta'):
            return self.color[self.sta_num2color[entity_num]]
        else:
            self.logger.error("error getting line color - no such entity ={}".format(entity))
            return None

    def addPlots(self):
        dpi=80; w=1200; h=720
        fig_num = 1
        for f in range(len(self.figsInfo)):
            if self.figsInfo[f] == None: continue

            fig_frame = QWidget()
            fig = plt.figure(fig_num, figsize=(w/dpi,h/dpi), dpi=dpi)
            canvas = FigureCanvas(fig)
            canvas.setParent(fig_frame)
            mpl_toolbar = NavigationToolbar(canvas, fig_frame)

            vbox = QVBoxLayout()
            vbox.addWidget(canvas)
            vbox.addWidget(mpl_toolbar)
            hbox = QHBoxLayout()
            hbox.addLayout(vbox)
            fig_frame.setLayout(hbox)

            fig_subplot_num = len(self.figsInfo[f])*100 + 11
            for p in range(len(self.figsInfo[f])):
                if self.fig_ax[f][p] == None:
                    ax = fig.add_subplot(fig_subplot_num)
                    if (len(self.figsTitle) > f) and (len(self.figsTitle[f]) > p):
                        ax.set_title(self.figsTitle[f][p])
                else: 
                    ax = fig.add_subplot(fig_subplot_num, sharex=self.fig_ax[f][0])
                    if (len(self.figsTitle) > f) and (len(self.figsTitle[f]) > p):
                        ax.set_title(self.figsTitle[f][p])
                ax.grid(True)
                if p == len(self.figsInfo[f]) -1: 
                    ax.set_xlabel('Time [sec]')
                else: 
                    ax.get_xaxis().set_visible(False)
                self.fig_ax[f][p] = ax
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles, labels, loc='center left', prop={'size':8}, bbox_to_anchor=(1, 0.5))

                plt.subplots_adjust(left=0.07, right=0.87, top=0.96, bottom=0.08)
                fig_subplot_num+=1
            self.figCanvas.append((fig,canvas))
            self.figs_tab_widget.addTab(fig_frame, "Graph %d" % (f+1))
            fig_num+=1

