#include <Adafruit_NeoPixel.h>
#define LED_PIN_MAINSTRIP 13
#define LED_PIN_MASKSTRIP_LEFT 9
#define LED_PIN_MASKSTRIP_RIGHT 4
#define LED_COUNT_MAINSTRIP 20
#define LED_COUNT_MASKSTRIP 7
Adafruit_NeoPixel MainStrip(LED_COUNT_MAINSTRIP, LED_PIN_MAINSTRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel MaskStripLeft(LED_COUNT_MASKSTRIP, LED_PIN_MASKSTRIP_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel MaskStripRight(LED_COUNT_MASKSTRIP, LED_PIN_MASKSTRIP_RIGHT, NEO_GRB + NEO_KHZ800);
uint32_t black = MainStrip.Color(0, 0, 0);
uint32_t white = MainStrip.Color(255, 255, 255);
uint32_t color = white;
int Color_Brightness = 50; // 0 - 255
int leds_on = 0;
int leds_brightness = 50;
int leds_color_r = 255;
int leds_color_g = 255;
int leds_color_b = 255;
int leds_effect = 0;
int leds_level = 100;

void setup() {
  Serial.begin(9600);
  MainStrip.begin();
  MaskStripLeft.begin();
  MaskStripRight.begin();
  setBrightnessStrips(Color_Brightness);
  clearStrips();
  showStrips();
  Serial.println("LEDs initialized - Ready for commands");
  Serial.println("Format: <animatronics_on>,<leds_on>,<leds_brightness>,<leds_color_r>,<leds_color_g>,<leds_color_b>,<leds_effect>,<leds_level>,<angry>,<disgusted>,<happy>,<neutral>,<sad>,<surprised>");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      parseCommand(command);
    }
  }
  if (leds_on) {
    executeEffect();
  }
}

void parseCommand(String command) {
  int values[14];
  int valueCount = 0;
  int startIndex = 0;
  
  // Parse comma-separated values
  for (int i = 0; i <= command.length(); i++) {
    if (i == command.length() || command.charAt(i) == ',') {
      if (valueCount < 14) {
        String valueStr = command.substring(startIndex, i);
        values[valueCount] = valueStr.toInt();
        valueCount++;
      }
      startIndex = i + 1;
    }
  }
  if (valueCount == 14) {
    Serial.println(command);
    leds_on = values[1];
    leds_brightness = values[2];
    leds_color_r = values[3];
    leds_color_g = values[4];
    leds_color_b = values[5];
    leds_effect = values[6];
    leds_level = values[7];
    Color_Brightness = leds_brightness;
    color = MainStrip.Color(leds_color_r, leds_color_g, leds_color_b);
    if (!leds_on) {
      off();
    }
  } else {
    Serial.println("Invalid message format!");
  }
}

void executeEffect() {
  switch (leds_effect) {
    case 0:
      colorStatic();
      break;
    case 1:
      colorFade();
      break;
    case 2:
      colorWipe();
      break;
    case 3:
      colorTheaterChase();
      break;
    case 4:
      Rainbow();
      break;
    case 5:
      colorStrobe();
      break;
    case 6:
      colorMovingSubstrips();
      break;
    case 7:
      colorLevel(leds_level);
      break;
    default:
      colorStatic();
      break;
  }
}

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
    delay(20);
  }
  for(int k = Color_Brightness*2; k > 0; k--) {
    fillStrips(color);
    setBrightnessStrips(k);
    showStrips();
    delay(20);
  }
}

void colorWipe() {
  setBrightnessStrips(Color_Brightness);
  if (MainStrip.getPixelColor(0) == 0) {
    for(uint16_t i=0; i<MainStrip.numPixels(); i++) {
      MainStrip.setPixelColor(i, color);
      showStrips();
      delay(20);
    }
    for(uint16_t i=0; i<MaskStripLeft.numPixels(); i++) {
      MaskStripLeft.setPixelColor(i, color);
      MaskStripRight.setPixelColor(i, color);
      showStrips();
      delay(20);
    }
  } else {
    for(uint16_t i=0; i<MainStrip.numPixels(); i++) {
      MainStrip.setPixelColor(i, black);
      showStrips();
      delay(20);
    }
    for(uint16_t i=0; i<MaskStripLeft.numPixels(); i++) {
      MaskStripLeft.setPixelColor(i, black);
      MaskStripRight.setPixelColor(i, black);
      showStrips();
      delay(20);
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
    delay(200);
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
    delay(50);
  }
}

void colorStrobe() {
  setBrightnessStrips(Color_Brightness/2);
  for(int j = 0; j < 5; j++) {
    fillStrips(color);
    showStrips();
    delay(50);
    clearStrips();
    showStrips();
    delay(50);
  }
  delay(1000);
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
    delay(20);
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