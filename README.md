# VehicleHUD

This is for my Senior Design project. The plan is to build a projected heads-
up display for a car that is projected on to the windshield. This system will 
pull vehicle information from the OBD jack located in all modern cars.

The initial implementation will have only two steps:  
1. Initialize the serial connection struct
2. Start the serial connection.
3. Enter an infinite loop, displaying speed and RPM periodically.