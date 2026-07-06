#include <Arduino.h>

#define SERVO_PIN 13
#define MOTOR_PWM_FREQ 50
#define MOTOR_PWM_BITS 14

void servoWrite(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  
  // Ánh xạ góc từ 0-180 độ sang duty cycle 14-bit (1638 đến 7864)
  // 1638 tương ứng khoảng 1ms (góc 0)
  // 7864 tương ứng khoảng 2.4ms (góc 180)
  int duty = map(angle, 0, 180, 1638, 7864);
  ledcWrite(SERVO_PIN, duty);
  
  Serial.printf("Servo Angle: %d -> PWM Duty: %d\n", angle, duty);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=============================================");
  Serial.println("       ESP32 SG90 SERVO SWEEP TEST SKETCH    ");
  Serial.println("=============================================");
  Serial.println("Kiem tra Servo SG90 gan chan GPIO 13.");
  Serial.println("Servo se tu dong quay tu 0 -> 90 -> 180 va nguoc lai.");
  Serial.println("=============================================");

  // Cau hinh chan va LEDC PWM
  pinMode(SERVO_PIN, OUTPUT);
  ledcAttach(SERVO_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_BITS);

  Serial.println("Khoi tao hoan tat. Bat dau test...");
  delay(1000);
}

void loop() {
  // 1. Quay ve 30 do
  Serial.println("Quay ve 30 do...");
  servoWrite(30);
  delay(2000);

  // 2. Quay ve 60 do
  Serial.println("Quay ve 60 do...");
  servoWrite(60);
  delay(2000);

  // 3. Quay ve 90 do
  Serial.println("Quay ve 90 do...");
  servoWrite(90);
  delay(2000);

  // 4. Quay ve 120 do
  Serial.println("Quay ve 120 do...");
  servoWrite(120);
  delay(2000);
}
