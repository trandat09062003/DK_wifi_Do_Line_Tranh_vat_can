#ifndef CONFIG_H
#define CONFIG_H

// ==================== CHÂN ĐỘNG CƠ - DRIVER L298N ====================
// Kênh A (IN1, IN2, ENA) = PHẢI | Kênh B (IN3, IN4, ENB) = TRÁI
#define RIN1 16
#define RIN2 17
#define RENA 14
#define LIN1 18
#define LIN2 19
#define LENA 27

// ==================== NÚT NHẤN & BUZZER ====================
#define BUTTON1 15  // SW1: học màu / lưu EEPROM / test PID
#define BUTTON2 22  // SW2: chạy chậm
#define BUTTON3 23  // SW3: chạy nhanh
#define BUZZER 4

// ==================== HC-SR04 & SERVO ====================
#define TRIG_PIN 5
#define ECHO_PIN 21
#define SERVO_PIN 13

#define OBSTACLE_STOP_CM 12
#define OBSTACLE_SLOW_CM 22
#define ULTRASONIC_READ_MS 80
#define SERVO_CENTER 90

// ==================== CẢM BIẾN LINE (8 kênh analog) ====================
#define LINE_SENSOR_PINS {36, 39, 34, 35, 32, 33, 25, 26}
#define LINE_SENSOR_COUNT 8

// ==================== THAM SỐ CHẠY ====================
#define EEPROM_SIZE 40
#define SERIAL_BAUD 115200
#define MOTOR_PWM_FREQ 2000
#define MOTOR_PWM_BITS 12

#define NUT1_SPEED 0
#define NUT2_SPEED 1600
#define NUT3_SPEED 1800

#define PID_KP 1.0f
#define PID_KD 15

#endif
