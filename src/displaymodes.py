import vehiclehud

class DisplayMode:
    def draw(self):
        for desc, func in self.data_funcs:
            print("{}: {}".format(desc, func()))

class BasicMode1(DisplayMode):
    def __init__(self, device):
        self.data_funcs = [("Speed (km/h)", device.get_vehicle_speed),
                           ("RPM", device.get_engine_rpm)]

class BasicMode2(DisplayMode):
    def __init__(self, device):
        self.data_funcs = [("Speed (km/h)", device.get_vehicle_speed),
                  ("RPM", device.get_engine_rpm),
                  ("Fuel level", device.get_fuel_level),
                  ("Intake temp (C)", device.get_intake_temp)]

class TunerMode(DisplayMode):
    def __init__(self, device):
        self.data_funcs = [("RPM", device.get_engine_rpm),
                  ("Engine load", device.get_engine_load),
                  ("Throttle position", device.get_throttle_position),
                  ("Airflow (g/s)", device.get_maf_airflow)]

if __name__ == "__main__":
    device = vehiclehud.ELM327()
    display = BasicMode1(device)

    for i in range(5):
        display.draw()
