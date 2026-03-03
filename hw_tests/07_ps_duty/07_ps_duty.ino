/*!
 * @file test_ps_duty.ino
 * @brief HW test: Verify PS duty cycle settings work
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 at fixed position
 *
 * Test: All duty cycles should return valid (non-zero) readings
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define TEST_POS 110 // Peak signal position for testing
#define FAR_POS 180  // Safe return position

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

  Serial.println(F("=== test_ps_duty ==="));
  Serial.println(F("Testing proximity at different duty cycles"));
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
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_200MA);
  delay(100);

  // Test each duty cycle
  vcnl4030_ps_duty_t duties[] = {VCNL4030_PS_DUTY_40, VCNL4030_PS_DUTY_80,
                                 VCNL4030_PS_DUTY_160, VCNL4030_PS_DUTY_320};
  const char* names[] = {"1/40", "1/80", "1/160", "1/320"};
  uint16_t readings[4];

  Serial.println(F("Duty cycle vs proximity:"));
  Serial.println(F("(Lower duty = less power, similar readings expected)"));
  for (uint8_t i = 0; i < 4; i++) {
    vcnl.setProxDuty(duties[i]);
    delay(100);
    readings[i] = medianProximity();
    Serial.print(F("  "));
    Serial.print(names[i]);
    Serial.print(F(": "));
    Serial.println(readings[i]);
  }

  // Cleanup
  servo.write(180);
  delay(1000);
  servo.detach();

  // Results
  Serial.println();
  Serial.println(F("========================="));

  // All readings should be valid (non-zero)
  bool allValid = true;
  for (uint8_t i = 0; i < 4; i++) {
    if (readings[i] == 0) {
      allValid = false;
      break;
    }
  }

  if (allValid) {
    Serial.println(F("PASS: All duty cycles return valid readings"));
  } else {
    Serial.println(F("FAIL: Some duty cycles returned zero"));
  }

  // Readings should be somewhat similar (duty mainly affects power/speed)
  uint16_t minVal = readings[0];
  uint16_t maxVal = readings[0];
  for (uint8_t i = 1; i < 4; i++) {
    if (readings[i] < minVal)
      minVal = readings[i];
    if (readings[i] > maxVal)
      maxVal = readings[i];
  }

  if (maxVal < minVal * 3) {
    Serial.println(F("PASS: Readings are within expected range"));
  } else {
    Serial.println(F("INFO: Large variation between duty cycles"));
  }
}

void loop() {
  // Nothing to do
}
