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
#define MID_POS 90

Adafruit_VCNL4030 vcnl;
Servo servo;

uint16_t medianProximity() {
  uint16_t readings[3];
  for (uint8_t i = 0; i < 3; i++) {
    readings[i] = vcnl.readProximity();
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

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== test_ps_cancellation ==="));
  Serial.println(F("Testing PS crosstalk cancellation"));
  Serial.println();

  // Initialize servo at middle position
  servo.attach(SERVO_PIN);
  servo.write(MID_POS);
  delay(500);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enablePS(true);
  vcnl.setLEDCurrent(VCNL4030_LED_I_100MA);
  vcnl.setPSCancellation(0); // Start with no cancellation
  delay(100);

  // Read baseline with no cancellation
  Serial.println(F("--- No cancellation ---"));
  uint16_t baseline = medianProximity();
  Serial.print(F("  Baseline reading: "));
  Serial.println(baseline);

  // Set cancellation to half the baseline
  uint16_t cancelVal = baseline / 2;
  Serial.print(F("--- Setting cancellation to "));
  Serial.print(cancelVal);
  Serial.println(F(" ---"));
  vcnl.setPSCancellation(cancelVal);
  delay(100);

  uint16_t cancelled = medianProximity();
  Serial.print(F("  Cancelled reading: "));
  Serial.println(cancelled);

  // Expected: cancelled reading should be roughly baseline - cancelVal
  int16_t expected = baseline - cancelVal;
  if (expected < 0)
    expected = 0;
  Serial.print(F("  Expected ~"));
  Serial.println(expected);

  // Reset cancellation
  vcnl.setPSCancellation(0);

  // Cleanup
  servo.write(180);
  delay(300);
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
