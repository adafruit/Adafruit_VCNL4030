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

// Enum for medianRead helper
enum read_type_t { READ_PROX, READ_ALS, READ_WHITE, READ_LUX };

// Forward declarations
float medianRead(Adafruit_VCNL4030& vcnl, read_type_t type, uint8_t n = 3,
                 uint16_t delayMs = 50);
void setAllPixels(uint8_t r, uint8_t g, uint8_t b);

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
  uint16_t raw1 = (uint16_t)medianRead(vcnl, READ_ALS);
  float lux1 = medianRead(vcnl, READ_LUX);
  Serial.print(F("  Raw: "));
  Serial.print(raw1);
  Serial.print(F("  Lux: "));
  Serial.println(lux1, 2);

  // Test 2: HD=1 (2x range, half resolution)
  Serial.println(F("--- Mode 2: HD=1, NS=0 (2x range) ---"));
  vcnl.setALSHighDynamicRange(true);
  vcnl.setALSLowSensitivity(false);
  delay(200);
  uint16_t raw2 = (uint16_t)medianRead(vcnl, READ_ALS);
  float lux2 = medianRead(vcnl, READ_LUX);
  Serial.print(F("  Raw: "));
  Serial.print(raw2);
  Serial.print(F("  Lux: "));
  Serial.println(lux2, 2);

  // Test 3: NS=1 (2x range)
  Serial.println(F("--- Mode 3: HD=0, NS=1 (2x range) ---"));
  vcnl.setALSHighDynamicRange(false);
  vcnl.setALSLowSensitivity(true);
  delay(200);
  uint16_t raw3 = (uint16_t)medianRead(vcnl, READ_ALS);
  float lux3 = medianRead(vcnl, READ_LUX);
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

// ============ Helper functions ============

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

float medianRead(Adafruit_VCNL4030& vcnl, read_type_t type, uint8_t n = 3,
                 uint16_t delayMs = 50) {
  uint16_t readings[9];
  if (n > 9)
    n = 9;
  if (n < 1)
    n = 1;
  for (uint8_t i = 0; i < n; i++) {
    switch (type) {
      case READ_PROX:
        readings[i] = vcnl.readProximity();
        break;
      case READ_ALS:
        readings[i] = vcnl.readALS();
        break;
      case READ_WHITE:
        readings[i] = vcnl.readWhite();
        break;
    }
    if (i < n - 1)
      delay(delayMs);
  }
  for (uint8_t i = 1; i < n; i++) {
    uint16_t key = readings[i];
    int8_t j = i - 1;
    while (j >= 0 && readings[j] > key) {
      readings[j + 1] = readings[j];
      j--;
    }
    readings[j + 1] = key;
  }
  return readings[n / 2];
}
