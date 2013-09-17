"""
Code tests meant to be run without connection to the rest of the VehicleHUD
system.
"""

import serial
import unittest

import vehiclehud

class MockSerial:
    speed_str = "55" # 0x05
    expected_return = ""
    
    def readline(self):
        return ">echoed command \r empty line \r{}\r".format(MockSerial.expected_return)

    def write(self, text):
        if text.lower() == "010d\r":
            MockSerial.expected_return = str("410D" + MockSerial.speed_str) 
        
class TestInitialize(unittest.TestCase):
    def runTest(self):
        device = vehiclehud.ELM327()
        with self.assertRaises(serial.SerialException):
            device.connect()

class TestOBDRequest(unittest.TestCase):
    def setUp(self):
        self.device = vehiclehud.ELM327()
        self.device.ser = MockSerial()
        self.device.sio = MockSerial()
        
    def runTest(self):
        speed = self.device.get_vehicle_speed()
        self.assertEquals(speed, int(MockSerial.speed_str, 16))

if __name__ == "__main__":
    unittest.main(exit=False)
