#include <Arduino.h>
#include "MotorControl.h"
#include "Config.h"
#include "SharedState.h"

void motorInit() {
  pinMode(LIN1, OUTPUT);
  pinMode(LIN2, OUTPUT);
  pinMode(RIN1, OUTPUT);
  pinMode(RIN2, OUTPUT);
  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, LOW);
  digitalWrite(LIN1, LOW);
  digitalWrite(LIN2, LOW);
  ledcAttach(LENA, MOTOR_PWM_FREQ, MOTOR_PWM_BITS);
  ledcAttach(RENA, MOTOR_PWM_FREQ, MOTOR_PWM_BITS);
}

void speedRun(int speedDC_left, int speedDC_right) {
  if (speedDC_left < -4095) speedDC_left = -4095;
  if (speedDC_right < -4095) speedDC_right = -4095;
  if (speedDC_left > 4095) speedDC_left = 4095;
  if (speedDC_right > 4095) speedDC_right = 4095;

  if (speedDC_left > 0) {
    digitalWrite(LIN1, HIGH);
    digitalWrite(LIN2, LOW);
    ledcWrite(LENA, speedDC_left);
  } else if (speedDC_left < 0) {
    digitalWrite(LIN1, LOW);
    digitalWrite(LIN2, HIGH);
    ledcWrite(LENA, -speedDC_left);
  } else {
    digitalWrite(LIN1, LOW);
    digitalWrite(LIN2, LOW);
    ledcWrite(LENA, 0);
  }

  if (speedDC_right > 0) {
    digitalWrite(RIN1, HIGH);
    digitalWrite(RIN2, LOW);
    ledcWrite(RENA, speedDC_right);
  } else if (speedDC_right < 0) {
    digitalWrite(RIN1, LOW);
    digitalWrite(RIN2, HIGH);
    ledcWrite(RENA, -speedDC_right);
  } else {
    digitalWrite(RIN1, LOW);
    digitalWrite(RIN2, LOW);
    ledcWrite(RENA, 0);
  }
}

void handleAndSpeed(int angle, int speed1) {
  // Thuật toán giảm tốc ôm cua tự động (Dynamic Speed Control)
  float normError = abs(g_posPID) / 10500.0f;
  if (normError > 1.0f) normError = 1.0f;

  int minCurveSpeed = 1000;
  if (speed1 > minCurveSpeed) {
    speed1 = speed1 - (int)(normError * (speed1 - minCurveSpeed));
  }

  if ((speed1 + angle) > 4095) {
    speed1 = 4095 - angle;
  }
  if ((speed1 - angle) > 4095) {
    speed1 = 4095 + angle;
  }
  speedRun(speed1 + angle, speed1 - angle);
}
