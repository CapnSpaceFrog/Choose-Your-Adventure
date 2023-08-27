#include "ConsoleDisplay.h"
#include "Gameplay.h"
#include "Exceptions.h"
#include <fstream>
#include <ctime>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO cInfo;

int const CIN_IGNORE_LIMIT = 25;

//Some windows specific code to get information about the window.
void consoleHandleSetup()
{
   HWND wConsole = GetConsoleWindow();
   RECT r;
   GetWindowRect(wConsole, &r);

   GetConsoleScreenBufferInfo(hConsole, &cInfo);
}

//Draws the border of the display area within the console window.
void drawBorder()
{
   //Top of the screen
   SetConsoleCursorPosition(hConsole, { 0,0 });
   cout << "#";
   for (int i = 0; i < cInfo.srWindow.Right - 1; i++)
   {
      cout << "-";
   }
   cout << "#";

   //Bottom of the screen
   SetConsoleCursorPosition(hConsole, { 0, cInfo.srWindow.Bottom });
   cout << "#";
   for (int i = 0; i < cInfo.srWindow.Right - 1; i++)
   {
      cout << "-";
   }
   cout << "#";

   //Left side of screen
   for (int i = 0; i < cInfo.srWindow.Bottom - 1; i++)
   {
      SetConsoleCursorPosition(hConsole, { 0, (short)(i + 1) });
      cout << "|";
   }

   //Right side of screen
   for (int i = 0; i < cInfo.srWindow.Bottom - 1; i++)
   {
      SetConsoleCursorPosition(hConsole, { cInfo.srWindow.Right, (short)(i + 1) });
      cout << "|";
   }
}

//Display the main menu splash.
void displayMainMenu()
{
   string title = ">| CONSOLE BRAWLER (v1.0) |<";
   COORD cursor = { (cInfo.srWindow.Right - title.size()) / 2, (cInfo.srWindow.Bottom / 2) - 6 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << title << endl;

   string credits = "by Wyatt Llewellyn";
   cursor.X = (cInfo.srWindow.Right - credits.size()) / 2;
   cursor.Y += 1;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << credits << endl;

   string newGame = "[1] New Game";
   cursor.X = (cInfo.srWindow.Right - newGame.size()) / 2;
   cursor.Y += 8;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << newGame << endl;

   string loadGame = "[2] Load Game";
   cursor.Y += 1;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << loadGame << endl;

   string quit = "[3] Quit";
   cursor.Y += 1;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << quit << endl;
}

void displayMessage(string message)
{
   clearScreen();

   COORD cursor = { (cInfo.srWindow.Right - message.size()) / 2, cInfo.srWindow.Bottom / 2 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   message = "-+- [1] Continue -+-";
   cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
   cursor.Y = cInfo.srWindow.Bottom - 3;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   getInput("Continue [1]: ", 1, 1);
}

//given a range of values and a display message, prompt the user to input a value.
int getInput(string displayMessage, int min, int max)
{
   cin.clear();

   COORD cursor = { (cInfo.srWindow.Right - displayMessage.size()) / 2, cInfo.srWindow.Bottom - 1 };
   int response;
   do
   {
      SetConsoleCursorPosition(hConsole, cursor);
      cout << displayMessage;
      if (!(cin >> response))
      {
         cin.clear();
         cin.ignore(CIN_IGNORE_LIMIT, '\n');
      }

      clearRow(cursor.Y);
      drawBorder();

   } while (response < min || response > max);

   cin.ignore(CIN_IGNORE_LIMIT, '\n');

   return response;
}

//Display a message, retrieve a string input.
string getInput(string displayMessage)
{
   cin.clear();

   COORD cursor = { (cInfo.srWindow.Right - displayMessage.size()) / 2, cInfo.srWindow.Bottom - 2 };
   string response;

   SetConsoleCursorPosition(hConsole, cursor);
   cout << displayMessage << endl;
   cursor.Y++;

   SetConsoleCursorPosition(hConsole, cursor);
   getline(cin, response);

   clearRow(cursor.Y);
   clearRow(cursor.Y - 1);

   return response;
}

//Clears the entire screen within the border.
void clearScreen()
{
   for (short x = 1; x < cInfo.srWindow.Right; x++)
   {
      for (short y = 1; y < cInfo.srWindow.Bottom; y++)
      {
         SetConsoleCursorPosition(hConsole, { x, y });
         cout << " ";
      }
   }
}

//Clears a specific row within the border.
void clearRow(int row)
{
   for (short x = 1; x < cInfo.srWindow.Right; x++)
   {
      SetConsoleCursorPosition(hConsole, { x, (short)row });
      cout << " ";
   }
}

//Deletes the old debug file so we can re-write to a fresh file.
void refreshDebugFile()
{
   remove("Logs/log.txt");
}

//Clears the screen, and quits the application.
void displayQuit()
{
   clearScreen();

   string message = "Thanks for playing, bye!";
   COORD cursor = { (cInfo.srWindow.Right - message.size()) / 2, cInfo.srWindow.Bottom / 2 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   cursor.X = cInfo.srWindow.Right / 2;
   cursor.Y = cInfo.srWindow.Bottom + 1;
   SetConsoleCursorPosition(hConsole, cursor);
}