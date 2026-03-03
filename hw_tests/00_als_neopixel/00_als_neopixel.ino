/*!
 * @file test_als_neopixel.ino
 * @brief HW test: Verify ALS responds to NeoPixel light changes
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - NeoPixel ring (16 pixels) on pin D6, facing the sensor
 *
 * Test: ALS reading should increase when NeoPixels turn ON
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

  Serial.println(F("=== test_als_neopixel ==="));
  Serial.println(F("Testing ALS response to NeoPixel light"));
  Serial.println();

  // Initialize NeoPixels - start OFF
  pixels.begin();
  setAllPixels(0, 0, 0);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  // Enable ALS and white channel
  vcnl.enableALS(true);
  vcnl.enableWhiteChannel(true);
  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  delay(200); // Wait for first reading

  bool allPassed = true;

  // Test 1: ALS raw value increases with NeoPixels
  Serial.println(F("--- Test 1: ALS raw value ---"));
  setAllPixels(0, 0, 0);
  delay(300);
  uint16_t alsOff = (uint16_t)medianRead(vcnl, READ_ALS);
  Serial.print(F("  ALS OFF: "));
  Serial.println(alsOff);

  setAllPixels(255, 255, 255);
  delay(300);
  uint16_t alsOn = (uint16_t)medianRead(vcnl, READ_ALS);
  Serial.print(F("  ALS ON:  "));
  Serial.println(alsOn);

  if (alsOn > alsOff + 10) {
    Serial.println(F("  PASS: ALS increased with NeoPixels ON"));
  } else {
    Serial.println(F("  FAIL: ALS did not increase significantly"));
    allPassed = false;
  }
  Serial.println();

  // Test 2: Lux value increases with NeoPixels
  Serial.println(F("--- Test 2: Lux calculation ---"));
  setAllPixels(0, 0, 0);
  delay(300);
  float luxOff = medianRead(vcnl, READ_LUX);
  Serial.print(F("  Lux OFF: "));
  Serial.println(luxOff, 2);

  setAllPixels(255, 255, 255);
  delay(300);
  float luxOn = medianRead(vcnl, READ_LUX);
  Serial.print(F("  Lux ON:  "));
  Serial.println(luxOn, 2);

  if (luxOn > luxOff + 0.5) {
    Serial.println(F("  PASS: Lux increased with NeoPixels ON"));
  } else {
    Serial.println(F("  FAIL: Lux did not increase significantly"));
    allPassed = false;
  }
  Serial.println();

  // Test 3: White channel responds
  Serial.println(F("--- Test 3: White channel ---"));
  setAllPixels(0, 0, 0);
  delay(300);
  uint16_t whiteOff = (uint16_t)medianRead(vcnl, READ_WHITE);
  Serial.print(F("  White OFF: "));
  Serial.println(whiteOff);

  setAllPixels(255, 255, 255);
  delay(300);
  uint16_t whiteOn = (uint16_t)medianRead(vcnl, READ_WHITE);
  Serial.print(F("  White ON:  "));
  Serial.println(whiteOn);

  if (whiteOn > whiteOff + 10) {
    Serial.println(F("  PASS: White channel increased with NeoPixels ON"));
  } else {
    Serial.println(F("  FAIL: White channel did not increase significantly"));
    allPassed = false;
  }
  Serial.println();

  // Cleanup - turn off NeoPixels
  setAllPixels(0, 0, 0);

  // Final result
  Serial.println(F("========================="));
  if (allPassed) {
    Serial.println(F("ALL TESTS PASSED"));
  } else {
    Serial.println(F("SOME TESTS FAILED"));
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
