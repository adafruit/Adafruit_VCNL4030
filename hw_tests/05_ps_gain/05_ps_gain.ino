/*!
 * @file 05_ps_gain.ino
 * @brief HW test: Verify PS gain modes affect sensitivity
 *
 * Gain modes (most to least sensitive):
 *   TWO_STEP > SINGLE_1X > SINGLE_8X
 * The "8X" means 8x extended range, NOT 8x more gain.
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define CLOSE_POS 90

Adafruit_VCNL4030 vcnl;
Servo servo;

uint16_t readGain(vcnl4030_ps_gain_t gain) {
  vcnl.setProxGain(gain);
  delay(200);
  vcnl.readProximity();
  delay(50);
  return vcnl.readProximity();
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== 05_ps_gain ==="));
  Serial.println(F("Testing PS gain modes"));
  Serial.println();

  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);
  delay(1000);
  servo.detach();

  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enableProx(true);
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_200MA);
  delay(200);

  uint16_t twoStep = readGain(VCNL4030_PS_GAIN_TWO_STEP);
  uint16_t single1x = readGain(VCNL4030_PS_GAIN_SINGLE_1X);
  uint16_t single8x = readGain(VCNL4030_PS_GAIN_SINGLE_8X);

  Serial.println(F("Gain mode vs proximity (most to least sensitive):"));
  Serial.print(F("  TWO_STEP:  "));
  Serial.println(twoStep);
  Serial.print(F("  SINGLE_1X: "));
  Serial.println(single1x);
  Serial.print(F("  SINGLE_8X: "));
  Serial.println(single8x);

  Serial.println();
  Serial.println(F("========================="));

  bool pass = true;
  if (twoStep > single1x) {
    Serial.println(F("PASS: TWO_STEP > SINGLE_1X"));
  } else {
    Serial.println(F("FAIL: TWO_STEP should be > SINGLE_1X"));
    pass = false;
  }
  if (single1x > single8x) {
    Serial.println(F("PASS: SINGLE_1X > SINGLE_8X"));
  } else {
    Serial.println(F("FAIL: SINGLE_1X should be > SINGLE_8X"));
    pass = false;
  }
  if (pass) {
    Serial.println(F("ALL TESTS PASSED"));
  }
}

void loop() {}
