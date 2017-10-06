# c99-wisol-sigfox

A small c99 library for controlling Wisol's SFM10R1AT Sigfox node transceiver.

# What

WISOL SFM10R1AT has a serial interface and responds to an AT command set.
This library is a convenience wrapper for these AT commands. It is
independent of the serial implementation but requires the user to provide
communication adapter code (that is, sending commands via serial and waiting
for a response). An example for the Arduino platform is given.

# How to

For a small example, see `examples/arduino/main.ino`. 

# Contributing

Pull requests are welcome!

# License

(C)opyright Andreas Schmidt, andreas.schmidt@thingforward.io
GNU Lesser General Public License, Version 3
See LICENSE
