//Internal Version: V4.7.0
//Release Version: V1.0.0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <time.h>
#include <direct.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
//#include <pthread.h>

#define RED   "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW   "\x1B[33m"
#define BLUE   "\x1B[34m"
#define MAGENTA   "\x1B[35m"
#define CYAN   "\x1B[36m"
#define WHITE   "\x1B[37m"
#define UNK1   "\x1B[38m"
#define UNK2   "\x1B[39m"
#define RESET "\x1B[0m"

#define avgArraySize 42
#define MAX_SIZE 33554432
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

//printf("\033[XA"); // Move up X lines;
//printf("\033[XB"); // Move down X lines;
//printf("\033[XC"); // Move right X column;
//printf("\033[XD"); // Move left X column;

//--------------------------------------------------------

long long Number[MAX_SIZE]; //original array
long long Mirror[MAX_SIZE]; //mirror array

//--------------------------------------------------------

int settingsView = 1;

long long iMin = 0;
long long iMax = 0; //iMin and iMax are for testing a range of numbers
long long seed = 0; //Starting value for single number calculation

int limit = 0; // The actual limit is 10^x, the hard limit is 603 million
long long maxIterations = 0; // Iteration limit for simulation. 0 will remove the limit
long long maxTime = 0; // Time limit for simulation in seconds. 0 will remove the limit

int outputFrequencySetting = 4;
float outputFrequency = 0; //This will control the actual output rate, the outcome will be 1/x updates per second
int outputMode = 3;
bool outputToTXT = false;

long long cutoffArray = 0;
long long cutoffValue = 0; //These variables will stop the loop when the number reaches a certain size, 10^x, Maximum number length
int digits = 0; //Length of the number currently is digits + 1

const int arrayMax = 33554432; //Max length of both arrays, max readable value will be arrayMax - 1
int arrayLength = 1; //Currently occupied length of array, number of 18 digit variables

long long iterations = 0;
long long operations = 0;

bool palindrome = false;

//Helps in centering text cuz I'm lazy
int length = 0;
int width = 120;
int msgBoxWidth = 70;
int pad = 0;

// Progress Bar values
float percent = 0;
int roundPercent = 0;

double totalElapsedTime = 0; // this includes the save file's elapsed time too
double LoadElapsedTime = 0; // the save file's stored elapsed time
double elapsedTime = 0; // elapsed time since the calculation started
double lastUpdateTime = 0; // this helps control when the data refreshes on screen

unsigned int autoSaveInterval = 0; // seconds
bool autosaveEnabled = false;
double lastSaveTime = 0;

clock_t start, end, update, total;

FILE* configptr;
FILE* resultptr;
FILE* saveptr;

//--------------------------------------------------------------------------

// Global Strings

char invalidInput[] = "Invalid Input";
char noLimit[] = "No Limit";
char escToUndo[] = "Press /Esc/ to undo";
char quitSettings[] = "Quitting settings menu, nothing changed";

char tempStr[58] = "";
char buffer[120] = "";

//--------------------------------------------------------------------------

int numberSpacer(long long*);
int inputCheck(char*);
int scanf_uint(int*);
int scanf_ullong(long long*);
int printBuffer(int*, int*);

// < Initialization >

int main(void) { // Setup at startup

	srand(NULL);

	consoleSetup();
	localConfigFileRead();

	gotoxy(0, 0);

	mainMenu();

	return 0;
}

int consoleSetup(void) {

	srand(time(NULL));
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(h, &mode);
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(h, mode);
	return 0;
}

int localConfigFileRead(void) { //This makes a folder structure for storing the config file and saves that will be included later

	int retval = 0, settingValue = 0;
	long long insertValue = 0;

	retval = _mkdir("Palindrome App");
	retval = _mkdir("Palindrome App\\results");
	retval = _mkdir("Palindrome App\\saves");

	configptr = fopen("Palindrome App\\config.txt", "r");

	//If config file doesn't exist, make it
	if (configptr == NULL) {

		limit = 25000;
		maxTime = 0;
		maxIterations = 0;
		outputMode = 3;
		outputFrequencySetting = 3;
		outputToTXT = false;
		autoSaveInterval = 300;

		configFileWrite();
	}

	//If it does, load the settings from it
	else {

		configptr = fopen("Palindrome App\\config.txt", "r");

		const unsigned MAX_LENGTH = 256;
		char buffer[256];

		while (fgets(buffer, MAX_LENGTH, configptr)) {

			int stringLength = strlen(buffer);
			char delimiter[] = " :";
			char* ptr = strtok(buffer, delimiter);

			ptr = strtok(NULL, delimiter);
			//printf("%s", ptr);
			insertValue = atoll(ptr);

			switch (settingValue) {
			case 0:
				limit = insertValue;
				break;
			case 1:
				maxIterations = insertValue;
				break;
			case 2:
				maxTime = insertValue;
				break;
			case 3:
				outputMode = insertValue;
				if (insertValue <= 0 || insertValue > 4)
					outputMode = 3;
				break;
			case 4:
				outputFrequencySetting = insertValue;
				if (insertValue <= 0 || insertValue > 6) // Ensures that the value is valid
					outputFrequencySetting = 4;
				break;
			case 5:
				outputToTXT = insertValue;
				if (insertValue > 1 || insertValue < 0)
					outputToTXT = 0;
				break;
			case 6:
				autoSaveInterval = insertValue;
				if (autoSaveInterval == 0)
					autosaveEnabled = false;
				else
					autosaveEnabled = true;
				break;
			default:
				break;
			}
			settingValue++;
		}
		fclose(configptr);
	}
	return 0;
}

// </ Initialization >

	// < Current Settings Print >

int mainSettingsPrint(void) {

	for (int i = 2; i < 12; i++) {
		gotoxy(70, i);
		printf("                                                  ");
	}

	gotoxy(70, 2);
	int width = 51;

	char settings[] = "- Current Settings -";
	length = sizeof(settings) - 1;
	pad = (width - length) / 2;
	printf("%*.*s%s", pad, pad, " ", settings);

	if (settingsView == 1) {
		gotoxy(70, 4);
		mainPrintMaxValue();
		gotoxy(70, 5);
		mainPrintIteration();
		gotoxy(70, 6);
		mainPrintTime();
		gotoxy(70, 7);
		mainPrintOutputMode();
		gotoxy(70, 8);
		mainPrintUpdateFrequency();
		gotoxy(70, 9);
		mainPrintTxtFile();
		gotoxy(70, 10);
		mainPrintAutoSave();
	}

	return 0;
}

int mainPrintMaxValue(void) {

	int width = 51, colour = 33;

	strcpy(buffer, "Maximum Number Length: ");
	if (limit <= 0 || limit >= 603979750) {
		limit = 603979750;
		strcat(buffer, noLimit);
	}
	else {
		colour = 32;
		long long n = limit;
		numberSpacer(&n);
		strcat(buffer, " Digits");
	}
	printBuffer(&width, &colour);
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

int mainPrintIteration(void) {

	int width = 51, colour = 33;

	strcpy(buffer, "Maximum Iterations: ");
	if (maxIterations <= 0 || maxIterations >= 10000000000000) {
		maxIterations = 0;
		strcat(buffer, noLimit);
	}
	else {
		colour = 32;
		long long n = maxIterations;
		numberSpacer(&n);
	}
	printBuffer(&width, &colour);
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

int mainPrintTime(void) {

	int width = 51, temp = maxTime;

	char time[] = "Maximum Time: ";

	strcpy(buffer, time);
	if (maxTime <= 0 || maxTime >= 1576800000) {
		maxTime = 0;
		int colour = 33;
		strcat(buffer, noLimit);
		printBuffer(&width, &colour);
	}
	else {

		int i = 0;
		long long seconds = temp % 60;
		temp /= 60;
		long long minutes = temp % 60;
		temp /= 60;
		long long hours = temp % 24;
		temp /= 24;
		long long days = temp % 365;
		long long years = temp / 365;

		if (years != 0) {
			numberSpacer(&years);
			strcat(buffer, " Year");
			if (years != 1)
				strcat(buffer, "s");
			strcat(buffer, " ");
			i++;
		}
		if (days != 0) {
			numberSpacer(&days);
			strcat(buffer, " Day");
			if (days != 1)
				strcat(buffer, "s");
			strcat(buffer, " ");
			i++;
		}
		if (hours != 0) {
			numberSpacer(&hours);
			strcat(buffer, " Hour");
			if (hours != 1)
				strcat(buffer, "s");
			i++;
			if (i < 3)
				strcat(buffer, " ");
		}
		if (minutes != 0 && i < 3) {
			numberSpacer(&minutes);
			strcat(buffer, " Minute");
			if (minutes != 1)
				strcat(buffer, "s");
			i++;
			if (i < 3)
				strcat(buffer, " ");
		}
		if (seconds != 0 && i < 3) {
			numberSpacer(&seconds);
			strcat(buffer, " Second");
			if (seconds != 1)
				strcat(buffer, "s");
		}
		int colour = 32;
		printBuffer(&width, &colour);
	}
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

int mainPrintOutputMode(void) {

	int width = 51, colour = 32;

	strcpy(buffer, "Output mode: ");

	switch (outputMode) {
	case 1:
		strcat(buffer, "Elapsed Time only");
		colour = 36;
		break;

	case 2:
		strcat(buffer, "Progress Bar & Elapsed Time");
		break;

	case 3:
		strcat(buffer, "Progress Bar & Some Stats");
		break;

	case 4:
		strcat(buffer, "Progress Bar with Lots Of Stats");
		colour = 33;
		break;

	default:
		break;
	}
	printBuffer(&width, &colour);
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

int mainPrintUpdateFrequency(void) {

	int num = 0, width = 51, colour = 32;

	float settingValues[6] = { 0.977, 0.237, 0.107, 0.0647, 0.0283, 0.0082 };

	outputFrequency = settingValues[outputFrequencySetting - 1];

	strcpy(buffer, "Refreshing Frequency: ");

	if (outputFrequencySetting == 1) {
		strcat(buffer, "Very Low");
	}
	else if (outputFrequencySetting == 2) {
		strcat(buffer, "Low");
	}
	else if (outputFrequencySetting == 3) {
		strcat(buffer, "Medium");
	}
	else if (outputFrequencySetting == 4) {
		strcat(buffer, "High");
	}
	else if (outputFrequencySetting == 5) {
		strcat(buffer, "Very High");
	}
	else {
		strcat(buffer, "Realtime");
	}

	if (outputFrequencySetting > 3)
		colour = 33;

	printBuffer(&width, &colour);
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

int mainPrintTxtFile(void) {

	int width = 51, colour = 32;
	char ch = 0;

	strcpy(buffer, "Output To txt File: ");

	if (outputToTXT == true) {
		strcat(buffer, "True");
	}

	else {
		colour = 31;
		strcat(buffer, "False");
	}

	printBuffer(&width, &colour);
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

int mainPrintAutoSave(void) {

	int width = 51, colour = 31;
	char ch = 0;

	if (autoSaveInterval <= 0 || autoSaveInterval >= UINT_MAX) {
		autoSaveInterval = UINT_MAX;
		strcpy(buffer, "Autosaves: Disabled");
	}
	else {
		long long n = autoSaveInterval;
		strcpy(buffer, "Autosave Interval: ");
		numberSpacer(&n);
		strcat(buffer, " Seconds");
	}

	if (autoSaveInterval == UINT_MAX)
		colour = 31;
	else if (autoSaveInterval < 30)
		colour = 35;
	else if (autoSaveInterval < 120)
		colour = 31;
	else if (autoSaveInterval < 300)
		colour = 33;
	else
		colour = 32;

	printBuffer(&width, &colour);
	strcpy(buffer, "");
	printf(RESET);
	return 0;
}

// </ Current Settings Print >

// < Main Menu >

int mainPrint(void) {

	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	gotoxy(0, 0);

	printf(
		"                                                                    |\n"
		"             - - -  Palindrome Calculator V1.0.0  - - -             |\n" // Current Settings
		"                 - - -  App Made By Hydrough  - - -                 |\n"
		"                                                                    |\n" //Max Number of Digits
		"      This app will test if a number will become a palindromic      |\n" //Maximum Iterations
		"     if you repeatedly reverse the number and add it to itself.     |\n" //Maximum Time
		"  Palindromic numbers are the same in both directions, e.g.: 7887.  |\n" //Output mode
		"                                                                    |\n" //Output Update Frequency
		" Don't resize the window if you can, it will mess up the rendering! |\n" //Output To txt File
		"    Many things can be changed in the settings menu, look around!   |\n" //Multithreading
		"                                                                    |\n"
		" ----------------------------------------------------------------------------------------------------------------------"
	);

	gotoxy(100, 17);
	printf("("GREEN"S"RESET") Save Menu");

	gotoxy(0, 14);
	return 0;
}

int mainMenuPrint(void) {

	gotoxy(0, 14);
	char menuName[] = "- - - Main Menu - - -";
	pad = (width - (sizeof(menuName) - 1)) / 2;
	printf("%*.*s%s\n\n", pad, pad, " ", menuName);

	printf(
		"                                    - Press "GREEN"1"RESET" to test just one number\n"
		"                                    - Press "GREEN"2"RESET" to test a range of numbers\n"
		"                                    - Press "GREEN"3"RESET" to go to the settings menu\n"
		"                                    - Press "GREEN"4"RESET" to learn about the app and the dev\n"
		"                                    - Press "GREEN"5"RESET" to show planned features\n"
		"                                    - Press "GREEN"6"RESET" to show the patch notes\n"
		"                                    - Press "GREEN"Esc"RESET" to quit the program"
	);

	gotoxy(0, 14);
	return 0;
}

int mainMenu(void) {

	mainPrint();
	mainMenuPrint();
	mainSettingsPrint();

	for (int i = 0; i < arrayMax; i++) { //Nullifies all the values in Number[] and Mirror[] array

		Number[i] = 0;
		Mirror[i] = 0;
	}

	int colour = 0;
	char ch = _getch();
	clr();

	switch (ch) {

	case 9: // Switch displayed settings (Tab) - incomplete
		break;

	case 27: // Quit the app (Esc)
		gotoxy(0, 14);
		exit(0);

	case 49: // Single number (1)
		singleNumberCalculationSetup();
		break;

	case 50: // Range of numbers (2)
		range();
		break;

	case 51: // Settings (3)
		settings();
		break;

	case 52: // Info Menu (4)
		details();
		break;

	case 53: // incomplete (5)
		plannedFeatures();
		break;

	case 54: // incomplete (6)
		patchNotes();
		break;

	case 115: // Save Menu (S)
		if (saveMenu() == -1) {
			strcpy(buffer, "The saves folder is empty right now");
			gotoxy(0, 28);
			printBuffer(&width, &colour);
			strcpy(buffer, "");
		}
		break;

	default:
		gotoxy(52, 28);
		printf("%s", invalidInput);
	}

	gotoxy(0, 0);
	mainMenu();
	return 0;
}

// < /Main Menu >

// < Settings >

int settings(void) { //Settings tab

	char menuName[] = "- - - Settings Menu - - -";

	pad = (width - (sizeof(menuName) - 1)) / 2;
	printf("%*.*s%s\n\n", pad, pad, " ", menuName);

	printf(
		"                                    - Press "GREEN"1"RESET" to change Maximum Number of Digits\n"
		"                                    - Press "GREEN"2"RESET" to change Maximum Iteration Count\n"
		"                                    - Press "GREEN"3"RESET" to change Maximum Time\n"
		"                                    - Press "GREEN"4"RESET" to change Output Settings\n"
		"                                    - Press "GREEN"5"RESET" to change Output Frequency\n"
		"                                    - Press "GREEN"6"RESET" to enable or disable TXT output\n"
		"                                    - Press "GREEN"7"RESET" to change the Autosave Interval\n"
		"                                    - Press "GREEN"Esc"RESET" to quit the settings tab\n"
	);

	char ch = _getch();

	clr();

	switch (ch) {

	case 27: // Escape
		gotoxy(40, 28);
		printf("%s\n", quitSettings);
		break;

	case 49: // 1
		maxValueSettings();
		break;

	case 50: // 2
		iterationsSettings();
		break;

	case 51: // 3
		timeSettings();
		break;

	case 52: // 4
		outputSettings();
		break;

	case 53: // 5
		outputFrequencySettings();
		break;

	case 54: // 6
		TXTOutputSettings();
		break;

	case 55: // 7
		autosaveSettings();
		break;

	default: //Error
		gotoxy(53, 28);
		printf("%s", invalidInput);
		gotoxy(0, 14);
		settings();
	}
	gotoxy(0, 14);
	return 0;
}

int maxValueSettings(void) { // Complete

	unsigned int userInput = 0, prevValue = limit;
	int colour = 0;

	char arr[][120] = {
		"- - - Max Value Settings - - -",
		"Type in the maximum value you want to calculate to. Typing in 0 will remove the limit.",
		"For now you can only determine it as a power of 10, so typing in 50 will make it 10^50 (50 digits)",
		"The maximum allowed value is 10^603 Million or 603 Million digits (yes, really!)",
		"Warning: Changing the max value to a huge number will slow the program down",
	};

	for (int i = 0; i < 5; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
			printf("\n");
			break;
		default:
			break;
		}
	}

	do {
		gotoxy(55, 22);
		if (scanf_uint(&userInput) == -1) {
			clr();
			return 0;
		}
		gotoxy(0, 20);
		clrLn();
	} while (userInput < 0);
	limit = userInput;

	clr();
	drawMsgBox1();

	if (limit >= 603979750 || limit <= 0) {
		limit = 603979750;

		strcpy(buffer, "Number length limit was removed");
	}
	else {

		long long n = limit;
		strcpy(buffer, "Max number of digits updated to ");
		numberSpacer(&n);
	}

	int width = msgBoxWidth;
	printBuffer(&width, &colour);

	if (prevValue != limit)
		configFileWrite();

	strcpy(buffer, "");
	return 0;
}

int iterationsSettings(void) {

	long long userInput = 0, prevValue = maxIterations;
	int colour = 0;

	char arr[][120] = {
	"- - - Maximum Iterations Menu - - -",
	"Type in the maximum iterations you want the program to do before stopping",
	"If you type 0, the limit will be removed completely",
	"The higher the value the longer you will allow the program to run",
	};

	for (int i = 0; i < 4; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
			printf("\n");
			break;
		default:
			break;
		}
	}

	gotoxy(55, 21);

	do {
		gotoxy(55, 21);
		if (scanf_ullong(&userInput) == -1) {
			clr();
			return 0;
		}
		gotoxy(0, 20);
		clrLn();
	} while (userInput < 0);
	maxIterations = userInput;

	clr();
	drawMsgBox1();

	if (maxIterations <= 0 || maxIterations > 10000000000000) {
		maxIterations = 0;

		strcpy(buffer, "Max iteration limit was removed");
	}
	else {

		long long n = maxIterations;
		strcpy(buffer, "Max number of iterations updated to ");
		numberSpacer(&n);
	}

	int width = msgBoxWidth;
	printBuffer(&width, &colour);

	if (prevValue != maxIterations)
		configFileWrite();

	strcpy(buffer, "");
	return 0;
}

int timeSettings(void) {

	long long userInput = 0, prevValue = maxTime;
	int colour = 0;

	char arr[][120] = {
	"- - - Max Time Settings - - -",
	"Type in the maximum amount of time you want the program to run for",
	"For now you can only determine it in seconds. You can type in 0 to remove the limit",
	"If you're doing a range test, the time limit will change 1 calculation's duration",
	};

	for (int i = 0; i < 4; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
			printf("\n");
			break;
		default:
			break;
		}
	}

	do {
		gotoxy(55, 21);
		if (scanf_ullong(&userInput) == -1) {
			clr();
			return 0;
		}
		gotoxy(0, 20);
		clrLn();
	} while (userInput < 0);
	maxTime = userInput;

	clr();
	drawMsgBox1();

	if (maxTime >= 1576800000 || maxTime <= 0) {
		maxTime = 0;

		strcpy(buffer, "Max time limit was removed");
	}
	else {

		strcpy(buffer, "Max time limit was changed");
	}

	int width = msgBoxWidth;
	printBuffer(&width, &colour);

	if (prevValue != maxTime)
		configFileWrite();

	strcpy(buffer, "");
	return 0;
}

int outputSettings(void) {

	long long prevValue = outputMode;
	int colour = 0;

	gotoxy(0, 14);

	char line1[] = "- - - Output Settings - - -";
	length = sizeof(line1) - 1;
	pad = (width - length) / 2;
	printf("%*.*s%s", pad, pad, " ", line1);

	gotoxy(37, 17);
	printf("Press "GREEN"1"RESET" to print Elapsed Time only");
	gotoxy(37, 18);
	printf("Press "GREEN"2"RESET" to show a Progress Bar and Elapsed Time");
	gotoxy(37, 19);
	printf("Press "GREEN"3"RESET" to show a Progress Bar with Some Stats");
	gotoxy(37, 20);
	printf("Press "GREEN"4"RESET" to show a Progress Bar with Lots Of Stats");

	gotoxy(55, 22);

	char ch = _getch();
	clr();

	if (ch > 48 && ch < 54) {
		outputMode = ch - 48;
		strcpy(buffer, "Output changed to ");
		drawMsgBox1();
	}
	else if (ch != 27) {
		gotoxy(0, 29);
		strcpy(buffer, invalidInput);
		printBuffer(&width, &colour);
		return 0;
	}
	else { // Esc

		gotoxy(0, 29);
		strcpy(buffer, quitSettings);
		printBuffer(&width, &colour);
		return 0;
	}

	switch (ch) {

	case 49: // 1
		strcat(buffer, "Elapsed Time only");
		break;
	case 50: // 2
		strcat(buffer, "Progress Bar only");
		break;
	case 51: // 3
		strcat(buffer, "Progress Bar with Some Stats");
		break;
	case 52: // 4
		strcat(buffer, "Progress Bar with Lots Of Stats");
		break;
	case 53: // 5
		strcat(buffer, "Give me ALL The Stats");
		break;
	default:
		break;
	}

	int width = msgBoxWidth;
	printBuffer(&width, &colour);

	if (prevValue != outputMode)
		configFileWrite();

	strcpy(buffer, "");
	return 0;
}

int outputFrequencySettings(void) {

	long long prevValue = outputFrequencySetting;
	int colour = 0;

	char arr[][120] = {
	"- - - Output Frequency Settings - - -",
	"This menu allows you to change the output frequency of progress bars, stats, etc.",
	"This setting might cause performance issues and/or slow down calculations at higher settings",
	};

	for (int i = 0; i < 3; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
			printf("\n");
			break;
		default:
			break;
		}
	}

	gotoxy(37, 19);
	printf("Press "GREEN"1"RESET" to make update rate Very Low");
	gotoxy(37, 20);
	printf("Press "GREEN"2"RESET" to make update rate Low");
	gotoxy(37, 21);
	printf("Press "GREEN"3"RESET" to make update rate Medium");
	gotoxy(37, 22);
	printf("Press "GREEN"4"RESET" to make update rate High");
	gotoxy(37, 23);
	printf("Press "GREEN"5"RESET" to make update rate Very High");
	gotoxy(37, 24);
	printf("Press "GREEN"6"RESET" to make update rate Realtime");

	gotoxy(55, 26);

	char ch = _getch();
	clr();

	if (ch > 48 && ch < 55) {
		outputFrequencySetting = ch - 48;
		strcpy(buffer, "Update rate changed to ");
		drawMsgBox1();
	}
	else if (ch != 27) {
		gotoxy(0, 29);
		strcpy(buffer, invalidInput);
		printBuffer(&width, &colour);
		return 0;
	}
	else { // Esc
		gotoxy(0, 29);
		strcpy(buffer, quitSettings);
		printBuffer(&width, &colour);
		return 0;
	}

	switch (ch) {

	case 49: // 1
		strcat(buffer, "Very Low");
		break;
	case 50: // 2
		strcat(buffer, "Low");
		break;
	case 51: // 3
		strcat(buffer, "Medium");
		break;
	case 52: // 4
		strcat(buffer, "High");
		break;
	case 53: // 5
		strcat(buffer, "Very High");
		break;
	case 54: // 6
		strcat(buffer, "Realtime");
	default:
		break;
	}

	int width = msgBoxWidth;
	printBuffer(&width, &colour);

	if (prevValue != outputFrequencySetting)
		configFileWrite();

	strcpy(buffer, "");
	return 0;
}

int TXTOutputSettings(void) {

	int num = 0, colour = 0;

	strcpy(buffer, "Text file output has been ");

	if (outputToTXT == false) {
		outputToTXT = true;
		strcat(buffer, "Enabled");
	}
	else {
		outputToTXT = false;
		strcat(buffer, "Disabled");
	}

	clr();
	drawMsgBox1();
	printBuffer(&msgBoxWidth, &colour);

	configFileWrite();

	strcpy(buffer, "");
	return 0;
}

int autosaveSettings(void) {

	unsigned int userInput = 0;
	int prevValue = autoSaveInterval, colour = 0;

	char arr[][120] = {
	"- - - Autosave Setting Menu - - -",
	"Here you can change frequency of autosaves, typing 0 disables the autosaving entirely",
	"Warning: Setting it to a small number will hurt performance and will cause wear to your drive!",
	"The save files can become large, like 600MB large! Consider moving this app to a Hard Disk Drive if you can!",
	};

	for (int i = 0; i < 4; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
			printf("\n");
			break;
		default:
			break;
		}
	}

	do {
		gotoxy(55, 20);
		if (scanf_uint(&userInput) == -1)
			return 0;
		gotoxy(0, 20);
		clrLn();
	} while (userInput < 0);
	autoSaveInterval = userInput;

	if (prevValue != autoSaveInterval)
		configFileWrite();

	clr();
	drawMsgBox1();

	if (autoSaveInterval > 0) {
		snprintf(buffer, sizeof(buffer), "Autosave interval was changed to %u Seconds", autoSaveInterval);
	}
	else {
		strcpy(buffer, "Autosaves have been disabled");
		autoSaveInterval = UINT_MAX;
	}
	printBuffer(&msgBoxWidth, &colour);

	strcpy(buffer, "");
	return 0;
}

// </ Settings >

// < Save File Functions >

int saveMenu(void) {

	//gotoxy(1, 15);
	//printf(" ----------------------------------------------------------  ----------------------------------------------------------");
	//gotoxy(1, 22);
	//printf(" ----------------------------------------------------------  ----------------------------------------------------------");
	//gotoxy(1, 29);
	//printf(" ----------------------------------------------------------  ----------------------------------------------------------");

	char ch = 0;
	int saveNr = 1, colour = 0, boxWidth = 59, selection = 1;

	saveptr = fopen("Palindrome App\\saves\\save1.txt", "r");
	if (saveptr == NULL) return -1; //Could not open file

	gotoxy(0, 14);
	char menuName[] = "- - - Saves Menu - - -";
	pad = (width - (sizeof(menuName) - 1)) / 2;
	printf("%*.*s%s\n\n", pad, pad, " ", menuName);

	readSaveFileUntilComma();

	gotoxy(2, 16);
	snprintf(buffer, sizeof(buffer), "- Save %d - (%s)", saveNr, tempStr);
	printBuffer(&boxWidth, &colour);

	strcpy(buffer, "Seed: ");
	readSaveFileUntilComma();
	strcat(buffer, tempStr);

	strcat(buffer, " - Number Length: ");
	readSaveFileUntilComma();
	strcat(buffer, tempStr);
	gotoxy(2, 17);
	printBuffer(&boxWidth, &colour);

	strcpy(buffer, "Elapsed Time: ");
	readSaveFileUntilComma();
	strcat(buffer, tempStr);
	strcat(buffer, " Seconds");
	gotoxy(2, 18);
	printBuffer(&boxWidth, &colour);

	strcpy(buffer, "Total Iterations: ");
	readSaveFileUntilComma();
	strcat(buffer, tempStr);
	gotoxy(2, 19);
	printBuffer(&boxWidth, &colour);

	strcpy(buffer, "Total Operations: ");
	readSaveFileUntilComma();
	strcat(buffer, tempStr);
	gotoxy(2, 20);
	printBuffer(&boxWidth, &colour);
	strcpy(buffer, "");

	for (int i = 0; i < sizeof(tempStr); i++) // Wipe tempStr
		tempStr[i] = 0;

	fclose(saveptr);

	gotoxy(2, 30);
	printf("Press /Esc/ to quit");
	//gotoxy(43, 30);
	//printf("<- Previous Page || Next Page ->");
	gotoxy(90, 30);
	printf("Press /Enter/ to load the save");

	gotoxy(12, 16);
	printf(">>");

	do {
		ch = _getch();

		switch (ch) {
		case 13: // Enter for confirmation
			break;

		case 27:
			clr(); // Esc
			return 0;

		case 72: // Up arrow and W for menu navigation (future feature)
		case 119:
			if (selection > 2)
				selection -= 2;
			break;

		case 77: // Right arrow and D for menu navigation (future feature)
		case 100:
			if (selection % 2 == 1)
				selection += 1;
			break;

		case 80: // Down arrow and S for menu navigation (future feature)
		case 115:
			if (selection < 3)
				selection += 2;
			break;

		case 75: // Left arrow and A for menu navigation (future feature)
		case 97:
			if (selection % 2 == 0)
				selection -= 1;
			break;

		default:
			break;
		}
		selection = 1; // temporary line

	} while (ch != 13);

	clr();
	readSaveFileData();
	resumeCalculationFromSaveFile();
	singleNumberCalculation();

	return 0;
}

int readSaveFileUntilComma(void) {

	char ch;

	for (int i = 0; i < sizeof(tempStr); i++) // Wipe tempStr
		tempStr[i] = 0;

	for (int i = 0; i < sizeof(tempStr); i++) {
		ch = fgetc(saveptr);
		if (ch == ',') {
			ch = fgetc(saveptr);
			break;
		}
		tempStr[i] = ch;
	}
	return 0;
}

int readSaveFileData(void) {

	saveptr = fopen("Palindrome App\\saves\\save1.txt", "r");
	if (saveptr == NULL) return NULL; //Could not open file
	//fseek(saveptr, 0, SEEK_END);
	//long f_size = ftell(saveptr);
	//fseek(saveptr, 0, SEEK_SET);

	char ch = 0;
	int temp = 0;
	int value = 0;
	int decimalPlaces = 0;
	double result = 0;
	long long num = 0;
	long long fraction = 0;
	bool isWhole = true;
	while ((ch = fgetc(saveptr)) != EOF) {
		if (ch == '.') {
			isWhole = false;
		}
		else if (ch > 47 && ch < 58) {
			if (isWhole == true) {
				num = num * 10 + ch - 48;
			}
			if (isWhole == false) {
				fraction = fraction * 10 + (ch - 48);
				decimalPlaces++;
			}
		}
		else if (ch == ',' || ch == ';') {
			if (isWhole == false) {
				result = fraction;
				for (int i = decimalPlaces; i > 0; i--) {
					result /= 10;
				}
				result += num;
			}
			switch (value)
			{
			case 1:
				seed = num;
				break;
			case 2:
				digits = num;
				break;
			case 3:
				LoadElapsedTime = result;
				break;
			case 4:
				iterations = num;
				break;
			case 5:
				operations = num;
				break;
			case 6:
				arrayLength = num;
				break;
			default:
				break;
			}
			num = 0;
			fraction = 0;
			decimalPlaces = 0;
			isWhole = true;
			value++;
			if (ch == ';')
				break;
		}
	}
	(ch = fgetc(saveptr)); //Gets rid of an extra whitespace

	long long arrayValue = 0;
	int i;

	for (i = arrayMax - 1; i != arrayMax - arrayLength; i--) {

		for (int j = 0; j < 18; j++) {
			(ch = fgetc(saveptr));
			arrayValue *= 10;
			arrayValue += ch - 48;
		}
		Number[i] = arrayValue;
		arrayValue = 0;
	}

	while ((ch = fgetc(saveptr)) != EOF) {
		arrayValue *= 10;
		arrayValue += ch - 48;
	}
	Number[i--] = arrayValue;

	fclose(saveptr);

	gotoxy(2, 26);
	return 0;
}

int resumeCalculationFromSaveFile(void) {

	palindrome = false;

	long long divider = pow(10, 17);
	int temp = Number[arrayMax - arrayLength];
	digits = arrayLength * 18;

	while (temp / divider == 0) { //Checks digits number

		divider /= 10;
		digits--;
	}

	cutoffArray = (arrayMax - 1) - ((limit - 1) / 18);
	cutoffValue = pow(10, ((limit - 1) % 18));

	return 0;
}

// < /Save File Functions >

// < Palindrome >

int setup(void) { //This is for initialising the mirror loop

	iterations = 0;
	operations = 0;
	palindrome = false;
	arrayLength = 1;

	digits = 17;

	long long divider = pow(10, 17);

	while (Number[arrayMax - arrayLength] / divider == 0) { //Checks digits number

		divider /= 10;
		digits--;
	}

	cutoffArray = (arrayMax - 1) - ((limit - 1) / 18);
	cutoffValue = pow(10, ((limit - 1) % 18));

	return 0;
}

int singleNumberCalculationSetup(void) {

	long long userInput = 0;

	char line1[] = "Enter the number that you want to test. It must be smaller than 1 Quintillion (10^18)";
	pad = (width - (sizeof(line1) - 1)) / 2;
	printf("%*.*s%s\n", pad, pad, " ", line1);

	gotoxy(55, 17);

	do {
		gotoxy(55, 20);
		if (scanf_ullong(&userInput) == -1) {
			clr();
			mainMenu();
			return 0;
		}
		gotoxy(0, 20);
		clrLn();
	} while (userInput < 1);
	seed = userInput;

	clr();

	Number[arrayMax - 1] = seed;

	setup();
	singleNumberCalculation();
	return 0;
}

int singleNumberCalculation(void) { //This is for testing a single number

	bool autosavePrt = false;
	long long n = limit;
	int num = 0, colour = 0;
	char ch = 0;

	// Print text that will remain on screen until the calculation is done or cancelled
	gotoxy(0, 14);
	strcpy(buffer, "The input number is ");
	numberSpacer(&seed);
	strcat(buffer, ", we are searching for a palindrome");
	printBuffer(&width, &colour);
	gotoxy(0, 15);
	strcpy(buffer, "The program will calculate until the number becomes palindromic, or becomes ");
	numberSpacer(&n);
	strcat(buffer, " digits long");
	printBuffer(&width, &colour);

	if (outputMode == 1)
		gotoxy(3, 28);
	else
		gotoxy(3, 26);
	printf("The number is currently           digits long");

	if (outputMode == 4)
	{
		gotoxy(52, 24);
		printf("Iterations/sec:");
		gotoxy(86, 24);
		printf("Operations/sec:");
	}
	if (outputMode >= 3)
	{
		gotoxy(52, 26);
		printf("Total Iterations:");
		gotoxy(84, 26);
		printf("Total Operations:");
	}
	gotoxy(3, 30);
	printf("Elapsed time:                seconds");
	gotoxy(43, 30);
	printf("Total Time:                seconds");

	if (maxTime != 0)
	{
		gotoxy(81, 30);
		printf("Remaining Time:                seconds");
	}

	gotoxy(98, 18);
	printf("Press "GREEN"Esc"RESET" to Quit");
	gotoxy(98, 19);
	printf("Press "GREEN"E"RESET" to Export");
	gotoxy(99, 20);
	printf("Press "GREEN"S"RESET" to Save");

	elapsedTime = 0;
	lastUpdateTime = 0;

	float avgIterationsPerSecond = 0;
	float avgOperationsPerSecond = 0;

	int iterationsSinceUpdate[avgArraySize] = { 0 };
	int operationsSinceUpdate[avgArraySize] = { 0 };

	int avgDivider = 0;
	double timeDiff = 0;

	start = clock();

	while (Number[cutoffArray] < cutoffValue)
	{
		mirror();

		iterationsSinceUpdate[0]++;
		operationsSinceUpdate[0] += (arrayLength / 18) + 4;

		operations += (arrayLength / 18) + 4;

		end = clock();
		elapsedTime = ((float)(end - start)) / CLOCKS_PER_SEC;

		if (palindrome == true)
			break;

		if (totalElapsedTime >= maxTime && maxTime != 0)
			break;

		if (elapsedTime >= maxTime && maxTime != 0)
			break;

		if (elapsedTime - lastUpdateTime > outputFrequency)
		{
			totalElapsedTime = elapsedTime + LoadElapsedTime;

			if (outputMode == 1)
				gotoxy(27, 28);
			else
				gotoxy(27, 26);
			printf("%9d", digits + 1); //Prints number length

			if (outputMode == 4)
			{
				//Calculates avg iterations and operations
				avgIterationsPerSecond = 0;
				avgOperationsPerSecond = 0;

				if (avgDivider < avgArraySize)
					avgDivider++;

				for (int i = (avgDivider - 1); i != 0; i--)
				{
					iterationsSinceUpdate[i] = iterationsSinceUpdate[i - 1];
					operationsSinceUpdate[i] = operationsSinceUpdate[i - 1];
					avgIterationsPerSecond += iterationsSinceUpdate[i];
					avgOperationsPerSecond += operationsSinceUpdate[i];
				}
				avgIterationsPerSecond += iterationsSinceUpdate[0];
				avgOperationsPerSecond += operationsSinceUpdate[0];
				avgIterationsPerSecond /= avgDivider;
				avgOperationsPerSecond /= avgDivider;

				timeDiff = (1 / (elapsedTime - lastUpdateTime));

				gotoxy(67, 24); // Prints average iterations/second
				printf("%13.3lf", (avgIterationsPerSecond * timeDiff));
				gotoxy(101, 24); // Prints average operations/second
				printf("%18.3lf", (avgOperationsPerSecond * timeDiff));

				iterationsSinceUpdate[0] = 0;
				operationsSinceUpdate[0] = 0;
			}

			if (outputMode >= 3)
			{
				gotoxy(69, 26);
				printf("%11lld", iterations);
				gotoxy(104, 26);
				printf("%15lld", operations);
			}

			if (outputMode >= 2)
			{
				gotoxy(0, 28);
				percent = ((float)digits / limit) * 100;

				progressBar();
			}

			gotoxy(17, 30);
			printf("%14.3lf", elapsedTime);
			gotoxy(55, 30);
			printf("%14.3lf", totalElapsedTime);

			if (maxTime != 0)
			{
				gotoxy(97, 30);
				printf("%14.3lf", maxTime - totalElapsedTime);
			}

			lastUpdateTime = elapsedTime;
		}

		if (clock() - update > 99)
		{
			update = clock();
			float timeUntilAutosave = lastSaveTime - elapsedTime + autoSaveInterval;
			if (timeUntilAutosave < 15 && timeUntilAutosave > 0)
			{
				gotoxy(8, 17);
				printf(YELLOW"Autosave in %4.01f Seconds"RESET, lastSaveTime - elapsedTime + autoSaveInterval);
				autosavePrt = true;
			}
			else if (autosavePrt == true)
			{
				gotoxy(8, 17);
				printf("                        ");
				autosavePrt = false;
			}

			inputCheck(&ch);
			switch (ch)
			{
			case 27: // Esc
				clr();
				mainPrint();
				drawMsgBox1();
				char quit[] = "Quitting calculation";
				pad = ((msgBoxWidth - sizeof(quit)) / 2);
				printf("%*.*s%s", pad, pad, " ", quit);
				return 0;

			case 101: // E
				gotoxy(50, 17);
				printf("Exporting in Progress");
				writeTxtResult();
				gotoxy(50, 17);
				printf("                     ");
				break;

			case 115: // S
				gotoxy(13, 17);
				printf("                        ");
				autosavePrt = false;
				gotoxy(51, 17);
				printf("Saving in Progress");
				saveTxtFile();
				gotoxy(51, 17);
				printf("                  ");
				lastSaveTime = elapsedTime;
				break;

			default:
				break;
			}
			if (elapsedTime - lastSaveTime >= autoSaveInterval && autoSaveInterval != 0)
			{
				gotoxy(13, 17);
				printf("                        ");
				autosavePrt = false;
				gotoxy(50, 17);
				printf("Autosaving in Progress");
				saveTxtFile();
				gotoxy(50, 17);
				printf("                      ");
				lastSaveTime = elapsedTime;
			}
			ch = 0;
		}

		if (++iterations >= maxIterations && maxIterations != 0)
			break;
	}

	end = clock();
	elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;
	totalElapsedTime = elapsedTime + LoadElapsedTime;
	singleResult();

	return 0;
}

int singleResult(void) {

	clr();
	gotoxy(0, 14);

	bool save = false, export = false, resume = false, returnToMenu, print = false;
	int colour = 0;
	long long n;

	strcpy(buffer, "The starting number was ");
	numberSpacer(&seed);
	strcat(buffer, " and it became ");
	digits++;
	n = digits;
	numberSpacer(&n);
	strcat(buffer, " digits long.");
	printBuffer(&width, &colour);
	printf("\n");

	if (palindrome == true) {
		strcpy(buffer, "The number became palindrome after ");
		n = iterations;
		numberSpacer(&n);
		strcat(buffer, " iterations.");
	}
	else if (digits >= limit) {
		strcpy(buffer, "The number didn't become palindrome before it became ");
		n = digits;
		numberSpacer(&n);
		strcat(buffer, " digits long.");
	}
	else if (iterations >= maxIterations) {
		strcpy(buffer, "The number didn't become palindrome even after ");
		n = iterations;
		numberSpacer(&n);
		strcat(buffer, " iterations.");
	}
	else {
		strcpy(buffer, "The number didn't become palindrome before the given time limit.");
	}
	printBuffer(&width, &colour);
	printf("\n");

	strcpy(buffer, "This calculation took ");
	prtTime();
	printf("\n");

	if (palindrome == false && digits > 250) {
		printf("\n");
		strcpy(buffer, "There is a good chance it's a Lychrel number, a number that never becomes a palindrome.");
		printBuffer(&width, &colour);
		printf("\n");
	}
	if (digits <= 960) {
		printf("\n ----------------------------------------------------------------------------------------------------------------------\n\n");
		strcpy(buffer, "The final number is:");
		printBuffer(&width, &colour);
		printf("\n\n");
		strcpy(buffer, "");
		printf("%lld", Number[arrayMax - arrayLength]);
		for (int i = arrayMax - arrayLength + 1; i < arrayMax; i++)
			printf("%018lld", Number[i]);
		printf("\n\n ----------------------------------------------------------------------------------------------------------------------\n\n");
	}
	else {
		printf("\n");
		strcpy(buffer, "The number isn't printed out automatically, it's rather long, you know");
		printBuffer(&width, &colour);
		printf("\n");
	}
	if (rand() % 8 == 0) {
		strcpy(buffer, "It is theorised that all numbers will become palindrome");
		printBuffer(&width, &colour);
		printf("\n");
		strcpy(buffer, "if you repeat the mirroring and adding process over and over.");
		printBuffer(&width, &colour);
		printf("\n");
		strcpy(buffer, "However the number 196 is especially interesting, since it might never become one!");
		printBuffer(&width, &colour);
		printf("\n\n");
	}

	singleResultInputPrint(&save, &export, &print);

	char ch = 0;

	do {
		ch = _getch();
		printf("\033[120D\033[1A");
		clrLn();
		returnToMenu = true;
		switch (ch) {
		case 83: // S
		case 115:
			if (save == true) {
				if (palindrome == false)
					strcpy(buffer, "Save file has already been overwritten");
				else
					strcpy(buffer, "Save file wasn't overwritten, calculation is complete");
			}
			else if (palindrome == false) {
				saveTxtFile();
				strcpy(buffer, "Save data was written to Save 1");
			}
			else
				strcpy(buffer, "Save file wasn't overwritten, calculation is complete");
			printBuffer(&width, &colour);
			returnToMenu = false;
			save = true;
			break;

		case 69: // E
		case 101:
			if (export == true) {
				strcpy(buffer, "Results file has already been exported");
			}
			else {
				writeTxtResult();
				strcpy(buffer, "Done! You can find the txt file for the final number and some stats in the results folder.");
			}
			printBuffer(&width, &colour);
			returnToMenu = false;
			export = true;
			break;

		case 80: // P
		case 112:
			if (print == false) {
				printf(" ----------------------------------------------------------------------------------------------------------------------\n\n");
				strcpy(buffer, "The final number is:");
				printBuffer(&width, &colour);
				printf("\n\n");
				strcpy(buffer, "");
				printf("%lld", Number[arrayMax - arrayLength]);
				for (int i = arrayMax - arrayLength + 1; i < arrayMax; i++)
					printf("%018lld", Number[i]);
				printf("\n\n ----------------------------------------------------------------------------------------------------------------------\n\n");
				singleResultInputPrint(&save, &export, &print);
			}
			else
				strcpy(buffer, "Results have already been printed. Look at the results text file instead!");
			printBuffer(&width, &colour);
			print = true;
			returnToMenu = false;
			break;

		case 32: // Space - resume calculation
			limit = 0, maxIterations = 0, maxTime = 0;
			printf(" ----------------------------------------------------------------------------------------------------------------------\n");
			mainPrint();
			mainSettingsPrint();
			cutoffArray = (arrayMax - 1) - ((limit - 1) / 18);
			cutoffValue = pow(10, ((limit - 1) % 18));
			LoadElapsedTime = elapsedTime;
			elapsedTime = 0;
			singleNumberCalculation();
			return 0;

		default:
			break;
		}

	} while (returnToMenu == false);

	if (outputToTXT == true && export == false) {
		writeTxtResult();
	}

	clrLn();
	printf("\033[1A ----------------------------------------------------------------------------------------------------------------------\n");

	strcpy(buffer, "");
	mainMenu();
	return 0;
}

int singleResultInputPrint(bool* sav, bool* exp, bool* prt) {

	bool save = *sav, export = *exp, print = *prt;
	int colour = 0;

	printf("\033[120D");
	if (palindrome == false && save == false) {
		strcpy(buffer, "Press 'S' to save the calculation to Save 1");
		printBuffer(&width, &colour);
		printf("\n");
	}
	if (outputToTXT == false && export == false) {
		strcpy(buffer, "Press 'E' to export the results to a txt file'");
		printBuffer(&width, &colour);
		printf("\n");
	}
	if (print == false) {
		strcpy(buffer, "Press 'P' to print the entire number. Might take a while");
		printBuffer(&width, &colour);
		printf("\n");
	}
	strcpy(buffer, "Press 'Space' to continue the calculation");
	printBuffer(&width, &colour);
	printf("\n");
	strcpy(buffer, "Press any other button to go to the Main Menu");
	printBuffer(&width, &colour);
	printf("\n\n\n\033[1A");
	strcpy(buffer, "");

	return save, export, print;
}

int range(void) { //This is for testing a range of numbers

	char ch = 0;
	int colour = 0;
	long long userInput = 0, Lychrel = 0;
	totalElapsedTime = 0;

	gotoxy(0, 14);
	strcpy(buffer, "Enter the smallest value that you want to test:");
	printBuffer(&width, &colour);
	do {
		gotoxy(0, 16);
		clrLn();
		gotoxy(55, 16);
		if (scanf_ullong(&userInput) == -1) {
			clr();
			mainMenu();
			return 0;
		}
	} while (userInput < 1);
	iMin = userInput;
	long long minValue = iMin - 1;

	gotoxy(0, 18);
	strcpy(buffer, "Enter the biggest value that you want to test:");
	printBuffer(&width, &colour);
	do {
		gotoxy(0, 20);
		clrLn();
		gotoxy(55, 20);
		if (scanf_ullong(&userInput) == -1) {
			clr();
			mainMenu();
			return 0;
		}
	} while (userInput < 1);
	iMax = userInput;
	clr();

	total = clock();

	while (iMin <= iMax) {

		palindrome = false;

		Number[33554431] = iMin;

		setup();

		lastUpdateTime = 0;

		start = clock();

		gotoxy(0, 30);
		printf(" - - Testing %lld.", iMin);
		gotoxy(94, 30);
		printf("Total time: %11.3lf s", totalElapsedTime);

		while (Number[cutoffArray] < cutoffValue) {

			mirror();

			if (palindrome == true)
				break;

			end = clock();

			totalElapsedTime = ((float)(end - total)) / CLOCKS_PER_SEC;
			elapsedTime = ((float)(end - start)) / CLOCKS_PER_SEC;

			if (elapsedTime >= maxTime && maxTime != 0)
				break;

			if (elapsedTime - lastUpdateTime > outputFrequency) {

				gotoxy(34, 30);
				printf("Number length: %9d Digits", digits);
				gotoxy(67, 30);
				printf("Elapsed time: %9.3lf s", elapsedTime);
				gotoxy(106, 30);
				printf("%11.3lf s", totalElapsedTime);

				lastUpdateTime = elapsedTime;
			}

			iterations++;

			inputCheck(&ch);
			switch (ch)
			{
			case 27: // Esc
				gotoxy(34, 30);
				printf("                                                                                     \n\n");

				strcpy(buffer, "Calculation stopped");
				printBuffer(&width, &colour);
				printf("\n\n");
				strcpy(buffer, "");

				numberSpacer(&Lychrel);
				strcat(buffer, " out of the ");
				long long n = iMin - minValue - 1;
				numberSpacer(&n);
				strcat(buffer, " tested numbers didn't become a palindrome.");
				printBuffer(&width, &colour);
				printf("\n");

				strcpy(buffer, "These calculations took ");
				prtTime();
				printf("\n\n");
				pressToContinue();
				printf("\n ----------------------------------------------------------------------------------------------------------------------\n");
				mainMenu();
				return 0;
			default:
				break;
			}
			ch = 0;

			if (iterations >= maxIterations && maxIterations != 0)
				break;
		}

		gotoxy(34, 30);
		printf("                                                                                     ");
		gotoxy(0, 30);

		if (palindrome == true) {

			printf(" %lld became a palindrome number after %lld iterations.\n", iMin, iterations);
			printf("  - The resulting number is: ");
		}
		else {

			printf(" %lld did not became a palindrome number after %lld iterations. The number became %d digits long.\n", iMin, iterations, (digits + 1));
			printf("  - - The resulting number is: ");
			Lychrel++;
		}
		for (int i = arrayMax - arrayLength; i < arrayMax; i++) { //outputs all of Number[] array when the code is done running

			printf("%lld", Number[i]);
		}
		printf("\n\n");

		iMin++;

		for (int i = arrayMax - arrayLength; i < arrayMax; i++) { //Nullifies all the values in Number[] and Mirror[] array

			Number[i] = 0;
			Mirror[i] = 0;
		}
	}
	strcpy(buffer, "Calculations complete, ");
	numberSpacer(&Lychrel);
	strcat(buffer, " out of the ");
	long long n = iMax - minValue;
	numberSpacer(&n);
	strcat(buffer, " tested numbers didn't become a palindrome.");
	printBuffer(&width, &colour);
	printf("\n");

	strcpy(buffer, "These calculations took ");
	prtTime();
	printf("\n\n");

	strcpy(buffer, "");
	pressToContinue();
	printf("\n ----------------------------------------------------------------------------------------------------------------------\n\n");
	mainMenu();
	return 0;
}

int mirror(void) { //Mirroring algorithm, best to look away tbh

	int firstHalf, secondHalf, firstHalfFlipped, secondHalfFlipped;
	int arrayLocation = arrayMax - arrayLength;
	int arrayGrab = arrayMax - 1;

	long long temp = digits - (arrayLength - 1) * 18 + 1;
	long long mult = pow(10, (18 - temp));
	long long modulus = pow(10, (temp));

	//Mirrors the Number[] array and inputs the results into Mirror[] array
	for (int i = arrayLength; i > 0; i--) {

		temp = (Number[arrayGrab] % modulus * mult) + (Number[arrayGrab + 1] / modulus);

		firstHalf = temp / 1000000000;
		secondHalf = temp % 1000000000;

		secondHalfFlipped = (secondHalf % 10) * 100000000;
		secondHalf /= 10;

		secondHalfFlipped += (secondHalf % 10) * 10000000;
		secondHalf /= 10;

		secondHalfFlipped += (secondHalf % 10) * 1000000;
		secondHalf /= 10;

		secondHalfFlipped += (secondHalf % 10) * 100000;
		secondHalf /= 10;

		secondHalfFlipped += (secondHalf % 10) * 10000;
		secondHalf /= 10;

		secondHalfFlipped += (secondHalf % 10) * 1000;
		secondHalf /= 10;

		secondHalfFlipped += (secondHalf % 10) * 100;
		secondHalf /= 10;

		secondHalfFlipped += ((secondHalf % 10) * 10) + (secondHalf / 10);

		firstHalfFlipped = (firstHalf % 10) * 100000000;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 10000000;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 1000000;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 100000;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 10000;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 1000;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 100;
		firstHalf /= 10;

		firstHalfFlipped += (firstHalf % 10) * 10 + (firstHalf / 10);

		Mirror[arrayLocation] = secondHalfFlipped;
		Mirror[arrayLocation] *= 1000000000;
		Mirror[arrayLocation] += firstHalfFlipped;

		arrayLocation++;
		arrayGrab--;

	}
	temp = arrayMax - arrayLength; //new use of temp variable

	//Checks if the number is palindrome
	for (int i = arrayMax - 1; i >= temp; i--) {

		if (Number[i] != Mirror[i]) {

			palindrome = false;
			break;
		}
		else {

			palindrome = true;
		}
	}

	//If the number is not palindrome it ands the mirrored and original number and wipes Mirror[] array
	if (palindrome == false) {

		for (int i = arrayMax - 1; i > arrayGrab; i--) { //adds the number together and nullifies Mirror[] array

			Number[i] += Mirror[i];
			Number[i - 1] += Number[i] / 1000000000000000000;
			Number[i] %= 1000000000000000000;
			Mirror[i] = 0;
		}
	}

	temp = (pow(10, ((digits % 18) + 1)));

	//Finally we ckeck if the number increased in size
	if (Number[arrayGrab + 1] / temp > 0) {

		digits++;
	}

	else if (Number[(arrayMax - 1) - arrayLength] > 0) {

		arrayLength++;
		digits++;
	}
	return 0;
}

int progressBar(void) {

	roundPercent = (int)(percent + 0.333333);

	printf("Progress:%6.2f %% [", percent);
	printf("%.*s", roundPercent, "####################################################################################################");

	gotoxy(120, 28);
	printf("]");

	return 0;
}

// </ Palindrome >

int details(void) {

	char arr[][120] = {
		"Palindrome Calculator - Initial release date: TBD",
		"Developer: Gabor - Online alias: Hydrough",
		"Contact me - GitHub: Hydrough - Discord: Hydrough#7165",
		"Hello, I am a Hungarian programmer and I started writing code a few years ago",
		"I started with Javascript and Java and made it over to the better language; C",
		"The making of this application was inspired by Numberphile who made an excellent video about palindromes!",
		"196 was the number I made this application for, and to test if it will ever become a palindrome",
		"This seemed like a nice and easy app to make, but here I am 7 months in now, so what can you do",
		"This has been a pretty fun project to work on and I hope you guys found this useful somehow",
		"Feel free to reach out to me regarding feedback, opinionsand bugs, I'll do my best to fix them",
	};

	for (int i = 0; i < 10; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 2:
		case 5:
		case 9:
			printf("\n");
			break;
		default:
			break;
		}
	}

	gotoxy(0, 29);
	pressToContinue();
	clr();
	mainPrint();
	return 0;
}

int plannedFeatures(void) {

	char arr[][120] = {
		"- - - Planned Features - - -",
		"These are some planned features for this app in no particular order:",
		"Introducing multithreading to speed up the program",
		"Including a graphing tool to output resuls in a non-text format",
		"Increasing the max range of calculations to over 1 billion digits",
		"Giving this program a UI instead of just having text on a grey screen",
	};

	for (int i = 0; i < 6; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
		case 1:
			printf("\n");
			break;
		default:
			break;
		}
	}

	gotoxy(0, 29);
	pressToContinue();
	clr();
	mainPrint();
	return 0;
}

int patchNotes(void) {

	char arr[][120] = {
		"- - - Patch Notes - - -",
		"- - Version 1.0.0 - - [Release] - (TBD)",
		"This version allows to calculate number up to 603 million digits",
		"The program can calculate 1 number or a range of numbers and stop calculations",
		"You can customise max number length, max iterations, max time and many more",
		"You can save and load calculations and export them into txt file for later use",
		"It's possible to set an autosave period in seconds, but there's only 1 save file for now",
	};

	for (int i = 0; i < 7; i++) {
		int pad = (width - strlen(arr[i]) - 1) / 2;
		printf("%*.*s%s\n", pad, pad, " ", arr[i]);
		switch (i) {
		case 0:
		case 1:
			printf("\n");
			break;
		default:
			break;
		}
	}

	gotoxy(0, 29);
	pressToContinue();
	clr();
	mainPrint();
	return 0;
}

// < Custom Functions >

int clrScr(void) {

	gotoxy(0, 0);
	for (int i = 0; i < 30; i++) {

		clrLn();
	}
	gotoxy(0, 0);

	return 0;
}

int clr(void) {

	gotoxy(0, 14);
	for (int i = 0; i < 17; i++) {
		clrLn();
	}

	gotoxy(0, 14);
	return 0;
}

int clrLn(void) {

	printf("                                                                                                                        ");
	return 0;
}

int drawMsgBox1(void) {

	char line1[] = "+----------------------------------------------------------------------+"; //70 dashes
	char line2[] = "|                                                                      |";

	gotoxy(0, 27);
	pad = (width - (sizeof(line1) - 1)) / 2;
	printf("%*.*s%s\n", pad, pad, " ", line1);
	pad = (width - (sizeof(line2) - 1)) / 2;
	printf("%*.*s%s\n", pad, pad, " ", line2);
	pad = (width - (sizeof(line1) - 1)) / 2;
	printf("%*.*s%s\n", pad, pad, " ", line1);

	gotoxy(pad + 2, 28);
	return 0;
}

int clrMsgBox(void) {

	for (int i = 27; i < 30; i++) {
		gotoxy(0, i);
		clrLn();
	}
	return 0;
}

int pressToContinue(void) {

	char line1[] = "Press any button to continue";
	pad = (width - (sizeof(line1) - 1)) / 2;
	printf("%*.*s%s\n", pad, pad, " ", line1);

	char ch = _getch();
	return 0;
}

int prtTime(void) {

	char str[10] = "";
	int colour = 0;
	long long sec = totalElapsedTime;
	float seconds = (totalElapsedTime - (int)totalElapsedTime) + (int)sec % 60;
	long long minutes = (sec / 60) % 60;
	long long hours = (sec / 3600) % 24;
	long long days = (sec / 86400) % 365;
	long long years = (sec / 31536000);

	if (years != 0) {
		numberSpacer(&years);
		strcat(buffer, " Year");
		if (years != 1)
			strcat(buffer, "s");
		strcat(buffer, " ");
	}
	if (days != 0) {
		numberSpacer(&days);
		strcat(buffer, " Day");
		if (days != 1)
			strcat(buffer, "s");
		strcat(buffer, " ");
	}
	if (hours != 0) {
		numberSpacer(&hours);
		strcat(buffer, " Hour");
		if (hours != 1)
			strcat(buffer, "s");
		strcat(buffer, " ");
	}
	if (minutes != 0) {
		numberSpacer(&minutes);
		strcat(buffer, " Minute");
		if (minutes != 1)
			strcat(buffer, "s");
		strcat(buffer, " ");
	}
	snprintf(str, sizeof(str), "%.03f", seconds);
	strcat(buffer, str);
	strcat(buffer, " Seconds.");
	printBuffer(&width, &colour);

	return 0;
}

int numberSpacer(long long* num) {

	// This fn takes a long long number, and adds it to the end of buffer after separating it and making it easier to read
	long long val = *num;

	char str[26] = "";
	char final[23] = "";
	int digits = 0;
	int location = 0;

	if (val == 0) {
		strcat(buffer, "0");
		return 0;
	}

	for (int i = 0; i < sizeof(str); i++) {
		if (val == 0)
			str[i] = 0;
		else {
			str[i] = (val % 10) + 48;
			val /= 10;
			digits++;
		}
	}
	for (int i = 19; i > 2; i--) {
		str[i + (i / 3)] = str[i];
		str[i] = ' ';
	}
	digits = digits + ((digits - 1) / 3) - 1;
	for (int i = digits; i >= 0; --i) {
		final[location] = str[i];
		location++;
	}
	strcat(buffer, final);
	return 0;
}

int configFileWrite(void) {

	configptr = fopen("Palindrome App\\config.txt", "w");

	if (limit <= 0 || limit >= 603979750)
		fprintf(configptr, "MaxDigits: 0\n");
	else
		fprintf(configptr, "MaxDigits: %d\n", limit);

	if (maxIterations <= 0 || maxIterations >= 10000000000000)
		fprintf(configptr, "MaxIteraions: 0\n");
	else
		fprintf(configptr, "MaxIteraions: %lld\n", maxIterations);

	if (maxTime <= 0 || maxTime >= 1576800000)
		fprintf(configptr, "MaxTime: 0\n");
	else
		fprintf(configptr, "MaxTime: %lld\n", maxTime);

	fprintf(configptr, "OutputMode: %d\n", outputMode);
	fprintf(configptr, "OutputFrequency: %d\n", outputFrequencySetting);
	fprintf(configptr, "Output_To_TXT: %d\n", outputToTXT);

	if (autoSaveInterval == 0 || autoSaveInterval == UINT_MAX)
		fprintf(configptr, "Autosave_Interval: 0\n");
	else
		fprintf(configptr, "Autosave_Interval: %u", autoSaveInterval);

	fclose(configptr);

	return 0;
}

int writeTxtResult(void) {

	resultptr = fopen("Palindrome App\\results\\result.txt", "w");

	fprintf(resultptr, " - Seed: %lld\n", seed);
	fprintf(resultptr, " - Number Length: %d Digits\n", digits);
	fprintf(resultptr, " - Total Iterations: %lld\n", iterations);
	fprintf(resultptr, " - Total Operations: %lld\n", operations);
	fprintf(resultptr, " - Total Time: %.3lf Seconds\n", totalElapsedTime);

	fprintf(resultptr, "\n- - Final Number:\n");

	fprintf(resultptr, "%lld", Number[arrayMax - arrayLength]);

	for (int i = arrayMax - arrayLength + 1; i != arrayMax; i++) {

		fprintf(resultptr, "%018lld", Number[i]);
	}

	fclose(resultptr);
	return 0;
}

int saveTxtFile(void) {

	saveptr = fopen("Palindrome App\\saves\\save1.txt", "w");
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	fprintf(saveptr, "%d-%02d-%02d %02d:%02d:%02d, ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	fprintf(saveptr, "%lld, %d, %.3lf, %lld, %lld, %d; ", seed, digits, totalElapsedTime, iterations, operations, arrayLength);
	fprintf(saveptr, "%lld", Number[arrayMax - arrayLength]);

	for (int i = arrayMax - arrayLength + 1; i < arrayMax; i++) {

		fprintf(saveptr, "%018lld", Number[i]);
	}
	fclose(saveptr);
	return 0;
}

int inputCheck(char* ch) {

	if (_kbhit())
		*ch = _getch();
	return ch;
}

int scanf_uint(int* val) {

	char invalid[] = "Input value is too big!";
	char errMsgClr[] = "                           ";

	char ch = 0;
	int	length = 0;
	bool zero = false;
	long long tempVal = 0;
	unsigned int num = 0;
	do {
		ch = _getch();
		if (ch > 48 && ch < 58 || ch == 48 && length > 0) {
			if (zero == true) {
				printf("\033[1D \033[1D");
				zero = false;
			}
			if (length < 10) {
				tempVal *= 10;
				tempVal += ch - 48;
				printf("%d", (ch - 48));
				length++;
			}
		}
		else if (ch == 48 && zero == false) {
			zero = true;
			printf("0");
		}
		switch (ch) {
		case 8: // Backspace
			if (length == 0 && zero == true) {
				zero = false;
				printf("\033[1D \033[1D");
			}
			if (length > 0) {
				tempVal /= 10;
				printf("\033[1D \033[1D");
				length--;
			}
			break;
		case 27: // Esc
			return -1;
		default:
			break;
		}
		if (tempVal > UINT_MAX) {
			printf("\033[%dD"YELLOW"%lld"RESET" - %s\033[%lldD", length, tempVal, invalid, sizeof(invalid) + 2);
		}
		else if (length > 0) {
			printf("\033[%dD%lld%s\033[%lldD", length, tempVal, errMsgClr, sizeof(errMsgClr) - 1);
		}
		if (ch == 13 && tempVal > UINT_MAX) {
			tempVal = -1;
		}
	} while (ch != 13);
	num = tempVal;
	*val = num;
}

int scanf_ullong(long long* val) {

	char ch = 0;
	int	length = 0;
	bool zero = false;
	long long tempVal = 0;
	long long num = 0;
	do {
		ch = _getch();
		if (ch > 48 && ch < 58 || ch == 48 && length > 0) {
			if (zero == true) {
				printf("\033[1D \033[1D");
				zero = false;
			}
			if (length < 18) {
				tempVal *= 10;
				tempVal += ch - 48;
				printf("%d", (ch - 48));
				length++;
			}
		}
		else if (ch == 48 && zero == false) {
			zero = true;
			printf("0");
		}
		switch (ch) {
		case 8: // Backspace
			if (length == 0 && zero == true) {
				zero = false;
				printf("\033[1D \033[1D");
			}
			if (length > 0) {
				tempVal /= 10;
				printf("\033[1D \033[1D");
				length--;
			}
			break;
		case 27: // Esc
			return -1;
		default:
			break;
		}
	} while (ch != 13);
	num = tempVal;
	*val = num;
}

int printBuffer(int* width, int* colour) {
	int num = strlen(buffer);
	int pad = (*width - num) / 2;
	printf("%*.*s", pad, pad, " ");
	for (int i = 0; i < sizeof(buffer); i++) {
		if (buffer[i] == '\0') {
			break;
		}
		if (buffer[i] != ':')
			printf("%c", buffer[i]);
		else
			printf("%c\x1B[%dm", buffer[i], *colour);
	}
	printf(RESET);
	return 0;
}

// </ Custom Functions >