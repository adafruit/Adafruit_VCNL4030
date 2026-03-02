/*!
 * @file test_proximity_servo.ino
 * @brief HW test: Verify proximity responds to servo distance changes
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 with a reflective surface attached
 *
 * Test: Proximity reading should increase when servo moves surface closer
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define FAR_POS 150
#define CLOSE_POS 110

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

  Serial.println(F("=== test_proximity_servo ==="));
  Serial.println(F("Testing proximity response to servo position"));
  Serial.println();

  // Initialize servo
  servo.attach(SERVO_PIN);
  servo.write(FAR_POS);
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

  // Test: FAR position
  Serial.println(F("--- Moving to FAR position ---"));
  servo.write(FAR_POS);
  delay(700);
  uint16_t psFar = medianProximity();
  Serial.print(F("  FAR ("));
  Serial.print(FAR_POS);
  Serial.print(F(" deg): "));
  Serial.println(psFar);

  // Test: CLOSE position
  Serial.println(F("--- Moving to CLOSE position ---"));
  servo.write(CLOSE_POS);
  delay(700);
  uint16_t psClose = medianProximity();
  Serial.print(F("  CLOSE ("));
  Serial.print(CLOSE_POS);
  Serial.print(F(" deg): "));
  Serial.println(psClose);

  // Test distance response curve
  Serial.println();
  Serial.println(F("--- Distance response curve ---"));
  uint8_t positions[] = {150, 140, 130, 120, 110, 100};
  for (uint8_t i = 0; i < 6; i++) {
    servo.write(positions[i]);
    delay(400);
    uint16_t ps = medianProximity();
    Serial.print(F("  "));
    Serial.print(positions[i]);
    Serial.print(F(" deg: "));
    Serial.println(ps);
  }

  // Return to safe position
  servo.write(FAR_POS);
  delay(300);
  servo.detach();

  // Results
  Serial.println();
  Serial.println(F("========================="));
  if (psClose > psFar + 50) {
    Serial.println(F("PASS: Close reading significantly higher than far"));
  } else {
    Serial.println(F("FAIL: Close reading not significantly higher"));
    Serial.print(F("  Difference: "));
    Serial.println(psClose - psFar);
  }
}

void loop() {
  // Nothing to do
}
