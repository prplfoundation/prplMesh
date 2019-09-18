#!/usr/bin/python
import sys
import os
import ctypes
import threading
import socket
import time

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import QWidget, QTextEdit, QTabWidget, QFormLayout


class UpdateSig(QObject):
    sig = Signal(float)

class LoggerWidget(QWidget):
    def __init__(self, parent=None):
        super(LoggerWidget, self).__init__(parent)
        self.logOutput = QTextEdit()
        self.logOutput.setReadOnly(True)
        self.logOutput.setLineWrapMode(QTextEdit.NoWrap)
        self.logOutput.moveCursor(QTextCursor.End)
        #self.font = self.logOutput.font()
        #self.font.setFamily("Courier")
        #self.font.setPointSize(10)
        #self.logOutput.setCurrentFont(font)
        #self.logOutput.setTextColor(color)
        #self.logOutput.resize(800,200)
        #self.setMinimumWidth(1000)
        
        self.tab_widget = QTabWidget(self)
        self.tab_widget.addTab(self.logOutput, "EventsLogger")

        policy = self.logOutput.sizePolicy()
        policy.setVerticalStretch(1)
        policy.setHorizontalStretch(1)
        self.logOutput.setSizePolicy(policy)
        self.tab_widget.setSizePolicy(policy)

        self.Layout = QFormLayout()
        self.Layout.addWidget(self.tab_widget)
        self.setLayout(self.Layout)

        self.updateSig = UpdateSig()
        self.restartSig = UpdateSig()

    def readSampleAndUpdateLogger(self, line, param_t1, is_start, is_stop):
        if "BML_EVENT" in line:
            self.logOutput.insertPlainText(line)
            self.logOutput.moveCursor(QTextCursor.End)
