/*!
 * @file test_active_force.ino
 * @brief HW test: Verify active force mode for triggered readings
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 at fixed position
 *
 * Test: In AF mode, readings should only update after trigger
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define MID_POS 90

Adafruit_VCNL4030 vcnl;
Servo servo;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== test_active_force ==="));
  Serial.println(F("Testing PS active force mode"));
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
  delay(100);

  bool allPassed = true;

  // Test 1: Normal continuous mode
  Serial.println(F("--- Continuous mode (AF=0) ---"));
  vcnl.enableActiveForceMode(false);
  delay(100);

  uint16_t ps1 = vcnl.readProximity();
  delay(100);
  uint16_t ps2 = vcnl.readProximity();
  Serial.print(F("  Reading 1: "));
  Serial.println(ps1);
  Serial.print(F("  Reading 2: "));
  Serial.println(ps2);

  // In continuous mode, both readings should be valid
  if (ps1 > 0 && ps2 > 0) {
    Serial.println(F("  PASS: Continuous readings work"));
  } else {
    Serial.println(F("  FAIL: Continuous readings not working"));
    allPassed = false;
  }
  Serial.println();

  // Test 2: Active force mode
  Serial.println(F("--- Active force mode (AF=1) ---"));
  vcnl.enableActiveForceMode(true);
  delay(100);

  // First reading might be stale from before AF mode
  uint16_t psStale = vcnl.readProximity();
  Serial.print(F("  Stale reading (before trigger): "));
  Serial.println(psStale);

  // Trigger a new reading
  Serial.println(F("  Triggering PS reading..."));
  vcnl.triggerPSReading();
  delay(100); // Wait for reading to complete

  uint16_t psTriggered = vcnl.readProximity();
  Serial.print(F("  Triggered reading: "));
  Serial.println(psTriggered);

  // Triggered reading should be valid
  if (psTriggered > 0) {
    Serial.println(F("  PASS: Triggered reading is valid"));
  } else {
    Serial.println(F("  FAIL: Triggered reading is zero"));
    allPassed = false;
  }
  Serial.println();

  // Test 3: Verify AF mode is actually engaged
  Serial.println(F("--- Verify AF mode engaged ---"));
  bool afEnabled = vcnl.activeForceMode();
  Serial.print(F("  AF mode enabled: "));
  Serial.println(afEnabled ? F("YES") : F("NO"));
  if (afEnabled) {
    Serial.println(F("  PASS: AF mode getter works"));
  } else {
    Serial.println(F("  FAIL: AF mode not reported as enabled"));
    allPassed = false;
  }
  Serial.println();

  // Test 4: Disable AF mode, verify continuous readings resume
  Serial.println(F("--- Disabling AF mode ---"));
  vcnl.enableActiveForceMode(false);
  delay(100);

  uint16_t psResumed = vcnl.readProximity();
  Serial.print(F("  Reading after AF disabled: "));
  Serial.println(psResumed);

  if (psResumed > 0) {
    Serial.println(F("  PASS: Continuous readings resumed"));
  } else {
    Serial.println(F("  FAIL: Readings not resumed after AF disabled"));
    allPassed = false;
  }

  // Cleanup
  servo.write(180);
  delay(300);
  servo.detach();

  // Results
  Serial.println();
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
