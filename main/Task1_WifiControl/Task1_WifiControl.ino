#include <EEPROM.h>
#include <Arduino.h>
#include "Config.h"
#include "SharedState.h"
#include "MotorControl.h"
#include "LineSensor.h"
#include "Ultrasonic.h"
#include "ServoBracket.h"
#include "LineFollower.h"
#include "WifiControl.h"

static void logSystemState();

void setup() {
  motorInit();
  pinMode(BUTTON2, INPUT_PULLUP); // Chỉ dùng SW2
  pinMode(BUZZER, OUTPUT);

  speedRun(0, 0);
  g_manualCommand = 0; // Bắt đầu ở trạng thái dừng

  EEPROM.begin(EEPROM_SIZE);
  lineSensorReadEeprom();
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("\n============================================="));
  Serial.println(F("       TASK 1: WIFI MANUAL RC CONTROL        "));
  Serial.println(F("============================================="));
  Serial.println(F("HUONG DAN VAN HANH:"));
  Serial.println(F("1. Xe tu ket noi vao Wi-Fi 'VIETSET_TECH' (pass: vs68686868)."));
  Serial.println(F("   Neu khong thay, tu dong phat AP: ESP32_Robot_Line (pass: 12345678)."));
  Serial.println(F("2. Mo trinh duyet truy cap IP: http://192.168.1.150 (hoac IP duoc cap)."));
  Serial.println(F("3. Dung D-pad tren web: Tien, Lui, Trai, Phai, Dung."));
  Serial.println(F("============================================="));

  lineSensorTimerInit();
  g_isCalib = 0;
  ultrasonicInit();
  servoBracketInit();
  
  // Khoi tao Wifi va Web Server
  wifiControlInit();
  
  beep(50);
}

void loop() {
  // Cap nhat WiFi Client va API
  wifiControlUpdate();

  // Vẫn đọc cảm biến & siêu âm để hiển thị trên telemetry của dashboard
  lineSensorUpdate();
  ultrasonicUpdate();

  // Thực thi lệnh điều khiển thủ công nhận được từ Web
  switch (g_manualCommand) {
    case 1: // Tiến thẳng
      speedRun(g_manualSpeed, g_manualSpeed);
      break;

    case 2: // Lùi thẳng
      speedRun(-g_manualSpeed, -g_manualSpeed);
      break;

    case 3: { // Xoay Trái tại chỗ (tăng mạnh lực kéo)
      int turnSpeed = g_manualSpeed + 600;
      if (turnSpeed > 4095) turnSpeed = 4095;
      if (turnSpeed < 2200) turnSpeed = 2200;
      speedRun(-turnSpeed, turnSpeed);
      break;
    }

    case 4: { // Xoay Phải tại chỗ (tăng mạnh lực kéo)
      int turnSpeed = g_manualSpeed + 600;
      if (turnSpeed > 4095) turnSpeed = 4095;
      if (turnSpeed < 2200) turnSpeed = 2200;
      speedRun(turnSpeed, -turnSpeed);
      break;
    }

    case 0: // Dừng hẳn
    default:
      speedRun(0, 0);
      break;
  }

  logSystemState();
}

static void logSystemState() {
  static unsigned long lastLogMs = 0;
  if (millis() - lastLogMs < 500) {
    return;
  }
  lastLogMs = millis();

  Serial.print("[RC] Command: ");
  switch(g_manualCommand) {
    case 1: Serial.print("TIEN"); break;
    case 2: Serial.print("LUI"); break;
    case 3: Serial.print("TRAI"); break;
    case 4: Serial.print("PHAI"); break;
    default: Serial.print("DUNG"); break;
  }
  Serial.print(" | Speed: ");
  Serial.print(g_manualSpeed);
  Serial.print(" | Dist: ");
  if (g_distanceCm >= 999) {
    Serial.print("MAX");
  } else {
    Serial.print(g_distanceCm);
    Serial.print("cm");
  }
  Serial.print(" | Line S0-S7: 0b");
  for (int i = 7; i >= 0; i--) {
    Serial.print((g_sensor >> i) & 0x01);
  }
  Serial.println();
}
