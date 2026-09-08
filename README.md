# SimButtonBoxTFT

A touchscreen USB button box for simulators, built with an Arduino-compatible
native-USB board and a 2.4-inch ILI9341 resistive TFT shield.

It appears on the computer as a standard USB HID joystick and provides 32
momentary buttons plus seven adjustable axes from a 240x320 touchscreen.

## Features

- 32 joystick buttons across two 4x4 pages
- Rx, Ry, Rz, X, Y, Z, and throttle controls (0-100)
- Press-and-hold axis adjustment
- Immediate visual feedback for button presses
- Non-blocking touch handling for reliable HID button releases
- Centralized touch calibration and behavior settings

## Hardware

- Arduino Leonardo, Micro, or another ATmega32U4 board with native USB HID
- 2.4-inch 240x320 ILI9341 TFT shield with a resistive touchscreen
- USB data cable

An Arduino Uno does not provide native USB HID with this sketch. The shield may
physically fit an Uno, but use a Leonardo-compatible pin layout or adapt the
constructor pins in `SimButtonBoxTFT.ino`.

## Required Arduino libraries

Install these through **Tools > Manage Libraries** in Arduino IDE:

1. **Adafruit GFX Library** by Adafruit
2. **Adafruit TFTLCD Library** by Adafruit
3. **TouchScreen** by Adafruit
4. **Joystick Library** by Matthew Heironimus

The final dependency is
[MHeironimus/ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary),
not a similarly named package.

## Build and upload

1. Clone or download this repository.
2. Open `SimButtonBoxTFT.ino` in Arduino IDE.
3. Select the native-USB board and its serial port.
4. Install the required libraries and upload the sketch.
5. On Windows, open **Set up USB game controllers** to verify the inputs before
   assigning them in a simulator.

## Controls

| Page | Controls | USB mapping |
| --- | --- | --- |
| A | Buttons 01-16 | HID buttons 1-16 |
| B | Buttons 17-32 | HID buttons 17-32 |
| C | Seven `-` / `+` controls | Rx, Ry, Rz, X, Y, Z, throttle |

Tap **ABOUT** at the lower-right of page C for project information. Use any tab
to leave that screen.

## Touch calibration

If touches are mirrored or offset, adjust the four constants in the `Config`
namespace near the top of the sketch:

```cpp
constexpr int16_t touchMinX = 150, touchMaxX = 920;
constexpr int16_t touchMinY = 120, touchMaxY = 940;
```

Settings for touch pressure, axis repeat speed, axis step size, and colors are
also grouped near the top of the sketch.

## Troubleshooting

- **No joystick appears:** confirm the board supports native USB and install the
  MHeironimus Joystick library.
- **White screen:** verify that the TFT uses ILI9341 controller ID `0x9341`.
- **Touch is offset:** recalibrate the four touch limits.
- **Upload port disappears:** double-tap reset on a Leonardo/Micro, select the
  temporary bootloader port, and upload again.

## Project photos

![Button page](IMG_20190125_133329.png)

![Second button page](IMG_20190125_133338.png)

![Axis page](IMG_20190125_133355.png)

![Project information](IMG_20190125_133408.png)

## License

No license has been selected yet. Until one is added, standard copyright rules
apply. Add an open-source license if you want others to reuse and contribute.

Bug reports and ideas are welcome through
[GitHub Issues](https://github.com/bokiloki/SimButtonBoxTFT/issues).
