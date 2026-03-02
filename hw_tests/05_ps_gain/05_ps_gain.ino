/*!
 * @file test_ps_gain.ino
 * @brief HW test: Verify PS gain settings affect proximity readings
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 at fixed position
 *
 * Test: Higher gain should give higher proximity counts
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

  Serial.println(F("=== test_ps_gain ==="));
  Serial.println(F("Testing proximity at different gain settings"));
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

  // Test each gain setting
  Serial.println(F("Gain setting vs proximity:"));

  vcnl.setPSGain(VCNL4030_PS_GAIN_SINGLE_1X);
  delay(100);
  uint16_t ps1x = medianProximity();
  Serial.print(F("  SINGLE_1X: "));
  Serial.println(ps1x);

  vcnl.setPSGain(VCNL4030_PS_GAIN_SINGLE_8X);
  delay(100);
  uint16_t ps8x = medianProximity();
  Serial.print(F("  SINGLE_8X: "));
  Serial.println(ps8x);

  vcnl.setPSGain(VCNL4030_PS_GAIN_TWO_STEP);
  delay(100);
  uint16_t psTwoStep = medianProximity();
  Serial.print(F("  TWO_STEP:  "));
  Serial.println(psTwoStep);

  // Cleanup
  servo.write(150);
  delay(300);
  servo.detach();

  // Results
  Serial.println();
  Serial.println(F("========================="));

  // 8X gain should give higher counts than 1X
  if (ps8x > ps1x) {
    Serial.println(F("PASS: 8X gain higher than 1X gain"));
  } else {
    Serial.println(F("FAIL: 8X gain not higher than 1X (may be saturated)"));
  }

  // All readings should be valid (non-zero with object present)
  if (ps1x > 0 && ps8x > 0 && psTwoStep > 0) {
    Serial.println(F("PASS: All gain modes return valid readings"));
  } else {
    Serial.println(F("FAIL: Some gain modes returned zero"));
  }
}

void loop() {
  // Nothing to do
}
