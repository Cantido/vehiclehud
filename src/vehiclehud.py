import io
import serial
import sys
import time

class ELM327:
    def connect(self, port=None):
        if port == None:
            if sys.platform == "win32":
                port = "COM4"
            elif sys.platform == "linux2":
                port = "/dev/ttyUSB0"
        self.ser = serial.Serial(port,baudrate=38400, timeout=1)
        self.sio = io.TextIOWrapper(io.BufferedRWPair(self.ser, self.ser), encoding="ASCII", newline="\r", line_buffering=True)

        self.disable_space_printing()
        self.disable_command_echo()

    def isOpen(self):
        return ser.isOpen()
    
    def write(self, text):
        self.ser.write(text + "\r")

    def readline(self):
        return self.sio.readline()
        
    def write(self, text):
        self.ser.write(text + "\r")

    def readlines(self):
        return self.ser.readline().split("\r")

    def at_read(self):
        return self.readlines()

    def at_command(self, request):
        self.write(request)
        return self.at_read()

    def data_request(self, request):
        self.write(request)
        return self.readlines()

    def disable_space_printing(self):
        self.write("ATS0")

    def enable_space_printing(self):
        self.write("ATS1")

    def disable_command_echo(self):
        self.write("ATE0")

    def enable_command_echo(self):
        self.write("ATE1")

    def reset(self):
        self.write("ATZ")

    def auto_find_protocol(self):
        self.write("ATSP00")
    
    def get_data(self, request):
        """
        Request vehicle data by PID

        This should return ONLY the bytes representing the data requested.
        It should be possible to parse an integer straight from the data
        this method returns, if the given PID requests an integer value.
        """
        request_mode = int(request[0:2], 16)
        request_pid = int(request[2:4], 16)
        
        self.write(request)
        data = device.readlines()[2]
        if "UNABLE TO CONNECT" in data:
            raise Exception("Unable to connect to the vehicle")

        response_mode = int(data[0:2], 16)
        response_pid = int(data[2:4], 16)
        response_payload = data[4:]

        assert(request_mode == response_mode - 0x40)
        assert(request_pid == response_pid)
        
        return response_payload
        
    def get_engine_rpm(self):
        data = self.data_request("010c")
        return int(data, 16) / 4

    def get_vehicle_speed(self):
        data = self.data_request("010d")
        return int(data, 16)

if __name__ == "__main__":
    device = ELM327()
    print(device.at_command("ATZ"))
    print(device.at_command("ATSP0"))
    print(device.data_request("010c"))


