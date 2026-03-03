/*!
 * @file 01_proximity_servo.ino
 * @brief HW test: Verify proximity responds to servo-positioned reflector
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 with a reflective surface attached
 * - 180 deg = reflector away, 90 deg = reflector over sensor
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define FAR_POS 180
#define CLOSE_POS 90

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

  Serial.println(F("=== 01_proximity_servo ==="));
  Serial.println(F("Verify proximity with servo-positioned reflector"));
  Serial.println();

  servo.attach(SERVO_PIN);

  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enableProx(true);
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_200MA);
  delay(200);

  // FAR: reflector away from sensor
  Serial.println(F("--- FAR (180 deg) ---"));
  servo.write(FAR_POS);
  delay(1000);
  uint16_t psFar = medianProximity();
  Serial.print(F("  Proximity: "));
  Serial.println(psFar);

  // CLOSE: reflector over sensor
  Serial.println(F("--- CLOSE (90 deg) ---"));
  servo.write(CLOSE_POS);
  delay(1000);
  uint16_t psClose = medianProximity();
  Serial.print(F("  Proximity: "));
  Serial.println(psClose);

  // Park far and detach
  servo.write(FAR_POS);
  delay(1000);
  servo.detach();

  Serial.println();
  Serial.println(F("========================="));
  Serial.print(F("  Far: "));
  Serial.print(psFar);
  Serial.print(F("  Close: "));
  Serial.print(psClose);
  Serial.print(F("  Diff: "));
  Serial.println((int16_t)(psClose - psFar));
  if (psClose > psFar + 50) {
    Serial.println(F("PASS: Close reading significantly higher than far"));
  } else {
    Serial.println(F("FAIL: Close reading not significantly higher"));
  }
}

void loop() {}
