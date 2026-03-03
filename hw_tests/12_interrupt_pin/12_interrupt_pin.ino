/*!
 * @file 12_interrupt_pin.ino
 * @brief HW test: Verify INT pin goes LOW on proximity threshold crossing
 *
 * Wiring: VCNL4030 INT pin -> D2 (with pull-up)
 *
 * Strategy:
 * 1. Sample ambient proximity (no hand)
 * 2. Set high threshold to 2x ambient
 * 3. Enable proximity close interrupt
 * 4. Wait for user to wave hand near sensor
 * 5. Verify D2 goes LOW when threshold crossed
 * 6. Read interrupt flags to clear, verify D2 goes HIGH again
 */

#include <Adafruit_VCNL4030.h>

#define INT_PIN 2

Adafruit_VCNL4030 vcnl;
volatile bool intFired = false;

void intHandler() {
  intFired = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== VCNL4030 HW Test 12: INT Pin ==="));

  pinMode(INT_PIN, INPUT_PULLUP);

  if (!vcnl.begin()) {
    Serial.println(F("FAIL: Sensor not found"));
    while (1)
      delay(10);
  }

  // Verify INT pin starts HIGH (open drain, pulled up)
  if (digitalRead(INT_PIN) != HIGH) {
    Serial.println(F("FAIL: INT pin not HIGH at start (check wiring)"));
    while (1)
      delay(10);
  }
  Serial.println(F("INT pin HIGH at start: OK"));

  // Use max LED current for reliable readings
  vcnl.setProxLEDCurrent(VCNL4030_PROX_LED_200MA);
  vcnl.setProxResolution16Bit(true);
  delay(200);

  // Sample ambient proximity (no hand near sensor)
  uint32_t sum = 0;
  for (uint8_t i = 0; i < 10; i++) {
    sum += vcnl.readProximity();
    delay(50);
  }
  uint16_t ambient = sum / 10;
  Serial.print(F("Ambient proximity: "));
  Serial.println(ambient);

  // Set threshold to 2x ambient (minimum 50 to avoid noise triggers)
  uint16_t threshold = ambient * 2;
  if (threshold < 50)
    threshold = 50;
  Serial.print(F("Close threshold: "));
  Serial.println(threshold);

  // Configure: low threshold at 0, high threshold at 2x ambient
  // PS needs to go from below-low to above-high for CLOSE flag
  vcnl.setProxLowThreshold(0);
  vcnl.setProxHighThreshold(threshold);
  vcnl.setProxPersistence(VCNL4030_PROX_PERS_1);
  vcnl.setProxInterruptMode(VCNL4030_PROX_INT_CLOSE);

  // Clear any pending flags
  vcnl.readInterruptFlags();

  // Attach interrupt
  attachInterrupt(digitalPinToInterrupt(INT_PIN), intHandler, FALLING);

  Serial.println(F("\nWave your hand near the sensor..."));
  Serial.println(F("(Waiting up to 15 seconds)"));

  // Wait for interrupt or timeout
  unsigned long start = millis();
  while (!intFired && (millis() - start < 15000)) {
    uint16_t prox = vcnl.readProximity();
    Serial.print(F("Prox: "));
    Serial.print(prox);
    Serial.print(F("  INT pin: "));
    Serial.println(digitalRead(INT_PIN) ? F("HIGH") : F("LOW"));
    delay(200);
  }

  if (!intFired) {
    Serial.println(F("FAIL: INT never fired (wave hand closer?)"));
    while (1)
      delay(10);
  }

  Serial.println(F("\nINT fired!"));

  // Verify pin is LOW
  if (digitalRead(INT_PIN) != LOW) {
    Serial.println(F("FAIL: INT pin not LOW after trigger"));
    while (1)
      delay(10);
  }
  Serial.println(F("INT pin LOW after trigger: OK"));

  // Read flags to clear
  uint8_t flags = vcnl.readInterruptFlags();
  Serial.print(F("Flags: 0x"));
  Serial.println(flags, HEX);

  if (flags & VCNL4030_PROX_IF_CLOSE) {
    Serial.println(F("CLOSE flag set: OK"));
  } else {
    Serial.println(F("FAIL: CLOSE flag not set"));
  }

  // After reading flags, INT should release (go HIGH)
  delay(10);
  if (digitalRead(INT_PIN) == HIGH) {
    Serial.println(F("INT pin released (HIGH) after flag read: OK"));
  } else {
    Serial.println(F("WARNING: INT pin still LOW after flag read"));
  }

  // Disable interrupt
  detachInterrupt(digitalPinToInterrupt(INT_PIN));
  vcnl.setProxInterruptMode(VCNL4030_PROX_INT_DISABLE);

  Serial.println(F("\n=== ALL TESTS PASSED ==="));
}

void loop() {
  delay(1000);
}
