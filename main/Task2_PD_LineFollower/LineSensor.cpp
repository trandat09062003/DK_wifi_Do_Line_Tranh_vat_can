#include <Arduino.h>
#include <EEPROM.h>
#include "LineSensor.h"
#include "Config.h"
#include "SharedState.h"

static const uint8_t kLinePins[LINE_SENSOR_COUNT] = LINE_SENSOR_PINS;

static void ARDUINO_ISR_ATTR onLineTimer() {
  portENTER_CRITICAL_ISR(&g_timerMux);
  g_cnt = g_cnt + 1;
  g_cnt1 = g_cnt1 + 1;
  g_sensorReadPending = true;
  portEXIT_CRITICAL_ISR(&g_timerMux);
}

void lineSensorTimerInit() {
  g_timer = timerBegin(1000000);
  timerAttachInterrupt(g_timer, &onLineTimer);
  timerAlarm(g_timer, 1000000, true, 999000);
}

void lineSensorUpdate() {
  if (g_sensorReadPending) {
    portENTER_CRITICAL(&g_timerMux);
    g_sensorReadPending = false;
    portEXIT_CRITICAL(&g_timerMux);
    lineSensorRead();
  }
}

void lineSensorPid() {
  if (g_calPID != 1) {
    return;
  }

  long sum = 0;
  long avg = 0;
  long error;
  long iP;
  long iD;
  long iRet;

  if (g_isIMG == 1) {
    avg = avg + (g_sensorPID[3] * (3 * 3000));
    sum = sum + g_sensorPID[3];
    avg = avg + (g_sensorPID[4] * (4 * 3000));
    sum = sum + g_sensorPID[4];
  } else {
    for (int j = 0; j < LINE_SENSOR_COUNT; j++) {
      avg = avg + (g_sensorPID[j] * (j * 3000));
      sum = sum + g_sensorPID[j];
    }
  }

  error = ((avg / sum) - 10500);

  // Bộ lọc thông thấp (Low-pass Filter)
  static float smoothedError = 0;
  smoothedError = 0.75f * error + 0.25f * smoothedError;
  error = (long)smoothedError;

  // Thuật toán Tăng ích phi tuyến (Variable Gain)
  float norm = (float)error / 10500.0f;
  error = (long)(error * (0.4f + 0.6f * abs(norm)));

  g_posPID = error;
  iP = (long)(PID_KP * error);
  iD = PID_KD * (g_lastPos - error);
  iRet = (iP - iD);
  if (iRet < -12000) {
    iRet = 0;
  }
  if (iRet > 12000) {
    iRet = 0;
  }

  g_servoPwm = iRet / 3;
  g_lastPos = error;
  g_calPID = 0;
}

void lineSensorRead() {
  unsigned char temp = 0;
  long sum = 0;
  
  for (int j = 0; j < LINE_SENSOR_COUNT; j++) {
    g_sensorValue[j] = analogRead(kLinePins[j]);
    
    // Thuật toán tự phục hồi lỗi cảm biến biên (Smart Calib Fallback)
    long calDifference = (long)g_whiteValue[j] - (long)g_blackValue[j];
    if (calDifference < 150) {
      // Nếu cảm biến biên lỗi Calib, dùng chế độ nhị phân dự phòng
      g_sensorPID[j] = (g_sensorValue[j] > g_compareValue[j]) ? 1000 : 0;
    } else {
      // Ngược lại dùng nội suy tuyến tính chuẩn
      long val = g_sensorValue[j] - g_blackValue[j];
      if (val < 0) val = 0;
      g_sensorPID[j] = (val * 1000) / calDifference;
      if (g_sensorPID[j] > 1000) g_sensorPID[j] = 1000;
    }
    
    sum = sum + g_sensorPID[j];
    
    if (g_sensorValue[j] < g_compareValue[j]) {
      temp = temp | (0x80 >> j);
    }
  }

  if (sum > 500) {
    g_pidFre = g_pidFre + 1;
    g_sensor = temp;
  }

  if (g_pidFre == 2) {
    g_pidFre = 0;
    g_calPID = 1;
  }
}

void lineSensorUpdateCalib() {
  // Không sử dụng hiệu chuẩn động nữa
}

void lineSensorReadEeprom() {
  // Gán cứng giá trị tĩnh tối ưu cho 8 mắt cảm biến hồng ngoại
  for (int i = 0; i < LINE_SENSOR_COUNT; i++) {
    g_blackValue[i] = 700;    // Đen (vạch đen hấp thụ IR tốt, điện áp phản hồi thấp)
    g_whiteValue[i] = 3200;   // Trắng (nền trắng phản xạ IR tốt, điện áp phản hồi cao)
    g_compareValue[i] = 1950;  // Ngưỡng so sánh nhị phân ở giữa
  }
}

unsigned char lineSensorMask(unsigned char mask) {
  return (g_sensor & mask);
}

void lineSensorSaveCalib() {
  // Không lưu hiệu chuẩn vào EEPROM nữa
}
