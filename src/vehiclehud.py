import io
import serial
import sys
import time

def obd_connect(device=None):
    if device:
        obd = OBD(device)
    else:
        obd = OBD()

def obd_close():
    obd.close()

class ELM327:
    def connect(self, port=None):
        if port == None:
            if sys.platform == "win32":
                port = "COM5"
            elif sys.platform == "linux2":
                port = "/dev/ttyUSB0"
        try:
            self.ser = serial.Serial(port,baudrate=38400, timeout=0.5)
            self.sio = io.TextIOWrapper(io.BufferedRWPair(self.ser, self.ser), encoding="ASCII", newline="\r", line_buffering=True)
        except serial.SerialException as e:
            raise VehicleHUDException("Cannot connect to ELM327: {}".format(e.message))
        
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
        time.sleep(1)
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

class OBDData:
    pid = ""
    supported = False
    title = ""
    unit = ""
    @staticmethod
    def get():
        pass

class EngineLoad(OBDData):
    pid = "0104"
    title = "engine load"
    unit = "%"
    @staticmethod
    def get():
        """Return the engine load value, as a percentage"""
        data = self.get_data(EngineLoad.pid)
        load = int(data, 16) * 100 / 255.0
        return load

class CoolantTemp(OBDData):
    pid = "0105"
    title = "engine coolant temperature"
    unit = u"\N{DEGREE SIGN}C"
    @staticmethod
    def get():
        data = self.get_data(CoolantTemp.pid)
        temp = int(data, 16) - 40
        return temp

class EngineRPM(OBDData):
    pid = "010c"
    title = "engine RPM"
    unit = "RPM"
    @staticmethod
    def get():
        """Return the engine's RPM, in revolutions per minute, with a
        resolution of 0.25 of a revolution"""
        data = OBD.get_data(EngineRPM.pid)
        rpm = int(data, 16) / 4.0
        return rpm

class VehicleSpeed(OBDData):
    pid = "010d"
    title = "vehicle speed"
    unit = "km/h"
    @staticmethod
    def get():
        """Return the vehicle speed, in kilometers per hour"""
        data = OBD.get_data(VehicleSpeed.pid)
        speed = int(data, 16)
        return speed

class IntakeTemp(OBDData):
    pid = "010f"
    title = "intake temperature"
    unit = u"\N{DEGREE SIGN}C"
    @staticmethod
    def get():
        """Return the intake air temperature, in degrees celcius"""
        data = self.get_data(IntakeTemp.pid)
        intake_temp = int(data, 16) - 40
        return intake_temp

class MAFAirflow(OBDData):
    pid = "0110"
    title = "airflow"
    unit = "g/s"
    @staticmethod
    def get():
        """Get the rate of air flow as measured by the MAF sensor, in
        grams per second"""
        data = self.get_data(MAFAirflow.pid);
        airflow = int(data, 16) / 100.0
        return airflow

class ThrottlePosition(OBDData):
    pid = "0111"
    title = "throttle position"
    unit = "%"
    @staticmethod
    def get():
        """Return the throttle position, as a percentage"""
        data = self.get_data(ThrottlePosition.pid)
        throttle_pos = int(data, 16) * 100 / 255.0
        return throttle_pos

class FuelLevel(OBDData):
    pid = "012f"
    title = "fuel level"
    unit = "%"
    def get_fuel_level(self):
        """Return the vehicle's fuel level, as a percentage"""
        data = self.get_data(FuelLevel.pid)
        fuel_level = int(data, 16) * 100 / 255.0
        return fuel_level

class BatteryVoltage(OBDData):
    title = "battery voltage"
    unit ="V"
    supported = True
    def get():
        return OBD.device.get_battery_voltage()

class OBD:
    device = None
    
    def __init__(self, device=None):
        if device:
            OBD.device = device
        else:
            OBD.device = ELM327()
            OBD.device.connect()
            OBD.device.config()
        self.update_supported_pids()

    def close(self):
        OBD.device.close()


    @staticmethod
    def update_supported_pids():
        supp_pid_hex = self.get_data("0100")
        OBD.supported_pids = bin(int(supp_pid_hex, 16))[2:].zfill(32)

    @staticmethod
    def assert_supported(pid):
        if not OBD.pid_supported(pid):
            raise VehicleHUDException("The PID {} is not supported by this vehicle".format(pid))

    @staticmethod
    def pid_supported(pid):
        index = pid[2:]
        is_pid_supported = OBD.supported_pids[index] == "1"
        return is_pid_supported

    @staticmethod
    def get_data(request):
        OBD.assert_supported(request)
        return OBD.device.get_data(request)


class VehicleHUDException(Exception):
    pass

def demonstrate():
    try:
        device = ELM327()
        device.connect()
        device.config()
        obd = OBD(device)
        
        for i in range(10):
            try:
                rpm = obd.get_engine_rpm()
                speed = obd.get_vehicle_speed()
                print("Vehicle RPM: {} rev/min".format(rpm))
                print("Vehicle speed: {} km/h".format(speed))
            except ValueError:
                print("Couldn't print on this pass")
    finally:
        device.close()

if __name__ == "__main__":
    from tests import *
    unittest.main(exit=False)
    #demonstrate()

