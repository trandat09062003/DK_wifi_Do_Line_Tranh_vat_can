#include <EEPROM.h>
#include <Arduino.h>
#include "Config.h"
#include "SharedState.h"
#include "MotorControl.h"
#include "LineSensor.h"
#include "LineFollower.h"

static void handleWaitForStart();
static void handleRunState();
static void logSystemState();

void setup() {
  motorInit();
  pinMode(BUTTON2, INPUT_PULLUP); // Chỉ giữ nút nhấn SW2 làm nút Start/Stop
  pinMode(BUZZER, OUTPUT);

  speedRun(0, 0);
  g_trangThai = 9;
  g_pattern = 11;
  g_start = 0;

  EEPROM.begin(EEPROM_SIZE);
  lineSensorReadEeprom();
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("\n============================================="));
  Serial.println(F("       TASK 2: PD LINE FOLLOWER SKETCH        "));
  Serial.println(F("============================================="));
  Serial.println(F("HUONG DAN VAN HANH:"));
  Serial.println(F("1. Dat xe vao vach xuất phát."));
  Serial.println(F("2. Nhan SW2 (22) -> Bắt đầu chạy Dò Line (Toc do 1600)"));
  Serial.println(F("3. Nhan SW2 (22) lan nua khi dang chay -> Dung xe khẩn cấp."));
  Serial.println(F("============================================="));

  lineSensorTimerInit();
  g_isCalib = 0;
  beep(50);
}

void loop() {
  lineSensorUpdate();
  lineSensorPid();
  logSystemState();

  // Cơ chế bấm nút SW2 để tắt xe khẩn cấp khi đang chạy
  if (g_start == 1 && digitalRead(BUTTON2) == 0) {
    Serial.println(F("[BUTTON] SW2 Pressed while running -> STOP ROBOT"));
    g_start = 0;
    g_trangThai = 9;
    speedRun(0, 0);
    beep(200);
    while (digitalRead(BUTTON2) == 0) {} // Chờ nhả tay
    delay(100);
  }

  handleWaitForStart();
  handleRunState();
}

static void handleWaitForStart() {
  while (g_start == 0) {
    lineSensorUpdate();
    lineSensorPid();
    logSystemState();

    if (digitalRead(BUTTON2) == 0) {
      Serial.println(F("[BUTTON] SW2 Pressed -> Start Running"));
      g_start = 1;
      g_trangThai = 10;
      g_cnt = 0;
      g_cnt1 = 0;
      g_speedRunForward = NUT2_SPEED; // Sử dụng tốc độ Mode 2 mặc định (1600)
      beep(100);
      while (digitalRead(BUTTON2) == 0) {} // Chờ nhả tay
      delay(100);
      break;
    }
  }
}

static void handleRunState() {
  if (g_start == 0) {
    return;
  }

  switch (g_trangThai) {
    case 9:
      speedRun(0, 0);
      break;

    case 91:
      handleAndSpeed(g_servoPwm, 0);
      break;

    case 10:
      if (g_cnt1 < 800) {
        runForwardLine(g_speedRunForward + 400);
      } else {
        g_isIMG = 0;
        g_trangThai = 11;
      }
      break;

    case 11:
      runForwardLine(g_speedRunForward);
      break;

    case 15:
      runForwardLine(g_speedRunForward);
      break;

    case 100:
      speedRun(0, 0);
      break;

    default:
      break;
  }
}

static void logSystemState() {
  static unsigned long lastLogMs = 0;
  if (millis() - lastLogMs < 250) {
    return;
  }
  lastLogMs = millis();

  if (g_start == 1) {
    Serial.print("RUNNING | Mode: ");
    if (g_speedRunForward == NUT2_SPEED) {
      Serial.print("2 (Slow)");
    } else if (g_speedRunForward == NUT3_SPEED) {
      Serial.print("3 (Fast)");
    } else if (g_speedRunForward == NUT1_SPEED) {
      Serial.print("1 (Test)");
    } else {
      Serial.print("Unknown");
    }
    Serial.print(" | State: ");
    Serial.print(g_trangThai);
    Serial.print(" | Pattern: ");
    Serial.print(g_pattern);
    Serial.print(" | Sensor: 0b");
    
    for (int i = 7; i >= 0; i--) {
      Serial.print((g_sensor >> i) & 0x01);
    }

    Serial.print(" | PID Err: ");
    Serial.print(g_posPID);
    Serial.print(" | ServoPWM: ");
    Serial.println(g_servoPwm);
  } else {
    Serial.print("WAITING | State: ");
    Serial.print(g_trangThai);
    Serial.print(" | CalibMode: ");
    Serial.println(g_isCalib);
  }
}
