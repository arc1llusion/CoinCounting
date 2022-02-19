
#include "lcd.h"
#include "irsensor.h"

#define CHANNEL 0
#define Hz 500000

#define RST 6
#define DC 5

int main() {

	wiringPiSetup();

	IrSensor sensor1;
	IrSensor sensor2;
	IrSensor sensor3;
	IrSensor sensor4;

	irSensorInit(&sensor1, 0);
	irSensorInit(&sensor2, 2);
	irSensorInit(&sensor3, 3);
	irSensorInit(&sensor4, 4);

	if(wiringPiSPISetup(CHANNEL, Hz) == -1) {
		printf("Couldn't setup wiring pi spi \n");
		return -1;
	}

	printf("Wiring PI Setup Successful \n");

	LCDinit(RST, DC, CHANNEL);
	LCDclearBuffer();
	LCDdisplay();

	while(1) 
	{
		LCDsetCursor(0, 0);
		LCDprintString((unsigned char*)"Hello World");

		while(irSensorRead(&sensor1) == 1 && irSensorRead(&sensor2) == 1 && irSensorRead(&sensor3) == 1 && irSensorRead(&sensor4) == 1) {
			irSensorUpdate(&sensor1);
			irSensorUpdate(&sensor2);
			irSensorUpdate(&sensor3);
			irSensorUpdate(&sensor4);
		}

		irSensorUpdate(&sensor1);
		irSensorUpdate(&sensor2);
		irSensorUpdate(&sensor3);
		irSensorUpdate(&sensor4);

		LCDclearBuffer();
		LCDdisplay();

		LCDsetCursor(0,0);
		LCDprintCharacter('Q');

		LCDsetCursor(3,0);
		LCDprintCharacter('D');

		LCDsetCursor(6,0);
		LCDprintCharacter('N');

		LCDsetCursor(9,0);
		LCDprintCharacter('P');

		// LCDsetCursor(0, 3);
		// LCDprintCharacter((unsigned char)sensor1.count);

		// LCDsetCursor(3, 3);
		// LCDprintCharacter((unsigned char)sensor2.count);

		// LCDsetCursor(6, 3);
		// LCDprintCharacter((unsigned char)sensor3.count);

		// LCDsetCursor(9, 3);
		// LCDprintCharacter((unsigned char)sensor4.count);

		// printf("Count %d ticks %d dumbClock %d\n", count0, count0Ticks, dumbClock);
		// printf("Digital Read 0 %d\n", sensor1.count);
		// printf("Digital Read 2 %d\n", sensor2.count);
		// printf("Digital Read 3 %d\n", sensor3.count);
		// printf("Digital Read 4 %d\n", sensor4.count);

		//delay(500);
	}
	
	return 0;
}
