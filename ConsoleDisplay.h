#ifndef _ConsoleDisplay_
#define _ConsoleDisplay_

#include <Windows.h>
#include <iostream>
#include <string>

extern HANDLE hConsole;
extern CONSOLE_SCREEN_BUFFER_INFO cInfo;

void consoleHandleSetup();

void drawBorder();

void displayMainMenu();

void displayMessage(std::string message);

int getInput(std::string displayMessage, int min, int max);

std::string getInput(std::string displayMessage);

void clearScreen();

void clearRow(int row);

void refreshDebugFile();

void displayQuit();

#endif