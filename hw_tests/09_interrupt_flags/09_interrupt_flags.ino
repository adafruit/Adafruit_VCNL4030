/*!
 * @file test_interrupt_flags.ino
 * @brief HW test: Verify interrupt flags fire correctly
 *
 * Hardware setup:
 * - VCNL4030 sensor connected via I2C
 * - Servo on pin D4 to trigger PS thresholds
 * - NeoPixel ring on pin D6 to trigger ALS thresholds
 *
 * Test: Interrupt flags should set when thresholds are crossed
 */

#include <Adafruit_NeoPixel.h>
#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16
#define SERVO_PIN 4
#define FAR_POS 150   // Reliable baseline position
#define CLOSE_POS 110 // Reliable high-reading position

Adafruit_VCNL4030 vcnl;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Servo servo;

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== test_interrupt_flags ==="));
  Serial.println(F("Testing PS and ALS interrupt flags"));
  Serial.println();

  // Initialize hardware
  pixels.begin();
  setAllPixels(0, 0, 0);
  servo.attach(SERVO_PIN);
  servo.write(FAR_POS); // Start far
  delay(500);

  // Initialize sensor
  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enablePS(true);
  vcnl.enableALS(true);
  vcnl.setLEDCurrent(VCNL4030_LED_I_100MA);
  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  delay(200);

  bool allPassed = true;

  // ===== PS Interrupt Test =====
  Serial.println(F("--- PS Interrupt Test ---"));

  // Get baseline readings
  servo.write(FAR_POS);
  delay(500);
  uint16_t psFar = vcnl.readProximity();
  servo.write(CLOSE_POS);
  delay(500);
  uint16_t psClose = vcnl.readProximity();
  Serial.print(F("  PS Far: "));
  Serial.print(psFar);
  Serial.print(F("  Close: "));
  Serial.println(psClose);

  // Set thresholds between far and close
  uint16_t psLowThresh = psFar + (psClose - psFar) / 4;
  uint16_t psHighThresh = psFar + (psClose - psFar) * 3 / 4;
  vcnl.setPSLowThreshold(psLowThresh);
  vcnl.setPSHighThreshold(psHighThresh);
  vcnl.setPSInterruptMode(VCNL4030_PS_INT_BOTH);
  vcnl.setPSPersistence(VCNL4030_PS_PERS_1);
  Serial.print(F("  Thresholds: Low="));
  Serial.print(psLowThresh);
  Serial.print(F(" High="));
  Serial.println(psHighThresh);

  // Clear any pending flags
  vcnl.readInterruptFlags();

  // Move close to trigger CLOSE flag
  servo.write(CLOSE_POS);
  delay(300);
  vcnl.readProximity(); // Trigger reading
  delay(100);
  uint8_t flags = vcnl.readInterruptFlags();
  bool psCloseFired = vcnl.getPSCloseFlag();
  Serial.print(F("  CLOSE flag after moving close: "));
  Serial.println(psCloseFired ? F("SET") : F("NOT SET"));
  if (!psCloseFired)
    allPassed = false;

  // Move far to trigger AWAY flag
  servo.write(FAR_POS);
  delay(300);
  vcnl.readProximity();
  delay(100);
  flags = vcnl.readInterruptFlags();
  bool psAwayFired = vcnl.getPSAwayFlag();
  Serial.print(F("  AWAY flag after moving away: "));
  Serial.println(psAwayFired ? F("SET") : F("NOT SET"));
  if (!psAwayFired)
    allPassed = false;

  Serial.println();

  // ===== ALS Interrupt Test =====
  Serial.println(F("--- ALS Interrupt Test ---"));

  // Get baseline readings
  setAllPixels(0, 0, 0);
  delay(300);
  uint16_t alsOff = vcnl.readALS();
  setAllPixels(255, 255, 255);
  delay(300);
  uint16_t alsOn = vcnl.readALS();
  Serial.print(F("  ALS Off: "));
  Serial.print(alsOff);
  Serial.print(F("  On: "));
  Serial.println(alsOn);

  // Set thresholds
  uint16_t alsLowThresh = alsOff + (alsOn - alsOff) / 4;
  uint16_t alsHighThresh = alsOff + (alsOn - alsOff) * 3 / 4;
  vcnl.setALSLowThreshold(alsLowThresh);
  vcnl.setALSHighThreshold(alsHighThresh);
  vcnl.enableALSInterrupt(true);
  vcnl.setALSPersistence(VCNL4030_ALS_PERS_1);
  Serial.print(F("  Thresholds: Low="));
  Serial.print(alsLowThresh);
  Serial.print(F(" High="));
  Serial.println(alsHighThresh);

  // Clear flags
  vcnl.readInterruptFlags();

  // Turn on NeoPixels to trigger HIGH flag
  setAllPixels(255, 255, 255);
  delay(300);
  vcnl.readALS();
  delay(100);
  flags = vcnl.readInterruptFlags();
  bool alsHighFired = vcnl.getALSHighFlag();
  Serial.print(F("  HIGH flag after NeoPixels ON: "));
  Serial.println(alsHighFired ? F("SET") : F("NOT SET"));
  if (!alsHighFired)
    allPassed = false;

  // Turn off NeoPixels to trigger LOW flag
  setAllPixels(0, 0, 0);
  delay(300);
  vcnl.readALS();
  delay(100);
  flags = vcnl.readInterruptFlags();
  bool alsLowFired = vcnl.getALSLowFlag();
  Serial.print(F("  LOW flag after NeoPixels OFF: "));
  Serial.println(alsLowFired ? F("SET") : F("NOT SET"));
  if (!alsLowFired)
    allPassed = false;

  // Cleanup
  setAllPixels(0, 0, 0);
  servo.write(FAR_POS);
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
