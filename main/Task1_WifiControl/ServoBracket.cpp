#include <Arduino.h>
#include "ServoBracket.h"
#include "Config.h"

void servoBracketInit() {
  ledcAttach(SERVO_PIN, 50, 14);
  servoBracketWrite(SERVO_CENTER);
}

void servoBracketWrite(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  int duty = map(angle, 0, 180, 1638, 7864);
  ledcWrite(SERVO_PIN, duty);
}
