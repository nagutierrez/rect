# rect

Rotary Engine Compression Testing with an Arduino.

## Background

Measuring compression on a Rotary engine can provide insight into its internal
health. Typically this is done with an analog gauge meant for reciprocating
piston engines. Because we need to individually measure three consecutive faces
of a rotor, this means watching the meter bounce as the engine spins; this can
be tricky for the inexperienced.

This repo provides the Arduino code for measuring and logging the pressure peaks
for each face, so we can more easily see the health of the engine.

## Setup

### Hardware

- 0 -> 150 PSI Pressure Sensor
- Arduino Nano Every
- Spark Plug Adapter

[TwistedRotors Technology](https://www.twistedrotorstechnology.com/) sells a
[very nice DIY kit](https://www.twistedrotorstechnology.com/store) with the
sensor and adapter, and even provides their own OS! If you're not feeling like
DIY, they also [sell a complete
tool](https://www.twistedrotorstechnology.com/tr-01-v3-0).

_Full disclosure; this repo has no affiliation with Twisted Rotors, but their
kit inspired this work._

#### Construction

The sensor and Arduino should be wired as follows;

- Red sensor lead to Arduino `+5V`
- Black sensor lead to Arduino `Gnd`
- Green sensor lead to Arduino `A7`

### Software

After physical construction, upload this code to the Arduino using the Arduino
IDE.

## Usage

See [this
article](http://www.racingbeat.com/mazda/performance/rotary-tech-tips/compression-check.html)
on how to properly measure your rotary engine's compression.

The primary difference is we'll be using our sensor, Arduino, and a laptop.

1. After following the article's directions, install the pressure sensor in the
   leading spark plug of the rotor to test.
1. Connect the arduino and launch the serial monitor on your computer. It will
   zero out the current pressure.
1. Proceed to crank the engine, and collect your compression data.

## License

[MIT](https://choosealicense.com/licenses/mit/)
