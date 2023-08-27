#include "Exceptions.h"
#include "ConsoleDisplay.h"
#include "Gameplay.h"
#include "Location.h"
#include "Item.h"
#include "Entity.h"

#pragma region Declarations
LocationList* LOCATION_DATA;
ItemManager* ITEM_DATA;
EntityManager* ENTITY_DATA;
Player* PLAYER;
Location* CURRENT_LOCATION;

int const BASE_HEALTH = 10;
int const BASE_ATTACK_POWER = 4;
int const BASE_DEFENSE_POWER = 1;
int const REST_RECOVERY_RATE = 2;

bool dataInitialization();

void newGame();
bool loadGame();
void saveGame();

void postLocation();
void advanceLocation();

void gameOver(string message);
void quit();
#pragma endregion

int main()
{
   refreshDebugFile();
   consoleHandleSetup();

   if (!dataInitialization())
   {
      return 0;
   }

   drawBorder();
   displayMainMenu();

   switch (getInput("Please input a selection [1-3]: ", 1, 3))
   {
      case 1:
         newGame();
         break;

      case 2:
         if (!loadGame())
         {
            newGame();
         }
         break;
   
      case 3:
         quit();
         break;
   }

#pragma region The Game Loop
   while (true)
   {
      //Display the current location we are at.
      displayLocationInfo(*CURRENT_LOCATION);
      COORD displayPoint = { (cInfo.srWindow.Right - 11) / 2, (cInfo.srWindow.Bottom - 7) / 2 };
      PLAYER->displayInfo(displayPoint);
      
      #pragma region Player Location Decision Handling
      //randomEnemy used in at least one place place later, possibly two.
      Entity* randomEnemy = nullptr;
      try
      {
         randomEnemy = ENTITY_DATA->getRandomEntity();
      }
      catch (NullDataException nullData)
      {
         debug(nullData.message);
         return 0;
      }

      bool ran = false;
      switch (handlePromptChoice(*CURRENT_LOCATION))
      {
      case GAIN_ITEM:
         PLAYER->addToInventory(ITEM_DATA->gainItem());
         displayMessage("You find a useable item, but attracted some unwanted attention.");
         break;

      case FIGHT:
         if (!fight(*randomEnemy, *PLAYER, ran))
         {
            delete randomEnemy;
            randomEnemy = nullptr;
            gameOver("You died!");
            continue;
         }
         delete randomEnemy;
         randomEnemy = nullptr;

         if (ran)
         {
            displayMessage("You run as fast as you can, not looking back for a second.");
            advanceLocation();
            continue;
         }
         else
         {
            PLAYER->addToInventory(ITEM_DATA->gainItem());
            displayMessage("As your adversary falls, you find an item, but another opponent emerges to fight.");
         }
         break;

      case SHOP:
         //NOT IMPLEMENTED
         shop();
         break;
      }
      #pragma endregion

      #pragma region End Of Location Fight
      if (randomEnemy == nullptr)
      {
         try
         {
            randomEnemy = ENTITY_DATA->getRandomEntity();
         }
         catch (NullDataException nullData)
         {
            debug(nullData.message);
            return 0;
         }
      }

      ran = false;
      if (!fight(*randomEnemy, *PLAYER, ran))
      {
         delete randomEnemy;
         randomEnemy = nullptr;
         gameOver("You died!");
         continue;
      }
      delete randomEnemy;
      randomEnemy = nullptr;
      
      if (ran)
      {
         displayMessage("You run as fast as you can, not looking back for a second.");
         advanceLocation();
         continue;
      }
      else
      {
         displayMessage("You have emerged victorious!");
         postLocation();
      }
      #pragma endregion
   }
#pragma endregion

   return 0;
}

bool dataInitialization()
{
   try
   {
      LOCATION_DATA = loadLocationContent();

      if (LOCATION_DATA == nullptr)
      {
         debug("Invalid data read from LocationInfo.txt");
         return false;
      }
   }
   catch (FileFailureException fileException)
   {
      debug(fileException.message);
      return false;
   }

   try
   {
      ITEM_DATA = loadItemContent();

      if (ITEM_DATA == nullptr)
      {
         debug("Invalid data read from ItemInfo.txt");
         return false;
      }
   }
   catch (FileFailureException fileException)
   {
      debug(fileException.message);
      return false;
   }

   try
   {
      ENTITY_DATA = loadEntityContent();

      if (ENTITY_DATA == nullptr)
      {
         debug("Invalid data read from EntityInfo.txt");
         return false;
      }
   }
   catch (FileFailureException fileException)
   {
      debug(fileException.message);
      return false;
   }

   PLAYER = new Player(BASE_HEALTH, BASE_ATTACK_POWER, BASE_DEFENSE_POWER);

   return true;
}

void newGame()
{
   PLAYER->reset(BASE_HEALTH);

   prologue();

   CURRENT_LOCATION = LOCATION_DATA->getStartLocation();
}

bool loadGame()
{
   ifstream saveFile;

   saveFile.open("Save/SaveInfo.txt");

   if (!saveFile.is_open())
   {
      throw FileFailureException("Failed to open LocationInfo.txt.");
   }

   int index = 0;

   string readLine;
   getline(saveFile, readLine);

   if (readLine == "")
   {
      debug("Save data file was empty.");
      saveFile.close();
      return false;
   }

   CURRENT_LOCATION = LOCATION_DATA->getLocation(parseInfo(index, readLine));

   index = 0;
   getline(saveFile, readLine);

   PLAYER->reset(stoi(parseInfo(index, readLine)));
   PLAYER->setEquippedItem(ITEM_DATA->getItemCopy(parseInfo(index, readLine)));
   PLAYER->setEquippedItem(ITEM_DATA->getItemCopy(parseInfo(index, readLine)));

   saveFile.close();

   debug("Successfully read save data.");

   return true;
}

void saveGame()
{
   if (PLAYER->isDead())
   {
      debug("Did not save as Player's state was dead.");
      return;
   }
   
   ofstream saveFile;

   saveFile.open("Save/SaveInfo.txt");

   if (!saveFile.is_open())
   {
      throw FileFailureException("Failed to open SaveInfo.txt.");
   }

   if (CURRENT_LOCATION->mConnectedLocation == nullptr)
   {
      saveFile << CURRENT_LOCATION->getName() << ";" << endl;
   }
   else
   {
      saveFile << CURRENT_LOCATION->mConnectedLocation->getName() << ";" << endl;
   }

   saveFile << PLAYER->saveState();

   saveFile.close();

   debug("Successfully saved the game state.");
}

void postLocation()
{
   clearScreen();

   string message = "All now seems quiet at the " + CURRENT_LOCATION->getName() + ".";
   COORD cursor = { (cInfo.srWindow.Right - message.size()) / 2, (cInfo.srWindow.Bottom / 2) - 3 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   message = "You rest, recovering some health [+2].";
   PLAYER->recoverHealth(REST_RECOVERY_RATE);
   cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
   cursor.Y = cInfo.srWindow.Bottom / 2;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   message = "You have some time before the sun sets.";
   cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
   cursor.Y = (cInfo.srWindow.Bottom / 2) + 3;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   message = "[1] Manage Inventory -+- [2] Move On -+- [3] Save & Quit";
   cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
   cursor.Y = cInfo.srWindow.Bottom - 3;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   switch (getInput("What do you do [1-3]? ", 1, 3))
   {
   case 1:
      manageInventory(*PLAYER);
      break;

   case 3:
      saveGame();
      quit();
      break;
   }

   advanceLocation();
}

//Advance the location forward, but end the game if we are at the last location.
void advanceLocation()
{
   if (CURRENT_LOCATION->mConnectedLocation == nullptr)
   {
      epilogue();
      gameOver("You win!");
   }
   else
   {
      CURRENT_LOCATION = CURRENT_LOCATION->mConnectedLocation;
   }
}

void quit()
{
   delete LOCATION_DATA;
   LOCATION_DATA = nullptr;
   delete ITEM_DATA;
   ITEM_DATA = nullptr;
   delete ENTITY_DATA;
   ENTITY_DATA = nullptr;
   delete PLAYER;
   PLAYER = nullptr;

   displayQuit();
   exit(0);
}

void gameOver(string message)
{
   clearScreen();

   COORD cursor = { (cInfo.srWindow.Right - message.size()) / 2, cInfo.srWindow.Top + 8 };
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   message = "[1] New Game  --+--  [2] Load Previous Save  --+-- [3] Quit";
   cursor.X = (cInfo.srWindow.Right - message.size()) / 2;
   cursor.Y = cInfo.srWindow.Bottom - 4;
   SetConsoleCursorPosition(hConsole, cursor);
   cout << message;

   switch (getInput("Please select an option [1-3]: ", 1, 3))
   {
   case 1:
      //Release the current remaining entity data
      delete ENTITY_DATA;

      //Reload all entityData from the text files to a fresh EntityManager
      ENTITY_DATA = reloadEntityData();

      newGame();
      break;

   case 2:
      if (!loadGame())
      {
         newGame();
      }
      break;

   case 3:
      quit();
      break;
   }
}