#!/usr/bin/python
import logging
import os
import sys
import networkx as nx
import threading
import time
import math

from PySide2.QtCore import *
from PySide2.QtGui import *

import matplotlib
from PySide2.QtWidgets import QTabWidget, QVBoxLayout, QWidget

matplotlib.use('Qt5Agg')

try:
    from matplotlib.backends.backend_qt5 import NavigationToolbar2QT as NavigationToolbar
except ImportError:
    from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

import pylab
import matplotlib.pyplot as plt

class UpdateSig(QObject):
    sig = Signal(int)

class ConnectivityMapWidget(QWidget):
    def __init__(self, parent=None):
        super(ConnectivityMapWidget, self).__init__(parent)
        self.logger = logging.getLogger(__name__)
        self.graph = nx.DiGraph()
        self.non_radio_labels_dict={}
        self.radio_labels_dict={}
        self.color_map=[]
        self.size_map=[]
        self.hierarchy_max=0
        self.nodes_num_in_hierarchy={}
        self.last_x_in_hierarchy=[]
        self.delta_y=0
        self.delta_x={}
        self.width=0
        self.height=0

        self.thread = None
        self.threadEvent = threading.Event()
        self.updateSig = UpdateSig()
        self.restartSig = UpdateSig()
        
        self.figs_tab_widget = QTabWidget(self)
        vbox = QVBoxLayout()
        vbox.addWidget(self.figs_tab_widget)
        self.setLayout(vbox)
        self.fig = None 
        self.ax = None
        self.addConnMap()
        
        self.fig.canvas.mpl_connect('resize_event', self.onresize)
        self.fig.canvas.mpl_connect("motion_notify_event", self.hover)
        self.fig.patch.set_facecolor('white')

        self.wait_for_start = True
        self.last_radio_channel = -1
        self.last_radio_bandwidth = -1
        self.last_radio_active = False
        self.last_radio_cac_completed = False
        self.last_ap_mac = ""
        self.gw_eth_mac = None

        self.sc = plt.scatter([],[],c='r', s=1000,alpha=0)
        self.annot = self.ax.annotate("", xy=(0,0), xytext=(20,20),textcoords="offset points", fontsize=13,
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
        self.annot.set_visible(False)

        self.updateSig.sig.connect(self.redraw_graph)
        pylab.axis('off')
        self.fig.canvas.draw()

    def closeEvent(self, event):
        try: self.updateSig.sig.disconnect()
        except: pass
        try: self.restartSig.sig.disconnect()
        except: pass
        try: plt.close('all')
        except: pass
        try: QWidget.closeEvent(self, event)
        except: pass
    
    class node:
        def __init__(self, node_type, mac, parent_mac, backhaul_mac="", channel=-1, bandwidth=-1, cac_completed=False, ap_active=False, name="",ip="", analyzer_id=-1):
            self.logger = logging.getLogger(__name__)
            self.type = node_type
            self.mac = mac
            self.backhaul_mac = backhaul_mac
            self.parent_mac = parent_mac
            self.channel=channel
            self.bandwidth=bandwidth
            self.cac_completed=cac_completed
            self.ap_active = ap_active
            self.name=name
            self.ip=ip
            self.analyzer_id=analyzer_id
            self.hierarchy=-1
            self.x=0
            self.y=0
            self.size=1400
            self.color='gray'
            self.label= self.type
            self.has_left_radio=False #for GW/IRE only
            self.is_left_radio=False #for RADIO only

        def print_node(self):
            self.logger.info("type=%s",str(self.type))
            self.logger.info("mac=%s",str(self.mac))
            self.logger.info("backhaul_mac=%s",str(self.backhaul_mac))
            self.logger.info("parent_mac=%s",str(self.parent_mac))
            self.logger.info("channel=%s",str(self.channel))
            self.logger.info("bandwidth=%s",str(self.bandwidth))
            self.logger.info("name=%s",str(self.name))
            self.logger.info("ip=%s",str(self.ip))
            self.logger.info("analyzer_id=%s",str(self.analyzer_id))
            self.logger.info("hierarchy=%s",str(self.hierarchy))
            self.logger.info("x=%s",str(self.x))
            self.logger.info("y=%s",str(self.y))
            self.logger.info("size=%s",str(self.size))
            self.logger.info("color=%s",str(self.color))
            self.logger.info("label=%s",str(self.label))
            self.logger.info("has_left_radio=%s",str(self.has_left_radio))
            self.logger.info("is_left_radio=%s",str(self.is_left_radio))

    def is_dfs_channel(self, channel):
        if ((channel>=52) and (channel<=64)) or ((channel>=100) and (channel<=144)):
            return True
        return False
    
    def set_node_size_color_label(self, n):
        #set size
        n.size = min(self.width,self.height) * 4
        #set color & label
        if n.type == 'GW':
            n.color = 'steelblue'#'dodgerblue'#'mediumblue'#'royalblue'##'darkblue'
        elif n.type == 'IRE':
            n.color = 'deepskyblue'#'lightskyblue'#'royalblue'#'blue'#'dodgerblue'
        elif n.type == 'Client':
            n.color = 'cyan'
            n.label = 'STA' + str(n.analyzer_id)
        elif n.type == 'ETH':
            n.color = 'mediumseagreen'#'forestgreen'
            n.label = 'ETH'
            #update size for ETH
            n.size = n.size / 3
        elif n.type == 'RADIO':
            if n.channel != -1:
                n.label = n.channel
            else:
                n.label = 'N/A'
            if not n.ap_active:
                n.color = 'gray'
            elif (not n.cac_completed) and self.is_dfs_channel(n.channel):
                n.color = 'orange'
            else:
                if n.channel<14:
                    n.color = 'green'
                else:
                    n.color = 'springgreen'
            #update size for radio
            n.size = n.size / 3
        else:
            n.color = 'red'

    def update_hierarchies_from_node(self, n):
        for child in self.graph.successors(n):
            child.hierarchy = n.hierarchy + 1
            #update max_hierarchy
            if child.hierarchy > self.hierarchy_max:
                self.hierarchy_max = child.hierarchy
            #update nodes_num_in_hierarchy dict
            if child.hierarchy in self.nodes_num_in_hierarchy:
                self.nodes_num_in_hierarchy[child.hierarchy] += 1
            else:
                self.nodes_num_in_hierarchy[child.hierarchy] = 1
            #update child's successors hierarchies recursively
            self.update_hierarchies_from_node(child)

    def add_delta_x_from_node(self, n, delta):
        n.x += delta
        if n.x > self.last_x_in_hierarchy[n.hierarchy]:
            self.last_x_in_hierarchy[n.hierarchy] = n.x
        for child in self.graph.successors(n):
            self.add_delta_x_from_node(child,delta)
    
    def update_x_positions_from_node(self, n):
        #check if n has clients connected
        has_clients_connected = False
        if self.graph.out_degree(n): #IRE/GW - check if have children after eth/radios
            for ap in self.graph.successors(n):
                if self.graph.out_degree(ap):
                    has_clients_connected = True
                    break
            
        if not has_clients_connected: # leaf case
            if self.last_x_in_hierarchy[n.hierarchy] is not None:
                self.last_x_in_hierarchy[n.hierarchy] += self.delta_x[n.hierarchy]
            else:
                max_nodes_in_hierarchy = 0
                for i in range(0,self.hierarchy_max+1,2):
                    if self.nodes_num_in_hierarchy[i] > max_nodes_in_hierarchy: 
                        max_nodes_in_hierarchy=self.nodes_num_in_hierarchy[i]
                if self.hierarchy_max < 2 or max_nodes_in_hierarchy < 2:
                    self.last_x_in_hierarchy[n.hierarchy] = 0
                else:
                    self.last_x_in_hierarchy[n.hierarchy] = -self.width/2.0
            n.x = self.last_x_in_hierarchy[n.hierarchy]
        else:#non-leaf case
            #Update x positions to left_radio's children->eth's children->right_radio's children
            #Update x positions to left_radio's children
            if n.has_left_radio:
                for ap in self.graph.successors(n):
                    if (ap.type == 'RADIO') and (ap.is_left_radio):
                        for child in self.graph.successors(ap):
                            self.update_x_positions_from_node(child)
                        break
            #Update x positions to eth's children
            for ap in self.graph.successors(n):
                if ap.type == 'ETH':
                    for child in self.graph.successors(ap):
                        self.update_x_positions_from_node(child)
                    break
            #Update x positions to right_radio's children
            for ap in self.graph.successors(n):
                if (ap.type == 'RADIO') and (not ap.is_left_radio):
                    for child in self.graph.successors(ap):
                        self.update_x_positions_from_node(child)
                    break
            #Update n.x position according to connected clients positions
            min_child_x = None
            max_child_x = None
            for ap in self.graph.successors(n):
                for child in self.graph.successors(ap):
                    if (min_child_x is None) or (max_child_x is None):
                        min_child_x = child.x
                        max_child_x = child.x
                    elif child.x > max_child_x:
                        max_child_x = child.x
                    elif child.x < min_child_x:
                        min_child_x = child.x
            n.x = (min_child_x + max_child_x) / 2.0
            if (self.last_x_in_hierarchy[n.hierarchy] is not None) and (n.x < self.last_x_in_hierarchy[n.hierarchy] + self.delta_x[n.hierarchy]):
                delta = self.last_x_in_hierarchy[n.hierarchy] + self.delta_x[n.hierarchy] - n.x
                self.add_delta_x_from_node(n, delta)
            self.last_x_in_hierarchy[n.hierarchy] = n.x
        
        #Update n's aps x positions
        if self.graph.out_degree(n):
            factor = 0.065
            radius = 0.45*n.size*factor
            alpha = math.radians(60)
            for ap in self.graph.successors(n):
                if ap.type == 'RADIO':
                    d_x = radius*math.sin(alpha)*1.2
                    if ap.is_left_radio:#move left
                        ap.x= n.x - d_x
                    else:#move right
                        ap.x= n.x + d_x
                else:#child.type == 'ETH'
                    ap.x = n.x
    
    def update_y_positions_from_node(self, n):
        num_of_successors =len(list(self.graph.successors(n)))
        if num_of_successors == 0:
            return
        else:
            if (n.type == 'GW') or (n.type == 'IRE'):#update y positions to radio/eth children
                factor = 0.065
                radius = 0.45*n.size*factor
                alpha = math.radians(60)
                for child in self.graph.successors(n):
                    if child.type == 'RADIO':
                        child.y= n.y - radius*math.cos(alpha)
                    else:#child.type == 'ETH'
                        child.y = n.y - radius
                    self.update_y_positions_from_node(child)
            else:#update y positions to non-radio children
                for child in self.graph.successors(n):
                    child.y = n.y - self.delta_y
                    self.update_y_positions_from_node(child)

    def update_positions_from_node(self, n):
        self.update_y_positions_from_node(n)
        self.last_x_in_hierarchy = [None] * (self.hierarchy_max+1)
        self.update_x_positions_from_node(n)

    def update_graph(self):
        #set nodes size according to current window size
        for n in self.graph:
            self.set_node_size_color_label(n)
        #update labels dict & colors map & sizes map
        del self.size_map[:]
        del self.color_map[:]
        self.non_radio_labels_dict.clear()
        self.radio_labels_dict.clear()
        for n in self.graph:
            self.color_map.append(n.color)
            self.size_map.append(n.size)
            if (n.type == 'RADIO') or (n.type == 'ETH'):
                self.radio_labels_dict[n] = n.label
                self.non_radio_labels_dict[n] = ""
            else:
                self.radio_labels_dict[n] = ""
                self.non_radio_labels_dict[n] = n.label
        #update nodes hierarchies & positions
        self.hierarchy_max = 0
        self.nodes_num_in_hierarchy.clear()
        found_gw = False
        #update nodes hierarchies
        for n in self.graph:
            if n.parent_mac == '':
                found_gw = True
                n.hierarchy = 0
                self.nodes_num_in_hierarchy[0] = 1
                self.update_hierarchies_from_node(n)
                break
        if found_gw:
            #update delta_y value
            if (self.hierarchy_max//2) > 0:
                self.delta_y = float(self.height / (self.hierarchy_max//2))
            else:
                self.delta_y = self.height
            #update delta_x dict according to updated nodes_num_in_hierarchy
            self.delta_x.clear()
            for i in self.nodes_num_in_hierarchy:
                if i%2 == 0:
                    if self.nodes_num_in_hierarchy[i] > 1:
                        self.delta_x[i] = float(self.width / (self.nodes_num_in_hierarchy[i] -1))
                    else:
                        self.delta_x[i] = self.width
            #update nodes positions
            found_gw = False
            for n in self.graph:
                if n.parent_mac == '':
                    found_gw = True
                    n.x = 0
                    n.y = self.height/2
                    self.update_positions_from_node(n)
                    break
            if not found_gw:
                self.logger.error("could not update graph - gw node was not found(2)")
                return
        else:
            self.logger.error("could not update graph - gw node was not found")
            return

        #update nodes "pos" attribute
        for n in self.graph:
            self.graph.nodes[n]['pos']=[n.x,n.y]
        #draw new graph
        pos=nx.get_node_attributes(self.graph,'pos')
        #draw nodes
        radio_pos={}
        radio_size=[]
        radio_color=[]
        radio_nodes=[]

        eth_pos={}
        eth_size=[]
        eth_color=[]
        eth_nodes=[]

        non_ap_pos={}
        non_ap_size=[]
        non_ap_color=[]
        non_ap_nodes=[]

        for n in self.graph:
            if n.type == 'RADIO':
                radio_pos[n] = [n.x,n.y]
                radio_size.append(n.size)
                radio_color.append(n.color)
                radio_nodes.append(n)
            elif n.type == 'ETH':
                eth_pos[n] = [n.x,n.y]
                eth_size.append(n.size)
                eth_color.append(n.color)
                eth_nodes.append(n)
            else:
                non_ap_pos[n] = [n.x,n.y]
                non_ap_size.append(n.size)
                non_ap_color.append(n.color)
                non_ap_nodes.append(n)
        nx.draw_networkx_nodes(self.graph,pos=non_ap_pos,nodelist=non_ap_nodes,node_color=non_ap_color,node_size=non_ap_size) 
        nx.draw_networkx_nodes(self.graph,pos=eth_pos,nodelist=eth_nodes,node_color=eth_color,node_size=eth_size, node_shape='s') 
        nx.draw_networkx_nodes(self.graph,pos=radio_pos,nodelist=radio_nodes,node_color=radio_color,node_size=radio_size)   
        #draw edges
        ap_edges=[]
        non_ap_edges=[]
        for edge in self.graph.edges():
            if  (edge[1].type == 'RADIO') or (edge[1].type == 'ETH'):
                ap_edges.append(edge)
            else:
                non_ap_edges.append(edge)
        nx.draw_networkx_edges(self.graph, pos, edgelist=ap_edges,edge_color='gray',arrows=False, width=1.6)
        nx.draw_networkx_edges(self.graph, pos, edgelist=non_ap_edges,arrows=False, width=1.6)
        #draw labels
        non_radio_font_size = 10
        radio_font_size = 8
        for n in self.graph:
            if (n.type == 'RADIO') or (n.type == 'ETH'):
                if n.size > (radio_font_size*60):
                    radio_font_size = n.size / 60
            else:
                if n.size > (non_radio_font_size*140):
                    non_radio_font_size = n.size / 140
        nx.draw_networkx_labels(self.graph,pos,self.radio_labels_dict,font_size=radio_font_size)
        nx.draw_networkx_labels(self.graph,pos,self.non_radio_labels_dict,font_size=non_radio_font_size)

    def redraw_graph(self):
        if self.threadEvent.isSet(): return
        pylab.clf()
        self.updateWidthHeight()
        self.update_graph()

        #set x/y min/max values
        x_min, x_max = [-self.width/2.0, self.width/2.0]
        y_min, y_max = [-self.height/2.0, self.height/2.0]
        for n in self.graph:
            if (x_min is None) or (x_max is None):
                x_min = n.x
                x_max = n.x
            elif n.x < x_min: x_min = n.x
            elif n.x > x_max: x_max = n.x

            if (y_min is None) or (y_max is None):
                y_min = n.y
                y_max = n.y
            elif n.y < y_min: y_min = n.y
            elif n.y > y_max: y_max = n.y
        x_min-=1
        x_max+=1
        y_min-=1
        y_max+=1
        x_margin = self.width / 10.0
        y_margin = self.height / 10.0
        if (x_min is not None) and (x_max is not None) and (y_min is not None) and (y_max is not None):
            plt.xlim([x_min-x_margin, x_max+x_margin])
            plt.ylim([y_min-y_margin, y_max+y_margin])

        #add scatter (for annotations while hovering)
        x=[]
        y=[]
        size=1000
        for n in self.graph:
            x.append(n.x)
            y.append(n.y)
            if n.size < size: size = n.size
        self.sc=plt.scatter(x,y, s=size,alpha=0)

        pylab.axis('off')
        try:
            self.logger.debug("Redrawing canvas")
            self.fig.canvas.draw()
        except Exception as e: # TODO: too broad exception
            self.logger.exception(e)
        self.threadEvent.set()

    def add_node_to_graph(self, n):
        #if n already in self.graph:
        #   if with same data - return
        #   else - update data and return
        for m in self.graph:
            if m.mac == n.mac:
                if m.parent_mac != n.parent_mac:
                        break
                elif m.type == n.type and \
                  m.backhaul_mac == n.backhaul_mac and \
                  m.parent_mac == n.parent_mac and \
                  m.channel == n.channel and \
                  m.bandwidth == n.bandwidth and \
                  m.cac_completed == n.cac_completed and \
                  m.ap_active == n.ap_active and \
                  m.name == n.name and \
                  m.ip == n.ip and \
                  (m.analyzer_id == n.analyzer_id or n.analyzer_id == -1):
                    return
                else:
                    m.type = n.type
                    m.backhaul_mac = n.backhaul_mac
                    m.parent_mac = n.parent_mac
                    m.channel = n.channel
                    m.bandwidth = n.bandwidth
                    m.cac_completed = n.cac_completed
                    m.ap_active = n.ap_active
                    m.name = n.name
                    m.ip = n.ip
                    m.analyzer_id = n.analyzer_id
                    self.sendSig()
                    return

        self.sendSig()

        #remove if already connected
        self.remove_node_by_mac(n.mac)

        #find parent node in graph
        parent_node = self.node('','','')
        for m in self.graph:
            if n.parent_mac == m.mac:
                parent_node = m
                break

        #add new node and edge to parent node (if parent node exist)
        if (parent_node.mac != '') or (n.type == 'GW'):
            self.graph.add_node(n)
        if parent_node.mac != '':
            self.graph.add_edge(parent_node, n)
            #set left/right radio attributes - for nodes positioning
            if (((parent_node.type == 'GW') or (parent_node.type == 'IRE')) and (n.type == 'RADIO') and not(parent_node.has_left_radio)):
                for m in self.graph:
                    if m.mac == parent_node.mac:
                        m.has_left_radio = True
                        break
                for m in self.graph:
                    if m.mac == n.mac:
                        m.is_left_radio = True
                        break
        if n.type == 'GW': # keep the GW mac to use as parent mac
            self.gw_eth_mac = self.mac_plus_one(n.mac)
        #add ETH node with mac = ire_mac/gw_mac+1
        if (n.type == 'GW') or (n.type == 'IRE'):
            eth_mac = self.mac_plus_one(n.mac)
            eth_node = self.node('ETH', eth_mac, n.mac)
            self.graph.add_node(eth_node)
            self.graph.add_edge(n, eth_node)

        self.sendSig()

    def remove_node_by_mac(self, mac):
        for n in self.graph:
            if n.mac == mac:
                if n.is_left_radio:
                    #change parent has_left_radio attribute
                    for m in self.graph:
                        if n.parent_mac == m.mac:
                            m.has_left_radio = False
                            break
                #remove all node's children
                for child in self.graph.successors(n):
                    self.remove_node_by_mac(child.mac)
                self.graph.remove_node(n)
                self.sendSig()
                return

    def mac_plus_one(self, mac):
        mac_int = int(mac.replace(':', ''), 16)
        mac_p1_int = mac_int+1
        mac_p1 = '{:012x}'.format(mac_p1_int) # convert to str
        mac_p1 = ':'.join(mac_p1[i:i + 2] for i in range(0, len(mac_p1), 2))  # add ':' between every 2 chars
        return mac_p1

    def onresize(self, event):
        self.sendSig()

    def set_in_frame(self):
        bbox = self.annot.get_bbox_patch()

        #check if upper left corners are in window
        uleft_coord = (0, bbox.get_height())
        uleft_pnt = bbox.get_data_transform().transform_point(uleft_coord)
        is_in_window_uleft = self.fig.get_window_extent().contains(*uleft_pnt)
        curr_x,curr_y = self.annot.get_position()
        #modify annot's y pos if upper left corner not in window
        if not is_in_window_uleft:
            self.annot.set_position((curr_x,curr_y-bbox.get_height()))
        
        #check if lower right corners is in window
        dright_coord = (bbox.get_width(), 0)
        dright_pnt = bbox.get_data_transform().transform_point(dright_coord)
        is_in_window_dright = self.fig.get_window_extent().contains(*dright_pnt)
        curr_x,curr_y = self.annot.get_position()
        #modify annot's x pos if lower right corner not in window
        if not is_in_window_dright:
            self.annot.set_position((curr_x-bbox.get_width(),curr_y))

    def update_annot(self,ind):
        nodes_info=[]
        for n in self.graph:
            info = ""
            if n.analyzer_id != -1:
                if (n.type == "RADIO") or (n.type == "ETH"):
                    info += "Analyzer id: ap" + str(n.analyzer_id) + "\n"
                else:
                    info += "Analyzer id: sta" + str(n.analyzer_id) + "\n"
            if n.name:
                info += "Name: " + n.name + "\n"
            if n.ip:
                info += "IP: " + n.ip + "\n"
            if n.parent_mac:
                info += "parent MAC: " + n.parent_mac + "\n"
            if n.backhaul_mac:
                info += "backhaul MAC: " + n.backhaul_mac + "\n"
            info += "MAC: " + n.mac + "\n"
            if n.channel != -1:
                info += "channel: " + str(n.channel) + "\n"
            else:
                if (n.type == "RADIO"):
                    info += "channel: N/A" + "\n"
            if n.bandwidth != -1:
                info += "bandwidth: " + str(n.bandwidth) + "\n"
            nodes_info.append(info)
        
        pos = self.sc.get_offsets()[ind["ind"][0]]
        self.annot.xy = pos
        text = "{}".format(" ".join([nodes_info[n] for n in ind["ind"]]))
        self.annot.set_text(text)
        self.annot.set_text(text)
        self.set_in_frame()

    def hover(self,event):
        vis = self.annot.get_visible()
        if event.inaxes != self.ax:
            self.annot.set_visible(False)
            self.ax = pylab.gca()
            self.annot = self.ax.annotate("", xy=(0,0), xytext=(20,20),textcoords="offset points", fontsize=13,
                                            bbox=dict(boxstyle="round", fc="w"),
                                            arrowprops=dict(arrowstyle="->"))
            self.annot.set_visible(False)

        self.sc._picker = True
        cont, ind = self.sc.contains(event)
        if cont:
            self.update_annot(ind)
            self.annot.set_visible(True)
            try:
                self.fig.canvas.draw_idle()
            except Exception as e: # TODO: too broad exception
                self.logger.exception(e)
        else:
            if vis:
                self.annot.set_position((20,20))
                self.annot.set_visible(False)
                try:
                    self.fig.canvas.draw_idle()
                except Exception as e: # TODO: too broad exception
                    self.logger.exception(e)

    def readSample(self, line, param_t, is_start, is_stop):
        if is_start:
            self.wait_for_start=False
            return
        elif is_stop:
            self.restartSig.sig.emit(1)
            return

        param_n = []
        param_v = []
        i1 = line.find("|")
        if(i1 == -1) : return

        if ("Radio" in line) or ("VAP" in line):
            line = line.replace(":",":x,",1)

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

        if param_v is None:
            self.logger.debug("self.param_v is null, returning")
            return

        param_m_val = param_m.split(':')
        param_m_n = param_m_val[0].strip()
        param_m_n_val = ""
        if len(param_m_val) > 1:
            param_m_n_val = param_m_val[1].strip()

        if param_m_n == "Name":#nw_map_update
            try:
                i_state=param_n.index('state')
                i_mac=param_n.index('mac')
                i_type=param_n.index('type')
                i_ip=param_n.index('ip')

                state = (param_v[i_state].split())[0]
                mac = param_v[i_mac]
                line_type = param_v[i_type]
                ip = param_v[i_ip]
                name = param_m_n_val
                channel = -1
                bandwidth = -1
                cac_completed = False
                backhaul_mac = ""

                if "2" in line_type:
                    try:
                        i_backhaul=param_n.index('backhaul')
                        backhaul_mac=param_v[i_backhaul]
                    except Exception as e: # TODO: too broad exception
                        self.logger.error("readSample()  --> {}, nw_map_update IRE line does not contain a backhaul mac address".format(line))
                        self.logger.exception(e)
                        return
                
                if (not "1" in line_type) and (not "2" in line_type):
                    try:
                        i_channel=param_n.index('channel')
                        channel=param_v[i_channel]
                    except Exception as e:
                        self.logger.debug("channel exception")
                        self.logger.exception(e)
                    try:
                        i_bandwidth=param_n.index('bandwidth')
                        bandwidth=param_v[i_bandwidth]
                    except Exception as e:
                        self.logger.debug("bandwidth exception")
                        self.logger.exception(e)
            except Exception as e: # TODO: too broad Exception
                self.logger.error("readSample()  --> {}, nw_map_update line does not contain state or mac address or parent bssid or type".format(line))
                self.logger.exception(e)
                return

            if state == "Connected":
                if "1" in line_type:#GW
                    cm = ConnectivityMapWidget.node('GW', mac, "", backhaul_mac, channel, bandwidth, cac_completed, False, name, ip)
                    self.add_node_to_graph(cm)
                    self.last_ap_mac = mac
                else:
                    i_sta_id=-1
                    i_ap_id=-1
                    sta_id=-1
                    ap_id=-1
                    try: 
                        i_sta_id=param_n.index('sta_id')
                        sta_id = int(param_v[i_sta_id])
                    except: pass
                    try: 
                        i_ap_id=param_n.index('ap_id')
                        ap_id = int(param_v[i_ap_id])
                    except: pass
                    try:
                        i_parent_mac=param_n.index('parent bssid')
                        parent_mac = param_v[i_parent_mac]
                    except ValueError:
                        # if it has no parent bssid, it should be connected to the gateway
                        parent_mac = self.gw_eth_mac
                    if ap_id != -1:
                        for n in self.graph:
                            if n.mac == parent_mac:
                                n.analyzer_id = ap_id
                    if "2" in line_type:#IRE
                        cm = ConnectivityMapWidget.node('IRE', mac, parent_mac, backhaul_mac, channel, bandwidth, cac_completed, False, name, ip, sta_id)
                        self.add_node_to_graph(cm)
                        self.last_ap_mac = mac
                    if "3" in line_type:#client
                        cm = ConnectivityMapWidget.node('Client', mac, parent_mac, backhaul_mac, channel, bandwidth, cac_completed, False, name, ip,sta_id)
                        self.add_node_to_graph(cm)
            elif state == "Disconnected":
                self.remove_node_by_mac(mac)
                
        elif "Radio" in param_m_n:
            try:
                i_channel=param_n.index('channel')
                i_bandwidth=param_n.index('bandwidth')
                i_cac_completed=param_n.index('cac completed')
                i_ap_active=param_n.index('ap active')
            except Exception as e:  # TODO: too broad exception
                self.logger.error("readSample()  --> {}, nw_map_update line "
                                  "does not contain channel or bandwidth or cac_completed or ap_active".format(line))
                return
            
            bandwidth = param_v[i_bandwidth]
            self.last_radio_bandwidth = int(bandwidth)

            channel_string = param_v[i_channel]
            if (channel_string == 'N/A'):
                self.last_radio_channel = -1
            else:
                self.last_radio_channel = int(channel_string)
            
            cac_completed_string = param_v[i_cac_completed]
            if cac_completed_string == "1":
                self.last_radio_cac_completed = True
            else:
                self.last_radio_cac_completed = False
            
            self.last_radio_active_string = param_v[i_ap_active]
            if self.last_radio_active_string == "true":
                self.last_radio_active = True
            else:
                self.last_radio_active = False

        elif "VAP" in param_m_n:
            try:
                i_bssid=param_n.index('bssid')
            except Exception as e:  # TODO: too broad exception
                self.logger.error("readSample()  --> {}, nw_map_update line does not contain bssid".format(line))
                return
            mac = param_v[i_bssid]
            self.add_node_to_graph(ConnectivityMapWidget.node('RADIO', mac, self.last_ap_mac, "", self.last_radio_channel, self.last_radio_bandwidth, self.last_radio_cac_completed, self.last_radio_active))

        elif param_m_n == "type" or param_m_n == "Type":
            param_m_v = int(param_m_val[1].strip())
            try:
                i1=param_n.index('mac')
            except Exception as e:  # TODO: too broad exception
                self.logger.error("Error, readSample() --> {}, 'stats_update' line not contain mac address".format(line))
                return

            mac = param_v[i1]
            i1+=1

            if param_m_v == 1: #AP stats update
                i_ap_id=-1
                ap_id=-1
                try: 
                    i_ap_id=param_n.index('ap_id')
                    ap_id = int(param_v[i_ap_id])
                except: pass
                if ap_id != -1:# new ap mac addr
                    for n in self.graph:
                        if mac == n.mac:
                            n.analyzer_id = ap_id
            elif param_m_v == 3: #Client stats update
                i_sta_id=-1
                sta_id=-1
                try: 
                    i_sta_id=param_n.index('sta_id')
                    sta_id = int(param_v[i_sta_id])
                except: pass
                if sta_id != -1:# new sta mac addr
                    for n in self.graph:
                        if mac == n.mac or mac == n.backhaul_mac:
                            n.analyzer_id = sta_id

        return # End of readSample()
    
    def addConnMap(self):
        dpi=80; w=1200; h=720
        fig_frame = QWidget()
        self.fig = plt.figure(0, figsize=(w/dpi,h/dpi), dpi=dpi)
        self.ax = self.fig.add_subplot(1,1,1)
        plt.subplots_adjust(left=0.07, right=0.87, top=0.96, bottom=0.08)
        self.fig.canvas.setParent(fig_frame)
        mpl_toolbar = NavigationToolbar(self.fig.canvas, fig_frame)

        vbox = QVBoxLayout()
        vbox.addWidget(self.fig.canvas)
        vbox.addWidget(mpl_toolbar)
        fig_frame.setLayout(vbox)
        self.figs_tab_widget.addTab(fig_frame, "ConnectivityMap")

    def sendSig(self):
        self.updateSig.sig.emit(0)
        self.threadEvent.wait(2.0)
        self.threadEvent.clear()

    def updateWidthHeight(self):
        #get window width & height
        fig = self.fig
        bbox = fig.get_window_extent().transformed(fig.dpi_scale_trans.inverted())
        self.width, self.height = bbox.width*fig.dpi, bbox.height*fig.dpi