#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <wiringPi.h>
#include <stdio.h>

typedef struct IrSensor {
    int pinNumber;
    int count;
    int ticks;
    int counted;
    int clock;
    int clockCount;

} IrSensor;

void irSensorInit(IrSensor* sensor, int pinNumber);
int irSensorRead(IrSensor* sensor);
void irSensorUpdate(IrSensor* sensor);


#endif