# Sensorboard v1.0

This library handler the interfacing for the Arduino nano sensorboard. The sensorboard
has 2 i2c connectors and 4 hardware SPI connectors and a canbus using SPI. The CS is
port 10, combined with an address selector on pins 20, 17 and 16. The selector
can be set using the `SelectSpi` method

pins 21, 22, 23 and 24 can be used to set options on or off.  They are wired with a pull up resistor
with a dip-switch array.