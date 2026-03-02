/*!
 * @file test_sunlight_cancel.ino
 * @brief HW test: Verify sunlight cancellation settings work
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 at fixed position
 *
 * Test: Sunlight cancellation enable/disable should work
 * (Hard to test actual effect indoors, just verify settings apply)
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

  Serial.println(F("=== test_sunlight_cancel ==="));
  Serial.println(F("Testing sunlight cancellation settings"));
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

  // Test 1: Sunlight cancellation OFF
  Serial.println(F("--- Sunlight cancellation OFF ---"));
  vcnl.enableSunlightCancellation(false);
  delay(100);

  bool scOff = vcnl.sunlightCancellationEnabled();
  Serial.print(F("  SC enabled (getter): "));
  Serial.println(scOff ? F("YES") : F("NO"));

  uint16_t psOff = medianProximity();
  Serial.print(F("  Proximity: "));
  Serial.println(psOff);

  if (!scOff) {
    Serial.println(F("  PASS: SC reported as disabled"));
  } else {
    Serial.println(F("  FAIL: SC should be disabled"));
    allPassed = false;
  }
  Serial.println();

  // Test 2: Sunlight cancellation ON
  Serial.println(F("--- Sunlight cancellation ON ---"));
  vcnl.enableSunlightCancellation(true);
  delay(100);

  bool scOn = vcnl.sunlightCancellationEnabled();
  Serial.print(F("  SC enabled (getter): "));
  Serial.println(scOn ? F("YES") : F("NO"));

  uint16_t psOn = medianProximity();
  Serial.print(F("  Proximity: "));
  Serial.println(psOn);

  if (scOn) {
    Serial.println(F("  PASS: SC reported as enabled"));
  } else {
    Serial.println(F("  FAIL: SC should be enabled"));
    allPassed = false;
  }
  Serial.println();

  // Test 3: Sunlight cancel current settings
  Serial.println(F("--- SC current multiplier ---"));
  vcnl4030_ps_sc_cur_t levels[] = {VCNL4030_PS_SC_CUR_1X, VCNL4030_PS_SC_CUR_2X,
                                   VCNL4030_PS_SC_CUR_4X,
                                   VCNL4030_PS_SC_CUR_8X};
  const char* names[] = {"1X", "2X", "4X", "8X"};

  for (uint8_t i = 0; i < 4; i++) {
    vcnl.setSunlightCancelCurrent(levels[i]);
    delay(50);
    vcnl4030_ps_sc_cur_t readBack = vcnl.getSunlightCancelCurrent();
    Serial.print(F("  Set "));
    Serial.print(names[i]);
    Serial.print(F(", read "));
    Serial.println(names[(uint8_t)readBack]);
    if (readBack != levels[i]) {
      allPassed = false;
    }
  }
  Serial.println();

  // Test 4: Sunlight protection settings
  Serial.println(F("--- Sunlight protection ---"));

  vcnl.setSunlightProtection(false);
  bool spOff = vcnl.getSunlightProtection();
  Serial.print(F("  SP=0 (1x): "));
  Serial.println(spOff ? F("FAIL") : F("OK"));
  if (spOff)
    allPassed = false;

  vcnl.setSunlightProtection(true);
  bool spOn = vcnl.getSunlightProtection();
  Serial.print(F("  SP=1 (1.5x): "));
  Serial.println(spOn ? F("OK") : F("FAIL"));
  if (!spOn)
    allPassed = false;
  Serial.println();

  // Cleanup
  vcnl.enableSunlightCancellation(false);
  servo.write(180);
  delay(300);
  servo.detach();

  // Results
  Serial.println(F("========================="));

  // Both readings should be valid
  if (psOff > 0 && psOn > 0) {
    Serial.println(F("PASS: Both readings valid"));
  } else {
    Serial.println(F("FAIL: Invalid readings"));
    allPassed = false;
  }

  Serial.println(F("(Note: Indoor testing may not show SC effect)"));

  if (allPassed) {
    Serial.println(F("ALL TESTS PASSED"));
  } else {
    Serial.println(F("SOME TESTS FAILED"));
  }
}

void loop() {
  // Nothing to do
}
