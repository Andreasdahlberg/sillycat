import unittest
import vram

class TestStringFunctions(unittest.TestCase):

  def test_clean_string(self):
    text_stream = 'spam and eggs\n'
    byte_stream = text_stream.encode('utf-8')

    self.assertIsNone(vram.clean_string(text_stream))
    self.assertEqual(vram.clean_string(byte_stream), 'spam and eggs')

  def test_extract_command(self):
    pass


if __name__ == '__main__':
  unittest.main()