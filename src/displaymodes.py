import vehiclehud

class DisplayMode:
    def __init__(self, device):
        if device:
            self.device = device
            
    def draw(self):
        for func in self.data_funcs:
            print("{}: {} {}".format(func.title, func(), func.unit))

class BasicMode1(DisplayMode):
    def __init__(self, device):
        DisplayMode.__init__(self, device)
        self.data_funcs = [device.get_vehicle_speed,
                           device.get_engine_rpm]

class BasicMode2(DisplayMode):
    def __init__(self, device):
        DisplayMode.__init__(self, device)
        self.data_funcs = [device.get_vehicle_speed,
                           device.get_engine_rpm,
                           device.get_fuel_level,
                           device.get_intake_temp]

class TunerMode(DisplayMode):
    def __init__(self, device):
        DisplayMode.__init__(self, device)
        self.data_funcs = [device.get_engine_rpm,
                           device.get_engine_load,
                           device.get_throttle_position,
                           device.get_maf_airflow]

if __name__ == "__main__":
    device = vehiclehud.ELM327()
    display = BasicMode1(device)

    for i in range(5):
        display.draw()
