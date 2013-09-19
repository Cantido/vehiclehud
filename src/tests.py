"""
Code tests meant to be run without connection to the rest of the VehicleHUD
system.
"""

import serial
import unittest

import vehiclehud

class MockDevice(vehiclehud.ELM327):
    def connect(self):
        pass
    def config(self):
        pass
    def isOpen(self):
        return True
    def assert_connected(self):
        pass
    def readline(self):
        return "{}\r\r>".format(self.expected_response)
    def readlines(self):
        return self.readline().split("\r")
    def write(self, text):
        pass
    def set_expected_data(self, expected):
        self.expected_response = expected
        
class TestInitialize(unittest.TestCase):
    def test_connect(self):
        device = vehiclehud.ELM327()
        with self.assertRaises(vehiclehud.VehicleHUDException):
            device.connect()

    def test_config(self):
        device = vehiclehud.ELM327()
        with self.assertRaises(vehiclehud.VehicleHUDException):
            device.config()

class TestGetSpeed(unittest.TestCase):
    def setUp(self):
        self.device = MockDevice()
        self.expected_speed = 85
        self.hex_speed = hex(self.expected_speed)[2:]

    def test_valid_device_response(self):
        self.device.set_expected_data("410d" + self.hex_speed)
        speed = self.device.get_vehicle_speed()
        self.assertEquals(speed, self.expected_speed)
    
    def test_invalid_device_response(self):
        self.device.set_expected_data("410c" + self.hex_speed)
        with self.assertRaises(vehiclehud.VehicleHUDException):
            speed = self.device.get_vehicle_speed()

if __name__ == "__main__":
    unittest.main(exit=False)
