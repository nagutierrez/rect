# rect

Rotary Engine Compression Testing with an Arduino

## Setup

### Hardware

- 0 -> 150 PSI Pressure Sensor
- Arduino Nano Every
- Adapter to thread the sensor into the rotor housing

[TwistedRotors Technology](https://www.twistedrotorstechnology.com/) sells a [very nice DIY kit](https://www.twistedrotorstechnology.com/store) with the sensor and adapter, and even provides their own OS! If you're not feeling like DIY, they also [sell a complete tool](https://www.twistedrotorstechnology.com/tr-01-v3-0).

_Full disclosure; this repo has no affiliation with Twisted Rotors, but their kit inspired this work._

#### Construction

The sensor and Arduino should be wired as follows;

- Red sensor lead to Arduino `+5V`
- Black sensor lead to Arduino `Gnd`
- Green sensor lead to Arduino `A7`

_TODO: add arduino wiring diagram_

### Software

After physical construction, upload this code to the Arduino using the Arduino IDE.

## Usage

_TODO: explain how to use with a real car_

## License

[MIT](https://choosealicense.com/licenses/mit/)
