/*!
 * @file fulltest.ino
 *
 * Comprehensive test for the Adafruit VCNL4030 library.
 * Tests all setter/getter pairs and reads sensor data.
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 */

#include <Adafruit_VCNL4030.h>

Adafruit_VCNL4030 vcnl;
uint8_t passCount = 0;
uint8_t failCount = 0;

void testBool(const __FlashStringHelper* name, bool expected, bool actual) {
  Serial.print(name);
  Serial.print(F(": "));
  if (expected == actual) {
    Serial.println(F("PASS"));
    passCount++;
  } else {
    Serial.print(F("FAIL (expected "));
    Serial.print(expected);
    Serial.print(F(", got "));
    Serial.print(actual);
    Serial.println(F(")"));
    failCount++;
  }
}

void testUint8(const __FlashStringHelper* name, uint8_t expected,
               uint8_t actual) {
  Serial.print(name);
  Serial.print(F(": "));
  if (expected == actual) {
    Serial.println(F("PASS"));
    passCount++;
  } else {
    Serial.print(F("FAIL (expected "));
    Serial.print(expected);
    Serial.print(F(", got "));
    Serial.print(actual);
    Serial.println(F(")"));
    failCount++;
  }
}

void testUint16(const __FlashStringHelper* name, uint16_t expected,
                uint16_t actual) {
  Serial.print(name);
  Serial.print(F(": "));
  if (expected == actual) {
    Serial.println(F("PASS"));
    passCount++;
  } else {
    Serial.print(F("FAIL (expected "));
    Serial.print(expected);
    Serial.print(F(", got "));
    Serial.print(actual);
    Serial.println(F(")"));
    failCount++;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("Adafruit VCNL4030 Full Test"));
  Serial.println(F("==========================="));
  Serial.println();

  if (!vcnl.begin()) {
    Serial.println(F("Could not find a valid VCNL4030 sensor, check wiring!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("VCNL4030 Found!"));
  Serial.println();

  // ========== ALS Integration Time ==========
  Serial.println(F("--- ALS Integration Time ---"));
  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_50MS);
  testUint8(F("ALS IT 50ms"), VCNL4030_ALS_IT_50MS,
            vcnl.getALSIntegrationTime());

  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  testUint8(F("ALS IT 100ms"), VCNL4030_ALS_IT_100MS,
            vcnl.getALSIntegrationTime());

  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_200MS);
  testUint8(F("ALS IT 200ms"), VCNL4030_ALS_IT_200MS,
            vcnl.getALSIntegrationTime());

  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_400MS);
  testUint8(F("ALS IT 400ms"), VCNL4030_ALS_IT_400MS,
            vcnl.getALSIntegrationTime());

  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_800MS);
  testUint8(F("ALS IT 800ms"), VCNL4030_ALS_IT_800MS,
            vcnl.getALSIntegrationTime());
  Serial.println();

  // ========== ALS High Dynamic Range ==========
  Serial.println(F("--- ALS High Dynamic Range ---"));
  vcnl.setALSHighDynamicRange(true);
  testBool(F("ALS HD on"), true, vcnl.getALSHighDynamicRange());
  vcnl.setALSHighDynamicRange(false);
  testBool(F("ALS HD off"), false, vcnl.getALSHighDynamicRange());
  Serial.println();

  // ========== ALS Low Sensitivity ==========
  Serial.println(F("--- ALS Low Sensitivity (NS) ---"));
  vcnl.setALSLowSensitivity(true);
  testBool(F("ALS NS on"), true, vcnl.getALSLowSensitivity());
  vcnl.setALSLowSensitivity(false);
  testBool(F("ALS NS off"), false, vcnl.getALSLowSensitivity());
  Serial.println();

  // ========== ALS Enable ==========
  Serial.println(F("--- ALS Enable ---"));
  vcnl.enableALS(true);
  testBool(F("ALS enabled"), true, vcnl.ALSEnabled());
  vcnl.enableALS(false);
  testBool(F("ALS disabled"), false, vcnl.ALSEnabled());
  vcnl.enableALS(true); // Re-enable
  Serial.println();

  // ========== White Channel Enable ==========
  Serial.println(F("--- White Channel Enable ---"));
  vcnl.enableWhiteChannel(true);
  testBool(F("White enabled"), true, vcnl.whiteChannelEnabled());
  vcnl.enableWhiteChannel(false);
  testBool(F("White disabled"), false, vcnl.whiteChannelEnabled());
  vcnl.enableWhiteChannel(true); // Re-enable
  Serial.println();

  // ========== ALS Persistence ==========
  Serial.println(F("--- ALS Persistence ---"));
  vcnl.setALSPersistence(VCNL4030_ALS_PERS_1);
  testUint8(F("ALS PERS 1"), VCNL4030_ALS_PERS_1, vcnl.getALSPersistence());
  vcnl.setALSPersistence(VCNL4030_ALS_PERS_2);
  testUint8(F("ALS PERS 2"), VCNL4030_ALS_PERS_2, vcnl.getALSPersistence());
  vcnl.setALSPersistence(VCNL4030_ALS_PERS_4);
  testUint8(F("ALS PERS 4"), VCNL4030_ALS_PERS_4, vcnl.getALSPersistence());
  vcnl.setALSPersistence(VCNL4030_ALS_PERS_8);
  testUint8(F("ALS PERS 8"), VCNL4030_ALS_PERS_8, vcnl.getALSPersistence());
  Serial.println();

  // ========== ALS Thresholds ==========
  Serial.println(F("--- ALS Thresholds ---"));
  vcnl.setALSHighThreshold(0x1234);
  testUint16(F("ALS High Thresh"), 0x1234, vcnl.getALSHighThreshold());
  vcnl.setALSLowThreshold(0x5678);
  testUint16(F("ALS Low Thresh"), 0x5678, vcnl.getALSLowThreshold());
  Serial.println();

  // ========== ALS Interrupt ==========
  Serial.println(F("--- ALS Interrupt ---"));
  vcnl.enableALSInterrupt(true);
  testBool(F("ALS INT enabled"), true, vcnl.ALSInterruptEnabled());
  vcnl.enableALSInterrupt(false);
  testBool(F("ALS INT disabled"), false, vcnl.ALSInterruptEnabled());
  Serial.println();

  // ========== PS Duty Cycle ==========
  Serial.println(F("--- PS Duty Cycle ---"));
  vcnl.setProxDuty(VCNL4030_PS_DUTY_40);
  testUint8(F("PS Duty 1/40"), VCNL4030_PS_DUTY_40, vcnl.getProxDuty());
  vcnl.setProxDuty(VCNL4030_PS_DUTY_80);
  testUint8(F("PS Duty 1/80"), VCNL4030_PS_DUTY_80, vcnl.getProxDuty());
  vcnl.setProxDuty(VCNL4030_PS_DUTY_160);
  testUint8(F("PS Duty 1/160"), VCNL4030_PS_DUTY_160, vcnl.getProxDuty());
  vcnl.setProxDuty(VCNL4030_PS_DUTY_320);
  testUint8(F("PS Duty 1/320"), VCNL4030_PS_DUTY_320, vcnl.getProxDuty());
  Serial.println();

  // ========== PS Integration Time ==========
  Serial.println(F("--- PS Integration Time ---"));
  vcnl.setProxIntegrationTime(VCNL4030_PS_IT_1T);
  testUint8(F("PS IT 1T"), VCNL4030_PS_IT_1T, vcnl.getProxIntegrationTime());
  vcnl.setProxIntegrationTime(VCNL4030_PS_IT_2T);
  testUint8(F("PS IT 2T"), VCNL4030_PS_IT_2T, vcnl.getProxIntegrationTime());
  vcnl.setProxIntegrationTime(VCNL4030_PS_IT_4T);
  testUint8(F("PS IT 4T"), VCNL4030_PS_IT_4T, vcnl.getProxIntegrationTime());
  vcnl.setProxIntegrationTime(VCNL4030_PS_IT_8T);
  testUint8(F("PS IT 8T"), VCNL4030_PS_IT_8T, vcnl.getProxIntegrationTime());
  Serial.println();

  // ========== PS Gain ==========
  Serial.println(F("--- PS Gain ---"));
  vcnl.setProxGain(VCNL4030_PS_GAIN_TWO_STEP);
  testUint8(F("PS Gain 2-step"), VCNL4030_PS_GAIN_TWO_STEP, vcnl.getProxGain());
  vcnl.setProxGain(VCNL4030_PS_GAIN_SINGLE_8X);
  testUint8(F("PS Gain 8x"), VCNL4030_PS_GAIN_SINGLE_8X, vcnl.getProxGain());
  vcnl.setProxGain(VCNL4030_PS_GAIN_SINGLE_1X);
  testUint8(F("PS Gain 1x"), VCNL4030_PS_GAIN_SINGLE_1X, vcnl.getProxGain());
  Serial.println();

  // ========== PS Resolution ==========
  Serial.println(F("--- PS Resolution ---"));
  vcnl.setProxResolution16Bit(true);
  testBool(F("PS 16-bit"), true, vcnl.getProxResolution16Bit());
  vcnl.setProxResolution16Bit(false);
  testBool(F("PS 12-bit"), false, vcnl.getProxResolution16Bit());
  vcnl.setProxResolution16Bit(true); // Set back to 16-bit
  Serial.println();

  // ========== PS Low Sensitivity ==========
  Serial.println(F("--- PS Low Sensitivity ---"));
  vcnl.setProxLowSensitivity(true);
  testBool(F("PS NS on"), true, vcnl.getProxLowSensitivity());
  vcnl.setProxLowSensitivity(false);
  testBool(F("PS NS off"), false, vcnl.getProxLowSensitivity());
  Serial.println();

  // ========== PS Persistence ==========
  Serial.println(F("--- PS Persistence ---"));
  vcnl.setProxPersistence(VCNL4030_PS_PERS_1);
  testUint8(F("PS PERS 1"), VCNL4030_PS_PERS_1, vcnl.getProxPersistence());
  vcnl.setProxPersistence(VCNL4030_PS_PERS_2);
  testUint8(F("PS PERS 2"), VCNL4030_PS_PERS_2, vcnl.getProxPersistence());
  vcnl.setProxPersistence(VCNL4030_PS_PERS_3);
  testUint8(F("PS PERS 3"), VCNL4030_PS_PERS_3, vcnl.getProxPersistence());
  vcnl.setProxPersistence(VCNL4030_PS_PERS_4);
  testUint8(F("PS PERS 4"), VCNL4030_PS_PERS_4, vcnl.getProxPersistence());
  Serial.println();

  // ========== PS Interrupt Mode ==========
  Serial.println(F("--- PS Interrupt Mode ---"));
  vcnl.setProxInterruptMode(VCNL4030_PS_INT_DISABLE);
  testUint8(F("PS INT disable"), VCNL4030_PS_INT_DISABLE,
            vcnl.getProxInterruptMode());
  vcnl.setProxInterruptMode(VCNL4030_PS_INT_CLOSE);
  testUint8(F("PS INT close"), VCNL4030_PS_INT_CLOSE,
            vcnl.getProxInterruptMode());
  vcnl.setProxInterruptMode(VCNL4030_PS_INT_AWAY);
  testUint8(F("PS INT away"), VCNL4030_PS_INT_AWAY,
            vcnl.getProxInterruptMode());
  vcnl.setProxInterruptMode(VCNL4030_PS_INT_BOTH);
  testUint8(F("PS INT both"), VCNL4030_PS_INT_BOTH,
            vcnl.getProxInterruptMode());
  vcnl.setProxInterruptMode(VCNL4030_PS_INT_DISABLE);
  Serial.println();

  // ========== PS Thresholds ==========
  Serial.println(F("--- PS Thresholds ---"));
  vcnl.setProxHighThreshold(0xABCD);
  testUint16(F("PS High Thresh"), 0xABCD, vcnl.getProxHighThreshold());
  vcnl.setProxLowThreshold(0x9876);
  testUint16(F("PS Low Thresh"), 0x9876, vcnl.getProxLowThreshold());
  Serial.println();

  // ========== PS Smart Persistence ==========
  Serial.println(F("--- PS Smart Persistence ---"));
  vcnl.setProxSmartPersistence(true);
  testBool(F("PS Smart Pers on"), true, vcnl.getProxSmartPersistence());
  vcnl.setProxSmartPersistence(false);
  testBool(F("PS Smart Pers off"), false, vcnl.getProxSmartPersistence());
  Serial.println();

  // ========== PS Enable ==========
  Serial.println(F("--- PS Enable ---"));
  vcnl.enableProx(true);
  testBool(F("PS enabled"), true, vcnl.proxEnabled());
  vcnl.enableProx(false);
  testBool(F("PS disabled"), false, vcnl.proxEnabled());
  vcnl.enableProx(true); // Re-enable
  Serial.println();

  // ========== LED Current ==========
  Serial.println(F("--- LED Current ---"));
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_50MA);
  testUint8(F("LED 50mA"), VCNL4030_LED_I_50MA, vcnl.getProxLEDCurrent());
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_100MA);
  testUint8(F("LED 100mA"), VCNL4030_LED_I_100MA, vcnl.getProxLEDCurrent());
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_200MA);
  testUint8(F("LED 200mA"), VCNL4030_LED_I_200MA, vcnl.getProxLEDCurrent());
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_50MA); // Set back to default
  Serial.println();

  // ========== LED Low Current ==========
  Serial.println(F("--- LED Low Current (1/10) ---"));
  vcnl.setLEDLowCurrent(true);
  testBool(F("LED Low on"), true, vcnl.getLEDLowCurrent());
  vcnl.setLEDLowCurrent(false);
  testBool(F("LED Low off"), false, vcnl.getLEDLowCurrent());
  Serial.println();

  // ========== PS Cancellation ==========
  Serial.println(F("--- PS Cancellation ---"));
  vcnl.setProxCancellation(0x0100);
  testUint16(F("PS Canc 0x0100"), 0x0100, vcnl.getProxCancellation());
  vcnl.setProxCancellation(0x0000);
  testUint16(F("PS Canc 0x0000"), 0x0000, vcnl.getProxCancellation());
  Serial.println();

  // ========== Active Force Mode ==========
  Serial.println(F("--- Active Force Mode ---"));
  vcnl.enableActiveForceMode(true);
  testBool(F("AF mode on"), true, vcnl.activeForceMode());
  vcnl.enableActiveForceMode(false);
  testBool(F("AF mode off"), false, vcnl.activeForceMode());
  Serial.println();

  // ========== PS Logic Mode ==========
  Serial.println(F("--- PS Logic Mode ---"));
  vcnl.enableProxLogicMode(true);
  testBool(F("Logic mode on"), true, vcnl.proxLogicMode());
  vcnl.enableProxLogicMode(false);
  testBool(F("Logic mode off"), false, vcnl.proxLogicMode());
  Serial.println();

  // ========== Sunlight Cancellation ==========
  Serial.println(F("--- Sunlight Cancellation ---"));
  vcnl.enableSunlightCancellation(true);
  testBool(F("SC enabled"), true, vcnl.sunlightCancellationEnabled());
  vcnl.enableSunlightCancellation(false);
  testBool(F("SC disabled"), false, vcnl.sunlightCancellationEnabled());
  Serial.println();

  // ========== Sunlight Cancel Current ==========
  Serial.println(F("--- Sunlight Cancel Current ---"));
  vcnl.setSunlightCancelCurrent(VCNL4030_PS_SC_CUR_1X);
  testUint8(F("SC Cur 1x"), VCNL4030_PS_SC_CUR_1X,
            vcnl.getSunlightCancelCurrent());
  vcnl.setSunlightCancelCurrent(VCNL4030_PS_SC_CUR_2X);
  testUint8(F("SC Cur 2x"), VCNL4030_PS_SC_CUR_2X,
            vcnl.getSunlightCancelCurrent());
  vcnl.setSunlightCancelCurrent(VCNL4030_PS_SC_CUR_4X);
  testUint8(F("SC Cur 4x"), VCNL4030_PS_SC_CUR_4X,
            vcnl.getSunlightCancelCurrent());
  vcnl.setSunlightCancelCurrent(VCNL4030_PS_SC_CUR_8X);
  testUint8(F("SC Cur 8x"), VCNL4030_PS_SC_CUR_8X,
            vcnl.getSunlightCancelCurrent());
  Serial.println();

  // ========== Sunlight Protection ==========
  Serial.println(F("--- Sunlight Protection ---"));
  vcnl.setSunlightProtection(true);
  testBool(F("SP 1.5x on"), true, vcnl.getSunlightProtection());
  vcnl.setSunlightProtection(false);
  testBool(F("SP 1x"), false, vcnl.getSunlightProtection());
  Serial.println();

  // ========== Sunlight Protect Output ==========
  Serial.println(F("--- Sunlight Protect Output ---"));
  vcnl.setSunlightProtectOutput(true);
  testBool(F("SPO high"), true, vcnl.getSunlightProtectOutput());
  vcnl.setSunlightProtectOutput(false);
  testBool(F("SPO low"), false, vcnl.getSunlightProtectOutput());
  Serial.println();

  // ========== Summary ==========
  Serial.println(F("==========================="));
  Serial.print(F("Tests passed: "));
  Serial.println(passCount);
  Serial.print(F("Tests failed: "));
  Serial.println(failCount);
  Serial.println(F("==========================="));
  Serial.println();

  // ========== Read Sensor Data ==========
  Serial.println(F("--- Sensor Data ---"));

  // Set sensible defaults for reading
  vcnl.setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  vcnl.setALSHighDynamicRange(false);
  vcnl.setALSLowSensitivity(false);
  vcnl.enableALS(true);
  vcnl.enableWhiteChannel(true);
  vcnl.enableProx(true);
  vcnl.setProxResolution16Bit(true);
  vcnl.setProxLEDCurrent(VCNL4030_LED_I_50MA);

  delay(200); // Allow sensor to settle

  Serial.print(F("Proximity: "));
  Serial.println(vcnl.readProximity());
  Serial.print(F("ALS Raw: "));
  Serial.println(vcnl.readALS());
  Serial.print(F("Lux: "));
  Serial.println(vcnl.readLux());
  Serial.print(F("White: "));
  Serial.println(vcnl.readWhite());
  Serial.println();

  // Read interrupt flags
  Serial.println(F("--- Interrupt Flags ---"));
  uint8_t flags = vcnl.readInterruptFlags();
  Serial.print(F("Raw flags: 0x"));
  Serial.println(flags, HEX);
  Serial.print(F("ALS High: "));
  Serial.println(vcnl.getALSHighFlag());
  Serial.print(F("ALS Low: "));
  Serial.println(vcnl.getALSLowFlag());
  Serial.print(F("PS Close: "));
  Serial.println(vcnl.getProxCloseFlag());
  Serial.print(F("PS Away: "));
  Serial.println(vcnl.getProxAwayFlag());
  Serial.print(F("PS Sunlight: "));
  Serial.println(vcnl.getProxSunlightFlag());
  Serial.println();

  Serial.println(F("Test complete!"));
}

void loop() {
  // Nothing to do here
  delay(1000);
}
