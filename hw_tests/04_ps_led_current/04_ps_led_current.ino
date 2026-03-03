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

  Serial.println(F("=== test_ps_led_current ==="));
  Serial.println(F("Testing proximity at different LED currents"));
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
  Serial.print(F("Servo at "));
  Serial.print(TEST_POS);
  Serial.println(F(" degrees"));
  Serial.println();

  vcnl.enableProx(true);
  vcnl.setLEDLowCurrent(false); // Normal mode first
  delay(100);

  // Test different currents
  vcnl4030_prox_led_t currents[] = {VCNL4030_PROX_LED_200MA,
                                    VCNL4030_PROX_LED_200MA,
                                    VCNL4030_PROX_LED_200MA};
  const char* names[] = {"50mA", "100mA", "200mA"};
  uint16_t readings[3];

  Serial.println(F("LED current vs proximity:"));
  for (uint8_t i = 0; i < 3; i++) {
    vcnl.setProxLEDCurrent(currents[i]);
    delay(100);
    readings[i] = medianRead(vcnl, READ_PROX);
    Serial.print(F("  "));
    Serial.print(names[i]);
    Serial.print(F(": "));
    Serial.println(readings[i]);
  }
  Serial.println();

  // Test LED_I_LOW mode (1/10 current)
  Serial.println(F("--- LED_I_LOW mode test ---"));
  vcnl.setProxLEDCurrent(VCNL4030_PROX_LED_200MA); // 200mA setting
  vcnl.setLEDLowCurrent(false);
  delay(100);
  uint16_t psNormal = (uint16_t)medianRead(vcnl, READ_PROX);
  Serial.print(F("  200mA normal: "));
  Serial.println(psNormal);

  vcnl.setLEDLowCurrent(true); // Should be ~20mA actual
  delay(100);
  uint16_t psLow = (uint16_t)medianRead(vcnl, READ_PROX);
  Serial.print(F("  200mA + LOW mode (~20mA): "));
  Serial.println(psLow);

  // Reset
  vcnl.setLEDLowCurrent(false);
  servo.write(180);
  delay(1000);
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
