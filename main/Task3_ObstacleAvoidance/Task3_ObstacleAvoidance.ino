#include <EEPROM.h>
#include <Arduino.h>
#include "Config.h"
#include "SharedState.h"
#include "MotorControl.h"
#include "Ultrasonic.h"
#include "ServoBracket.h"

static void handleWaitForStart();
static void logSystemState();

void setup() {
  motorInit();
  pinMode(BUTTON2, INPUT_PULLUP); // Chỉ dùng SW2 làm phím Start/Stop
  pinMode(BUZZER, OUTPUT);

  speedRun(0, 0);
  g_trangThai = 9;
  g_start = 0;

  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("\n============================================="));
  Serial.println(F("       TASK 3: PURE OBSTACLE AVOIDANCE        "));
  Serial.println(F("============================================="));
  Serial.println(F("HUONG DAN VAN HANH:"));
  Serial.println(F("1. Dat xe tren mat dat vung thoang."));
  Serial.println(F("2. Nhan SW2 (22) -> Xe bat dau chay tranh vat can (Toc do 1600)"));
  Serial.println(F("3. Nhan SW2 (22) lan nua khi dang chay -> Dung xe khan cap."));
  Serial.println(F("============================================="));

  ultrasonicInit();
  servoBracketInit();
  beep(50);
}

void loop() {
  // Cập nhật cảm biến khoảng cách liên tục
  ultrasonicUpdate();

  // Khi đang chạy: Tiến thẳng tự do (loăng quăng)
  if (g_start == 1 && !g_obstacleStop) {
    speedRun(g_speedRunForward, g_speedRunForward);
  }

  // Cơ chế bấm nút SW2 dừng xe khẩn cấp khi đang chạy
  if (g_start == 1 && digitalRead(BUTTON2) == 0) {
    Serial.println(F("[BUTTON] SW2 Pressed while running -> STOP Robot"));
    g_start = 0;
    speedRun(0, 0);
    beep(200);
    while (digitalRead(BUTTON2) == 0) {} // Chờ nhả tay
    delay(100);
  }

  logSystemState();
  handleWaitForStart();
}

static void handleWaitForStart() {
  while (g_start == 0) {
    // Vẫn đo khoảng cách trong lúc chờ
    ultrasonicUpdate();
    logSystemState();

    if (digitalRead(BUTTON2) == 0) {
      Serial.println(F("[BUTTON] SW2 Pressed -> Start Obstacle Avoidance"));
      g_start = 1;
      g_speedRunForward = NUT2_SPEED;
      beep(100);
      while (digitalRead(BUTTON2) == 0) {} // Chờ nhả tay
      delay(100);
      break;
    }
    delay(20);
  }
}

static void logSystemState() {
  static unsigned long lastLogMs = 0;
  if (millis() - lastLogMs < 500) {
    return;
  }
  lastLogMs = millis();

  if (g_start == 1) {
    Serial.print("RUNNING (Wander) | Speed: ");
    Serial.print(g_speedRunForward);
    Serial.print(" | Dist: ");
    if (g_distanceCm >= 999) {
      Serial.print("MAX");
    } else {
      Serial.print(g_distanceCm);
      Serial.print("cm");
    }
    Serial.println();
  } else {
    Serial.println("WAITING | Press SW2/SW3 to Start Wander Avoidance...");
  }
}
