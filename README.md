# Adafruit VCNL4030X01 Library

This is the Adafruit VCNL4030X01 Proximity and Ambient Light Sensor library for Arduino.

Tested and works great with the Adafruit VCNL4030 Breakout Board
* https://www.adafruit.com/products/466

This chip uses I2C to communicate, 2 pins are required to interface. An optional interrupt pin can be used for threshold-based alerts.

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

## Features

* **Proximity sensing** - 12/16-bit resolution, configurable LED current (50–200mA), duty cycle, integration time, and gain
* **Ambient light sensing** - 16-bit with calibrated lux output, configurable integration time (50–800ms)
* **White channel** - Dedicated white light measurement
* **Interrupt support** - Threshold-based interrupts for both proximity and ALS with configurable persistence
* **Sunlight cancellation** - Built-in sunlight immunity with configurable cancellation current
* **Active force mode** - Single-shot proximity measurements on demand
* **Logic output mode** - Direct proximity threshold output on INT pin

## Installation

To install, use the Arduino Library Manager and search for "Adafruit VCNL4030" and install the library.

## Dependencies

* [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)

## Examples

* **simpletest** - Basic proximity, ALS, lux, and white channel readings
* **fulltest** - Complete configuration display of all sensor features with continuous readings

## Usage

```cpp
#include <Adafruit_VCNL4030.h>

Adafruit_VCNL4030 vcnl;

void setup() {
  Serial.begin(115200);

  if (!vcnl.begin()) {
    Serial.println("Failed to find VCNL4030");
    while (1) delay(10);
  }
}

void loop() {
  Serial.print("Proximity: ");
  Serial.print(vcnl.readProximity());

  Serial.print("\tLux: ");
  Serial.print(vcnl.readLux());

  Serial.print("\tWhite: ");
  Serial.println(vcnl.readWhite());

  delay(100);
}
```

Written by Limor 'ladyada' Fried with assistance from Claude Code for Adafruit Industries.
MIT license, check LICENSE for more information.
All text above must be included in any redistribution.
