from vehiclehud import *

class DisplayMode:      
    def draw(self):
        for func in self.data_funcs:
            print(u"{}: {} {}".format(func.title, func(), func.unit.encode("utf-8")))

class BasicMode1(DisplayMode):
    def __init__(self, device):
        DisplayMode.__init__(self, device)
        self.data_funcs = [VehicleSpeed, EngineRPM]

class BasicMode2(DisplayMode):
    def __init__(self, device):
        DisplayMode.__init__(self, device)
        self.data_funcs = [VehicleSpeed, EngineRPM, FuelLevel, IntakeTemp]

class TunerMode(DisplayMode):
    def __init__(self, device):
        DisplayMode.__init__(self, device)
        self.data_funcs = [EngineRPM, EngineLoad, ThrottlePosition, MAFAirflow]

if __name__ == "__main__":
    obd = OBD()
    display = BasicMode2()

    for i in range(5):
        display.draw()
