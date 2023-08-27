#include "ConsoleDisplay.h"
#include "Exceptions.h"
#include "Entity.h"
#include <iostream>
#include <fstream>
using namespace std;

void prologue()
{
   clearScreen();

   string first = "You awake on the floor of a run-down mill.";
   COORD cursor = { (cInfo.srWindow.Right - first.size()) / 2, cInfo.srWindow.Top + 1 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << first << endl;

   string second = "You are unsure how you got here, or what you were doing beforehand.";
   cursor.X = (cInfo.srWindow.Right - second.size()) / 2;
   cursor.Y += 2;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << second << endl;

   string third = "Your satchel is empty, and there are no items on your person.";
   cursor.X = (cInfo.srWindow.Right - third.size()) / 2;
   cursor.Y += 2;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << third << endl;

   getInput("What do you value the most?");

   string classes = "[1] Warrior -+- [2] Rouge -+- [3] Wizard";
   cursor.X = (cInfo.srWindow.Right - classes.size()) / 2;
   cursor.Y = cInfo.srWindow.Bottom - 3;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << classes << endl;

   getInput("Please select a class [1-3]: ", 1, 3);
}

bool fight(Entity& enemy, Entity& player, bool& ran)
{
   clearScreen();

   //Display the Player & Entities health & combat information to the screen.
   COORD playerAnchor = { (cInfo.srWindow.Right / 2) - 25, (cInfo.srWindow.Bottom - 1) / 2 };
   COORD enemyAnchor = { (cInfo.srWindow.Right / 2) + 17 , (cInfo.srWindow.Bottom - 1) / 2 };
   enemy.drawHealthBarBorder(enemyAnchor);
   enemy.updateHealthBar(enemyAnchor);

   enemy.displayInfo(enemyAnchor);

   player.drawHealthBarBorder(playerAnchor);
   player.updateHealthBar(playerAnchor);

   player.displayInfo(playerAnchor);

   //Display the player choices.
   string message = "[1] Fight  ---+---  [2] (not implemented)  ---+---  [3] RUN!";
   COORD cursor = { (cInfo.srWindow.Right - message.size()) / 2, cInfo.srWindow.Bottom - 3 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   cursor.X = (cInfo.srWindow.Right - 20) / 2;
   cursor.Y = cInfo.srWindow.Top + 4;
   while (!enemy.isDead() || !player.isDead())
   {
      int playerSuffered = 0, enemySuffered = 0;

      string message;
      switch (getInput("What do you do [1-3]? ", 1, 3))
      {
      case 1:
         enemySuffered = enemy.takePaddedDamage(player.getAttackPower());
         playerSuffered = player.takePaddedDamage(enemy.getAttackPower());

         message = "You took " + to_string(playerSuffered) + " from " + enemy.getName() + ".";
         cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
         SetConsoleCursorPosition(hConsole, cursor);
         cout << message;
         break;

      case 2:
         break;

      case 3:
         //Do the dumb run stuff here
         srand(time(nullptr));
         int chance = rand() % 100;
         if (chance <= 75)
         {
            playerSuffered = player.takeFlatDamage(2);

            message = "You tried to run but fell over and took " + to_string(playerSuffered) + " damage.";
            cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
            SetConsoleCursorPosition(hConsole, cursor);
            cout << message;
         }
         else
         {
            ran = true;
            return true;
         }
         break;
      }

      //Pop up a line
      cursor.Y -= 1;

      message = "You dealt " + to_string(enemySuffered) + " to " + enemy.getName() + ".";
      cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
      SetConsoleCursorPosition(hConsole, cursor);
      cout << message;

      //Return to the original line so when we come back through the loop we display at the right line
      cursor.Y += 1;
      
      //Check if the player is dead or if the enemy is dead
      if (player.isDead())
      {
         //The Player did not win!
         return false;
      }
      else if (enemy.isDead())
      {
         //The Player won!
         return true;
      }

      if (enemySuffered > 0)
      {
         enemy.updateHealthBar(enemyAnchor);
      }

      if (playerSuffered > 0)
      {
         player.updateHealthBar(playerAnchor);
      }
   }
}

void shop()
{
   displayMessage("Shop is not implemented. Sorry!");
}

void epilogue()
{
   displayMessage("WIP");
}

string parseInfo(int& index, const string& info)
{
   string temp = "";

   for (int i = index; i < info.length(); i++)
   {
      if (info[i] == ';')
      {
         index = i + 1;
         break;
      }

      temp += info[i];
   }

   return temp;
}

void debug(string message)
{
   ofstream logFile;

   logFile.open("Logs/log.txt", fstream::app);

   if (!logFile.is_open())
   {
      throw FileFailureException("Failed to create new newlog.txt file.");
   }

   logFile << message << "\n";

   logFile.close();
}