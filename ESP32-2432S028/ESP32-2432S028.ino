#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#define TFT_PINK      0xFE19
#define TFT_PURPLE    0x780F
#define TFT_LIGHTBLUE 0x7DFF

struct EyeColors {
  uint16_t sclera;
  uint16_t pupil;
};

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite eyeSprite = TFT_eSprite(&tft);

float lx = 0, ly = 0;
float rx = 0, ry = 0;
float eyeclose = 0.2;
float expr = 3;

String logBuffer;
const int LOG_LINES = 6;  
String logLines[LOG_LINES];
bool logsChanged = true;

EyeColors getEyeColors(int expression) {
  switch (expression) {
    case 0:  return {TFT_WHITE, TFT_RED};       // 0 = white eyes, red pupils
    case 1:  return {TFT_WHITE, TFT_GREEN};     // 1 = white eyes, green pupils
    case 2:  return {TFT_YELLOW, TFT_BLACK};    // 2 = yellow eyes, black pupils
    case 3:  return {TFT_WHITE, TFT_BLACK};     // 3 = white eyes, black pupils
    case 4:  return {TFT_WHITE, TFT_BLUE};      // 4 = white eyes, blue pupils
    case 5:  return {TFT_WHITE, TFT_BLACK};     // 5 = white eyes, black pupils
    case 6:  return {TFT_BLACK, TFT_WHITE};     // 6 = black eyes, white pupils
    case 7:  return {TFT_PINK, TFT_RED};        // 7 = pink eyes, red pupils
    case 8:  return {TFT_PURPLE, TFT_YELLOW};   // 8 = purple eyes, yellow pupils
    case 9:  return {TFT_BLACK, TFT_RED};       // 9 = black eyes, red pupils
    case 10: return {TFT_BLACK, TFT_WHITE};     // 10 = black eyes, white pupils
    case 11: return {TFT_BLACK, TFT_LIGHTBLUE}; // 11 = black eyes, light blue pupils
    case 12: return {TFT_WHITE, TFT_PINK};      // 12 = white eyes, pink pupils
    default: return {TFT_WHITE, TFT_BLACK};     // Default
  }
}

void pushLog(const String &msg) {
  for (int i = LOG_LINES - 1; i > 0; i--) { logLines[i] = logLines[i - 1]; }
  logLines[0] = msg;
  logsChanged = true;
}

void drawEyes() {
  eyeSprite.fillSprite(TFT_BLACK);
  EyeColors colors = getEyeColors(expr);
  int eyeW = 130;
  int eyeH = 120;
  int padding = 15;
  int lx0 = padding;
  int ly0 = padding;
  int rx0 = 160 + padding;
  int ry0 = padding;
  int pupilR = 20;
  int moveRange = 25;
  int eyelidDrop = (int)((eyeclose - 0.2) * eyeH);
  eyeSprite.fillRoundRect(lx0, ly0 + eyelidDrop, eyeW, eyeH - eyelidDrop, 20, colors.sclera);
  int lcx = lx0 + eyeW/2 + lx * moveRange;
  int lcy = ly0 + eyeH/2 + ly * moveRange;
  eyeSprite.fillCircle(lcx, lcy, pupilR, colors.pupil);
  eyeSprite.fillRoundRect(rx0, ry0 + eyelidDrop, eyeW, eyeH - eyelidDrop, 20, colors.sclera);
  int rcx = rx0 + eyeW/2 + rx * moveRange;
  int rcy = ry0 + eyeH/2 + ry * moveRange;
  eyeSprite.fillCircle(rcx, rcy, pupilR, colors.pupil);
  eyeSprite.pushSprite(0, 0);
}

void drawLogs() {
  if (!logsChanged) return;
  int y0 = 170;
  tft.fillRect(0, 160, 320, 80, 0x2104);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  for (int i = LOG_LINES - 1; i >= 0; i--) { 
    tft.drawString(logLines[i], 160, y0);
    y0 += tft.fontHeight();
  }
  tft.setTextDatum(TL_DATUM);
  logsChanged = false;
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  eyeSprite.createSprite(320, 160);
  pushLog("Boot OK");
}

void loop() {
  static String incoming = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (incoming.length() > 0) {
        if (incoming.startsWith("E,")) {
          int idx1 = incoming.indexOf(',');
          int idx2 = incoming.indexOf(',', idx1+1);
          int idx3 = incoming.indexOf(',', idx2+1);
          int idx4 = incoming.indexOf(',', idx3+1);
          int idx5 = incoming.indexOf(',', idx4+1);
          int idx6 = incoming.indexOf(',', idx5+1);
          if (idx1 > 0 && idx6 > idx5) {
            lx = 2*incoming.substring(idx1+1, idx2).toFloat();
            ly = -2*incoming.substring(idx2+1, idx3).toFloat();
            rx = 2*incoming.substring(idx3+1, idx4).toFloat();
            ry = -2*incoming.substring(idx4+1, idx5).toFloat();
            eyeclose = incoming.substring(idx5+1, idx6).toFloat();
            expr = incoming.substring(idx6+1).toInt();
          }
        } else { pushLog(incoming); }
      }
      incoming = "";
    } else { incoming += c; }
  }
  drawEyes();
  drawLogs();
  delay(15);
}
