#include "TestConfig.h"
#ifndef SIMULATE_HARDWARE
#include <Adafruit_NeoPixel.h>
#else
#include "HardwareSim.h"
#include "PCSandbox.h"
#endif
#define LED_PIN_MAINSTRIP 13
#define LED_PIN_MASKSTRIP_LEFT 0
#define LED_PIN_MASKSTRIP_RIGHT 0
#define LED_COUNT_MAINSTRIP 240
#define LED_COUNT_MASKSTRIP 6
Adafruit_NeoPixel MainStrip(LED_COUNT_MAINSTRIP, LED_PIN_MAINSTRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel MaskStripLeft(LED_COUNT_MASKSTRIP, LED_PIN_MASKSTRIP_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel MaskStripRight(LED_COUNT_MASKSTRIP, LED_PIN_MASKSTRIP_RIGHT, NEO_GRB + NEO_KHZ800);
uint32_t black = MainStrip.Color(0, 0, 0);
uint32_t white = MainStrip.Color(255, 255, 255);
uint32_t color = white;
int Color_Brightness = 25; // 0 - 255

struct LEDsTaskInput
{
  int leds_on;
  int leds_brightness;
  int leds_color_r;
  int leds_color_g;
  int leds_color_b;
  int leds_effect;
  int leds_level;
};

void setBrightnessStrips(int brightness) {
  MainStrip.setBrightness(brightness);
  MaskStripLeft.setBrightness(brightness);
  MaskStripRight.setBrightness(brightness);
}
void fillStrips(uint32_t color) {
  MainStrip.fill(color, 0, MainStrip.numPixels());
  MaskStripLeft.fill(color, 0, MaskStripLeft.numPixels());
  MaskStripRight.fill(color, 0, MaskStripRight.numPixels());
}
void showStrips() {
  MainStrip.show();
  MaskStripLeft.show();
  MaskStripRight.show();
}
void clearStrips() {
  MainStrip.clear();
  MaskStripLeft.clear();
  MaskStripRight.clear();
}

void colorStatic() {
  setBrightnessStrips(Color_Brightness/2);
  fillStrips(color);
  showStrips();
}

void colorFade() {
  setBrightnessStrips(Color_Brightness);
  for(int k = 0; k < Color_Brightness*2; k++) {
    fillStrips(color);
    setBrightnessStrips(k);
    showStrips();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
  for(int k = Color_Brightness*2; k > 0; k--) {
    fillStrips(color);
    setBrightnessStrips(k);
    showStrips();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void colorWipe() {
  setBrightnessStrips(Color_Brightness);
  if (MainStrip.getPixelColor(0) == 0)
  {
    for(uint16_t i=0; i<MainStrip.numPixels(); i++) {
      MainStrip.setPixelColor(i, color);
      showStrips();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    for(uint16_t i=0; i<MaskStripLeft.numPixels(); i++) {
      MaskStripLeft.setPixelColor(i, color);
      MaskStripRight.setPixelColor(i, color);
      showStrips();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
  } else {
    for(uint16_t i=0; i<MainStrip.numPixels(); i++) {
      MainStrip.setPixelColor(i, black);
      showStrips();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    for(uint16_t i=0; i<MaskStripLeft.numPixels(); i++) {
      MaskStripLeft.setPixelColor(i, black);
      MaskStripRight.setPixelColor(i, black);
      showStrips();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
  }
}

void colorTheaterChase() {
  setBrightnessStrips(Color_Brightness*2);
  for(int b=0; b<3; b++) {
    clearStrips();
    for(int c=b; c<MainStrip.numPixels(); c += 3) {
      MainStrip.setPixelColor(c, color);
    }
    for(int c=b; c<MaskStripLeft.numPixels(); c += 3) {
      MaskStripLeft.setPixelColor(c, color);
      MaskStripRight.setPixelColor(c, color);
    }
    showStrips();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void Rainbow() {
  setBrightnessStrips(Color_Brightness*2);
  for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 512) {
    for(int i=0; i<MainStrip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / MainStrip.numPixels());
      MainStrip.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
    }
    for(int i=0; i<MaskStripLeft.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / MaskStripLeft.numPixels());
      MaskStripLeft.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
      MaskStripRight.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
    }
    showStrips();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void colorStrobe() {
  setBrightnessStrips(Color_Brightness/2);
  for(int j = 0; j < 5; j++) {
    fillStrips(color);
    showStrips();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    clearStrips();
    showStrips();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void colorMovingSubstrips() {
  uint32_t color_a = color;
  uint32_t color_b = black;
  setBrightnessStrips(Color_Brightness*2);
  int numPixels = MainStrip.numPixels();
  int substrip_size = numPixels/10;
  for(int i = 0; i < numPixels; i++) {
    clearStrips();
    for(int j = 0; j < numPixels; j += substrip_size*2) {
      int startPixel = (i + j) % numPixels;
      MainStrip.fill(color_a, startPixel, substrip_size);
      int endPixel = (startPixel + substrip_size) % numPixels;
      MainStrip.fill(color_b, endPixel, substrip_size);
    }
    MainStrip.show();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void off() {
  clearStrips();
  showStrips();
}

void colorLevel(int level_percent) {
  setBrightnessStrips(Color_Brightness);
  clearStrips();
  MainStrip.fill(color, 0, int(MainStrip.numPixels() * level_percent / 100));
  MaskStripLeft.fill(color, 0, int(MaskStripLeft.numPixels() * level_percent / 100));
  MaskStripRight.fill(color, 0, int(MaskStripRight.numPixels() * level_percent / 100));
  showStrips();
}

void setupLEDs() {
  MainStrip.begin();
  MaskStripLeft.begin();
  MaskStripRight.begin();
  setBrightnessStrips(Color_Brightness);
  clearStrips();
  showStrips();
}
