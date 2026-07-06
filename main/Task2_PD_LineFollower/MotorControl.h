#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

void motorInit();
void speedRun(int speedLeft, int speedRight);
void handleAndSpeed(int angle, int speedBase);

#endif
