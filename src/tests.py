"""
Code tests meant to be run without connection to the rest of the VehicleHUD
system.
"""

import serial
import unittest

import vehiclehud

class MockDevice(vehiclehud.ELM327):
    speed_str = "55" # 0x05
    expected_return = ""

    def connect(self):
        pass

    def config(self):
        pass
    
    def isOpen(self):
        return True

    def assert_connected(self):
        pass
    
    def readline(self):
        return "{}\r\r>".format(MockDevice.expected_return)

    def readlines(self):
        return self.readline().split("\r")

    def write(self, text):
        if text.lower() == "010d":
            MockDevice.expected_return = str("410D" + MockDevice.speed_str) 
        
class TestInitialize(unittest.TestCase):
    def test_connect(self):
        device = vehiclehud.ELM327()
        with self.assertRaises(vehiclehud.VehicleHUDException):
            device.connect()

    def test_config(self):
        device = vehiclehud.ELM327()
        with self.assertRaises(vehiclehud.VehicleHUDException):
            device.config()

class TestOBDRequest(unittest.TestCase):
    def setUp(self):
        self.device = MockDevice()
        
    def runTest(self):
        speed = self.device.get_vehicle_speed()
        self.assertEquals(speed, int(MockDevice.speed_str, 16))

if __name__ == "__main__":
    unittest.main(exit=False)
