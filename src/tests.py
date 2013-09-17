"""
Code tests meant to be run without connection to the rest of the VehicleHUD
system.
"""

import serial
import unittest

import vehiclehud

class TestInitialize(unittest.TestCase):
    def runTest(self):
        device = vehiclehud.ELM327()
        with self.assertRaises(serial.SerialException):
            device.connect()

if __name__ == "__main__":
    unittest.main(exit=False)
