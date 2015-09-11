#!/usr/bin/python
# -*- coding: utf-8 -*

import os
import sys
from PyQt4 import QtGui, QtCore, Qt
import time
import serial
import math, struct

VRAM_PAGES = 4
VRAM_COLUMNS = 128
VRAM_SIZE = (VRAM_PAGES * VRAM_COLUMNS)

mult4 = lambda n: int(math.ceil(n/4))*4
mult8 = lambda n: int(math.ceil(n/8))*8
lh = lambda n: struct.pack("<h", n)
li = lambda n: struct.pack("<i", n)

def bmp(rows, w):
	h, wB = len(rows), int(mult8(w)/8)
	s, pad = li(mult4(wB)*h+0x20), [0]*(mult4(wB)-wB)
	s = li(mult4(w)*h+0x20)
	return (b"BM" + s + b"\x00\x00\x00\x00\x20\x00\x00\x00\x0C\x00\x00\x00" +
		lh(w) + lh(h) + b"\x01\x00\x01\x00\xff\xff\xff\x00\x00\x00" +
		b"".join([bytes(row+pad) for row in reversed(rows)]))



ser = serial.Serial("COM3", 38400, timeout=2, writeTimeout=0.2)





def FormatVRAMData(data_buffer):
	formated_data = []


	data = 0
	bit_cnt = 0;
	for y in range(0, 32):
		page_idx = math.floor(y / 8)
		page_offset = y % 8
		byte_offset = VRAM_COLUMNS * page_idx
		for x in range(0, 128):
			if (bit_cnt == 8):
				formated_data.append(data)
				data = 0
				bit_cnt = 0

			bit = (data_buffer[byte_offset + x] >> page_offset) & 0x01
			data = (data | (bit << (7 - bit_cnt)))
			bit_cnt = bit_cnt + 1


	cnt = 0

	img_data = []
	row_data = []
	for byte in formated_data:

		if(cnt == 16):
			img_data.append(row_data)
			row_data = []
			cnt = 0

		row_data.append(byte)
		cnt = cnt + 1

	barr = bmp(img_data, 128)

	f = open('vram.bmp', 'wb')
	f.write(bytes(barr))

	data_buffer = []




def CaptureVRAM():
	data_buffer = []
	prev = ''

	while 1:
		data = ser.read()
		if(data):



			if data == b'/' and prev == b'<':
				print('Capture done: ', end="")
				del data_buffer[-1]
				print(len(data_buffer), end="")
				print(" bytes")
				break;
			else:
				data_buffer.append(ord(data))
				prev = data
	FormatVRAMData(data_buffer)






def HandleCommand(command):
	command_map = {'VRAM': CaptureVRAM}
	command_function = command_map.get(command)

	return command_function()

class Display(QtGui.QWidget):
	def __init__(self, parent=None):
		super(Display, self).__init__(parent)

		self.setWindowTitle("DevTool") 

		self.label = QtGui.QLabel(self)

		pixmap = QtGui.QPixmap(os.getcwd() + '/vram.bmp')
		display_pixmap = pixmap.scaled(384, 93)
		self.label.setPixmap(display_pixmap)
		self.resize(display_pixmap.width(),display_pixmap.height())

		self.timer  = QtCore.QTimer(self)
		self.timer.setInterval(1000)
		self.timer.timeout.connect(self.update_display)

	def start(self):
		self.timer.start()

	def stop(self):
		self.timer.stop()

	def update_display(self):
		command_buffer = ''

		while True:
			data = ser.read()
			if data == b'<':
				print('New command started!')

				while data != b'>':
					data = ser.read()
					command_buffer = command_buffer + data.decode("utf-8")
				command_buffer = command_buffer[:-1]
				print('New command was: ' + command_buffer)
				HandleCommand(command_buffer)
				break;		

		pixmap = QtGui.QPixmap(os.getcwd() + '/vram.bmp')
		display_pixmap = pixmap.scaled(384, 93)


		mask = display_pixmap.createMaskFromColor(QtGui.QColor(0, 0, 0), QtCore.Qt.MaskOutColor)

		p = QtGui.QPainter(display_pixmap)
		p.setPen(QtGui.QColor(255, 201, 14))
		p.drawPixmap(display_pixmap.rect(), mask, mask.rect())
		p.end()	

		mask = display_pixmap.createMaskFromColor(QtGui.QColor(255, 255, 255), QtCore.Qt.MaskOutColor)

		p = QtGui.QPainter(display_pixmap)
		p.setPen(QtGui.QColor(0, 0, 0))
		p.drawPixmap(display_pixmap.rect(), mask, mask.rect())
		p.end()		

		self.label.setPixmap(display_pixmap)
		display_pixmap.save('vram.bmp', 'BMP')



if __name__ == '__main__':
	app = QtGui.QApplication(sys.argv)
	w = Display()
	w.show()
	w.start()

	sys.exit(app.exec_())
