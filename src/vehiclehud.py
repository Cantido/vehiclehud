import io
import serial
import sys
import time

class ELM327:
    def connect(self, port=None):
        if port == None:
            if sys.platform == "win32":
                port = "COM5"
            elif sys.platform == "linux2":
                port = "/dev/ttyUSB0"
        try:
            self.ser = serial.Serial(port,baudrate=38400, timeout=0.09)
            self.sio = io.TextIOWrapper(io.BufferedRWPair(self.ser, self.ser), encoding="ASCII", newline="\r", line_buffering=True)
        except serial.SerialException as e:
            raise VehicleHUDException("Cannot connect to ELM327: {}".format(e.message))
        finally:
            self.close()
        
    def config(self):
        """Apply the default device configurations"""
        self.assert_connected()
        self.disable_space_printing()
        self.disable_command_echo()
        self.enable_aggressive_timing()

    def isOpen(self):
        try:
            connection_open = self.ser.isOpen() and not self.sio.closed
        except AttributeError:
            return False
        return connection_open

    def close(self):
        try:
            self.sio.close()
        except:
            pass
        try:
            self.ser.close()
        except:
            pass
    
    def write(self, text):
        self.ser.write(text + "\r")

    def readlines(self):
        return self.ser.readline().split("\r")

    def at_command(self, request):
        self.write(request)
        time.sleep(0.5)
        return self.readlines()

    def enable_aggressive_timing(self):
        self.at_command("ATAT2")

    def disable_command_echo(self):
        self.at_command("ATE0")

    def enable_command_echo(self):
        self.at_command("ATE1")

    def get_battery_voltage(self):
        self.write("ATRV")
        response = self.readlines()
        voltage = response[0][:-1] # Strip the trailing "V"
        return float(voltage)
    get_battery_voltage.title = "battery voltage"
    get_battery_voltage.unit ="V"
    
    def disable_space_printing(self):
        self.at_command("ATS0")

    def enable_space_printing(self):
        self.at_command("ATS1")

    def reset(self):
        self.at_command("ATZ")

    def auto_find_protocol(self):
        self.at_command("ATSP00")
    
    def get_data(self, request):
        """
        Request vehicle data by PID

        This should return ONLY the bytes representing the data requested.
        It should be possible to parse an integer straight from the data
        this method returns, if the given PID requests an integer value.
        """
        self.assert_connected()
        request_mode, request_pid, request_payload = self.split_obd_string(request)
        
        self.write(request)
        response = self.readlines()[0]
        if "UNABLE TO CONNECT" in response:
            raise VehicleHUDException(
                """The ELM327 is unable to communicate with the vehicle""")

        response_mode, response_pid, response_payload = self.split_obd_string(response)

        if not (request_mode == response_mode - 0x40):
            raise VehicleHUDException(
                """ELM327 did not respond as expected: Expected mode {:#04x} but
                recieved {:#04x}""".format(request_mode + 0x40, response_mode))
        if not (request_pid == response_pid):
            raise VehicleHUDException(
                """ELM327 did not respond as expected: Expected PID {:#04x} but
                recieved {:#04x}""".format(request_pid, response_pid))
        
        return response_payload
    
    def split_obd_string(self, obd_string):
        mode = int(obd_string[0:2], 16)
        pid = int(obd_string[2:4], 16)
        payload = obd_string[4:]
        return [mode, pid, payload]

    def assert_connected(self):
        """Raise an exception if a connection to the ELM327 has not been
        established"""
        if not self.isOpen():
            raise VehicleHUDException(
                """The ELM327 is not connected""")

    def get_engine_load(self):
        """Return the engine load value, as a percentage"""
        data = self.get_data("0104")
        load = int(data, 16) * 100 / 255.0
        return load
    get_engine_load.title = "engine load"
    get_engine_load.unit = "%"

    def get_engine_coolant_temp(self):
        """Return the engine coolant temperature, in degrees Celcius"""
        data = self.get_data("0105")
        temp = int(data, 16) - 40
        return temp
    get_engine_coolant_temp.title = "engine coolant temperature"
    get_engine_coolant_temp.unit = u"\u00b0C" # \u00b0 is the degree symbol
    
    def get_engine_rpm(self):
        """Return the engine's RPM, in revolutions per minute, with a
        resolution of 0.25 of a revolution"""
        data = self.get_data("010c")
        rpm = int(data, 16) / 4.0
        return rpm
    get_engine_rpm.title = "engine RPM"
    get_engine_rpm.unit = "RPM"

    def get_vehicle_speed(self):
        """Return the vehicle speed, in kilometers per hour"""
        data = self.get_data("010d")
        speed = int(data, 16)
        return speed
    get_vehicle_speed.title = "vehicle speed"
    get_vehicle_speed.unit = "km/h"

    def get_intake_temp(self):
        """Return the intake air temperature, in degrees celcius"""
        data = self.get_data("010f")
        intake_temp = int(data) - 40
        return intake_temp
    get_intake_temp.title = "intake temperature"
    get_intake_temp.unit = u"\u00b0C" # \u00b0 is the degree symbol

    def get_maf_airflow(self):
        """Get the rate of air flow as measured by the MAF sensor, in
        grams per second"""
        data = self.get_data("0110");
        airflow = int(data, 16) / 100.0
        return airflow
    get_maf_airflow.title = "airflow"
    get_maf_airflow.unit = "g/s"
        
    def get_throttle_position(self):
        """Return the throttle position, as a percentage"""
        data = self.get_data("0111")
        throttle_pos = int(data, 16) * 100 / 255.0
        return throttle_pos
    get_throttle_position.title = "throttle position"
    get_throttle_position.unit = "%"

    def get_fuel_level(self):
        """Return the vehicle's fuel level, as a percentage"""
        data = self.get_data("012f")
        fuel_level = int(data, 16) * 100 / 255.0
        return fuel_level
    get_fuel_level.title = "fuel level"
    get_fuel_level.unit = "%"

class VehicleHUDException(Exception):
    pass

def demonstrate():
    try:
        device = ELM327()
        device.connect()
        device.config()
        for i in range(10):
            try:
                rpm = device.get_engine_rpm()
                speed = device.get_vehicle_speed()
                print("Vehicle RPM: {} rev/min".format(rpm))
                print("Vehicle speed: {} km/h".format(speed))
            except ValueError:
                pass
    finally:
        device.close()

if __name__ == "__main__":
    from tests import *
    unittest.main(exit=False)

