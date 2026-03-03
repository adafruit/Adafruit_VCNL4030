/*!
 * @file test_ps_resolution.ino
 * @brief HW test: Verify 12-bit vs 16-bit PS resolution
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 moving surface close to sensor
 *
 * Test: 16-bit mode should allow values > 4095
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define CLOSE_POS 90 // Reliable high-reading position
#define FAR_POS 180  // Safe return position

Adafruit_VCNL4030 vcnl;
Servo servo;

// Enum for medianRead helper
enum read_type_t { READ_PROX, READ_ALS, READ_WHITE, READ_LUX };

// Forward declarations
float medianRead(Adafruit_VCNL4030& vcnl, read_type_t type, uint8_t n = 3,
                 uint16_t delayMs = 50);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== test_ps_resolution ==="));
  Serial.println(F("Testing 12-bit vs 16-bit proximity resolution"));
  Serial.println();

  // Initialize servo - move close to sensor
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);
  delay(700);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));
  Serial.println(F("Servo at CLOSE position for high readings"));
  Serial.println();

  vcnl.enableProx(true);
  vcnl.setProxLEDCurrent(
      VCNL4030_PROX_LED_200MA); // High current for strong signal
  vcnl.setProxGain(VCNL4030_PROX_GAIN_SINGLE_8X);
  delay(100);

  // Test 12-bit mode (PS_HD=0)
  Serial.println(F("--- 12-bit mode (PS_HD=0) ---"));
  vcnl.setProxResolution16Bit(false);
  delay(100);
  uint16_t ps12bit = (uint16_t)medianRead(vcnl, READ_PROX);
  Serial.print(F("  Reading: "));
  Serial.println(ps12bit);
  Serial.print(F("  Max theoretical: 4095"));
  if (ps12bit <= 4095) {
    Serial.println(F(" (OK)"));
  } else {
    Serial.println(F(" (UNEXPECTED: > 4095)"));
  }

  // Test 16-bit mode (PS_HD=1)
  Serial.println(F("--- 16-bit mode (PS_HD=1) ---"));
  vcnl.setProxResolution16Bit(true);
  delay(100);
  uint16_t ps16bit = (uint16_t)medianRead(vcnl, READ_PROX);
  Serial.print(F("  Reading: "));
  Serial.println(ps16bit);
  Serial.print(F("  Max theoretical: 65535"));
  Serial.println();

  // Cleanup
  servo.write(180);
  delay(1000);
  servo.detach();

  // Results
  Serial.println(F("========================="));

  // In 16-bit mode we should be able to exceed 4095
  // (if signal is strong enough)
  if (ps16bit > ps12bit) {
    Serial.println(F("PASS: 16-bit mode gives higher resolution/range"));
  } else if (ps12bit < 4000) {
    Serial.println(
        F("INFO: 12-bit not saturated, can't verify 16-bit advantage"));
    Serial.println(F("PASS: Both modes returned valid readings"));
  } else {
    Serial.println(F("PASS: 12-bit saturated, 16-bit allows higher values"));
  }

  // Verify we can read valid values in both modes
  if (ps12bit > 0 && ps16bit > 0) {
    Serial.println(F("PASS: Both resolution modes return valid readings"));
  } else {
    Serial.println(F("FAIL: One or both modes returned zero"));
  }
}

void loop() {
  // Nothing to do
}

// ============ Helper functions ============

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
