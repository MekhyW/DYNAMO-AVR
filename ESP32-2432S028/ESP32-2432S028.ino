#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite eyeSprite = TFT_eSprite(&tft);

float lx = 0, ly = 0;
float rx = 0, ry = 0;
float eyeclose = 0.2;

String logBuffer;
const int LOG_LINES = 6;  
String logLines[LOG_LINES];
bool logsChanged = true;


void pushLog(const String &msg) {
  for (int i = LOG_LINES - 1; i > 0; i--) { logLines[i] = logLines[i - 1]; }
  logLines[0] = msg;
  logsChanged = true;
}

void drawEyes() {
  eyeSprite.fillSprite(TFT_BLACK);
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
  eyeSprite.fillRoundRect(lx0, ly0 + eyelidDrop, eyeW, eyeH - eyelidDrop, 20, TFT_WHITE);
  int lcx = lx0 + eyeW/2 + lx * moveRange;
  int lcy = ly0 + eyeH/2 + ly * moveRange;
  eyeSprite.fillCircle(lcx, lcy, pupilR, TFT_BLACK);
  eyeSprite.fillRoundRect(rx0, ry0 + eyelidDrop, eyeW, eyeH - eyelidDrop, 20, TFT_WHITE);
  int rcx = rx0 + eyeW/2 + rx * moveRange;
  int rcy = ry0 + eyeH/2 + ry * moveRange;
  eyeSprite.fillCircle(rcx, rcy, pupilR, TFT_BLACK);
  eyeSprite.pushSprite(0, 0);
}

void drawLogs() {
  if (!logsChanged) return;
  int y0 = 170;
  tft.fillRect(0, 160, 320, 80, 0x2104);
  tft.setCursor(5, y0);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  for (int i = LOG_LINES - 1; i >= 0; i--) { tft.println(logLines[i]); }
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
          if (idx1 > 0 && idx5 > idx4) {
            lx = 2*incoming.substring(idx1+1, idx2).toFloat();
            ly = -2*incoming.substring(idx2+1, idx3).toFloat();
            rx = 2*incoming.substring(idx3+1, idx4).toFloat();
            ry = -2*incoming.substring(idx4+1, idx5).toFloat();
            eyeclose = incoming.substring(idx5+1).toFloat();
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
