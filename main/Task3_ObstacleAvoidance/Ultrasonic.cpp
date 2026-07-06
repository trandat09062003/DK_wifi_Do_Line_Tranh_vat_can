#include <Arduino.h>
#include "Ultrasonic.h"
#include "Config.h"
#include "SharedState.h"
#include "ServoBracket.h"
#include "MotorControl.h"

static long readUltrasonicCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long durationUs = pulseIn(ECHO_PIN, HIGH, 30000);
  if (durationUs <= 0) {
    return 999;
  }
  return (long)(durationUs * 0.034 / 2);
}

static void avoidObstacleSequence() {
  Serial.println(F("[OBSTACLE] Phat hien vat can! Dung va quet tim duong..."));
  
  // 1. Dung xe immediately
  g_obstacleStop = true;
  speedRun(0, 0);
  beep(100);
  delay(300);

  // 2. Lùi xe tạo góc thoát rộng tránh va chạm (Tham khảo từ dự án mẫu Altium)
  g_obstacleStop = false;
  speedRun(-1500, -1500);
  delay(800);
  speedRun(0, 0);
  g_obstacleStop = true;
  delay(300);

  // 3. Quay Servo quet Trai (45 do)
  servoBracketWrite(45);
  delay(600);
  long distLeft = readUltrasonicCm();
  if (distLeft <= 0) distLeft = 999;

  // 4. Quay Servo quet Phai (135 do)
  servoBracketWrite(135);
  delay(600);
  long distRight = readUltrasonicCm();
  if (distRight <= 0) distRight = 999;

  // 5. Tra Servo ve trung tam (90 do)
  servoBracketWrite(90);
  delay(500);

  Serial.printf("[OBSTACLE] KET QUA - Trai: %ld cm | Phai: %ld cm\n", distLeft, distRight);

  // Bat dau quay xe ne vat can
  g_obstacleStop = false;

  if (distLeft < 25 && distRight < 25) {
    // Ca hai ben deu bị chan -> Quay dau 180 do de di nguoc lai
    Serial.println(F("[OBSTACLE] Bi chan ca hai ben! Quay dau 180 do..."));
    beep(300);
    speedRun(1400, -1000);
    delay(900); // Quay nguoc lai
    speedRun(0, 0);
  } 
  else if (distLeft >= distRight) {
    // Phía trai thoang hon -> Re sang Trai
    Serial.println(F("[OBSTACLE] Re sang TRAI..."));
    beep(100);
    speedRun(-1100, 1400); // Xoay trai
    delay(550);
    speedRun(0, 0);
  } 
  else {
    // Phia phai thoang hon -> Re sang Phai
    Serial.println(F("[OBSTACLE] Re sang PHAI..."));
    beep(100);
    speedRun(1400, -1100); // Xoay phai
    delay(550);
    speedRun(0, 0);
  }

  // Reset trang thai vat can để tiep tuc chay loang quang
  g_obstacleStop = false;
  g_obstacleBeeped = false;
}

void ultrasonicInit() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  g_distanceCm = 999;
  g_obstacleStop = false;
  g_obstacleSlow = false;
}

void ultrasonicUpdate() {
  if (millis() - g_lastUltrasonicMs < ULTRASONIC_READ_MS) {
    return;
  }
  g_lastUltrasonicMs = millis();
  
  long newDist = readUltrasonicCm();
  g_distanceCm = newDist;

  static int stopCount = 0;

  // Dat nguong vat can o muc 20cm de dam bao xe phanh kip truoc khi quet
  int stopThreshold = 20; 

  if (newDist > 0 && newDist < stopThreshold) {
    stopCount++;
  } else {
    stopCount = 0;
  }

  g_obstacleStop = (stopCount >= 2); // Dung phan hoi nhanh sau 2 lan check lien tiep

  // Neu phat hien vat can khi dang chay tu do -> chay kịch ban tranh
  if (g_obstacleStop && g_start == 1) {
    avoidObstacleSequence();
  }
}
