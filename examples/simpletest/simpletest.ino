/*!
 * @file simpletest.ino
 *
 * Basic test for the Adafruit VCNL4030 library.
 * Reads proximity, ambient light, and white channel values.
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 */

#include <Adafruit_VCNL4030.h>

Adafruit_VCNL4030 vcnl;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("Adafruit VCNL4030 Simple Test"));

  if (!vcnl.begin()) {
    Serial.println(F("Could not find a valid VCNL4030 sensor, check wiring!"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("VCNL4030 Found!"));
  Serial.println();
}

void loop() {
  uint16_t prox = vcnl.readProximity();
  float lux = vcnl.readLux();
  uint16_t white = vcnl.readWhite();

  Serial.print(F("Proximity: "));
  Serial.print(prox);
  Serial.print(F("  Lux: "));
  Serial.print(lux);
  Serial.print(F("  White: "));
  Serial.println(white);

  delay(100);
}
