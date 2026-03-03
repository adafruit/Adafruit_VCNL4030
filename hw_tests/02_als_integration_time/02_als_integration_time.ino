/*!
 * @file test_als_integration_time.ino
 * @brief HW test: Verify ALS integration time affects raw counts
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - NeoPixel ring (16 pixels) on pin D6 at fixed moderate brightness
 *
 * Test: Longer integration times should give higher raw counts
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

  Serial.println(F("=== test_als_integration_time ==="));
  Serial.println(F("Testing ALS at different integration times"));
  Serial.println();

  // Initialize NeoPixels at moderate brightness
  pixels.begin();
  setAllPixels(64, 64, 64);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enableALS(true);
  delay(100);

  // Test each integration time
  uint16_t readings[5];
  const char* names[] = {"50ms", "100ms", "200ms", "400ms", "800ms"};
  vcnl4030_als_it_t times[] = {VCNL4030_ALS_IT_50MS, VCNL4030_ALS_IT_100MS,
                               VCNL4030_ALS_IT_200MS, VCNL4030_ALS_IT_400MS,
                               VCNL4030_ALS_IT_800MS};
  uint16_t settleDelays[] = {100, 150, 300, 500, 900};

  Serial.println(F("Integration time vs raw counts:"));
  for (uint8_t i = 0; i < 5; i++) {
    vcnl.setALSIntegrationTime(times[i]);
    delay(settleDelays[i]); // Wait for at least one measurement cycle
    readings[i] = medianRead(vcnl, READ_ALS);
    Serial.print(F("  "));
    Serial.print(names[i]);
    Serial.print(F(": "));
    Serial.println(readings[i]);
  }
  Serial.println();

  // Verify trend: longer IT should give higher counts
  bool increasing = true;
  for (uint8_t i = 1; i < 5; i++) {
    if (readings[i] < readings[i - 1]) {
      increasing = false;
      break;
    }
  }

  // Cleanup - turn off NeoPixels
  setAllPixels(0, 0, 0);

  // Results
  Serial.println(F("========================="));
  if (increasing) {
    Serial.println(F("PASS: Longer integration times give higher counts"));
  } else {
    Serial.println(F("FAIL: Counts did not increase with integration time"));
    Serial.println(F("(Note: May saturate at high brightness)"));
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
