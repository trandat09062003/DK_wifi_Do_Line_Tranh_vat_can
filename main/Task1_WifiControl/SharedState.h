#ifndef SHARED_STATE_H
#define SHARED_STATE_H

#include <Arduino.h>

extern hw_timer_t* g_timer;
extern portMUX_TYPE g_timerMux;

extern volatile long g_lastPos;
extern volatile unsigned char g_isCalib;
extern int g_servoPwm;
extern volatile int g_calPID;
extern volatile unsigned char g_sensor;

extern unsigned int g_sensorValue[8];
extern unsigned int g_sensorPID[8];
extern unsigned int g_blackValue[8];
extern unsigned int g_whiteValue[8];
extern unsigned int g_compareValue[8];

extern int g_speedRunForward;
extern int g_isIMG;
extern volatile long g_posPID;
extern volatile int g_cnt;
extern volatile int g_cnt1;
extern unsigned char g_pattern;
extern unsigned char g_start;
extern unsigned char g_trangThai;
extern int g_rememberLine;
extern volatile int g_pidFre;

extern int g_distanceCm;
extern bool g_obstacleStop;
extern bool g_obstacleSlow;
extern unsigned long g_lastUltrasonicMs;
extern bool g_obstacleBeeped;
extern volatile bool g_sensorReadPending;

// Biến tinh chỉnh động qua Web
extern float g_pidKp;
extern int g_pidKd;
extern int g_speedMode2;
extern int g_speedMode3;

// Các biến điều khiển thủ công qua Wi-Fi
extern int g_manualCommand; // 0: Dừng, 1: Tiến, 2: Lùi, 3: Trái, 4: Phải
extern int g_manualSpeed;   // Tốc độ xe khi chạy thủ công

#endif
