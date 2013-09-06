VehicleHUD [![Build Status][ci-img]][ci-link]
=============================================

The purpose of this project is to design and build a projected heads-up
display for a car that is projected on to the windshield. This system will
pull vehicle information from the OBD-II diagnostic jack located in all modern
cars.

Using Vehiclehud
------------------
Vehiclehud is built on Python version 2.7.5 for compatibility with the Pyserial
library. Executing vehiclehud.py will start the program. There is no
configuration necessary. Vehiclehud will scan for a serial port to use and
attempt to connect to an OBD-II adapter. If it can connect, it will begin
polling the OBD device for vehicle information and printing the current RPM to
standard output.

OBD-II Adapter Support
----------------------
Currently this software only supports the ELM327 OBD-II adapter. It is a goal
of this project to encapsulate adapter functions so that adding support for
other adapters is possible.

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Copyright 2013 Robert Richter and Aaron Hall  
Distributed under the terms of the GNU GPL.

[ci-img]: https://travis-ci.org/Cantido/vehiclehud.png?branch=master
[ci-link]: https://travis-ci.org/Cantido/vehiclehud