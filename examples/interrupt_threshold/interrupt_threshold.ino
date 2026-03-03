/*!
 * @file interrupt_threshold.ino
 *
 * Proximity interrupt example for the VCNL4030X01.
 * Automatically calibrates a threshold from ambient readings,
 * then uses the INT pin to detect when a hand is nearby.
 *
 * Connect VCNL4030 INT pin to D2 on your Arduino.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 */

#include <Adafruit_VCNL4030.h>

#define INT_PIN 2

Adafruit_VCNL4030 vcnl;
volatile bool intFired = false;

void intHandler() {
  intFired = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("VCNL4030 Proximity Interrupt Example"));
  Serial.println(F("===================================="));

  pinMode(INT_PIN, INPUT_PULLUP);

  if (!vcnl.begin()) {
    Serial.println(F("Couldn't find VCNL4030 sensor!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 Found!"));

  // Calibrate: sample ambient proximity with nothing nearby
  Serial.println(F("\nCalibrating... keep sensor clear"));
  delay(500);

  uint32_t sum = 0;
  for (uint8_t i = 0; i < 10; i++) {
    sum += vcnl.readProximity();
    delay(50);
  }
  uint16_t ambient = sum / 10;
  uint16_t threshold = ambient * 2;
  if (threshold < 50)
    threshold = 50;

  Serial.print(F("Ambient: "));
  Serial.print(ambient);
  Serial.print(F("  Threshold: "));
  Serial.println(threshold);

  // Set up proximity interrupt for close detection
  vcnl.setProxLowThreshold(0);
  vcnl.setProxHighThreshold(threshold);
  vcnl.setProxPersistence(VCNL4030_PROX_PERS_1);
  vcnl.setProxInterruptMode(VCNL4030_PROX_INT_BOTH);

  // Clear any pending flags
  vcnl.readInterruptFlags();

  attachInterrupt(digitalPinToInterrupt(INT_PIN), intHandler, FALLING);

  Serial.println(F("\nReady! Wave your hand near the sensor."));
  Serial.println();
}

void loop() {
  Serial.print(F("Prox: "));
  Serial.print(vcnl.readProximity());

  if (intFired) {
    intFired = false;
    uint8_t flags = vcnl.readInterruptFlags();

    if (flags & VCNL4030_PROX_IF_CLOSE)
      Serial.print(F("  *** IRQ CLOSE ***"));
    if (flags & VCNL4030_PROX_IF_AWAY)
      Serial.print(F("  *** IRQ AWAY ***"));
  }

  Serial.println();
  delay(100);
}
