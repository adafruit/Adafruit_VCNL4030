/*!
 * @file test_als_sensitivity.ino
 * @brief HW test: Verify ALS_HD and ALS_NS mode changes
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - NeoPixel ring (16 pixels) on pin D6 at fixed brightness
 *
 * Test: Raw counts should differ between modes but lux should compensate
 */

#include <Adafruit_NeoPixel.h>
#include <Adafruit_VCNL4030.h>
#include <Wire.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16

Adafruit_VCNL4030 vcnl;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint16_t medianALS() {
  uint16_t readings[3];
  for (uint8_t i = 0; i < 3; i++) {
    readings[i] = vcnl.readALS();
    delay(50);
  }
  if (readings[0] > readings[1]) {
    uint16_t t = readings[0];
    readings[0] = readings[1];
    readings[1] = t;
  }
  if (readings[1] > readings[2]) {
    uint16_t t = readings[1];
    readings[1] = readings[2];
    readings[2] = t;
  }
  if (readings[0] > readings[1]) {
    uint16_t t = readings[0];
    readings[0] = readings[1];
    readings[1] = t;
  }
  return readings[1];
}

float medianLux() {
  float readings[3];
  for (uint8_t i = 0; i < 3; i++) {
    readings[i] = vcnl.readLux();
    delay(50);
  }
  if (readings[0] > readings[1]) {
    float t = readings[0];
    readings[0] = readings[1];
    readings[1] = t;
  }
  if (readings[1] > readings[2]) {
    float t = readings[1];
    readings[1] = readings[2];
    readings[2] = t;
  }
  if (readings[0] > readings[1]) {
    float t = readings[0];
    readings[0] = readings[1];
    readings[1] = t;
  }
  return readings[1];
}

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== test_als_sensitivity ==="));
  Serial.println(F("Testing ALS_HD and ALS_NS sensitivity modes"));
  Serial.println();

  // Initialize NeoPixels at moderate brightness
  pixels.begin();
  setAllPixels(100, 100, 100);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enableALS(true);
  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  delay(200);

  // Test 1: HD=0, NS=0 (most sensitive, default)
  Serial.println(F("--- Mode 1: HD=0, NS=0 (most sensitive) ---"));
  vcnl.setALSHighDynamicRange(false);
  vcnl.setALSLowSensitivity(false);
  delay(200);
  uint16_t raw1 = medianALS();
  float lux1 = medianLux();
  Serial.print(F("  Raw: "));
  Serial.print(raw1);
  Serial.print(F("  Lux: "));
  Serial.println(lux1, 2);

  // Test 2: HD=1 (2x range, half resolution)
  Serial.println(F("--- Mode 2: HD=1, NS=0 (2x range) ---"));
  vcnl.setALSHighDynamicRange(true);
  vcnl.setALSLowSensitivity(false);
  delay(200);
  uint16_t raw2 = medianALS();
  float lux2 = medianLux();
  Serial.print(F("  Raw: "));
  Serial.print(raw2);
  Serial.print(F("  Lux: "));
  Serial.println(lux2, 2);

  // Test 3: NS=1 (2x range)
  Serial.println(F("--- Mode 3: HD=0, NS=1 (2x range) ---"));
  vcnl.setALSHighDynamicRange(false);
  vcnl.setALSLowSensitivity(true);
  delay(200);
  uint16_t raw3 = medianALS();
  float lux3 = medianLux();
  Serial.print(F("  Raw: "));
  Serial.print(raw3);
  Serial.print(F("  Lux: "));
  Serial.println(lux3, 2);

  // Reset to default
  vcnl.setALSHighDynamicRange(false);
  vcnl.setALSLowSensitivity(false);

  // Cleanup - turn off NeoPixels
  setAllPixels(0, 0, 0);

  // Analyze results
  Serial.println();
  Serial.println(F("========================="));

  // Check if raw counts differ (HD/NS modes reduce sensitivity)
  bool rawsDiffer = (raw2 != raw1) || (raw3 != raw1);

  // Check if lux values are within 30% of each other (compensation working)
  float avgLux = (lux1 + lux2 + lux3) / 3.0;
  bool lux1Ok = (lux1 > avgLux * 0.7) && (lux1 < avgLux * 1.3);
  bool lux2Ok = (lux2 > avgLux * 0.7) && (lux2 < avgLux * 1.3);
  bool lux3Ok = (lux3 > avgLux * 0.7) && (lux3 < avgLux * 1.3);
  bool luxCompensated = lux1Ok && lux2Ok && lux3Ok;

  if (rawsDiffer) {
    Serial.println(F("PASS: Raw counts differ between modes"));
  } else {
    Serial.println(
        F("INFO: Raw counts similar (may be OK at this light level)"));
  }

  if (luxCompensated) {
    Serial.println(F("PASS: Lux values compensated within 30%"));
  } else {
    Serial.println(F("FAIL: Lux values vary more than 30% between modes"));
  }
}

void loop() {
  // Nothing to do
}
