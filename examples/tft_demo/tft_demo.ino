/*
 * VCNL4030 Proximity + Ambient Light TFT Demo for ESP32-S2 TFT Feather
 *
 * Big, bold display for product photos/video.
 * Proximity bar gauge + large lux readout.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_VCNL4030.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

Adafruit_VCNL4030 vcnl;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define SCREEN_W 240
#define SCREEN_H 135

GFXcanvas16 canvas(SCREEN_W, SCREEN_H);

// Layout
#define TITLE_Y 22
#define BAR_Y 32
#define BAR_H 30
#define BAR_MARGIN 4
#define LUX_Y 110

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(100);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  display.init(135, 240);
  display.setRotation(3);

  canvas.setTextWrap(false);

  if (!vcnl.begin()) {
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setFont(&FreeSansBold18pt7b);
    canvas.setTextColor(ST77XX_RED);
    canvas.setCursor(10, 80);
    canvas.print(F("No VCNL4030!"));
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
    while (1)
      delay(10);
  }

  Serial.println(F("VCNL4030 found!"));

  vcnl.enableALS(true);
  vcnl.enableWhiteChannel(true);
  vcnl.enableProx(true);
  vcnl.setProxResolution16Bit(true);
}

uint16_t proxMax = 1000; // auto-scales up

void loop() {
  uint16_t prox = vcnl.readProximity();
  float lux = vcnl.readLux();

  // Auto-scale prox bar to max seen value (with some headroom)
  if (prox > proxMax) {
    proxMax = prox + (prox / 4); // 25% headroom
  }

  canvas.fillScreen(ST77XX_BLACK);

  // ---- Title ----
  canvas.setFont(&FreeSansBold12pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  int16_t x1, y1;
  uint16_t tw, th;
  canvas.getTextBounds("Adafruit VCNL4030", 0, 0, &x1, &y1, &tw, &th);
  canvas.setCursor((SCREEN_W - tw) / 2, TITLE_Y);
  canvas.print(F("Adafruit VCNL4030"));

  // ---- Proximity bar ----
  uint16_t barX = BAR_MARGIN;
  uint16_t barW = SCREEN_W - (BAR_MARGIN * 2);
  canvas.drawRect(barX, BAR_Y, barW, BAR_H, ST77XX_WHITE);

  uint16_t fillW = (uint32_t)prox * (barW - 2) / proxMax;
  if (fillW > 0) {
    canvas.fillRect(barX + 1, BAR_Y + 1, fillW, BAR_H - 2, ST77XX_CYAN);
  }

  // "PROX" label centered in bar
  canvas.setFont(&FreeSansBold9pt7b);
  canvas.getTextBounds("PROX", 0, 0, &x1, &y1, &tw, &th);
  uint16_t labelX = (SCREEN_W - tw) / 2;
  uint16_t labelY = BAR_Y + BAR_H / 2 + th / 2;
  // Black outline for readability over cyan fill
  for (int8_t dx = -1; dx <= 1; dx++) {
    for (int8_t dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;
      canvas.setTextColor(ST77XX_BLACK);
      canvas.setCursor(labelX + dx, labelY + dy);
      canvas.print(F("PROX"));
    }
  }
  canvas.setTextColor(ST77XX_WHITE);
  canvas.setCursor(labelX, labelY);
  canvas.print(F("PROX"));

  // ---- Big lux readout ----
  // "lux" label is fixed; number is right-aligned to its left
  char luxBuf[12];
  if (lux < 10) {
    dtostrf(lux, 0, 2, luxBuf);
  } else if (lux < 100) {
    dtostrf(lux, 0, 1, luxBuf);
  } else {
    dtostrf(lux, 0, 0, luxBuf);
  }

  // Fixed position for "lux" label
  const int16_t luxLabelX = 160;

  // Draw "lux" label at fixed position
  canvas.setFont(&FreeSansBold12pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.setCursor(luxLabelX, LUX_Y);
  canvas.print(F("lux"));

  // Right-align the number just left of the label
  canvas.setFont(&FreeSansBold24pt7b);
  canvas.setTextColor(ST77XX_YELLOW);
  canvas.getTextBounds(luxBuf, 0, 0, &x1, &y1, &tw, &th);
  canvas.setCursor(luxLabelX - 8 - tw, LUX_Y);
  canvas.print(luxBuf);

  // Blit
  display.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
  delay(100);
}
