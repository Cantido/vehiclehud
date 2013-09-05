import os
import serial
import sys
import time

class ELM327:
    def __init__(self):
        if sys.platform == "win32":
            port = "COM4"
        elif sys.platform == "linux2":
            port = "/dev/ttyUSB0"
            
        self.ser = serial.Serial(port,baudrate=38400, timeout=1)

        #self.write("ATZ")
        #self.write("ATSP0")
        
    def write(self, text):
        self.ser.write(text + "\r")

    def readlines(self):
        return self.ser.readline().split('\r')

    def at_read(self):
        return self.readlines()[-3]

    def at_command(self, request):
        self.write(request)
        return self.at_read()

    def data_request(self, request):
        self.write(request)
        return self.readlines()[-1][2:]

    def getRPM(self):
        data = self.data_request("010c")

        if data == "":
            return 0
        else:
            return int(data, 16) / 4

if __name__ == "__main__":
    device = ELM327()
    print(device.at_command("ATZ"))
    print(device.at_command("ATSP0"))

    for i in range(5):
        print("RPM = {}".format(device.getRPM()))
        time.sleep(1)


