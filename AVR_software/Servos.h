#include "TestConfig.h"
#ifndef SIMULATE_HARDWARE
#include <Servo.h>
#else
#include "HardwareSim.h"
#include "PCSandbox.h"
#endif
#define NUM_SERVOS 2
#define NUM_EMOTIONS 6
#define SERVO_0_PIN 6
#define SERVO_1_PIN 11

Servo eyebrow_left;
Servo eyebrow_right;
Servo servos[] = {eyebrow_left, eyebrow_right};

int servo_calibration_matrix[NUM_EMOTIONS][NUM_SERVOS] = {
  {90, 90}, // angry
  {90, 90}, // disgusted
  {90, 90}, // happy
  {90, 90}, // neutral
  {90, 90}, // sad
  {90, 90}  // surprised
};

struct ServosTaskInput
{
  int animatronics_on;
  int emotion_angry;
  int emotion_disgusted;
  int emotion_happy;
  int emotion_neutral;
  int emotion_sad;
  int emotion_surprised;
};

void setupServos() {
  servos[0].attach(SERVO_0_PIN);
  servos[1].attach(SERVO_1_PIN);
}

void writepos(int eyebrow_left_pos, int eyebrow_right_pos, int ear_pan_left_pos, int ear_tilt_left_pos,
              int ear_pan_right_pos, int ear_tilt_right_pos, int mouth_left_pos, int mouth_right_pos) {
  int newServoPositions[NUM_SERVOS] = { eyebrow_left_pos, eyebrow_right_pos };
  for (int i = 0; i < NUM_SERVOS; i++) {
    if (newServoPositions[i] < 0 || newServoPositions[i] > 180) return;
    servos[i].write(newServoPositions[i]);
  }
}
