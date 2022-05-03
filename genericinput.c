#include "genericinput.h"

// Initializes a generic input with the wiringPi Pin number and whether it's active low or active high
void giSensorInit(GenericInput *sensor, int pinNumber, int inputTrue)
{
	sensor->pinNumber = pinNumber;
	sensor->inputTrue = inputTrue;
	pinMode(sensor->pinNumber, INPUT);

	sensor->count = 0;
	sensor->ticks = 0;
	sensor->clock = 0;
	sensor->clockCount = 0;
	sensor->counted = 0;
}

// Reads the pin number associated with this generic input
int giSensorRead(GenericInput *sensor)
{
	return digitalRead(sensor->pinNumber);
}

// In order to prevent multiple detections of a coin or switch press, we count the first instance then set a "timer" to handle the rest
void giSensorUpdate(GenericInput *sensor)
{
	if (giSensorRead(sensor) == sensor->inputTrue)
	{
		sensor->ticks = sensor->ticks + 1;
		sensor->clock = 1;
	}

	if (sensor->ticks > 1 && sensor->counted == 0)
	{
		sensor->count = sensor->count + 1;
		sensor->counted = 1;
	}

	if (sensor->clock == 1)
	{
		sensor->clockCount = sensor->clockCount + 1;
	}

	if (sensor->clockCount > 10000)
	{
		sensor->ticks = 0;
		sensor->clockCount = 0;
		sensor->clock = 0;
		sensor->counted = 0;
	}
}