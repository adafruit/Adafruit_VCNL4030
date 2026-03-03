/*!
 * @file Adafruit_VCNL4030.h
 *
 * This is a library for the VCNL4030X01 proximity and ambient light sensor.
 *
 * Designed specifically to work with the Adafruit VCNL4030 breakout:
 * https://www.adafruit.com/product/XXXX
 *
 * These sensors use I2C to communicate, 2 pins are required to interface.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * MIT license, all text above must be included in any redistribution
 */

#ifndef ADAFRUIT_VCNL4030_H
#define ADAFRUIT_VCNL4030_H

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Arduino.h>

// Register addresses
#define VCNL4030_REG_ALS_CONF 0x00    ///< ALS_CONF1 (L) + ALS_CONF2 (H)
#define VCNL4030_REG_ALS_THDH 0x01    ///< ALS high threshold (16-bit)
#define VCNL4030_REG_ALS_THDL 0x02    ///< ALS low threshold (16-bit)
#define VCNL4030_REG_PS_CONF1_2 0x03  ///< PS_CONF1 (L) + PS_CONF2 (H)
#define VCNL4030_REG_PS_CONF3_MS 0x04 ///< PS_CONF3 (L) + PS_MS (H)
#define VCNL4030_REG_PS_CANC 0x05     ///< PS cancellation (16-bit)
#define VCNL4030_REG_PS_THDL 0x06     ///< PS low threshold (16-bit)
#define VCNL4030_REG_PS_THDH 0x07     ///< PS high threshold (16-bit)
#define VCNL4030_REG_PS_DATA 0x08     ///< PS data output (16-bit)
#define VCNL4030_REG_ALS_DATA 0x0B    ///< ALS data output (16-bit)
#define VCNL4030_REG_WHITE_DATA 0x0C  ///< White data output (16-bit)
#define VCNL4030_REG_INT_FLAG 0x0D    ///< Reserved (L) + INT_Flag (H)
#define VCNL4030_REG_ID 0x0E          ///< ID_L (L) + ID_M (H)

#define VCNL4030_DEFAULT_ADDR 0x60  ///< Default I2C address
#define VCNL4030_ID_L_EXPECTED 0x80 ///< Expected ID_L value

// Interrupt flag bit masks (from high byte of 0x0D)
#define VCNL4030_ALS_IF_L 0x20      ///< ALS crossed low threshold
#define VCNL4030_ALS_IF_H 0x10      ///< ALS crossed high threshold
#define VCNL4030_PROX_SPFLAG 0x04   ///< PS sunlight protection mode active
#define VCNL4030_PROX_IF_CLOSE 0x02 ///< PS object approaching
#define VCNL4030_PROX_IF_AWAY 0x01  ///< PS object moving away

/** ALS integration time settings */
typedef enum {
  VCNL4030_ALS_IT_50MS = 0b000,  ///< 50ms, 0.064 lux/step
  VCNL4030_ALS_IT_100MS = 0b001, ///< 100ms, 0.032 lux/step
  VCNL4030_ALS_IT_200MS = 0b010, ///< 200ms, 0.016 lux/step
  VCNL4030_ALS_IT_400MS = 0b011, ///< 400ms, 0.008 lux/step
  VCNL4030_ALS_IT_800MS = 0b100, ///< 800ms, 0.004 lux/step
} vcnl4030_als_it_t;

/** ALS persistence settings */
typedef enum {
  VCNL4030_ALS_PERS_1 = 0b00, ///< 1 reading
  VCNL4030_ALS_PERS_2 = 0b01, ///< 2 readings
  VCNL4030_ALS_PERS_4 = 0b10, ///< 4 readings
  VCNL4030_ALS_PERS_8 = 0b11, ///< 8 readings
} vcnl4030_als_pers_t;

/** PS IRED duty ratio settings */
typedef enum {
  VCNL4030_PROX_DUTY_40 = 0b00,  ///< 1/40 duty cycle
  VCNL4030_PROX_DUTY_80 = 0b01,  ///< 1/80 duty cycle
  VCNL4030_PROX_DUTY_160 = 0b10, ///< 1/160 duty cycle
  VCNL4030_PROX_DUTY_320 = 0b11, ///< 1/320 duty cycle (lowest power)
} vcnl4030_prox_duty_t;

/** PS persistence settings */
typedef enum {
  VCNL4030_PROX_PERS_1 = 0b00, ///< 1 reading
  VCNL4030_PROX_PERS_2 = 0b01, ///< 2 readings
  VCNL4030_PROX_PERS_3 = 0b10, ///< 3 readings
  VCNL4030_PROX_PERS_4 = 0b11, ///< 4 readings
} vcnl4030_prox_pers_t;

/** PS integration time settings */
typedef enum {
  VCNL4030_PROX_IT_1T = 0b000,   ///< 1T
  VCNL4030_PROX_IT_1_5T = 0b001, ///< 1.5T
  VCNL4030_PROX_IT_2T = 0b010,   ///< 2T
  VCNL4030_PROX_IT_2_5T = 0b011, ///< 2.5T
  VCNL4030_PROX_IT_3T = 0b100,   ///< 3T
  VCNL4030_PROX_IT_3_5T = 0b101, ///< 3.5T
  VCNL4030_PROX_IT_4T = 0b110,   ///< 4T
  VCNL4030_PROX_IT_8T = 0b111,   ///< 8T (highest sensitivity)
} vcnl4030_prox_it_t;

/** PS gain settings */
typedef enum {
  VCNL4030_PROX_GAIN_TWO_STEP =
      0b00, ///< Two-step mode (most sensitive, ~4x single)
  VCNL4030_PROX_GAIN_SINGLE_8X =
      0b10, ///< Single mode x8 range (least sensitive)
  VCNL4030_PROX_GAIN_SINGLE_1X = 0b11, ///< Single mode x1 range
} vcnl4030_prox_gain_t;

/** PS interrupt mode settings */
typedef enum {
  VCNL4030_PROX_INT_DISABLE = 0b00, ///< Interrupt disabled
  VCNL4030_PROX_INT_CLOSE = 0b01,   ///< Interrupt on close
  VCNL4030_PROX_INT_AWAY = 0b10,    ///< Interrupt on away
  VCNL4030_PROX_INT_BOTH = 0b11,    ///< Interrupt on both
} vcnl4030_prox_int_t;

/** LED driving current settings */
typedef enum {
  VCNL4030_PROX_LED_50MA = 0b000,  ///< 50mA
  VCNL4030_PROX_LED_75MA = 0b001,  ///< 75mA
  VCNL4030_PROX_LED_100MA = 0b010, ///< 100mA
  VCNL4030_PROX_LED_120MA = 0b011, ///< 120mA
  VCNL4030_PROX_LED_140MA = 0b100, ///< 140mA
  VCNL4030_PROX_LED_160MA = 0b101, ///< 160mA
  VCNL4030_PROX_LED_180MA = 0b110, ///< 180mA
  VCNL4030_PROX_LED_200MA = 0b111, ///< 200mA
} vcnl4030_prox_led_t;

/** Sunlight cancellation current multiplier */
typedef enum {
  VCNL4030_PROX_SC_CUR_1X = 0b00, ///< 1x typical
  VCNL4030_PROX_SC_CUR_2X = 0b01, ///< 2x typical
  VCNL4030_PROX_SC_CUR_4X = 0b10, ///< 4x typical
  VCNL4030_PROX_SC_CUR_8X = 0b11, ///< 8x typical
} vcnl4030_prox_sc_cur_t;

/*!
 * @brief Class for interfacing with the VCNL4030 proximity and ALS sensor
 */
class Adafruit_VCNL4030 {
 public:
  Adafruit_VCNL4030();
  ~Adafruit_VCNL4030();

  bool begin(uint8_t i2c_addr = VCNL4030_DEFAULT_ADDR, TwoWire* wire = &Wire);

  // ALS functions
  bool enableALS(bool enable);
  bool ALSEnabled();
  bool setALSIntegrationTime(vcnl4030_als_it_t it);
  vcnl4030_als_it_t getALSIntegrationTime();
  bool setALSHighDynamicRange(bool enable);
  bool getALSHighDynamicRange();
  bool setALSLowSensitivity(bool enable);
  bool getALSLowSensitivity();
  uint16_t readALS();
  float readLux();

  // White channel
  bool enableWhiteChannel(bool enable);
  bool whiteChannelEnabled();
  uint16_t readWhite();

  // PS functions
  bool enableProx(bool enable);
  bool proxEnabled();
  bool setProxDuty(vcnl4030_prox_duty_t duty);
  vcnl4030_prox_duty_t getProxDuty();
  bool setProxIntegrationTime(vcnl4030_prox_it_t it);
  vcnl4030_prox_it_t getProxIntegrationTime();
  bool setProxGain(vcnl4030_prox_gain_t gain);
  vcnl4030_prox_gain_t getProxGain();
  bool setProxResolution16Bit(bool enable);
  bool getProxResolution16Bit();
  bool setProxLowSensitivity(bool enable);
  bool getProxLowSensitivity();
  uint16_t readProximity();

  // LED current
  bool setProxLEDCurrent(vcnl4030_prox_led_t current);
  vcnl4030_prox_led_t getProxLEDCurrent();
  bool setLEDLowCurrent(bool enable);
  bool getLEDLowCurrent();

  // Crosstalk cancellation
  bool setProxCancellation(uint16_t value);
  uint16_t getProxCancellation();

  // ALS interrupts
  bool enableALSInterrupt(bool enable);
  bool ALSInterruptEnabled();
  bool setALSPersistence(vcnl4030_als_pers_t pers);
  vcnl4030_als_pers_t getALSPersistence();
  bool setALSHighThreshold(uint16_t threshold);
  uint16_t getALSHighThreshold();
  bool setALSLowThreshold(uint16_t threshold);
  uint16_t getALSLowThreshold();

  // PS interrupts
  bool setProxInterruptMode(vcnl4030_prox_int_t mode);
  vcnl4030_prox_int_t getProxInterruptMode();
  bool setProxPersistence(vcnl4030_prox_pers_t pers);
  vcnl4030_prox_pers_t getProxPersistence();
  bool setProxHighThreshold(uint16_t threshold);
  uint16_t getProxHighThreshold();
  bool setProxLowThreshold(uint16_t threshold);
  uint16_t getProxLowThreshold();
  bool setProxSmartPersistence(bool enable);
  bool getProxSmartPersistence();

  // Interrupt flags
  uint8_t readInterruptFlags();
  bool getALSHighFlag();
  bool getALSLowFlag();
  bool getProxCloseFlag();
  bool getProxAwayFlag();
  bool getProxSunlightFlag();

  // Advanced PS features
  bool enableActiveForceMode(bool enable);
  bool activeForceMode();
  bool triggerProxReading();
  bool enableProxLogicMode(bool enable);
  bool proxLogicMode();
  bool enableSunlightCancellation(bool enable);
  bool sunlightCancellationEnabled();
  bool setSunlightCancelCurrent(vcnl4030_prox_sc_cur_t cur);
  vcnl4030_prox_sc_cur_t getSunlightCancelCurrent();
  bool setSunlightProtection(bool enhanced);
  bool getSunlightProtection();
  bool setSunlightProtectOutput(bool high);
  bool getSunlightProtectOutput();

 private:
  Adafruit_I2CDevice* i2c_dev = nullptr; ///< Pointer to I2C device
  vcnl4030_als_it_t _als_it;             ///< Cached ALS integration time
  bool _als_hd;                          ///< Cached ALS high dynamic range
  bool _als_ns;                          ///< Cached ALS low sensitivity
  uint8_t _int_flags;                    ///< Cached interrupt flags
};

#endif // ADAFRUIT_VCNL4030_H
