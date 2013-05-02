VehicleHUD
==========
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

My senior design project is to build a projected heads-up display for a car
that is projected on to the windshield. This system will pull vehicle
information from the OBD jack located in all modern cars.

The initial software implementation will have only three steps:
 
1. Initialize the serial connection struct
2. Start the serial connection.
3. Enter an infinite loop, displaying speed and RPM periodically.

At this point, this program is only an attempt to parse OBD data from the
vehicle. Once the data connection is established, we can begin adding other
components. The data will be reflected on to the windshield with a vacuum
fluorescent display. The light from the dispay will be collimated with either
mirrors or lenses.

This prgram will be installed on a Linux-based operating system, most likely a
pcDuino running Ubuntu 12.10 or 13.04.

OBD Communication
-----------------
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

We will use a consumer device to allow us to communicate with the CAN bus
via Bluetooth. This specific device/protocol may change, but in all likelihood
we will still be using a serial device, even if it isn't a Bluetooth device.

Specifically, we are performing initial testing with an ELM327 OBD to RS232
Interpreter. The serial requirements for the device are as follows:

* Baud rate: `9600` symbols/sec
* Data bits: `8`
* Parity: `None`
* Stop bits: `1`
* Line end: Carriage return (`'\\r'` or `0x0D`) (input and output)

A majority of messages will be less than 7 data bytes long. We must send a
a signal of `ATAL` to allow longer messages.

### Initialization

1. Open a non-blocking, read-write file descriptor, and apply the above
attributes to begin the serial connection.
2. Send the *Reset All* signal, `ATZ`.
3. *Disable Command Echoing* with `ATE0`.
4. Ask the chip to *find a protocol* with `ATSP0`.

At this point, we can begin communicating normally.

### Vehicle Data Queries

To request data from the vehicle's systems, we send it a byte string according
to the OBD standard. More information may be found on [Wikipedia](https://en.wikipedia.org/wiki/OBD-II_PIDs "OBD-II PIDs - Wikipedia, the free encyclopedia")

Commands take the form of:

* First byte: Mode number
* Second byte: PID

When we issue a command (terminated by a carriage return), the device will
respond with a hex string consisting of:

* First byte: Mode number + `0x40` to signify a response
* Second byte: Echo of requested PID
* Remaining bytes: Requested data

As a prototype implementation, our system will be requesting the speed
(`0x010C`) and RPM (`0x010D`) and printing this information
to the console.

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Copyright 2013 Robert Richter and Aaron Hall  
Distributed under the terms of the GNU GPL.