/*!
 * @file 09_interrupt_flags.ino
 * @brief HW test: Verify interrupt flags fire on threshold crossings
 *
 * Key insight: flags fire on TRANSITIONS. The sensor must cross from
 * below-low to above-high (CLOSE) or above-high to below-low (AWAY).
 * Start from a known state below threshold, then cross above.
 */

#include <Adafruit_NeoPixel.h>
#include <Adafruit_VCNL4030.h>
#include <Servo.h>
#include <Wire.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16
#define SERVO_PIN 4
#define FAR_POS 180
#define CLOSE_POS 90

Adafruit_VCNL4030 vcnl;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Servo servo;

uint8_t passes = 0;
uint8_t fails = 0;

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++)
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
}

void pass(const __FlashStringHelper* msg) {
  Serial.print(F("  PASS: "));
  Serial.println(msg);
  passes++;
}

void fail(const __FlashStringHelper* msg, uint8_t flags) {
  Serial.print(F("  FAIL: "));
  Serial.print(msg);
  Serial.print(F(" (flags=0x"));
  Serial.print(flags, HEX);
  Serial.println(F(")"));
  fails++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== 09_interrupt_flags ==="));
  Serial.println();

  pixels.begin();
  setAllPixels(0, 0, 0);
  servo.attach(SERVO_PIN);
  servo.write(FAR_POS);
  delay(1000);
  servo.detach();

  if (!vcnl.begin()) {
    Serial.println(F("ERROR: VCNL4030 not found!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 initialized"));

  vcnl.enableProx(true);
  vcnl.enableALS(true);
  vcnl.setProxLEDCurrent(VCNL4030_PROX_LED_200MA);
  vcnl.setProxPersistence(VCNL4030_PROX_PERS_1);
  vcnl.setALSPersistence(VCNL4030_ALS_PERS_1);
  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  delay(500);

  // ===== PS AWAY flag =====
  // Start CLOSE (above threshold), then move FAR to trigger AWAY
  Serial.println(F("--- PS AWAY Flag ---"));
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);
  delay(1000);
  servo.detach();
  delay(500);

  uint16_t psClose = vcnl.readProximity();
  Serial.print(F("  PS close: "));
  Serial.println(psClose);

  // Set low threshold just above far baseline
  vcnl.setProxLowThreshold(8);
  vcnl.setProxHighThreshold(psClose / 2);
  vcnl.setProxInterruptMode(VCNL4030_PROX_INT_BOTH);

  Serial.print(F("  Low thresh: 8  High thresh: "));
  Serial.println(psClose / 2);

  vcnl.readInterruptFlags(); // clear
  delay(200);

  // Move far — should trigger AWAY
  servo.attach(SERVO_PIN);
  servo.write(FAR_POS);
  delay(1500);
  servo.detach();
  delay(500);

  uint16_t psFar = vcnl.readProximity();
  Serial.print(F("  PS far: "));
  Serial.println(psFar);
  delay(200);

  uint8_t flags = vcnl.readInterruptFlags();
  if (flags & 0x01) {
    pass(F("PS AWAY flag set"));
  } else {
    fail(F("PS AWAY flag not set"), flags);
  }

  // ===== PS CLOSE flag =====
  // We're now FAR (below low threshold). Move CLOSE to trigger CLOSE.
  Serial.println(F("--- PS CLOSE Flag ---"));
  vcnl.readInterruptFlags(); // clear
  delay(200);

  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);
  delay(1500);
  servo.detach();
  delay(500);

  psClose = vcnl.readProximity();
  Serial.print(F("  PS close: "));
  Serial.println(psClose);
  delay(200);

  flags = vcnl.readInterruptFlags();
  if (flags & 0x02) {
    pass(F("PS CLOSE flag set"));
  } else {
    fail(F("PS CLOSE flag not set"), flags);
  }

  // Park servo far
  servo.attach(SERVO_PIN);
  servo.write(FAR_POS);
  delay(1000);
  servo.detach();

  // ===== ALS HIGH flag =====
  // Start dark, then go bright to trigger ALS_H
  Serial.println(F("--- ALS HIGH Flag ---"));
  setAllPixels(0, 0, 0);
  delay(1000);

  uint16_t alsOff = vcnl.readALS();
  Serial.print(F("  ALS dark: "));
  Serial.println(alsOff);

  // Set high threshold above dark, low threshold below dark
  vcnl.setALSHighThreshold(alsOff + 2000);
  vcnl.setALSLowThreshold(alsOff > 1000 ? alsOff - 1000 : 0);
  vcnl.enableALSInterrupt(true);

  Serial.print(F("  Low thresh: "));
  Serial.print(alsOff > 1000 ? alsOff - 1000 : 0);
  Serial.print(F("  High thresh: "));
  Serial.println(alsOff + 2000);

  vcnl.readInterruptFlags(); // clear
  delay(200);

  // Go bright — should trigger ALS_H
  setAllPixels(255, 255, 255);
  delay(1500);

  uint16_t alsOn = vcnl.readALS();
  Serial.print(F("  ALS bright: "));
  Serial.println(alsOn);

  flags = vcnl.readInterruptFlags();
  if (flags & 0x10) {
    pass(F("ALS HIGH flag set"));
  } else {
    fail(F("ALS HIGH flag not set"), flags);
  }

  // ===== ALS LOW flag =====
  // We're now bright (above high threshold). Go dark to trigger ALS_L.
  Serial.println(F("--- ALS LOW Flag ---"));
  // Adjust thresholds: low above OFF level so going dark crosses below
  vcnl.setALSLowThreshold(alsOff + 2000);
  vcnl.setALSHighThreshold(60000);

  Serial.print(F("  Low thresh: "));
  Serial.print(alsOff + 2000);
  Serial.println(F("  High thresh: 60000"));

  vcnl.readInterruptFlags(); // clear
  delay(200);

  // Go dark — should trigger ALS_L
  setAllPixels(0, 0, 0);
  delay(1500);

  uint16_t alsDark = vcnl.readALS();
  Serial.print(F("  ALS dark: "));
  Serial.println(alsDark);

  flags = vcnl.readInterruptFlags();
  if (flags & 0x20) {
    pass(F("ALS LOW flag set"));
  } else {
    fail(F("ALS LOW flag not set"), flags);
  }

  // Cleanup
  setAllPixels(0, 0, 0);

  Serial.println();
  Serial.println(F("========================="));
  Serial.print(F("PASSED: "));
  Serial.print(passes);
  Serial.print(F("  FAILED: "));
  Serial.println(fails);
  if (fails == 0) {
    Serial.println(F("ALL TESTS PASSED"));
  }
}

void loop() {}
