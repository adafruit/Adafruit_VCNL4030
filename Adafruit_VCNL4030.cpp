/*!
 * @file Adafruit_VCNL4030.cpp
 *
 * @mainpage Adafruit VCNL4030 Proximity and Ambient Light Sensor Library
 *
 * @section intro_sec Introduction
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
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * MIT license, all text above must be included in any redistribution
 */

#include "Adafruit_VCNL4030.h"

/*!
 * @brief Construct a new Adafruit_VCNL4030 object
 */
Adafruit_VCNL4030::Adafruit_VCNL4030() {}

/*!
 * @brief Destroy the Adafruit_VCNL4030 object
 */
Adafruit_VCNL4030::~Adafruit_VCNL4030() {
  if (i2c_dev) {
    delete i2c_dev;
  }
}

/*!
 * @brief Initialize the sensor
 * @param i2c_addr I2C address of the sensor (default 0x60)
 * @param wire Pointer to Wire interface (default &Wire)
 * @return true if initialization succeeded, false otherwise
 */
bool Adafruit_VCNL4030::begin(uint8_t i2c_addr, TwoWire* wire) {
  if (i2c_dev) {
    delete i2c_dev;
  }
  i2c_dev = new Adafruit_I2CDevice(i2c_addr, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // Read and verify device ID
  Adafruit_BusIO_Register id_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ID, 2, LSBFIRST);
  uint16_t id = id_reg.read();
  uint8_t id_l = id & 0xFF;

  if (id_l != VCNL4030_ID_L_EXPECTED) {
    return false;
  }

  // Set sane defaults
  // ALS: 100ms integration time, enable ALS and white channel
  setALSIntegrationTime(VCNL4030_ALS_IT_100MS);
  setALSHighDynamicRange(false);
  setALSLowSensitivity(false);
  enableALS(true);
  enableWhiteChannel(true);

  // PS: 16-bit mode, 50mA LED current, enable PS
  setProxResolution16Bit(true);
  setProxLEDCurrent(VCNL4030_LED_I_50MA);
  enableProx(true);

  return true;
}

// ========== ALS Functions ==========

/*!
 * @brief Enable or disable the ambient light sensor
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::enableALS(bool enable) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_sd =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 0);
  als_sd.write(!enable); // SD=0 means enabled
}

/*!
 * @brief Check if the ambient light sensor is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::ALSEnabled() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_sd =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 0);
  return !als_sd.read(); // SD=0 means enabled
}

/*!
 * @brief Set the ALS integration time
 * @param it Integration time setting
 */
void Adafruit_VCNL4030::setALSIntegrationTime(vcnl4030_als_it_t it) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_it =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 3, 5);
  als_it.write(it);
  _als_it = it;
}

/*!
 * @brief Get the current ALS integration time
 * @return Current integration time setting
 */
vcnl4030_als_it_t Adafruit_VCNL4030::getALSIntegrationTime() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_it =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 3, 5);
  return (vcnl4030_als_it_t)als_it.read();
}

/*!
 * @brief Enable or disable high dynamic range mode
 * @param enable true to enable (2x range), false for normal
 */
void Adafruit_VCNL4030::setALSHighDynamicRange(bool enable) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_hd =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 4);
  als_hd.write(enable);
  _als_hd = enable;
}

/*!
 * @brief Check if high dynamic range mode is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::getALSHighDynamicRange() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_hd =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 4);
  return als_hd.read();
}

/*!
 * @brief Enable or disable low sensitivity mode (ALS_NS)
 * @param enable true for 1x sensitivity, false for 2x sensitivity
 */
void Adafruit_VCNL4030::setALSLowSensitivity(bool enable) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  // ALS_NS is bit 1 of high byte = bit 9 of 16-bit word
  Adafruit_BusIO_RegisterBits als_ns =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 9);
  als_ns.write(enable);
  _als_ns = enable;
}

/*!
 * @brief Check if low sensitivity mode is enabled
 * @return true if 1x sensitivity, false if 2x sensitivity
 */
bool Adafruit_VCNL4030::getALSLowSensitivity() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_ns =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 9);
  return als_ns.read();
}

/*!
 * @brief Read the raw ALS value
 * @return Raw 16-bit ALS reading
 */
uint16_t Adafruit_VCNL4030::readALS() {
  Adafruit_BusIO_Register als_data_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_DATA, 2, LSBFIRST);
  return als_data_reg.read();
}

/*!
 * @brief Read the ALS value as lux
 * @return Calculated lux value
 */
float Adafruit_VCNL4030::readLux() {
  uint16_t raw = readALS();

  // Base resolution depends on integration time
  float resolution;
  switch (_als_it) {
    case VCNL4030_ALS_IT_50MS:
      resolution = 0.064;
      break;
    case VCNL4030_ALS_IT_100MS:
      resolution = 0.032;
      break;
    case VCNL4030_ALS_IT_200MS:
      resolution = 0.016;
      break;
    case VCNL4030_ALS_IT_400MS:
      resolution = 0.008;
      break;
    case VCNL4030_ALS_IT_800MS:
    default:
      resolution = 0.004;
      break;
  }

  // HD and NS each double the resolution (halve sensitivity)
  if (_als_hd) {
    resolution *= 2.0;
  }
  if (_als_ns) {
    resolution *= 2.0;
  }

  return raw * resolution;
}

// ========== White Channel Functions ==========

/*!
 * @brief Enable or disable the white channel
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::enableWhiteChannel(bool enable) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  // WHITE_SD is bit 0 of high byte = bit 8 of 16-bit word
  Adafruit_BusIO_RegisterBits white_sd =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 8);
  white_sd.write(!enable); // SD=0 means enabled
}

/*!
 * @brief Check if the white channel is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::whiteChannelEnabled() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits white_sd =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 8);
  return !white_sd.read(); // SD=0 means enabled
}

/*!
 * @brief Read the raw white channel value
 * @return Raw 16-bit white channel reading
 */
uint16_t Adafruit_VCNL4030::readWhite() {
  Adafruit_BusIO_Register white_data_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_WHITE_DATA, 2, LSBFIRST);
  return white_data_reg.read();
}

// ========== PS Functions ==========

/*!
 * @brief Enable or disable the proximity sensor
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::enableProx(bool enable) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_sd =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 1, 0);
  ps_sd.write(!enable); // SD=0 means enabled
}

/*!
 * @brief Check if the proximity sensor is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::proxEnabled() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_sd =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 1, 0);
  return !ps_sd.read(); // SD=0 means enabled
}

/*!
 * @brief Set the PS duty cycle
 * @param duty Duty cycle setting
 */
void Adafruit_VCNL4030::setProxDuty(vcnl4030_ps_duty_t duty) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_duty =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 6);
  ps_duty.write(duty);
}

/*!
 * @brief Get the current PS duty cycle
 * @return Current duty cycle setting
 */
vcnl4030_ps_duty_t Adafruit_VCNL4030::getProxDuty() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_duty =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 6);
  return (vcnl4030_ps_duty_t)ps_duty.read();
}

/*!
 * @brief Set the PS integration time
 * @param it Integration time setting
 */
void Adafruit_VCNL4030::setProxIntegrationTime(vcnl4030_ps_it_t it) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_it =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 3, 1);
  ps_it.write(it);
}

/*!
 * @brief Get the current PS integration time
 * @return Current integration time setting
 */
vcnl4030_ps_it_t Adafruit_VCNL4030::getProxIntegrationTime() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_it =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 3, 1);
  return (vcnl4030_ps_it_t)ps_it.read();
}

/*!
 * @brief Set the PS gain
 * @param gain Gain setting
 */
void Adafruit_VCNL4030::setProxGain(vcnl4030_ps_gain_t gain) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  // PS_GAIN is bits 5:4 of high byte = bits 13:12 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_gain =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 12);
  ps_gain.write(gain);
}

/*!
 * @brief Get the current PS gain
 * @return Current gain setting
 */
vcnl4030_ps_gain_t Adafruit_VCNL4030::getProxGain() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_gain =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 12);
  return (vcnl4030_ps_gain_t)ps_gain.read();
}

/*!
 * @brief Enable or disable 16-bit PS resolution
 * @param enable true for 16-bit, false for 12-bit
 */
void Adafruit_VCNL4030::setProxResolution16Bit(bool enable) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  // PS_HD is bit 3 of high byte = bit 11 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_hd =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 1, 11);
  ps_hd.write(enable);
}

/*!
 * @brief Check if 16-bit PS resolution is enabled
 * @return true if 16-bit, false if 12-bit
 */
bool Adafruit_VCNL4030::getProxResolution16Bit() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_hd =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 1, 11);
  return ps_hd.read();
}

/*!
 * @brief Enable or disable PS low sensitivity mode
 * @param enable true for low sensitivity, false for normal
 */
void Adafruit_VCNL4030::setProxLowSensitivity(bool enable) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  // PS_NS is bit 2 of high byte = bit 10 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_ns =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 1, 10);
  ps_ns.write(enable);
}

/*!
 * @brief Check if PS low sensitivity mode is enabled
 * @return true if low sensitivity, false if normal
 */
bool Adafruit_VCNL4030::getProxLowSensitivity() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_ns =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 1, 10);
  return ps_ns.read();
}

/*!
 * @brief Read the proximity sensor value
 * @return Raw proximity reading (12 or 16-bit depending on PS_HD)
 */
uint16_t Adafruit_VCNL4030::readProximity() {
  Adafruit_BusIO_Register ps_data_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_DATA, 2, LSBFIRST);
  return ps_data_reg.read();
}

// ========== LED Current Functions ==========

/*!
 * @brief Set the LED driving current
 * @param current LED current setting
 */
void Adafruit_VCNL4030::setProxLEDCurrent(vcnl4030_led_i_t current) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  // LED_I is bits 2:0 of high byte = bits 10:8 of 16-bit word
  Adafruit_BusIO_RegisterBits led_i =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 3, 8);
  led_i.write(current);
}

/*!
 * @brief Get the current LED driving current setting
 * @return Current LED current setting
 */
vcnl4030_led_i_t Adafruit_VCNL4030::getProxLEDCurrent() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits led_i =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 3, 8);
  return (vcnl4030_led_i_t)led_i.read();
}

/*!
 * @brief Enable or disable low LED current mode (1/10 of normal)
 * @param enable true for 1/10 current, false for normal
 */
void Adafruit_VCNL4030::setLEDLowCurrent(bool enable) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  // LED_I_LOW is bit 7 of low byte = bit 7 of 16-bit word
  Adafruit_BusIO_RegisterBits led_i_low =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 7);
  led_i_low.write(enable);
}

/*!
 * @brief Check if low LED current mode is enabled
 * @return true if 1/10 current, false if normal
 */
bool Adafruit_VCNL4030::getLEDLowCurrent() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits led_i_low =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 7);
  return led_i_low.read();
}

// ========== Crosstalk Cancellation ==========

/*!
 * @brief Set the PS cancellation level
 * @param value Cancellation value (subtracted from raw PS reading)
 */
void Adafruit_VCNL4030::setProxCancellation(uint16_t value) {
  Adafruit_BusIO_Register ps_canc_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CANC, 2, LSBFIRST);
  ps_canc_reg.write(value);
}

/*!
 * @brief Get the current PS cancellation level
 * @return Current cancellation value
 */
uint16_t Adafruit_VCNL4030::getProxCancellation() {
  Adafruit_BusIO_Register ps_canc_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CANC, 2, LSBFIRST);
  return ps_canc_reg.read();
}

// ========== ALS Interrupt Functions ==========

/*!
 * @brief Enable or disable ALS interrupt
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::enableALSInterrupt(bool enable) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_int_en =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 1);
  als_int_en.write(enable);
}

/*!
 * @brief Check if ALS interrupt is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::ALSInterruptEnabled() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_int_en =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 1, 1);
  return als_int_en.read();
}

/*!
 * @brief Set the ALS interrupt persistence
 * @param pers Persistence setting
 */
void Adafruit_VCNL4030::setALSPersistence(vcnl4030_als_pers_t pers) {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_pers =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 2, 2);
  als_pers.write(pers);
}

/*!
 * @brief Get the current ALS interrupt persistence
 * @return Current persistence setting
 */
vcnl4030_als_pers_t Adafruit_VCNL4030::getALSPersistence() {
  Adafruit_BusIO_Register als_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits als_pers =
      Adafruit_BusIO_RegisterBits(&als_conf_reg, 2, 2);
  return (vcnl4030_als_pers_t)als_pers.read();
}

/*!
 * @brief Set the ALS high threshold
 * @param threshold High threshold value
 */
void Adafruit_VCNL4030::setALSHighThreshold(uint16_t threshold) {
  Adafruit_BusIO_Register als_thdh_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_THDH, 2, LSBFIRST);
  als_thdh_reg.write(threshold);
}

/*!
 * @brief Get the current ALS high threshold
 * @return Current high threshold value
 */
uint16_t Adafruit_VCNL4030::getALSHighThreshold() {
  Adafruit_BusIO_Register als_thdh_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_THDH, 2, LSBFIRST);
  return als_thdh_reg.read();
}

/*!
 * @brief Set the ALS low threshold
 * @param threshold Low threshold value
 */
void Adafruit_VCNL4030::setALSLowThreshold(uint16_t threshold) {
  Adafruit_BusIO_Register als_thdl_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_THDL, 2, LSBFIRST);
  als_thdl_reg.write(threshold);
}

/*!
 * @brief Get the current ALS low threshold
 * @return Current low threshold value
 */
uint16_t Adafruit_VCNL4030::getALSLowThreshold() {
  Adafruit_BusIO_Register als_thdl_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_THDL, 2, LSBFIRST);
  return als_thdl_reg.read();
}

// ========== PS Interrupt Functions ==========

/*!
 * @brief Set the PS interrupt mode
 * @param mode Interrupt mode setting
 */
void Adafruit_VCNL4030::setProxInterruptMode(vcnl4030_ps_int_t mode) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  // PS_INT is bits 1:0 of high byte = bits 9:8 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_int =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 8);
  ps_int.write(mode);
}

/*!
 * @brief Get the current PS interrupt mode
 * @return Current interrupt mode setting
 */
vcnl4030_ps_int_t Adafruit_VCNL4030::getProxInterruptMode() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_int =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 8);
  return (vcnl4030_ps_int_t)ps_int.read();
}

/*!
 * @brief Set the PS interrupt persistence
 * @param pers Persistence setting
 */
void Adafruit_VCNL4030::setProxPersistence(vcnl4030_ps_pers_t pers) {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_pers =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 4);
  ps_pers.write(pers);
}

/*!
 * @brief Get the current PS interrupt persistence
 * @return Current persistence setting
 */
vcnl4030_ps_pers_t Adafruit_VCNL4030::getProxPersistence() {
  Adafruit_BusIO_Register ps_conf_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_pers =
      Adafruit_BusIO_RegisterBits(&ps_conf_reg, 2, 4);
  return (vcnl4030_ps_pers_t)ps_pers.read();
}

/*!
 * @brief Set the PS high threshold
 * @param threshold High threshold value
 */
void Adafruit_VCNL4030::setProxHighThreshold(uint16_t threshold) {
  Adafruit_BusIO_Register ps_thdh_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_THDH, 2, LSBFIRST);
  ps_thdh_reg.write(threshold);
}

/*!
 * @brief Get the current PS high threshold
 * @return Current high threshold value
 */
uint16_t Adafruit_VCNL4030::getProxHighThreshold() {
  Adafruit_BusIO_Register ps_thdh_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_THDH, 2, LSBFIRST);
  return ps_thdh_reg.read();
}

/*!
 * @brief Set the PS low threshold
 * @param threshold Low threshold value
 */
void Adafruit_VCNL4030::setProxLowThreshold(uint16_t threshold) {
  Adafruit_BusIO_Register ps_thdl_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_THDL, 2, LSBFIRST);
  ps_thdl_reg.write(threshold);
}

/*!
 * @brief Get the current PS low threshold
 * @return Current low threshold value
 */
uint16_t Adafruit_VCNL4030::getProxLowThreshold() {
  Adafruit_BusIO_Register ps_thdl_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_THDL, 2, LSBFIRST);
  return ps_thdl_reg.read();
}

/*!
 * @brief Enable or disable PS smart persistence
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::setProxSmartPersistence(bool enable) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_smart_pers =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 4);
  ps_smart_pers.write(enable);
}

/*!
 * @brief Check if PS smart persistence is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::getProxSmartPersistence() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_smart_pers =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 4);
  return ps_smart_pers.read();
}

// ========== Interrupt Flag Functions ==========

/*!
 * @brief Read the interrupt flags (reading clears them)
 * @return Interrupt flags byte from high byte of register 0x0D
 */
uint8_t Adafruit_VCNL4030::readInterruptFlags() {
  Adafruit_BusIO_Register int_flag_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_INT_FLAG, 2, LSBFIRST);
  uint16_t val = int_flag_reg.read();
  _int_flags = (val >> 8) & 0xFF; // High byte contains flags
  return _int_flags;
}

/*!
 * @brief Check if ALS high threshold flag is set (from last readInterruptFlags)
 * @return true if flag is set
 */
bool Adafruit_VCNL4030::getALSHighFlag() {
  return (_int_flags & VCNL4030_ALS_IF_H) != 0;
}

/*!
 * @brief Check if ALS low threshold flag is set (from last readInterruptFlags)
 * @return true if flag is set
 */
bool Adafruit_VCNL4030::getALSLowFlag() {
  return (_int_flags & VCNL4030_ALS_IF_L) != 0;
}

/*!
 * @brief Check if PS close flag is set (from last readInterruptFlags)
 * @return true if flag is set
 */
bool Adafruit_VCNL4030::getProxCloseFlag() {
  return (_int_flags & VCNL4030_PS_IF_CLOSE) != 0;
}

/*!
 * @brief Check if PS away flag is set (from last readInterruptFlags)
 * @return true if flag is set
 */
bool Adafruit_VCNL4030::getProxAwayFlag() {
  return (_int_flags & VCNL4030_PS_IF_AWAY) != 0;
}

/*!
 * @brief Check if PS sunlight flag is set (from last readInterruptFlags)
 * @return true if flag is set
 */
bool Adafruit_VCNL4030::getProxSunlightFlag() {
  return (_int_flags & VCNL4030_PS_SPFLAG) != 0;
}

// ========== Advanced PS Features ==========

/*!
 * @brief Enable or disable active force mode
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::enableActiveForceMode(bool enable) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_af =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 3);
  ps_af.write(enable);
}

/*!
 * @brief Check if active force mode is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::activeForceMode() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_af =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 3);
  return ps_af.read();
}

/*!
 * @brief Trigger a single PS reading (in active force mode)
 */
void Adafruit_VCNL4030::triggerProxReading() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_trig =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 2);
  ps_trig.write(1); // Auto-clears to 0
}

/*!
 * @brief Enable or disable PS logic output mode
 * @param enable true for logic output, false for normal+interrupt
 */
void Adafruit_VCNL4030::enableProxLogicMode(bool enable) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_ms =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 1);
  ps_ms.write(enable);
}

/*!
 * @brief Check if PS logic output mode is enabled
 * @return true if logic mode, false if normal+interrupt
 */
bool Adafruit_VCNL4030::proxLogicMode() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_ms =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 1);
  return ps_ms.read();
}

/*!
 * @brief Enable or disable sunlight cancellation
 * @param enable true to enable, false to disable
 */
void Adafruit_VCNL4030::enableSunlightCancellation(bool enable) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_sc_en =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 0);
  ps_sc_en.write(enable);
}

/*!
 * @brief Check if sunlight cancellation is enabled
 * @return true if enabled, false if disabled
 */
bool Adafruit_VCNL4030::sunlightCancellationEnabled() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_sc_en =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 0);
  return ps_sc_en.read();
}

/*!
 * @brief Set the sunlight cancellation current multiplier
 * @param cur Current multiplier setting
 */
void Adafruit_VCNL4030::setSunlightCancelCurrent(vcnl4030_ps_sc_cur_t cur) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  // PS_SC_CUR is bits 6:5 of high byte = bits 14:13 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_sc_cur =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 2, 13);
  ps_sc_cur.write(cur);
}

/*!
 * @brief Get the current sunlight cancellation current multiplier
 * @return Current multiplier setting
 */
vcnl4030_ps_sc_cur_t Adafruit_VCNL4030::getSunlightCancelCurrent() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_sc_cur =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 2, 13);
  return (vcnl4030_ps_sc_cur_t)ps_sc_cur.read();
}

/*!
 * @brief Enable or disable enhanced sunlight protection (1.5x capability)
 * @param enhanced true for 1.5x capability, false for 1x
 */
void Adafruit_VCNL4030::setSunlightProtection(bool enhanced) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  // PS_SP is bit 4 of high byte = bit 12 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_sp =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 12);
  ps_sp.write(enhanced);
}

/*!
 * @brief Check if enhanced sunlight protection is enabled
 * @return true if 1.5x capability, false if 1x
 */
bool Adafruit_VCNL4030::getSunlightProtection() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_sp =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 12);
  return ps_sp.read();
}

/*!
 * @brief Set sunlight protect output (0x00 or 0xFF during protection)
 * @param high true for 0xFF output, false for 0x00
 */
void Adafruit_VCNL4030::setSunlightProtectOutput(bool high) {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  // PS_SPO is bit 3 of high byte = bit 11 of 16-bit word
  Adafruit_BusIO_RegisterBits ps_spo =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 11);
  ps_spo.write(high);
}

/*!
 * @brief Get the sunlight protect output setting
 * @return true if 0xFF output, false if 0x00
 */
bool Adafruit_VCNL4030::getSunlightProtectOutput() {
  Adafruit_BusIO_Register ps_conf3_ms_reg =
      Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);
  Adafruit_BusIO_RegisterBits ps_spo =
      Adafruit_BusIO_RegisterBits(&ps_conf3_ms_reg, 1, 11);
  return ps_spo.read();
}
