#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import serial
from PyQt4 import QtGui, QtCore
from SerialInterface import *

import ctypes

PORT = 'com3'
BAUDRATE = 38400

DEV_ICON = os.path.join('icons', 'dev_tool_icon.png')

class DevTool(QtGui.QWidget):
    def __init__(self):
        super(DevTool, self).__init__()
        self.ser = SerialInterface()
        self.initUI()

        self.ser.open_port(PORT, BAUDRATE)
        self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.update_console_view)
        self.connect(self.ser, QtCore.SIGNAL('new_stream(QString)'), self.print_text)        
        self.ser.start()

        
    def initUI(self):

        self.console_view = QtGui.QPlainTextEdit()
        self.console_view.appendHtml('Waiting for input...')
        #self.console_view.appendPlainText('Foobar')



        vbox = QtGui.QVBoxLayout()
        #vbox.addStretch(1)
        vbox.addWidget(self.console_view)     

        self.setLayout(vbox)    

        self.resize(550, 400)
        self.center()
        self.setWindowTitle('SillyCat Development Tool')
        self.setWindowIcon(QtGui.QIcon(DEV_ICON))        
        self.show()


    def closeEvent(self, event):
        self.ser.stop()
        event.accept()

    
    def center(self):    
        qr = self.frameGeometry()
        cp = QtGui.QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())  


    def update_console_view(self, text):
        if text.count('fail') > 0:
            formated_text = '<b>'+text.split('fail')[0]+'<font color="red">'+'fail'+'</font>'+text.split('fail')[1]+'</b>'
            self.console_view.appendHtml(formated_text)
        else:
            self.console_view.appendPlainText(text) 

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
