#include "lcd.h"

int _reset;
int _dc;
int _channel;

void LCDinit(int reset, int dc, int channel) {
	int delay = 0;

    _reset = reset;
    _dc = dc;
    _channel = channel;

	pinMode(_reset, OUTPUT); //LCD reset and data/command pin are outputs on GPIO pins
	pinMode(_dc, OUTPUT);

	//5110 assumes an active low reset to clear the device
	digitalWrite(_reset, RESET_LOW);
	for(delay = 0; delay < 10; delay = delay + 1);
	digitalWrite(_reset, RESET_HIGH);

	LCDwrite(COMMAND, 0x21); //Chip active
	LCDwrite(COMMAND, CONTRAST); //setting contrast
	LCDwrite(COMMAND, 0x04); //temperature coefficient
	LCDwrite(COMMAND, 0x14); // LCD bias mode
	LCDwrite(COMMAND, 0x20); // Must be sent before modifying the display control mode
	LCDwrite(COMMAND, 0x0C); //Sets the display control mode. 0x0D for inverted

	printf("LCD Init Successful\n");
}

void LCDwrite(enum typeOfWrite type, unsigned char message) {
	int delay = 0;
	unsigned char buffer[1];
	buffer[0] = message; //wiringPiSpi requires a buffer even if it's just one character
	if(type == COMMAND) {
		digitalWrite(_dc, DC_COMMAND); //Set data/command to command mode
		wiringPiSPIDataRW(_channel, buffer, 1);

		for(delay = 0; delay < 20; ++delay); //wait a bit
	}
	else {
		for(delay = 0; delay < 20; ++delay);
		digitalWrite(_dc, DC_DATA); //Set data/command to data mode
		wiringPiSPIDataRW(_channel, buffer, 1); //Wait a bit
	}
}

void LCDsetCursor(unsigned char newX, unsigned char newY) {
	if((newX > 11) || (newY > 5)) {
		return;
	}
	unsigned char buffer[1];

	//multiply newX by 7 because each character is 7 columns wide
	buffer[0] = 0x80 | (newX * 7); //Setting bit 7 updates X-Position
	LCDwrite(COMMAND, buffer[0]);

	buffer[0] = 0x40 | newY; //Setting bit 6 updates Y-Position
	LCDwrite(COMMAND, buffer[0]);
}

// Draws a full screen bitmap image from the pointer
void LCDdrawFullImage(const unsigned char *ptr) {
	int i;
	LCDsetCursor(0, 0);
	for(i = 0; i < (MAX_X*MAX_Y/8); ++i) {
		LCDwrite(DATA, ptr[i]);
	}
}

unsigned char Screen[SCREENW*SCREENH / 8]; //A screen buffer to store the next image

//clears the in RAM screen buffer
void LCDclearBuffer(void) {
	int i = 0;
	for(i = 0; i < SCREENW*SCREENH / 8; ++i) {
		Screen[i] = 0;
	}
}

//Sets a single pixel in the screen buffer
void LCDsetPixel(unsigned char x, unsigned char y) {
	unsigned short pixelByte; //byte # in screen buffer
	unsigned char pixelBit; // bit# in byte
	if((x < 84) && (y < 48)) {
		pixelByte = ((y/8)*84) + x;
		pixelBit = y % 8;
		Screen[pixelByte] |= 1U<<pixelBit;
	}
}

//Clears a single pixel in the screen buffer
void LCDclearPixel(unsigned char x, unsigned char y) {
	unsigned short pixelByte; //byte # in screen buffer
	unsigned char pixelBit; // bit# in byte
	if((x < 84) && (y < 48)) {
		pixelByte = ((y/8)*84) + x;
		pixelBit = y % 8;
		Screen[pixelByte] &= ~(1U<<pixelBit);
	}
}

//Displays the buffer to the screen
void LCDdisplay(void) {
	LCDdrawFullImage(Screen);
}

void LCDprintCharacter(unsigned char c)
{
	int i;
	LCDwrite(DATA, 0x00); // Blank vertical line padding
	for(i = 0; i < 5; ++i) {
		LCDwrite(DATA, ASCII[c - 0x20][i]);
	}
	LCDwrite(DATA, 0x00); //Blank vertical line padding
}

void LCDprintString(unsigned char *ptr)
{
	while(*ptr) {
		LCDprintCharacter(*ptr);
		ptr = ptr + 1;
	}
}