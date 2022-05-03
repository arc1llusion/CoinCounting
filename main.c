
#include "lcd.h"
#include "genericinput.h"

#include <curl/curl.h>

#define CHANNEL 0
#define Hz 500000

#define RST 6
#define DC 5

/**
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukás Chmela
	 * Released under GPLv3.

	 */
char *itoa(int value, char *result, int base)
{
	// check that the base if valid
	if (base < 2 || base > 36)
	{
		*result = '\0';
		return result;
	}

	char *ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do
	{
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
	} while (value);

	// Apply negative sign
	if (tmp_value < 0)
		*ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

int main()
{

	CURL *curl;
	CURLcode res;

	// Set up cURL
	curl_global_init(CURL_GLOBAL_ALL);

	// Set up wiringPi
	wiringPiSetup();

	// Create the GenericInput structures
	GenericInput sensor1;
	GenericInput sensor2;
	GenericInput sensor3;
	GenericInput sensor4;
	GenericInput sw;

	// Initialize the sensors and switch
	giSensorInit(&sensor1, 0, 0);
	giSensorInit(&sensor2, 2, 0);
	giSensorInit(&sensor3, 3, 0);
	giSensorInit(&sensor4, 4, 0);
	giSensorInit(&sw, 25, 1);

	// Set up wiringPi SPI
	if (wiringPiSPISetup(CHANNEL, Hz) == -1)
	{
		printf("Couldn't setup wiring pi spi \n");
		return -1;
	}

	printf("Wiring PI Setup Successful \n");

	// Initialize the LCD Screen
	LCDinit(RST, DC, CHANNEL);
	LCDclearBuffer();
	LCDdisplay();

	// Enter infinite loop
	while (1)
	{
		// While any of the sensors and switch are not being read
		int switchRead = giSensorRead(&sw);
		while (giSensorRead(&sensor1) == 1 &&
			   giSensorRead(&sensor2) == 1 &&
			   giSensorRead(&sensor3) == 1 &&
			   giSensorRead(&sensor4) == 1 &&
			   switchRead == 0) // switch is positive logic
		{
			giSensorUpdate(&sensor1);
			giSensorUpdate(&sensor2);
			giSensorUpdate(&sensor3);
			giSensorUpdate(&sensor4);
			giSensorUpdate(&sw);
			switchRead = giSensorRead(&sw);
		}

		// Update the behind the scenes counters for handling multiple readings of the sensor or switch
		giSensorUpdate(&sensor1);
		giSensorUpdate(&sensor2);
		giSensorUpdate(&sensor3);
		giSensorUpdate(&sensor4);
		giSensorUpdate(&sw);

		printf("switch %d \n", sw.count);

		if ((sw.count > 0) == 1) // If the switch is read
		{
			struct curl_slist *list = NULL;
			curl = curl_easy_init(); // initialize cURL for the http request

			if (!curl)
			{
				printf("Couldn't initialize curl");
				return -1;
			}

			// Set the url for cURL to use to be the backend web API
			curl_easy_setopt(curl, CURLOPT_URL, "https://www.pandastic.net/Coins/Deposit");

			// Add HTTP headers that are required
			list = curl_slist_append(list, "Content-Type: application/json");
			list = curl_slist_append(list, "accept: */*");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

			// Formalize the JSON string being sent to the web api
			char data[400];
			sprintf(data, "{\"UserId\": 1, \"Quarters\": %d, \"Dimes\": %d, \"Nickels\": %d, \"Pennies\": %d}", sensor1.count, sensor2.count, sensor3.count, sensor4.count);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

			// Send the HTTP request
			res = curl_easy_perform(curl);

			if (res != CURLE_OK)
			{
				// If for some reason the request fails, print an error
				fprintf(stderr, "failed %s\n", curl_easy_strerror(res));
			}
			else
			{
				// If the request doesn't fail, reset all the counts.
				sensor1.count = 0;
				sensor2.count = 0;
				sensor3.count = 0;
				sensor4.count = 0;
				sw.count = 0;
			}

			// Cleanup cURL
			curl_slist_free_all(list);
		}

		// Print the information row on the LCD Screen
		LCDclearBuffer();
		LCDdisplay();

		LCDsetCursor(0, 0);
		LCDprintCharacter('Q');

		LCDsetCursor(3, 0);
		LCDprintCharacter('D');

		LCDsetCursor(6, 0);
		LCDprintCharacter('N');

		LCDsetCursor(9, 0);
		LCDprintCharacter('P');

		char buffer[100];
		// Print the coin row on the LCD screen showing how much of each coin has been detected
		LCDsetCursor(0, 3);
		LCDprintString((unsigned char *)itoa(sensor1.count, buffer, 10));

		LCDsetCursor(3, 3);
		LCDprintString((unsigned char *)itoa(sensor2.count, buffer, 10));

		LCDsetCursor(6, 3);
		LCDprintString((unsigned char *)itoa(sensor3.count, buffer, 10));

		LCDsetCursor(9, 3);
		LCDprintString((unsigned char *)itoa(sensor4.count, buffer, 10));

		printf("Digital Read 0 %d\n", sensor1.count);
		printf("Digital Read 2 %d\n", sensor2.count);
		printf("Digital Read 3 %d\n", sensor3.count);
		printf("Digital Read 4 %d\n", sensor4.count);

		// delay(500);
	}

	// Global cleanup for cURL
	curl_global_cleanup();

	return 0;
}
