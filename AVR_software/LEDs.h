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
#define LED_COUNT_MASKSTRIP 16
Adafruit_NeoPixel MainStrip(LED_COUNT_MAINSTRIP, LED_PIN_MAINSTRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel MaskStripLeft(LED_COUNT_MASKSTRIP, LED_PIN_MASKSTRIP_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel MaskStripRight(LED_COUNT_MASKSTRIP, LED_PIN_MASKSTRIP_RIGHT, NEO_GRB + NEO_KHZ800);
uint32_t black = MainStrip.Color(0, 0, 0);
uint32_t white = MainStrip.Color(255, 255, 255);
uint32_t red = MainStrip.Color(255, 0, 0);
uint32_t purple = MainStrip.Color(115, 0, 255);
uint32_t yellow = MainStrip.Color(255, 255, 0);
uint32_t pink = MainStrip.Color(255, 0, 255);
uint32_t deep_blue = MainStrip.Color(0, 0, 255);
uint32_t light_blue = MainStrip.Color(0, 255, 255);
uint32_t orange = MainStrip.Color(255, 165, 0);
uint32_t green = MainStrip.Color(0, 255, 0);

int Color_Brightness = 25;
uint32_t color = white;

struct LEDsTaskInput
{
  int leds_on;
  int leds_brightness;
  int leds_color;
  int leds_effect;
  int leds_level;
};

void colorStatic() {
  MainStrip.setBrightness(Color_Brightness/2);
  MainStrip.fill(color, 0, MainStrip.numPixels());
  MainStrip.show();
}

void colorFade() {
    MainStrip.setBrightness(Color_Brightness);
    for(int k = 0; k < Color_Brightness*2; k++) {
      MainStrip.fill(color, 0, MainStrip.numPixels());
      MainStrip.setBrightness(k);
      MainStrip.show();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    for(int k = Color_Brightness*2; k > 0; k--) {
      MainStrip.fill(color, 0, MainStrip.numPixels());
      MainStrip.setBrightness(k);
      MainStrip.show();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void colorWipe() {
  MainStrip.setBrightness(Color_Brightness);
  if (MainStrip.getPixelColor(0) == 0)
  {
    for(uint16_t i=0; i<MainStrip.numPixels(); i++) {
      MainStrip.setPixelColor(i, color);
      MainStrip.show();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
  } else {
    for(uint16_t i=0; i<MainStrip.numPixels(); i++) {
      MainStrip.setPixelColor(i, black);
      MainStrip.show();
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
  }
}


void colorTheaterChase() {
  MainStrip.setBrightness(Color_Brightness*2);
  for(int b=0; b<3; b++) {
    MainStrip.clear();
    for(int c=b; c<MainStrip.numPixels(); c += 3) {
      MainStrip.setPixelColor(c, color);
    }
    MainStrip.show();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void Rainbow() {
  MainStrip.setBrightness(Color_Brightness*2);
  for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 512) {
    for(int i=0; i<MainStrip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / MainStrip.numPixels());
      MainStrip.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
    }
    MainStrip.show();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void colorStrobe() {
  MainStrip.setBrightness(Color_Brightness/2);
  for(int j = 0; j < 5; j++) {
    MainStrip.fill(color, 0, MainStrip.numPixels());
    MainStrip.show();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    MainStrip.clear();
    MainStrip.show();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void colorMovingSubstrips() {
  uint32_t color_a = color;
  uint32_t color_b = black;
  MainStrip.setBrightness(Color_Brightness*2);
  int numPixels = MainStrip.numPixels();
  int substrip_size = numPixels/10;
  for(int i = 0; i < numPixels; i++) {
    MainStrip.clear();
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
  MainStrip.clear();
  MainStrip.show();
}

void colorLevel(int level_percent) {
  MainStrip.setBrightness(Color_Brightness);
  MainStrip.clear();
  MainStrip.fill(color, 0, int(MainStrip.numPixels() * level_percent / 100));
  MainStrip.show();
}

void setupLEDs() {
  MainStrip.begin();
  MaskStripLeft.begin();
  MaskStripRight.begin();
  MainStrip.setBrightness(Color_Brightness);
  MaskStripLeft.setBrightness(Color_Brightness);
  MaskStripRight.setBrightness(Color_Brightness);
  MainStrip.clear(); MainStrip.show();
  MaskStripLeft.clear(); MaskStripLeft.show();
  MaskStripRight.clear(); MaskStripRight.show();
}
