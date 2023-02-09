import unittest

from Configuration import Configuration
class TestConfiguration(unittest.TestCase):
    def test_bitfile(self):
        config = Configuration("bitfiles/led_run_10_times_faster/led_test.bit",0x0000)
        config.loadfile()
        self.assertEqual(config.size, 86116)
        self.assertEqual(config.skip, 117)
        self.assertEqual(config.address, 0x0)

    def test_binfile(self ):
        config = Configuration("bitfiles/led_run_10_times_faster/led_test.bin",0x0000)
        config.loadfile()
        self.assertEqual(config.size, 86116)
        self.assertEqual(config.skip, 0)
        self.assertEqual(config.address, 0x0)
if __name__ == '__main__':
    unittest.main()
