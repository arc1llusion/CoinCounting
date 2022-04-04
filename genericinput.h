#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <wiringPi.h>
#include <stdio.h>

typedef struct GenericInput {
    int pinNumber;
    int count;
    int ticks;
    int counted;
    int clock;
    int clockCount;
    int inputTrue;

} GenericInput;

void giSensorInit(GenericInput* sensor, int pinNumber, int inputTrue);
int giSensorRead(GenericInput* sensor);
void giSensorUpdate(GenericInput* sensor);


#endif