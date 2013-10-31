VehicleHUD [![Build Status][ci-img]][ci-link]
=============================================

The purpose of this project is to design and build a projected heads-up
display for a car that is projected on to the windshield. This system will
pull vehicle information from the OBD-II diagnostic jack located in all modern
cars.

Compiling Vehiclehud
--------------------

The main program can be built simply using Make:

```shell
cd src && make
```

Using Vehiclehud
------------------
There is no configuration necessary. Just execute the program that is created
by ```make```. Vehiclehud will attempt to connect to an OBD-II adapter at a
file location defined in the source. If it can connect, it will begin polling
the OBD device for vehicle information and printing the current RPM to
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
