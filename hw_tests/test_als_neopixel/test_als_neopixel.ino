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

// Get median of 3 readings
uint16_t medianALS() {
  uint16_t readings[3];
  for (uint8_t i = 0; i < 3; i++) {
    readings[i] = vcnl.readALS();
    delay(50);
  }
  // Simple sort for 3 elements
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

uint16_t medianWhite() {
  uint16_t readings[3];
  for (uint8_t i = 0; i < 3; i++) {
    readings[i] = vcnl.readWhite();
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
  uint16_t alsOff = medianALS();
  Serial.print(F("  ALS OFF: "));
  Serial.println(alsOff);

  setAllPixels(255, 255, 255);
  delay(300);
  uint16_t alsOn = medianALS();
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
  float luxOff = medianLux();
  Serial.print(F("  Lux OFF: "));
  Serial.println(luxOff, 2);

  setAllPixels(255, 255, 255);
  delay(300);
  float luxOn = medianLux();
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
  uint16_t whiteOff = medianWhite();
  Serial.print(F("  White OFF: "));
  Serial.println(whiteOff);

  setAllPixels(255, 255, 255);
  delay(300);
  uint16_t whiteOn = medianWhite();
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
