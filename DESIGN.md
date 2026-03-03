# VCNL4030X01 Library Design Document

## Chip Overview

| Property | Value |
|----------|-------|
| Part Number | VCNL4030X01 |
| Manufacturer | Vishay Semiconductors |
| Description | Fully Integrated Proximity and Ambient Light Sensor with IR Emitter |
| I2C Address | 0x60 (default), 0x51, 0x40, 0x41 (variant-dependent) |
| ID Register | 0x0E |
| Expected ID_L | 0x80 |
| Expected ID_M | 0x00 (0x60), 0x10 (0x51), 0x20 (0x40), 0x30 (0x41) |
| Operating Voltage | 2.5V - 3.6V |
| I2C Bus Voltage | 1.8V - 5.5V |
| I2C Speed | 10 - 400 kHz |
| Package | 8-pin QFN (4.0 x 2.36 x 0.75 mm) |

### Features
- 16-bit ambient light sensor (ALS) with 0.004 lux resolution
- 12-bit or 16-bit proximity sensor (PS) with programmable LED current (50-200mA)
- Integrated 940nm IR LED with programmable duty cycle
- White channel for light source detection
- Programmable interrupt thresholds for ALS and PS
- Intelligent crosstalk cancellation
- Sunlight cancellation
- Active force mode for ultra-low power operation
- FiltronTM technology for human-eye spectral response

---

## I2C Communication Protocol

### Command Code Scheme
This chip uses **8-bit command codes** that address 16-bit registers. Each register is accessed as a word (2 bytes):
- **Low byte** = DATA_BYTE_LOW (sent/received first)
- **High byte** = DATA_BYTE_HIGH (sent/received second)

### Write Protocol (Word Write)
```
START | SlaveAddr+W | ACK | CommandCode | ACK | DataByteLow | ACK | DataByteHigh | ACK | STOP
```

### Read Protocol (Word Read)
```
START | SlaveAddr+W | ACK | CommandCode | ACK | 
REPEATED_START | SlaveAddr+R | ACK | DataByteLow | ACK | DataByteHigh | NACK | STOP
```

### Multi-byte Read Order
- **LSB first, then MSB** (little-endian)
- All data registers are 16-bit, accessed as word reads/writes

---

## Register Map

| Cmd Code | Byte | Register Name | R/W | Default | Description |
|----------|------|---------------|-----|---------|-------------|
| 0x00 | L | ALS_CONF1 | R/W | 0x01 | ALS config: integration time, persistence, interrupt, enable |
| 0x00 | H | ALS_CONF2 | R/W | 0x01 | ALS sensitivity, white channel enable |
| 0x01 | L | ALS_THDH_L | R/W | 0x00 | ALS high threshold LSB |
| 0x01 | H | ALS_THDH_M | R/W | 0x00 | ALS high threshold MSB |
| 0x02 | L | ALS_THDL_L | R/W | 0x00 | ALS low threshold LSB |
| 0x02 | H | ALS_THDL_M | R/W | 0x00 | ALS low threshold MSB |
| 0x03 | L | PS_CONF1 | R/W | 0x01 | PS config: duty, persistence, integration time, enable |
| 0x03 | H | PS_CONF2 | R/W | 0x00 | PS gain, resolution, sensitivity, interrupt mode |
| 0x04 | L | PS_CONF3 | R/W | 0x00 | PS advanced: smart pers, active force, sunlight cancel |
| 0x04 | H | PS_MS | R/W | 0x00 | PS mode: LED current, sunlight protection settings |
| 0x05 | L | PS_CANC_L | R/W | 0x00 | PS cancellation level LSB |
| 0x05 | H | PS_CANC_M | R/W | 0x00 | PS cancellation level MSB |
| 0x06 | L | PS_THDL_L | R/W | 0x00 | PS low threshold LSB |
| 0x06 | H | PS_THDL_M | R/W | 0x00 | PS low threshold MSB |
| 0x07 | L | PS_THDH_L | R/W | 0x00 | PS high threshold LSB |
| 0x07 | H | PS_THDH_M | R/W | 0x00 | PS high threshold MSB |
| 0x08 | L | PS_Data_L | R | 0x00 | PS output data LSB |
| 0x08 | H | PS_Data_M | R | 0x00 | PS output data MSB |
| 0x09 | L/H | Reserved | R | 0x00 | Reserved |
| 0x0A | L/H | Reserved | R | 0x00 | Reserved |
| 0x0B | L | ALS_Data_L | R | 0x00 | ALS output data LSB |
| 0x0B | H | ALS_Data_M | R | 0x00 | ALS output data MSB |
| 0x0C | L | White_Data_L | R | 0x00 | White channel output LSB |
| 0x0C | H | White_Data_M | R | 0x00 | White channel output MSB |
| 0x0D | L | Reserved | R | 0x00 | Reserved |
| 0x0D | H | INT_Flag | R | 0x00 | Interrupt status flags |
| 0x0E | L | ID_L | R | 0x80 | Device ID LSB |
| 0x0E | H | ID_M | R | varies | Device ID MSB (encodes I2C address variant) |

---

## Bit Field Details

### Command 0x00 Low: ALS_CONF1

| Bits | Name | Description |
|------|------|-------------|
| 7:5 | ALS_IT | ALS integration time |
| 4 | ALS_HD | High dynamic range (0=1x, 1=2x) |
| 3:2 | ALS_PERS | ALS interrupt persistence |
| 1 | ALS_INT_EN | ALS interrupt enable (0=disable, 1=enable) |
| 0 | ALS_SD | ALS shutdown (0=power on, 1=shutdown) **Default=1** |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits ALS_IT      = Adafruit_BusIO_RegisterBits(&ALS_CONF1_reg, 3, 5);  // bits 7:5
Adafruit_BusIO_RegisterBits ALS_HD      = Adafruit_BusIO_RegisterBits(&ALS_CONF1_reg, 1, 4);  // bit 4
Adafruit_BusIO_RegisterBits ALS_PERS    = Adafruit_BusIO_RegisterBits(&ALS_CONF1_reg, 2, 2);  // bits 3:2
Adafruit_BusIO_RegisterBits ALS_INT_EN  = Adafruit_BusIO_RegisterBits(&ALS_CONF1_reg, 1, 1);  // bit 1
Adafruit_BusIO_RegisterBits ALS_SD      = Adafruit_BusIO_RegisterBits(&ALS_CONF1_reg, 1, 0);  // bit 0
```

### Command 0x00 High: ALS_CONF2

| Bits | Name | Description |
|------|------|-------------|
| 7:2 | Reserved | Keep as 0 |
| 1 | ALS_NS | ALS sensitivity (0=2x typical, 1=1x typical) |
| 0 | WHITE_SD | White channel shutdown (0=power on, 1=shutdown) **Default=1** |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits ALS_NS      = Adafruit_BusIO_RegisterBits(&ALS_CONF2_reg, 1, 1);  // bit 1
Adafruit_BusIO_RegisterBits WHITE_SD    = Adafruit_BusIO_RegisterBits(&ALS_CONF2_reg, 1, 0);  // bit 0
```

### Command 0x03 Low: PS_CONF1

| Bits | Name | Description |
|------|------|-------------|
| 7:6 | PS_DUTY | PS IRED duty ratio |
| 5:4 | PS_PERS | PS interrupt persistence |
| 3:1 | PS_IT | PS integration time |
| 0 | PS_SD | PS shutdown (0=power on, 1=shutdown) **Default=1** |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits PS_DUTY     = Adafruit_BusIO_RegisterBits(&PS_CONF1_reg, 2, 6);  // bits 7:6
Adafruit_BusIO_RegisterBits PS_PERS     = Adafruit_BusIO_RegisterBits(&PS_CONF1_reg, 2, 4);  // bits 5:4
Adafruit_BusIO_RegisterBits PS_IT       = Adafruit_BusIO_RegisterBits(&PS_CONF1_reg, 3, 1);  // bits 3:1
Adafruit_BusIO_RegisterBits PS_SD       = Adafruit_BusIO_RegisterBits(&PS_CONF1_reg, 1, 0);  // bit 0
```

### Command 0x03 High: PS_CONF2

| Bits | Name | Description |
|------|------|-------------|
| 7:6 | Reserved | Keep as 0 |
| 5:4 | PS_GAIN | PS gain mode |
| 3 | PS_HD | PS output resolution (0=12-bit, 1=16-bit) |
| 2 | PS_NS | PS sensitivity mode |
| 1:0 | PS_INT | PS interrupt trigger mode |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits PS_GAIN     = Adafruit_BusIO_RegisterBits(&PS_CONF2_reg, 2, 4);  // bits 5:4
Adafruit_BusIO_RegisterBits PS_HD       = Adafruit_BusIO_RegisterBits(&PS_CONF2_reg, 1, 3);  // bit 3
Adafruit_BusIO_RegisterBits PS_NS       = Adafruit_BusIO_RegisterBits(&PS_CONF2_reg, 1, 2);  // bit 2
Adafruit_BusIO_RegisterBits PS_INT      = Adafruit_BusIO_RegisterBits(&PS_CONF2_reg, 2, 0);  // bits 1:0
```

### Command 0x04 Low: PS_CONF3

| Bits | Name | Description |
|------|------|-------------|
| 7 | LED_I_LOW | Low LED current mode (0=normal, 1=1/10 current) |
| 6:5 | Reserved | Keep as 0 |
| 4 | PS_SMART_PERS | Smart persistence (0=disable, 1=enable) |
| 3 | PS_AF | Active force mode (0=disable/normal, 1=enable) |
| 2 | PS_TRIG | Trigger one PS reading (write 1, auto-clears to 0) |
| 1 | PS_MS | PS mode select (0=normal+interrupt, 1=logic output mode) |
| 0 | PS_SC_EN | Sunlight cancellation (0=off, 1=on) |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits LED_I_LOW      = Adafruit_BusIO_RegisterBits(&PS_CONF3_reg, 1, 7);  // bit 7
Adafruit_BusIO_RegisterBits PS_SMART_PERS  = Adafruit_BusIO_RegisterBits(&PS_CONF3_reg, 1, 4);  // bit 4
Adafruit_BusIO_RegisterBits PS_AF          = Adafruit_BusIO_RegisterBits(&PS_CONF3_reg, 1, 3);  // bit 3
Adafruit_BusIO_RegisterBits PS_TRIG        = Adafruit_BusIO_RegisterBits(&PS_CONF3_reg, 1, 2);  // bit 2
Adafruit_BusIO_RegisterBits PS_MS          = Adafruit_BusIO_RegisterBits(&PS_CONF3_reg, 1, 1);  // bit 1
Adafruit_BusIO_RegisterBits PS_SC_EN       = Adafruit_BusIO_RegisterBits(&PS_CONF3_reg, 1, 0);  // bit 0
```

### Command 0x04 High: PS_MS (LED/Sunlight Settings)

| Bits | Name | Description |
|------|------|-------------|
| 7 | Reserved | Keep as 0 |
| 6:5 | PS_SC_CUR | Sunlight cancellation current multiplier |
| 4 | PS_SP | Sunlight protection capability (0=1x, 1=1.5x) |
| 3 | PS_SPO | Sunlight protect output (0=0x00, 1=0xFF) |
| 2:0 | LED_I | LED driving current |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits PS_SC_CUR   = Adafruit_BusIO_RegisterBits(&PS_MS_reg, 2, 5);  // bits 6:5
Adafruit_BusIO_RegisterBits PS_SP       = Adafruit_BusIO_RegisterBits(&PS_MS_reg, 1, 4);  // bit 4
Adafruit_BusIO_RegisterBits PS_SPO      = Adafruit_BusIO_RegisterBits(&PS_MS_reg, 1, 3);  // bit 3
Adafruit_BusIO_RegisterBits LED_I       = Adafruit_BusIO_RegisterBits(&PS_MS_reg, 3, 0);  // bits 2:0
```

### Command 0x0D High: INT_Flag (Read-Only)

| Bits | Name | Description |
|------|------|-------------|
| 7 | Reserved | - |
| 6 | Reserved | - |
| 5 | ALS_IF_L | ALS crossed low threshold |
| 4 | ALS_IF_H | ALS crossed high threshold |
| 3 | Reserved | - |
| 2 | PS_SPFLAG | PS sunlight protection mode active |
| 1 | PS_IF_CLOSE | PS crossed high threshold (object approaching) |
| 0 | PS_IF_AWAY | PS crossed low threshold (object moving away) |

**Adafruit_BusIO RegisterBits:**
```cpp
Adafruit_BusIO_RegisterBits ALS_IF_L     = Adafruit_BusIO_RegisterBits(&INT_FLAG_reg, 1, 5);  // bit 5
Adafruit_BusIO_RegisterBits ALS_IF_H     = Adafruit_BusIO_RegisterBits(&INT_FLAG_reg, 1, 4);  // bit 4
Adafruit_BusIO_RegisterBits PS_SPFLAG    = Adafruit_BusIO_RegisterBits(&INT_FLAG_reg, 1, 2);  // bit 2
Adafruit_BusIO_RegisterBits PS_IF_CLOSE  = Adafruit_BusIO_RegisterBits(&INT_FLAG_reg, 1, 1);  // bit 1
Adafruit_BusIO_RegisterBits PS_IF_AWAY   = Adafruit_BusIO_RegisterBits(&INT_FLAG_reg, 1, 0);  // bit 0
```

### Command 0x0E High: ID_M (Read-Only)

| Bits | Name | Description |
|------|------|-------------|
| 7:6 | Reserved | Always 0 |
| 5:4 | SLAVE_ADDR_ID | I2C address variant (00=0x60, 01=0x51, 10=0x40, 11=0x41) |
| 3:0 | VERSION_CODE | Version code (0000) |

---

## Typed Enums

```cpp
/** ALS integration time settings */
typedef enum {
  VCNL4030_ALS_IT_50MS  = 0b000,  // 50ms, 0.064 lux/step
  VCNL4030_ALS_IT_100MS = 0b001,  // 100ms, 0.032 lux/step
  VCNL4030_ALS_IT_200MS = 0b010,  // 200ms, 0.016 lux/step
  VCNL4030_ALS_IT_400MS = 0b011,  // 400ms, 0.008 lux/step
  VCNL4030_ALS_IT_800MS = 0b100,  // 800ms, 0.004 lux/step (also 0b101, 0b110, 0b111)
} vcnl4030_als_it_t;

/** ALS persistence settings (consecutive out-of-range readings before interrupt) */
typedef enum {
  VCNL4030_ALS_PERS_1 = 0b00,  // 1 reading
  VCNL4030_ALS_PERS_2 = 0b01,  // 2 readings
  VCNL4030_ALS_PERS_4 = 0b10,  // 4 readings
  VCNL4030_ALS_PERS_8 = 0b11,  // 8 readings
} vcnl4030_als_pers_t;

/** PS IRED duty ratio settings */
typedef enum {
  VCNL4030_PS_DUTY_40   = 0b00,  // 1/40 duty cycle
  VCNL4030_PS_DUTY_80   = 0b01,  // 1/80 duty cycle
  VCNL4030_PS_DUTY_160  = 0b10,  // 1/160 duty cycle
  VCNL4030_PS_DUTY_320  = 0b11,  // 1/320 duty cycle (lowest power)
} vcnl4030_ps_duty_t;

/** PS persistence settings (consecutive out-of-range readings before interrupt) */
typedef enum {
  VCNL4030_PS_PERS_1 = 0b00,  // 1 reading
  VCNL4030_PS_PERS_2 = 0b01,  // 2 readings
  VCNL4030_PS_PERS_3 = 0b10,  // 3 readings
  VCNL4030_PS_PERS_4 = 0b11,  // 4 readings
} vcnl4030_ps_pers_t;

/** PS integration time settings */
typedef enum {
  VCNL4030_PS_IT_1T   = 0b000,  // 1T
  VCNL4030_PS_IT_1_5T = 0b001,  // 1.5T
  VCNL4030_PS_IT_2T   = 0b010,  // 2T
  VCNL4030_PS_IT_2_5T = 0b011,  // 2.5T
  VCNL4030_PS_IT_3T   = 0b100,  // 3T
  VCNL4030_PS_IT_3_5T = 0b101,  // 3.5T
  VCNL4030_PS_IT_4T   = 0b110,  // 4T
  VCNL4030_PS_IT_8T   = 0b111,  // 8T (highest sensitivity)
} vcnl4030_ps_it_t;

/** PS gain settings */
typedef enum {
  VCNL4030_PS_GAIN_TWO_STEP     = 0b00,  // Two-step mode (also 0b01)
  VCNL4030_PS_GAIN_SINGLE_8X    = 0b10,  // Single mode x8
  VCNL4030_PS_GAIN_SINGLE_1X    = 0b11,  // Single mode x1
} vcnl4030_ps_gain_t;

/** PS interrupt mode settings */
typedef enum {
  VCNL4030_PS_INT_DISABLE = 0b00,  // Interrupt disabled
  VCNL4030_PS_INT_CLOSE   = 0b01,  // Interrupt on close (rising above high threshold)
  VCNL4030_PS_INT_AWAY    = 0b10,  // Interrupt on away (falling below low threshold)
  VCNL4030_PS_INT_BOTH    = 0b11,  // Interrupt on both close and away
} vcnl4030_ps_int_t;

/** LED driving current settings */
typedef enum {
  VCNL4030_LED_I_50MA  = 0b000,  // 50mA
  VCNL4030_LED_I_75MA  = 0b001,  // 75mA
  VCNL4030_LED_I_100MA = 0b010,  // 100mA
  VCNL4030_LED_I_120MA = 0b011,  // 120mA
  VCNL4030_LED_I_140MA = 0b100,  // 140mA
  VCNL4030_LED_I_160MA = 0b101,  // 160mA
  VCNL4030_LED_I_180MA = 0b110,  // 180mA
  VCNL4030_LED_I_200MA = 0b111,  // 200mA
} vcnl4030_led_i_t;

/** Sunlight cancellation current multiplier */
typedef enum {
  VCNL4030_PS_SC_CUR_1X = 0b00,  // 1x typical
  VCNL4030_PS_SC_CUR_2X = 0b01,  // 2x typical
  VCNL4030_PS_SC_CUR_4X = 0b10,  // 4x typical
  VCNL4030_PS_SC_CUR_8X = 0b11,  // 8x typical
} vcnl4030_ps_sc_cur_t;
```

---

## ALS Resolution Table

| ALS_IT | Integration Time | Resolution (lux/step) | Max Range (lux) |
|--------|------------------|----------------------|-----------------|
| 0b000 | 50ms | 0.064 | 4192 |
| 0b001 | 100ms | 0.032 | 2096 |
| 0b010 | 200ms | 0.016 | 1048 |
| 0b011 | 400ms | 0.008 | 524 |
| 0b1xx | 800ms | 0.004 | 262 |

**Dynamic Range Modifiers:**
- `ALS_HD=1`: Multiplies range by 2x (halves resolution)
- `ALS_NS=1`: Multiplies range by 2x (halves resolution)
- Combined: Max range = 4192 x 2 x 2 = **16,768 lux**

---

## Public API Function Map

### Initialization
```cpp
bool begin(uint8_t i2c_addr = 0x60, TwoWire *wire = &Wire);
// - Verify device ID (ID_L should be 0x80)
// - Set default configuration
// - Enable ALS and PS
```

### ALS Functions
```cpp
// Enable/Disable
void enableALS(bool enable);
bool ALSEnabled();

// Configuration
void setALSIntegrationTime(vcnl4030_als_it_t it);
vcnl4030_als_it_t getALSIntegrationTime();
void setALSHighDynamicRange(bool enable);  // ALS_HD
bool getALSHighDynamicRange();
void setALSLowSensitivity(bool enable);    // ALS_NS
bool getALSLowSensitivity();

// Data reading
uint16_t readALS();       // Returns raw 16-bit value
float readLux();          // Returns calculated lux value

// White channel
void enableWhiteChannel(bool enable);
bool whiteChannelEnabled();
uint16_t readWhite();     // Returns raw 16-bit value
```

### Proximity Functions
```cpp
// Enable/Disable
void enablePS(bool enable);
bool PSEnabled();

// Configuration
void setPSDuty(vcnl4030_ps_duty_t duty);
vcnl4030_ps_duty_t getPSDuty();
void setPSIntegrationTime(vcnl4030_ps_it_t it);
vcnl4030_ps_it_t getPSIntegrationTime();
void setPSGain(vcnl4030_ps_gain_t gain);
vcnl4030_ps_gain_t getPSGain();
void setPSResolution16Bit(bool enable);   // PS_HD
bool getPSResolution16Bit();
void setPSLowSensitivity(bool enable);    // PS_NS
bool getPSLowSensitivity();

// LED current
void setLEDCurrent(vcnl4030_led_i_t current);
vcnl4030_led_i_t getLEDCurrent();
void setLEDLowCurrent(bool enable);       // 1/10 current mode
bool getLEDLowCurrent();

// Data reading
uint16_t readProximity();

// Crosstalk cancellation
void setPSCancellation(uint16_t value);
uint16_t getPSCancellation();
```

### Interrupt Functions
```cpp
// ALS interrupts
void enableALSInterrupt(bool enable);
bool ALSInterruptEnabled();
void setALSPersistence(vcnl4030_als_pers_t pers);
vcnl4030_als_pers_t getALSPersistence();
void setALSHighThreshold(uint16_t threshold);
uint16_t getALSHighThreshold();
void setALSLowThreshold(uint16_t threshold);
uint16_t getALSLowThreshold();

// PS interrupts
void setPSInterruptMode(vcnl4030_ps_int_t mode);
vcnl4030_ps_int_t getPSInterruptMode();
void setPSPersistence(vcnl4030_ps_pers_t pers);
vcnl4030_ps_pers_t getPSPersistence();
void setPSHighThreshold(uint16_t threshold);
uint16_t getPSHighThreshold();
void setPSLowThreshold(uint16_t threshold);
uint16_t getPSLowThreshold();
void setPSSmartPersistence(bool enable);
bool getPSSmartPersistence();

// Interrupt flags (read clears flags)
uint8_t readInterruptFlags();
bool getALSHighFlag();     // ALS crossed high threshold
bool getALSLowFlag();      // ALS crossed low threshold
bool getPSCloseFlag();     // PS object approaching
bool getPSAwayFlag();      // PS object moving away
bool getPSSunlightFlag();  // PS in sunlight protection mode
```

### Advanced PS Features
```cpp
// Active force mode (ultra-low power)
void enableActiveForceMode(bool enable);
bool activeForceMode();
void triggerPSReading();   // Trigger single reading (auto-clears)

// Proximity detection logic output mode
void enablePSLogicMode(bool enable);
bool PSLogicMode();

// Sunlight cancellation
void enableSunlightCancellation(bool enable);
bool sunlightCancellationEnabled();
void setSunlightCancelCurrent(vcnl4030_ps_sc_cur_t cur);
vcnl4030_ps_sc_cur_t getSunlightCancelCurrent();
void setSunlightProtection(bool enhanced);     // PS_SP: 1.5x capability
void setSunlightProtectOutput(bool high);      // PS_SPO: output 0xFF instead of 0x00
```

---

## Timing Requirements

### Power-On Sequence
1. Apply VDD (2.5V - 3.6V)
2. Wait for power stabilization (no specified delay, but >1ms recommended)
3. Chip boots with ALS and PS in shutdown mode (default registers)
4. Configure registers via I2C
5. Enable ALS and/or PS by clearing shutdown bits

### Integration Times
| Function | Setting | Time |
|----------|---------|------|
| ALS | 50ms - 800ms | Configurable |
| PS | Depends on duty cycle and IT setting | Variable |

### Response Time Factors
- **PS Duty Cycle**: Higher duty = faster response, more power
- **PS Integration Time**: Longer IT = higher sensitivity, slower
- **Persistence**: More readings required = slower interrupt response

---

## Quirks and Gotchas

### 1. Default Shutdown State
- Both ALS and PS are **shutdown by default** (ALS_SD=1, PS_SD=1)
- White channel is also shutdown by default (WHITE_SD=1)
- Must explicitly enable by writing 0 to these bits

### 2. Register Access Pattern
- All registers are 16-bit word access (even if only 8 bits used)
- Must read/write full word to avoid corrupting adjacent byte
- LSB is sent/received first

### 3. Interrupt Flag Behavior
- Reading INT_Flag register **clears all flags**
- Read flags promptly and cache locally if needed
- INT pin stays low until flags are read

### 4. Cancellation Level
- PS_CANC subtracts from raw PS reading
- If cancellation > raw reading, output is 0 (no negative)
- Set based on measured crosstalk in application

### 5. Active Force Mode
- When PS_AF=1, PS only measures when PS_TRIG written to 1
- PS_TRIG auto-clears to 0 after reading triggered
- Useful for battery-powered intermittent sensing

### 6. Logic Output Mode Limitation
- When PS_MS=1 (logic output mode), INT pin is proximity output only
- ALS interrupt function **cannot be used** in this mode
- Choose one: interrupt mode OR logic output mode

### 7. Multiple I2C Addresses
- Different part numbers have different fixed addresses
- ID_M register indicates which variant
- Cannot change address at runtime

### 8. LED Current with Low Current Mode
- When LED_I_LOW=1, actual current is 1/10 of LED_I setting
- Example: LED_I=200mA + LED_I_LOW=1 -> 20mA actual

### 9. Sunlight Saturation
- PS_SPFLAG indicates when sunlight protection is active
- Check this flag if PS readings seem invalid in bright conditions
- PS_SPO controls whether output is 0x00 or 0xFFFF during protection

### 10. 12-bit vs 16-bit PS Mode
- PS_HD=0: 12-bit output (0-4095)
- PS_HD=1: 16-bit output (0-65535)
- Higher resolution useful for fine distance sensing

---

## Register Address Constants

```cpp
#define VCNL4030_REG_ALS_CONF    0x00  // ALS_CONF1 (L) + ALS_CONF2 (H)
#define VCNL4030_REG_ALS_THDH    0x01  // ALS high threshold (16-bit)
#define VCNL4030_REG_ALS_THDL    0x02  // ALS low threshold (16-bit)
#define VCNL4030_REG_PS_CONF1_2  0x03  // PS_CONF1 (L) + PS_CONF2 (H)
#define VCNL4030_REG_PS_CONF3_MS 0x04  // PS_CONF3 (L) + PS_MS (H)
#define VCNL4030_REG_PS_CANC     0x05  // PS cancellation (16-bit)
#define VCNL4030_REG_PS_THDL     0x06  // PS low threshold (16-bit)
#define VCNL4030_REG_PS_THDH     0x07  // PS high threshold (16-bit)
#define VCNL4030_REG_PS_DATA     0x08  // PS data output (16-bit)
#define VCNL4030_REG_ALS_DATA    0x0B  // ALS data output (16-bit)
#define VCNL4030_REG_WHITE_DATA  0x0C  // White data output (16-bit)
#define VCNL4030_REG_INT_FLAG    0x0D  // Reserved (L) + INT_Flag (H)
#define VCNL4030_REG_ID          0x0E  // ID_L (L) + ID_M (H)

#define VCNL4030_DEFAULT_ADDR    0x60
#define VCNL4030_ID_L_EXPECTED   0x80
```

---

## Implementation Notes for Adafruit_BusIO

### Register Objects
```cpp
// 16-bit registers for word access
Adafruit_BusIO_Register ALS_CONF_reg    = Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_ALS_CONF, 2, LSBFIRST);
Adafruit_BusIO_Register PS_CONF1_2_reg  = Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF1_2, 2, LSBFIRST);
Adafruit_BusIO_Register PS_CONF3_MS_reg = Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_CONF3_MS, 2, LSBFIRST);

// For RegisterBits within the 16-bit word:
// - Low byte bits are at positions 0-7
// - High byte bits are at positions 8-15

// Example for ALS_IT (bits 7:5 of low byte = bits 7:5 of 16-bit word)
Adafruit_BusIO_RegisterBits ALS_IT = Adafruit_BusIO_RegisterBits(&ALS_CONF_reg, 3, 5);

// Example for ALS_NS (bit 1 of high byte = bit 9 of 16-bit word)
Adafruit_BusIO_RegisterBits ALS_NS = Adafruit_BusIO_RegisterBits(&ALS_CONF_reg, 1, 9);
```

### Reading Data
```cpp
uint16_t readPS() {
  Adafruit_BusIO_Register PS_DATA_reg = Adafruit_BusIO_Register(i2c_dev, VCNL4030_REG_PS_DATA, 2, LSBFIRST);
  return PS_DATA_reg.read();
}
```

---

## Test Plan Checklist

- [ ] Verify device ID at startup
- [ ] ALS enable/disable and readout
- [ ] ALS integration time changes affect resolution
- [ ] White channel enable/disable and readout
- [ ] PS enable/disable and readout
- [ ] PS resolution 12-bit vs 16-bit
- [ ] LED current changes affect PS reading amplitude
- [ ] Crosstalk cancellation reduces baseline
- [ ] Interrupt thresholds trigger at correct levels
- [ ] Interrupt flags read and clear correctly
- [ ] Active force mode triggers single readings
- [ ] Sunlight cancellation reduces false readings

## Quick Register Reference

```
VCNL4030X01 Register Quick Reference
====================================

COMMAND CODE REGISTER MAP
-------------------------
Cmd  | Byte | Name          | R/W | Default | Description
-----|------|---------------|-----|---------|---------------------------
0x00 | L    | ALS_CONF1     | R/W | 0x01    | ALS IT, HD, PERS, INT_EN, SD
0x00 | H    | ALS_CONF2     | R/W | 0x01    | ALS_NS, WHITE_SD
0x01 | L+H  | ALS_THDH      | R/W | 0x0000  | ALS high threshold (16-bit)
0x02 | L+H  | ALS_THDL      | R/W | 0x0000  | ALS low threshold (16-bit)
0x03 | L    | PS_CONF1      | R/W | 0x01    | PS DUTY, PERS, IT, SD
0x03 | H    | PS_CONF2      | R/W | 0x00    | PS GAIN, HD, NS, INT
0x04 | L    | PS_CONF3      | R/W | 0x00    | LED_I_LOW, SMART_PERS, AF, TRIG, MS, SC_EN
0x04 | H    | PS_MS         | R/W | 0x00    | SC_CUR, SP, SPO, LED_I
0x05 | L+H  | PS_CANC       | R/W | 0x0000  | PS cancellation (16-bit)
0x06 | L+H  | PS_THDL       | R/W | 0x0000  | PS low threshold (16-bit)
0x07 | L+H  | PS_THDH       | R/W | 0x0000  | PS high threshold (16-bit)
0x08 | L+H  | PS_DATA       | R   | -       | PS output (12/16-bit)
0x09 | L+H  | Reserved      | R   | 0x0000  | -
0x0A | L+H  | Reserved      | R   | 0x0000  | -
0x0B | L+H  | ALS_DATA      | R   | -       | ALS output (16-bit)
0x0C | L+H  | WHITE_DATA    | R   | -       | White channel output (16-bit)
0x0D | L    | Reserved      | R   | 0x00    | -
0x0D | H    | INT_FLAG      | R   | 0x00    | Interrupt flags
0x0E | L    | ID_L          | R   | 0x80    | Device ID LSB
0x0E | H    | ID_M          | R   | varies  | Device ID MSB (addr variant)

BIT FIELDS
----------

ALS_CONF1 (0x00_L):
  [7:5] ALS_IT      - 000=50ms, 001=100ms, 010=200ms, 011=400ms, 1xx=800ms
  [4]   ALS_HD      - 0=1x range, 1=2x range
  [3:2] ALS_PERS    - 00=1, 01=2, 10=4, 11=8 persistence
  [1]   ALS_INT_EN  - 0=disable, 1=enable ALS interrupt
  [0]   ALS_SD      - 0=power on, 1=shutdown (default=1)

ALS_CONF2 (0x00_H):
  [7:2] Reserved    - keep 0
  [1]   ALS_NS      - 0=2x sensitivity, 1=1x sensitivity
  [0]   WHITE_SD    - 0=power on, 1=shutdown (default=1)

PS_CONF1 (0x03_L):
  [7:6] PS_DUTY     - 00=1/40, 01=1/80, 10=1/160, 11=1/320
  [5:4] PS_PERS     - 00=1, 01=2, 10=3, 11=4 persistence
  [3:1] PS_IT       - 000=1T, 001=1.5T, 010=2T, 011=2.5T, 100=3T, 101=3.5T, 110=4T, 111=8T
  [0]   PS_SD       - 0=power on, 1=shutdown (default=1)

PS_CONF2 (0x03_H):
  [7:6] Reserved    - keep 0
  [5:4] PS_GAIN     - 00/01=two-step, 10=single x8, 11=single x1
  [3]   PS_HD       - 0=12-bit, 1=16-bit output
  [2]   PS_NS       - 0=4x sensitivity (two-step), 1=1x sensitivity
  [1:0] PS_INT      - 00=disable, 01=close, 10=away, 11=both

PS_CONF3 (0x04_L):
  [7]   LED_I_LOW   - 0=normal, 1=1/10 current
  [6:5] Reserved    - keep 0
  [4]   PS_SMART_PERS - 0=disable, 1=enable smart persistence
  [3]   PS_AF       - 0=normal, 1=active force mode
  [2]   PS_TRIG     - write 1 to trigger, auto-clears
  [1]   PS_MS       - 0=normal+int, 1=logic output mode
  [0]   PS_SC_EN    - 0=off, 1=sunlight cancel on

PS_MS (0x04_H):
  [7]   Reserved    - keep 0
  [6:5] PS_SC_CUR   - 00=1x, 01=2x, 10=4x, 11=8x sunlight cancel current
  [4]   PS_SP       - 0=1x, 1=1.5x sunlight protection
  [3]   PS_SPO      - 0=output 0x00, 1=output 0xFF in protection mode
  [2:0] LED_I       - 000=50mA, 001=75mA, 010=100mA, 011=120mA,
                      100=140mA, 101=160mA, 110=180mA, 111=200mA

INT_FLAG (0x0D_H) - Read-only, clears on read:
  [7:6] Reserved
  [5]   ALS_IF_L    - ALS crossed low threshold
  [4]   ALS_IF_H    - ALS crossed high threshold
  [3]   Reserved
  [2]   PS_SPFLAG   - PS sunlight protection active
  [1]   PS_IF_CLOSE - PS crossed high threshold (approaching)
  [0]   PS_IF_AWAY  - PS crossed low threshold (moving away)

ID_M (0x0E_H) - Read-only:
  [7:6] Reserved
  [5:4] SLAVE_ID    - 00=0x60, 01=0x51, 10=0x40, 11=0x41
  [3:0] VERSION     - 0000

I2C ADDRESS OPTIONS
-------------------
Part Number           | I2C Address (7-bit)
----------------------|--------------------
VCNL4030X01-GS08/18   | 0x60
VCNL40301X01-GS08/18  | 0x51
VCNL40302X01-GS08/18  | 0x40
VCNL40303X01-GS08/18  | 0x41

ALS RESOLUTION TABLE
--------------------
ALS_IT  | Time  | Resolution   | Max Range
--------|-------|--------------|----------
000     | 50ms  | 0.064 lux    | 4192 lux
001     | 100ms | 0.032 lux    | 2096 lux
010     | 200ms | 0.016 lux    | 1048 lux
011     | 400ms | 0.008 lux    | 524 lux
1xx     | 800ms | 0.004 lux    | 262 lux

Note: ALS_HD=1 doubles range (halves resolution)
      ALS_NS=1 doubles range (halves resolution)
      Max possible: 4192 x 2 x 2 = 16,768 lux
```
