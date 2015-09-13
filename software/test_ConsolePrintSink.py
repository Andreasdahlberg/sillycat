#!/usr/bin/python
# -*- coding: utf-8 -*

import unittest
from contextlib import redirect_stdout
from io import StringIO

from ConsolePrintSink import *


class TestConsolePrintSink(unittest.TestCase):

	def test_push(self):
		print_sink = ConsolePrintSink()
		text_stream = 'spam and eggs\r\n'
		byte_stream = text_stream.encode('utf-8')
		
		output = StringIO()
		with redirect_stdout(output):
			print_sink.push(byte_stream)

		#One newline char is expected since print() adds one
		self.assertEqual(output.getvalue(), 'spam and eggs\n')

		output = StringIO()
		with redirect_stdout(output):
			print_sink.push(text_stream)

		self.assertEqual(output.getvalue(), '')		


if __name__ == '__main__':
	unittest.main()