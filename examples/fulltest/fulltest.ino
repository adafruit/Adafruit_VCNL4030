/*!
 * @file fulltest.ino
 *
 * Full configuration and reading test for VCNL4030X01
 * proximity and ambient light sensor.
 *
 * Displays all configuration settings with human-readable names,
 * then continuously prints sensor readings.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 */

#include <Adafruit_VCNL4030.h>

Adafruit_VCNL4030 vcnl;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("VCNL4030 Full Test"));
  Serial.println(F("=================="));

  if (!vcnl.begin()) {
    Serial.println(F("Couldn't find VCNL4030 sensor!"));
    while (1)
      delay(10);
  }
  Serial.println(F("VCNL4030 Found!"));

  // === ALS Configuration ===
  Serial.println(F("\n--- ALS Configuration ---"));

  vcnl.enableALS(true);
  Serial.print(F("ALS Enabled: "));
  Serial.println(vcnl.ALSEnabled() ? F("Yes") : F("No"));

  vcnl.enableWhiteChannel(true);
  Serial.print(F("White Channel: "));
  Serial.println(vcnl.whiteChannelEnabled() ? F("Enabled") : F("Disabled"));

  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  Serial.print(F("Integration Time: "));
  switch (vcnl.getALSIntegrationTime()) {
    case VCNL4030_ALS_IT_50MS:
      Serial.println(F("50ms"));
      break;
    case VCNL4030_ALS_IT_100MS:
      Serial.println(F("100ms"));
      break;
    case VCNL4030_ALS_IT_200MS:
      Serial.println(F("200ms"));
      break;
    case VCNL4030_ALS_IT_400MS:
      Serial.println(F("400ms"));
      break;
    case VCNL4030_ALS_IT_800MS:
      Serial.println(F("800ms"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setALSPersistence(VCNL4030_ALS_PERS_1);
  Serial.print(F("Persistence: "));
  switch (vcnl.getALSPersistence()) {
    case VCNL4030_ALS_PERS_1:
      Serial.println(F("1 sample"));
      break;
    case VCNL4030_ALS_PERS_2:
      Serial.println(F("2 samples"));
      break;
    case VCNL4030_ALS_PERS_4:
      Serial.println(F("4 samples"));
      break;
    case VCNL4030_ALS_PERS_8:
      Serial.println(F("8 samples"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setALSHighDynamicRange(false);
  Serial.print(F("High Dynamic Range: "));
  Serial.println(vcnl.getALSHighDynamicRange() ? F("Yes (2x range)")
                                               : F("No (normal)"));

  vcnl.setALSLowSensitivity(false);
  Serial.print(F("Low Sensitivity (NS): "));
  Serial.println(vcnl.getALSLowSensitivity() ? F("Yes (2x range)")
                                             : F("No (normal)"));

  vcnl.enableALSInterrupt(false);
  Serial.print(F("Interrupt: "));
  Serial.println(vcnl.ALSInterruptEnabled() ? F("Enabled") : F("Disabled"));

  // === Proximity Configuration ===
  Serial.println(F("\n--- Proximity Configuration ---"));

  vcnl.enableProx(true);
  Serial.print(F("Prox Enabled: "));
  Serial.println(vcnl.proxEnabled() ? F("Yes") : F("No"));

  vcnl.setProxLEDCurrent(VCNL4030_PROX_LED_100MA);
  Serial.print(F("LED Current: "));
  switch (vcnl.getProxLEDCurrent()) {
    case VCNL4030_PROX_LED_50MA:
      Serial.println(F("50mA"));
      break;
    case VCNL4030_PROX_LED_75MA:
      Serial.println(F("75mA"));
      break;
    case VCNL4030_PROX_LED_100MA:
      Serial.println(F("100mA"));
      break;
    case VCNL4030_PROX_LED_120MA:
      Serial.println(F("120mA"));
      break;
    case VCNL4030_PROX_LED_140MA:
      Serial.println(F("140mA"));
      break;
    case VCNL4030_PROX_LED_160MA:
      Serial.println(F("160mA"));
      break;
    case VCNL4030_PROX_LED_180MA:
      Serial.println(F("180mA"));
      break;
    case VCNL4030_PROX_LED_200MA:
      Serial.println(F("200mA"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxDuty(VCNL4030_PROX_DUTY_160);
  Serial.print(F("Duty Cycle: "));
  switch (vcnl.getProxDuty()) {
    case VCNL4030_PROX_DUTY_40:
      Serial.println(F("1/40"));
      break;
    case VCNL4030_PROX_DUTY_80:
      Serial.println(F("1/80"));
      break;
    case VCNL4030_PROX_DUTY_160:
      Serial.println(F("1/160"));
      break;
    case VCNL4030_PROX_DUTY_320:
      Serial.println(F("1/320"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxIntegrationTime(VCNL4030_PROX_IT_4T);
  Serial.print(F("Integration Time: "));
  switch (vcnl.getProxIntegrationTime()) {
    case VCNL4030_PROX_IT_1T:
      Serial.println(F("1T"));
      break;
    case VCNL4030_PROX_IT_1_5T:
      Serial.println(F("1.5T"));
      break;
    case VCNL4030_PROX_IT_2T:
      Serial.println(F("2T"));
      break;
    case VCNL4030_PROX_IT_2_5T:
      Serial.println(F("2.5T"));
      break;
    case VCNL4030_PROX_IT_3T:
      Serial.println(F("3T"));
      break;
    case VCNL4030_PROX_IT_3_5T:
      Serial.println(F("3.5T"));
      break;
    case VCNL4030_PROX_IT_4T:
      Serial.println(F("4T"));
      break;
    case VCNL4030_PROX_IT_8T:
      Serial.println(F("8T"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxGain(VCNL4030_PROX_GAIN_TWO_STEP);
  Serial.print(F("Gain: "));
  switch (vcnl.getProxGain()) {
    case VCNL4030_PROX_GAIN_TWO_STEP:
      Serial.println(F("Two-step (most sensitive)"));
      break;
    case VCNL4030_PROX_GAIN_SINGLE_8X:
      Serial.println(F("Single 8x range (least sensitive)"));
      break;
    case VCNL4030_PROX_GAIN_SINGLE_1X:
      Serial.println(F("Single 1x range"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxPersistence(VCNL4030_PROX_PERS_1);
  Serial.print(F("Persistence: "));
  switch (vcnl.getProxPersistence()) {
    case VCNL4030_PROX_PERS_1:
      Serial.println(F("1 sample"));
      break;
    case VCNL4030_PROX_PERS_2:
      Serial.println(F("2 samples"));
      break;
    case VCNL4030_PROX_PERS_3:
      Serial.println(F("3 samples"));
      break;
    case VCNL4030_PROX_PERS_4:
      Serial.println(F("4 samples"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxInterruptMode(VCNL4030_PROX_INT_DISABLE);
  Serial.print(F("Interrupt Mode: "));
  switch (vcnl.getProxInterruptMode()) {
    case VCNL4030_PROX_INT_DISABLE:
      Serial.println(F("Disabled"));
      break;
    case VCNL4030_PROX_INT_CLOSE:
      Serial.println(F("Close only"));
      break;
    case VCNL4030_PROX_INT_AWAY:
      Serial.println(F("Away only"));
      break;
    case VCNL4030_PROX_INT_BOTH:
      Serial.println(F("Close and Away"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxResolution16Bit(true);
  Serial.print(F("Resolution: "));
  Serial.println(vcnl.getProxResolution16Bit() ? F("16-bit") : F("12-bit"));

  vcnl.setProxLowSensitivity(false);
  Serial.print(F("Low Sensitivity: "));
  Serial.println(vcnl.getProxLowSensitivity() ? F("Yes") : F("No"));

  vcnl.setProxSmartPersistence(false);
  Serial.print(F("Smart Persistence: "));
  Serial.println(vcnl.getProxSmartPersistence() ? F("Enabled") : F("Disabled"));

  vcnl.enableActiveForceMode(false);
  Serial.print(F("Active Force: "));
  Serial.println(vcnl.activeForceMode() ? F("Enabled") : F("Disabled"));

  vcnl.enableProxLogicMode(false);
  Serial.print(F("Logic Output Mode: "));
  Serial.println(vcnl.proxLogicMode() ? F("Enabled") : F("Disabled"));

  // === Sunlight Cancellation ===
  Serial.println(F("\n--- Sunlight Cancellation ---"));

  vcnl.enableSunlightCancellation(false);
  Serial.print(F("SC Enabled: "));
  Serial.println(vcnl.sunlightCancellationEnabled() ? F("Yes") : F("No"));

  vcnl.setSunlightCancelCurrent(VCNL4030_PROX_SC_CUR_1X);
  Serial.print(F("SC Current: "));
  switch (vcnl.getSunlightCancelCurrent()) {
    case VCNL4030_PROX_SC_CUR_1X:
      Serial.println(F("1x"));
      break;
    case VCNL4030_PROX_SC_CUR_2X:
      Serial.println(F("2x"));
      break;
    case VCNL4030_PROX_SC_CUR_4X:
      Serial.println(F("4x"));
      break;
    case VCNL4030_PROX_SC_CUR_8X:
      Serial.println(F("8x"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  vcnl.setProxCancellation(0);
  Serial.print(F("Cancellation Level: "));
  Serial.println(vcnl.getProxCancellation());

  // === Start continuous reading ===
  Serial.println(F("\n--- Sensor Readings ---\n"));

  delay(200); // Let sensors stabilize
}

void loop() {
  uint16_t prox = vcnl.readProximity();
  uint16_t als = vcnl.readALS();
  uint16_t white = vcnl.readWhite();

  if (prox == 0xFFFF || als == 0xFFFF || white == 0xFFFF) {
    Serial.println(F("I2C read error!"));
    delay(500);
    return;
  }

  Serial.print(F("Prox: "));
  Serial.print(prox);

  Serial.print(F("\tALS: "));
  Serial.print(als);

  Serial.print(F("\tLux: "));
  Serial.print(vcnl.readLux());

  Serial.print(F("\tWhite: "));
  Serial.print(white);

  uint8_t flags = vcnl.readInterruptFlags();
  if (flags) {
    Serial.print(F("\tFlags: 0x"));
    Serial.print(flags, HEX);
  }

  Serial.println();
  delay(500);
}
