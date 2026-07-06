#include <Arduino.h>
#include "Ultrasonic.h"
#include "Config.h"
#include "SharedState.h"

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
}
