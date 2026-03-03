/*!
 * @file test_ps_cancellation.ino
 * @brief HW test: Verify PS cancellation reduces baseline
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 at fixed position
 *
 * Test: Setting cancellation value should reduce proximity reading
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define TEST_POS 110 // Peak signal position for testing
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

  Serial.println(F("=== test_ps_cancellation ==="));
  Serial.println(F("Testing PS crosstalk cancellation"));
  Serial.println();

  // Initialize servo at middle position
  servo.attach(SERVO_PIN);
  servo.write(TEST_POS);
  delay(1000);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enableProx(true);
  vcnl.setProxLEDCurrent(VCNL4030_PROX_LED_200MA);
  vcnl.setProxCancellation(0); // Start with no cancellation
  delay(100);

  // Read baseline with no cancellation
  Serial.println(F("--- No cancellation ---"));
  uint16_t baseline = (uint16_t)medianRead(vcnl, READ_PROX);
  Serial.print(F("  Baseline reading: "));
  Serial.println(baseline);

  // Set cancellation to half the baseline
  uint16_t cancelVal = baseline / 2;
  Serial.print(F("--- Setting cancellation to "));
  Serial.print(cancelVal);
  Serial.println(F(" ---"));
  vcnl.setProxCancellation(cancelVal);
  delay(100);

  uint16_t cancelled = (uint16_t)medianRead(vcnl, READ_PROX);
  Serial.print(F("  Cancelled reading: "));
  Serial.println(cancelled);

  // Expected: cancelled reading should be roughly baseline - cancelVal
  int16_t expected = baseline - cancelVal;
  if (expected < 0)
    expected = 0;
  Serial.print(F("  Expected ~"));
  Serial.println(expected);

  // Reset cancellation
  vcnl.setProxCancellation(0);

  // Cleanup
  servo.write(180);
  delay(1000);
  servo.detach();

  // Results
  Serial.println();
  Serial.println(F("========================="));

  // Cancelled reading should be lower
  if (cancelled < baseline) {
    Serial.println(F("PASS: Cancellation reduced the reading"));
  } else {
    Serial.println(F("FAIL: Cancellation did not reduce reading"));
  }

  // Check if cancellation is roughly correct (within 25% of expected)
  int16_t diff = abs((int16_t)cancelled - expected);
  if (diff < baseline / 4) {
    Serial.println(F("PASS: Cancelled value close to expected"));
  } else {
    Serial.println(F("INFO: Cancelled value differs from expected"));
    Serial.print(F("  Difference: "));
    Serial.println(diff);
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
