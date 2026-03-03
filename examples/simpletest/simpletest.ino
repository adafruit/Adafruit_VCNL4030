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
  uint16_t als = vcnl.readALS();
  uint16_t white = vcnl.readWhite();

  if (prox == 0xFFFF || als == 0xFFFF || white == 0xFFFF) {
    Serial.println(F("I2C read error!"));
    delay(100);
    return;
  }

  Serial.print(F("Proximity: "));
  Serial.print(prox);
  Serial.print(F("  Lux: "));
  Serial.print(vcnl.readLux());
  Serial.print(F("  White: "));
  Serial.println(white);

  delay(100);
}
