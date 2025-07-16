#include <Servo.h>

Servo servo_1;
Servo servo_2;

int pos = 0;

void setup() {
    Serial.begin(9600);
    servo_1.attach(2);
    servo_2.attach(3);
    Serial.println("Attached");
}

void writepos(int pos) {
    servo_1.write(pos);
    servo_2.write(pos);
}

void loop() {
  Serial.println("Sweep...");
  for (pos = 0; pos <= 90; pos += 1) {
    writepos(pos);
    delay(15);
  }
  delay(1000);
  for (pos = 90; pos <= 180; pos += 1) {
    writepos(pos);
    delay(15);
  }
  for (pos = 180; pos >= 90; pos -= 1) {
    writepos(pos);
    delay(15); 
  }
  delay(1000);
  for (pos = 90; pos >= 0; pos -= 1) {
    writepos(pos);
    delay(15); 
  }
}
