#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

void lineSensorTimerInit();
void lineSensorPid();
void lineSensorRead();
void lineSensorUpdate();
void lineSensorUpdateCalib();
void lineSensorReadEeprom();
unsigned char lineSensorMask(unsigned char mask);
void lineSensorSaveCalib();

#endif
