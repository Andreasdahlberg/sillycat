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
        self.log = DataLog(LOG_PATH)        
        self.initUI()

        
    def initUI(self):

        self.console_view = ConsoleView()
        self.dv = DisplayView()
        self.dv.hide()
        self.pv = PacketView()

        self.checkbox = QtGui.QCheckBox('Pause')
        self.checkbox.stateChanged.connect(self.console_view.toogle_pause)

        self.port_select = QtGui.QComboBox();

        for port in self.ser.get_ports():
            self.port_select.addItem(port)

        self.connect_button = QtGui.QPushButton('Connect')
        self.connect_button.setCheckable(True)
        self.connect_button.clicked[bool].connect(self.connect_clicked)        

        self.pixel_label = QtGui.QLabel() 
        self.pixel_label.setText('X: 0, Y: 0');
        self.pixel_label.hide()

        self.save_display_button = QtGui.QPushButton('Save image')
        self.save_display_button.clicked.connect(self.dv.save_view_to_file)
        self.save_display_button.hide()

        self.display_hbox = QtGui.QHBoxLayout()
        self.display_hbox.addWidget(self.pixel_label)    
        self.display_hbox.addWidget(self.save_display_button)

        display_vbox = QtGui.QVBoxLayout()
        display_vbox.addWidget(self.dv)
        display_vbox.addLayout(self.display_hbox)
        display_vbox.addStretch(1)

        hbox = QtGui.QHBoxLayout()
        hbox.addLayout(display_vbox)
        hbox.addWidget(self.console_view)

        connect_box = QtGui.QHBoxLayout();
        connect_box.addWidget(self.port_select)
        connect_box.addWidget(self.connect_button)      
        connect_box.addStretch(1)
        connect_box.addWidget(self.checkbox)                

        vbox = QtGui.QVBoxLayout()
        vbox.addLayout(connect_box)
     
        vbox.addLayout(hbox) 

        vbox.addWidget(self.pv)      

        self.setLayout(vbox)    

        self.resize(1050, 600)
        self.center()
        self.setWindowTitle('SillyCat Development Tool')
        self.setWindowIcon(QtGui.QIcon(DEV_ICON))        
        self.show()

        self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.console_view.update_console_view)
        self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.print_text)
        self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.log.handle_signal)      

        self.connect(self.ser, QtCore.SIGNAL('new_vram(QByteArray)'), self.show_display_widgets)
        self.connect(self.ser, QtCore.SIGNAL('new_vram(QByteArray)'), self.dv.update_display_view)        
        self.connect(self.ser, QtCore.SIGNAL('new_pck(QString)'), self.pv.update_packet_view)                 
        self.connect(self.dv, QtCore.SIGNAL('new_pixel(QPoint)'), self.update_pixel_text)            


    def connect_clicked(self, pressed):
        if pressed:
            port = str(self.port_select.currentText()).lower()
            print(port)

            try:
                if self.ser.open_port(port, BAUDRATE) == True:                
                    self.ser.start()
                    self.console_view.appendHtml('<b>Connected to ' + port + '</b>') 

                else:
                    self.connect_button.setChecked(False)
                    self.console_view.appendHtml('<b>Failed to open interface on ' + port + '</b>')            
            except:
                    self.connect_button.setChecked(False)
                    self.console_view.appendHtml('<b>Exception when opening interface on ' + port + '</b>')                 

        else:
            self.ser.stop()
            self.console_view.appendHtml('<b>Disconnected<b>')


    def show_display_widgets(self, image_data):
        if (self.save_display_button.isHidden()):
            self.save_display_button.show()
        
        if (self.pixel_label.isHidden()):
            self.pixel_label.show()                 


    def update_pixel_text(self, point):

        self.dv.set_pixel(int(point.x()), int(point.y()))

        self.pixel_label.setText('X: ' + str(point.x()) + ',Y: ' + str(point.y()))


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
