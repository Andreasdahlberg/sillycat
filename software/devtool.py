#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import serial
from collections import deque
import ctypes


from PyQt4 import QtGui, QtCore
from SerialInterface import *
from DataLog import *
from DisplayView import DisplayView
from PacketView import PacketView


PORT = 'com3'
BAUDRATE = 250000

LOG_PATH = 'logs'
DEV_ICON = os.path.join('icons', 'dev_tool_icon.png')


class ConsoleView(QtGui.QPlainTextEdit):
    def __init__(self):
        super(ConsoleView, self).__init__()
        self._stream_buffer = deque()
        self._paused = False


    def pause(self, mode):
        if mode == False:
            self._flush_stream_buffer()
        self._paused = mode


    def toogle_pause(self, state):
        print(not self._paused)
        self.pause(not self._paused)


    def update_console_view(self, text):
        self._stream_buffer.append(text)
        if not self._paused:
            self._flush_stream_buffer()      


    def _flush_stream_buffer(self):
        while len(self._stream_buffer) > 0:
            self.appendPlainText(self._stream_buffer.popleft())



class DevTool(QtGui.QWidget):
    def __init__(self):
        super(DevTool, self).__init__()
        self.ser = SerialInterface()
        self.initUI()

    

        self.log = DataLog(LOG_PATH)

        if self.ser.open_port(PORT, BAUDRATE) == True:
            self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.console_view.update_console_view)
            self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.print_text)
            self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.log.handle_signal)      

            self.connect(self.ser, QtCore.SIGNAL('new_vram(QByteArray)'), self.dv.update_display_view)
            self.connect(self.ser, QtCore.SIGNAL('new_pck(QString)'), self.pv.update_packet_view)                 
            self.connect(self.dv, QtCore.SIGNAL('new_pixel(QPoint)'), self.update_pixel_text)   
                        
            self.ser.start()
        else:
            self.console_view.setPlainText('Failed to open interface on ' + PORT)

        
    def initUI(self):

        self.console_view = ConsoleView()
        self.console_view.appendHtml('<b>Waiting for input...<b>')

        self.dv = DisplayView()
        self.dv.hide()
        self.pv = PacketView()

        self.checkbox = QtGui.QCheckBox('Pause')
        self.checkbox.stateChanged.connect(self.console_view.toogle_pause)

        self.label = QtGui.QLabel() 
        self.label.setText('');
        self.label.hide()

        display_vbox = QtGui.QVBoxLayout()
        display_vbox.addWidget(self.dv)
        display_vbox.addWidget(self.label)

        hbox = QtGui.QHBoxLayout()
        hbox.addLayout(display_vbox)
        hbox.addWidget(self.console_view)

        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(self.checkbox)
        vbox.addLayout(hbox) 

        vbox.addWidget(self.pv)      

        self.setLayout(vbox)    

        self.resize(1050, 600)
        self.center()
        self.setWindowTitle('SillyCat Development Tool')
        self.setWindowIcon(QtGui.QIcon(DEV_ICON))        
        self.show()


    def update_pixel_text(self, point):
        if (self.label.isHidden()):
            self.label.show()
        self.label.setText('X: ' + str(point.x()) + ',Y: ' + str(point.y()))

    def closeEvent(self, event):
        self.ser.stop()
        event.accept()

    
    def center(self):    
        qr = self.frameGeometry()
        cp = QtGui.QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())  


    def print_text(self, text):
        print(text)
        
        
def main():
    myappid = 'sillycat.software.devtool.10' 
    ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    app = QtGui.QApplication(sys.argv)
    ex = DevTool()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()    
