
#include "lcd.h"
#include "irsensor.h"

#define CHANNEL 0
#define Hz 500000

#define RST 6
#define DC 5


/**
     * C++ version 0.4 char* style "itoa":
     * Written by Lukás Chmela
     * Released under GPLv3.

     */
char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

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
		// LCDsetCursor(0, 0);
		// LCDprintString((unsigned char*)"Hello World");

		while(irSensorRead(&sensor1) == 1 && 
			  irSensorRead(&sensor2) == 1 && 
			  irSensorRead(&sensor3) == 1 && 
			  irSensorRead(&sensor4) == 1) 
		{
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


		char buffer[100];
		//printf("itoa %s \n", itoa(sensor4.count, buffer, 10));

		LCDsetCursor(0, 3);
		LCDprintString((unsigned char*)itoa(sensor1.count, buffer, 10));

		LCDsetCursor(3, 3);
		LCDprintString((unsigned char*)itoa(sensor2.count, buffer, 10));

		LCDsetCursor(6, 3);
		LCDprintString((unsigned char*)itoa(sensor3.count, buffer, 10));

		LCDsetCursor(9, 3);
		LCDprintString((unsigned char*)itoa(sensor4.count, buffer, 10));

		// printf("Count %d ticks %d dumbClock %d\n", count0, count0Ticks, dumbClock);
		printf("Digital Read 0 %d\n", sensor1.count);
		printf("Digital Read 2 %d\n", sensor2.count);
		printf("Digital Read 3 %d\n", sensor3.count);
		printf("Digital Read 4 %d\n", sensor4.count);

		//delay(500);
	}
	
	return 0;
}
