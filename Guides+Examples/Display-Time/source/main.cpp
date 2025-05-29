#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
 
#include <switch.h> 
#include "settings.hpp"

 
const char* const months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

const char* const weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Game * game;

// Main program entrypoint 
int main(int argc, char* argv[]) 
{ 
	game = new Game();

	consoleInit(NULL); 
 	
	// Configuratie voor ondersteunend invoer layout: een speler met standaard controllerstijlen 
	padConfigureInput(1, HidNpadStyleSet_NpadStandard); 
 
	// Initialiseer de standaard gamepad 
	PadState pad; 
	padInitializeDefault(&pad); 
 	
		// Main loop 
		while (appletMainLoop()) 
		{ 
			//game->handleEvents();

			//if(game->state == game->welcome)
			//{
			//	game->update_welcome();
			//	game->render_welcome();
			//}

			padUpdate(&pad); 
 		
			u64 kDown = padGetButtonsDown(&pad);

			if (kDown & HidNpadButton_Plus) {break;} // Break om terug te gaan naar homebrew menu 
 		
			// Eigen gemaakte code hieronder 
			//Print current time
        	time_t unixTime = time(NULL);
        	struct tm* timeStruct = gmtime((const time_t *)&unixTime);//Gets UTC time. If you want local-time use localtime().

        	int hours = timeStruct->tm_hour;
        	int minutes = timeStruct->tm_min;
        	int seconds = timeStruct->tm_sec;
        	int day = timeStruct->tm_mday;
        	int month = timeStruct->tm_mon;
        	int year = timeStruct->tm_year +1900;
        	int wday = timeStruct->tm_wday;

			printf("\x1b[1;1H%s", weekDays[wday]);
        	
        	printf("\n%i %s %i", day, months[month], year);
			printf("\x1b[4;1H%02i:%02i:%02i", hours, minutes, seconds);

			// Update de console, nieuwe frame weergeven op scherm 
			consoleUpdate(NULL); 
		} 
 	
	consoleExit(NULL); 
	return 0; 
} 
