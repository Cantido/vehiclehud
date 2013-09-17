"""
Tests for integration of the installed VehicleHUD system. It will test
OBD and AVR communication, and any other feature of the system accessible
by script.

Run these tests only on a VehicleHUD system installed in a vehicle.
"""

import unittest

import vehiclehud

class TestInitialize(unittest.TestCase):
    def runTest(self):
        device = vehiclehud.ELM327()
        self.assertTrue(device.isOpen())

if __name__ == "__main__":
    unittest.main(exit=False)
