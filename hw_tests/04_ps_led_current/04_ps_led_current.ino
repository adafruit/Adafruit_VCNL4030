/*!
 * @file test_ps_led_current.ino
 * @brief HW test: Verify LED current affects proximity readings
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 at fixed middle position
 *
 * Test: Higher LED current should give higher proximity counts
 */

#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define TEST_POS 110 // Peak signal position for testing
#define FAR_POS 150  // Safe return position

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

  Serial.println(F("=== test_ps_led_current ==="));
  Serial.println(F("Testing proximity at different LED currents"));
  Serial.println();

  // Initialize servo at middle position
  servo.attach(SERVO_PIN);
  servo.write(TEST_POS);
  delay(500);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));
  Serial.print(F("Servo at "));
  Serial.print(TEST_POS);
  Serial.println(F(" degrees"));
  Serial.println();

  vcnl.enablePS(true);
  vcnl.setLEDLowCurrent(false); // Normal mode first
  delay(100);

  // Test different currents
  vcnl4030_led_i_t currents[] = {VCNL4030_LED_I_50MA, VCNL4030_LED_I_100MA,
                                 VCNL4030_LED_I_200MA};
  const char* names[] = {"50mA", "100mA", "200mA"};
  uint16_t readings[3];

  Serial.println(F("LED current vs proximity:"));
  for (uint8_t i = 0; i < 3; i++) {
    vcnl.setLEDCurrent(currents[i]);
    delay(100);
    readings[i] = medianProximity();
    Serial.print(F("  "));
    Serial.print(names[i]);
    Serial.print(F(": "));
    Serial.println(readings[i]);
  }
  Serial.println();

  // Test LED_I_LOW mode (1/10 current)
  Serial.println(F("--- LED_I_LOW mode test ---"));
  vcnl.setLEDCurrent(VCNL4030_LED_I_200MA); // 200mA setting
  vcnl.setLEDLowCurrent(false);
  delay(100);
  uint16_t psNormal = medianProximity();
  Serial.print(F("  200mA normal: "));
  Serial.println(psNormal);

  vcnl.setLEDLowCurrent(true); // Should be ~20mA actual
  delay(100);
  uint16_t psLow = medianProximity();
  Serial.print(F("  200mA + LOW mode (~20mA): "));
  Serial.println(psLow);

  // Reset
  vcnl.setLEDLowCurrent(false);
  servo.write(150);
  delay(300);
  servo.detach();

  // Results
  Serial.println();
  Serial.println(F("========================="));

  // Check increasing current gives increasing counts
  bool increasing = (readings[1] > readings[0]) && (readings[2] > readings[1]);
  if (increasing) {
    Serial.println(F("PASS: Higher LED current gives higher counts"));
  } else {
    Serial.println(F("FAIL: Counts did not increase with LED current"));
  }

  // Check LOW mode significantly reduces reading
  // Expect roughly 1/10, but allow for some variation
  if (psLow < psNormal / 3) {
    Serial.println(F("PASS: LED_I_LOW mode reduces reading significantly"));
  } else {
    Serial.println(F("FAIL: LED_I_LOW mode did not reduce reading enough"));
  }
}

void loop() {
  // Nothing to do
}
