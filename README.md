# roamer-pad
Programmable macro controller for Arduino Nano + Arduino Leonardo based on the Roamer robot. The Leonardo takes input from one keypad and acts as a HID keyboard and an I2C server, while the Nano acts as a client GPIO extender for the second keypad.

## Description
A device to allow pre-programmed inputs for video games. Allows for multiple simultaneous inputs and variable lengths of input, looping commands and the ability to swap between using the device like a standard controller and a programmable controller. An example input sequence for demonstration:

1) Hold Right and B for 2 seconds
2) Press A, Right and Up for 4 seconds
3) Press Down for 1 second
4) Repeat

## Needed 
* Arduino Leonardo
* Arduino Nano
* Two potentiometers 
* Two keypads
* Two LEDs


