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

// Effect state variables for continuous animation
int effect_step = 0;
long effect_timer = 0;
int fade_direction = 1;
int wipe_direction = 1;
int theater_step = 0;
long rainbow_hue = 0;
int strobe_count = 0;
int moving_position = 0;

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
      colorFadeContinuous();
      break;
    case 2:
      colorWipeContinuous();
      break;
    case 3:
      colorTheaterChaseContinuous();
      break;
    case 4:
      RainbowContinuous();
      break;
    case 5:
      colorStrobeContinuous();
      break;
    case 6:
      colorMovingSubstripsContinuous();
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

void colorFadeContinuous() {
  if (millis() - effect_timer > 20) {
    fillStrips(color);
    setBrightnessStrips(effect_step);
    showStrips();
    if (fade_direction == 1) {
      effect_step++;
      if (effect_step >= Color_Brightness*2) {
        fade_direction = -1;
      }
    } else {
      effect_step--;
      if (effect_step <= 0) {
        fade_direction = 1;
      }
    }
    effect_timer = millis();
  }
}

void colorWipeContinuous() {
  if (millis() - effect_timer > 50) {
    setBrightnessStrips(Color_Brightness);
    if (effect_step < MainStrip.numPixels()) {
      if (wipe_direction == 1) {
        MainStrip.setPixelColor(effect_step, color);
      } else {
        MainStrip.setPixelColor(effect_step, black);
      }
    } else if (effect_step < MainStrip.numPixels() + MaskStripLeft.numPixels()) {
      int maskIndex = effect_step - MainStrip.numPixels();
      if (wipe_direction == 1) {
        MaskStripLeft.setPixelColor(maskIndex, color);
        MaskStripRight.setPixelColor(maskIndex, color);
      } else {
        MaskStripLeft.setPixelColor(maskIndex, black);
        MaskStripRight.setPixelColor(maskIndex, black);
      }
    }
    showStrips();
    effect_step++;
    if (effect_step >= MainStrip.numPixels() + MaskStripLeft.numPixels()) {
      effect_step = 0;
      wipe_direction = -wipe_direction;
    }
    effect_timer = millis();
  }
}

void colorTheaterChaseContinuous() {
  if (millis() - effect_timer > 200) {
    setBrightnessStrips(Color_Brightness*2);
    clearStrips();
    for(int c=theater_step; c<MainStrip.numPixels(); c += 3) {
      MainStrip.setPixelColor(c, color);
    }
    for(int c=theater_step; c<MaskStripLeft.numPixels(); c += 3) {
      MaskStripLeft.setPixelColor(c, color);
      MaskStripRight.setPixelColor(c, color);
    }
    showStrips();
    theater_step = (theater_step + 1) % 3;
    effect_timer = millis();
  }
}

void RainbowContinuous() {
  if (millis() - effect_timer > 50) {
    setBrightnessStrips(Color_Brightness*2);
    for(int i=0; i<MainStrip.numPixels(); i++) {
      int pixelHue = rainbow_hue + (i * 65536L / MainStrip.numPixels());
      MainStrip.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
    }
    for(int i=0; i<MaskStripLeft.numPixels(); i++) {
      int pixelHue = rainbow_hue + (i * 65536L / MaskStripLeft.numPixels());
      MaskStripLeft.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
      MaskStripRight.setPixelColor(i, MainStrip.gamma32(MainStrip.ColorHSV(pixelHue)));
    }
    showStrips();
    rainbow_hue += 512;
    if (rainbow_hue >= 65536) {
      rainbow_hue = 0;
    }
    effect_timer = millis();
  }
}

void colorStrobeContinuous() {
  if (millis() - effect_timer > 100) {
    setBrightnessStrips(Color_Brightness/2);
    if (effect_step % 2 == 0) {
      fillStrips(color);
    } else {
      clearStrips();
    }
    showStrips();
    effect_step++;
    if (effect_step >= 10) {
      effect_step = 0;
    }
    effect_timer = millis();
  }
}

void colorMovingSubstripsContinuous() {
  if (millis() - effect_timer > 50) {
    uint32_t color_a = color;
    uint32_t color_b = black;
    setBrightnessStrips(Color_Brightness*2);
    int numPixels = MainStrip.numPixels();
    int substrip_size = numPixels/10;
    clearStrips();
    for(int j = 0; j < numPixels; j += substrip_size*2) {
      int startPixel = (moving_position + j) % numPixels;
      MainStrip.fill(color_a, startPixel, substrip_size);
      int endPixel = (startPixel + substrip_size) % numPixels;
      MainStrip.fill(color_b, endPixel, substrip_size);
    }
    MainStrip.show();
    moving_position = (moving_position + 1) % numPixels;
    effect_timer = millis();
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