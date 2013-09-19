"""
Tests for integration of the installed VehicleHUD system. It will test
OBD and AVR communication, and any other feature of the system accessible
by script.

Run these tests only on a VehicleHUD system installed in a vehicle.
"""

import time
import unittest

import vehiclehud

device = vehiclehud.ELM327()

def setUpModule():
    connect_to_device()

def tearDownModule():
    close_device()

def connect_to_device():
    device.connect()
    device.config()

def close_device():
    device.close()

class TestInitialize(unittest.TestCase):
    def runTest(self):
        self.assertTrue(device.isOpen())

class TestGetSpeed(unittest.TestCase):
    def runTest(self):
        speed = device.get_vehicle_speed()
        self.assertEqual(speed, 0)

class TestGetRPM(unittest.TestCase):
    def runTest(self):
        rpm = device.get_engine_rpm()
        self.assertGreater(rpm, 0)

class TestPerformance(unittest.TestCase):
    def runTest(self):
        start_time = time.time()
        device.get_vehicle_speed()
        end_time = time.time()
        exec_time = end_time - start_time
        self.assertLess(exec_time, 0.5)

if __name__ == "__main__":
    unittest.main(exit=False)
