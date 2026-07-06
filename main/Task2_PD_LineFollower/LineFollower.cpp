#include <Arduino.h>
#include "LineFollower.h"
#include "Config.h"
#include "SharedState.h"
#include "MotorControl.h"
#include "LineSensor.h"

void beep(int timer) {
  digitalWrite(BUZZER, 1);
  delay(timer);
  digitalWrite(BUZZER, 0);
}

void runForwardLine(int tocdo) {
  // Bộ giám sát mất vạch (Lost-Line Safety Stop) với thời gian chờ thích ứng
  static unsigned long noLineStartMs = 0;
  if (g_sensor != 0b00000000) {
    noLineStartMs = 0;
  } else {
    if (noLineStartMs == 0) {
      noLineStartMs = millis();
    } else {
      // Nếu có hướng tự cứu -> chờ tối đa 3000ms để xoay bánh tìm line.
      // Nếu không có hướng tự cứu -> dừng ngay lập tức sau 1000ms.
      unsigned long maxWaitMs = (g_rememberLine != 0) ? 3000 : 1000;
      
      if (millis() - noLineStartMs > maxWaitMs) {
        // Mất vạch quá thời gian chờ thích ứng -> Dừng xe và báo còi cảnh báo
        g_trangThai = 100;
        g_start = 0;
        speedRun(0, 0);
        beep(500); // Kêu còi bíp dài cảnh báo
        noLineStartMs = 0;
        return;
      }
    }
  }

  lineSensorPid();

  switch (g_pattern) {
    case 10:
      if (g_cnt >= 50) {
        g_pattern = 11;
        g_cnt = 0;
        break;
      }
      handleAndSpeed(g_servoPwm, tocdo);
      break;

    case 11:
      if (lineSensorMask(0x3f) == 0x3f) {
        if (g_trangThai == 14) {
          g_trangThai = 100;
          g_pattern = 100;
          break;
        }
      }
      // Tự động cập nhật nhớ hướng lệch vạch dựa trên sai số PID thực tế
      if (g_sensor != 0b00000000) {
        if (g_posPID < -3000) {
          g_rememberLine = -1; // Line lệch trái nhiều (S0, S1)
        } else if (g_posPID > 3000) {
          g_rememberLine = 1;  // Line lệch phải nhiều (S6, S7)
        } else {
          g_rememberLine = 0;  // Đi thẳng ở giữa vạch, không nhớ hướng
        }
      }

      if (g_sensor == 0b00000000) {
        if (g_rememberLine != 0) {
          if (g_rememberLine == 1) {
            speedRun(1400, -1000); // Xoay vi sai chủ động tìm vạch
            g_pattern = 12;
          } else if (g_rememberLine == -1) {
            speedRun(-1000, 1400); // Xoay vi sai chủ động tìm vạch
            g_pattern = 12;
          }
        } else {
          speedRun(1200, 1200); // Giảm tốc độ bò tìm vạch thẳng để hãm phanh
        }
        break;
      }

      switch (g_sensor) {
        case 0b00011000: case 0b00111100: case 0b00011100: case 0b00001000:
        case 0b00001100: case 0b00011110: case 0b00000100: case 0b00001110:
        case 0b00000110: case 0b00001111: case 0b00011111: case 0b00111111:
        case 0b01111111: case 0b01111100: case 0b01111110: case 0b00111110:
        case 0b00000010: case 0b00000111: case 0b00000011: case 0b00000001:
        case 0b00000000: case 0b00010000: case 0b00111000: case 0b00110000:
        case 0b01111000: case 0b00100000: case 0b01110000: case 0b01100000:
        case 0b11110000: case 0b11111000: case 0b11111100: case 0b11111110:
        case 0b01000000: case 0b11100000: case 0b11000000: case 0b10000000:
          handleAndSpeed(g_servoPwm, tocdo);
          break;

        case 0b10000001: case 0b11000001: case 0b10000011: case 0b11100001:
        case 0b10000111: case 0b11000011: case 0b11100011: case 0b11000111:
        case 0b11100111:
          if (g_trangThai == 13) {
            g_rememberLine = 1;
            g_cnt = 0;
            handleAndSpeed(1000, tocdo);
          } else {
            handleAndSpeed(g_servoPwm, tocdo);
          }
          break;

        default:
          handleAndSpeed(g_servoPwm, tocdo);
          break;
      }
      break;

    case 12:
      if (g_rememberLine == 1) {
        speedRun(1400, -1000);
        g_pattern = 21;
      } else if (g_rememberLine == -1) {
        speedRun(-1000, 1400);
        g_pattern = 31;
      } else {
        g_pattern = 11;
      }
      break;

    case 21:
      speedRun(1400, -1000);
      if (lineSensorMask(0x07) != 0) {
        speedRun(1200, 400); // Tiến lại vạch chậm rãi
        g_pattern = 22;
      }
      break;

    case 22:
      speedRun(1200, 400);
      if (lineSensorMask(0xfc) != 0) {
        g_pattern = 11;
      }
      break;

    case 31:
      speedRun(-1000, 1400);
      if (lineSensorMask(0xe0) != 0) {
        speedRun(400, 1200); // Tiến lại vạch chậm rãi
        g_pattern = 32;
      }
      break;

    case 32:
      speedRun(400, 1200);
      if (lineSensorMask(0x3f) != 0) {
        g_pattern = 11;
      }
      break;

    case 100:
      speedRun(0, 0);
      break;

    default:
      break;
  }
}
