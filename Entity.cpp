#include "Entity.h"
#include "Exceptions.h"
#include "Gameplay.h"
#include "ConsoleDisplay.h"
#include <fstream>
#include <windows.h>
using namespace std;

int PLAYER_MAX_INV = 10;

#pragma region Base Entity
Entity::Entity()
{
   mName = "";
   mHealth = 0;
   mAttackPower = 0;
   mDefensePower = 0;
}

Entity::Entity(const std::string& info)
{
	int index = 0;

	mName = parseInfo(index, info);

	mHealth = stoi(parseInfo(index, info));
	mAttackPower = stoi(parseInfo(index, info));
	mDefensePower = stoi(parseInfo(index, info));
}

void Entity::displayInfo(COORD cursor) const
{
	drawHealthBarBorder(cursor);
	updateHealthBar(cursor);

	cursor.X += 6;
	int origX = cursor.X;
	cursor.Y -= 1;

	string message = mName;
	cursor.X = origX - (message.size() / 2);
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;

	message = "(" + to_string(mAttackPower) + ") Attack Power";
	cursor.X = origX - (message.size() / 2);
	cursor.Y += 5;
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;

	message = "(" + to_string(mDefensePower) + ") Defense Power";
	cursor.X = origX - (message.size() / 2);
	cursor.Y += 1;
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;
}

void Entity::updateHealthBar(COORD lftHndCorner) const
{
	lftHndCorner.X += 1;
	lftHndCorner.Y += 1;
	COORD origin = lftHndCorner;

	//Clear the current health bar
	for (int i = 0, x = 0; i < 20; i++, x++)
	{
		if (x == 10)
		{
			lftHndCorner.Y += 1;
			x = 0;
			lftHndCorner.X -= 10;
		}

		SetConsoleCursorPosition(hConsole, lftHndCorner);
		cout << " ";

		lftHndCorner.X += 1;
	}

	//Redraw the health bar
	lftHndCorner = origin;
	for (int i = 0, x = 0; i < mHealth; i++, x++)
	{
		if (x == 10)
		{
			lftHndCorner.Y += 1;
			x = 0;
			lftHndCorner.X -= 10;
		}

		SetConsoleCursorPosition(hConsole, lftHndCorner);
		cout << "X";

		lftHndCorner.X += 1;
	}
}

void Entity::drawHealthBarBorder(COORD lftHndCorner) const
{
	lftHndCorner.X += 1;
	SetConsoleCursorPosition(hConsole, lftHndCorner);
	for (int i = 0, x = 0; i < 10; i++, x++)
	{
		lftHndCorner.X += 1;
		cout << "-";
	}

	for (int i = 0, x = 0; i < 2; i++, x++)
	{
		lftHndCorner.Y += 1;
		SetConsoleCursorPosition(hConsole, lftHndCorner);
		cout << "|";
	}

	lftHndCorner.Y += 1;
	for (int i = 0, x = 0; i < 10; i++, x++)
	{
		lftHndCorner.X -= 1;
		SetConsoleCursorPosition(hConsole, lftHndCorner);
		cout << "-";
	}

	lftHndCorner.X -= 1;
	for (int i = 0, x = 0; i < 2; i++, x++)
	{
		lftHndCorner.Y -= 1;
		SetConsoleCursorPosition(hConsole, lftHndCorner);
		cout << "|";
	}
}

int Entity::getAttackPower() const
{
	return mAttackPower;
}

int Entity::getDefensePower() const
{
	return mDefensePower;
}

std::string Entity::getName()
{
	return mName;
}

void Entity::recoverHealth(int healthRecovered)
{
	mHealth += healthRecovered;

	if (mHealth > 20)
	{
		mHealth = 20;
	}
}

int Entity::takePaddedDamage(int damageTaken)
{
	int paddedDamage = damageTaken - getDefensePower();

	if (paddedDamage <= 0)
	{
		return 0;
	}

	int tempHealth = mHealth - paddedDamage;

	if (tempHealth <= 0)
	{
		mHealth = 0;
	}
	else
	{
		mHealth = tempHealth;
	}

	return paddedDamage;
}

int Entity::takeFlatDamage(int damageTaken)
{
	int tempHealth = mHealth - damageTaken;

	if (tempHealth <= 0)
	{
		mHealth = 0;
	}
	else
	{
		mHealth = tempHealth;
	}

	return damageTaken;
}

bool Entity::isDead()
{
	return (mHealth == 0);
}

bool Entity::operator==(const Entity& comparison) 
{
	if (mName == comparison.mName)
	{
		return true;
	}
	
	return false;
}

Entity::~Entity()
{
	debug("Entity destructor called.");
}
#pragma endregion

#pragma region Player
Player::Player() : Entity()
{
	mEquippedArmor = nullptr;
	mEquippedWeapon = nullptr;
}

Player::Player(int baseHealth, int baseAttackPower, int baseDefensePower)
{
	mHealth = baseHealth;
	mAttackPower = baseAttackPower;
	mDefensePower = baseDefensePower;
	mEquippedArmor = nullptr;
	mEquippedWeapon = nullptr;
}

void Player::reset(int baseHealth)
{
	mHealth = baseHealth;
	mEquippedArmor = nullptr;
	mEquippedWeapon = nullptr;

	//deallocate all items in the inventory
	for (int i = 0; i < mInventory.size(); i++)
	{
		delete mInventory[i];
	}

	mInventory.clear();
}

void Player::displayInfo(COORD cursor) const
{
	drawHealthBarBorder(cursor);
	updateHealthBar(cursor);

	cursor.X += 6;
	int origX = cursor.X;
	cursor.Y += 4;

	string message;
	if (mEquippedWeapon == nullptr)
	{
		message = "(" + to_string(mAttackPower) + ") No weapon equipped.";
		cursor.X -= (message.size() / 2);
	}
	else
	{
		message = mEquippedWeapon->getName() + ": (" + to_string(getAttackPower()) + ") Total Attack Power";
		cursor.X -= message.size() / 2;
	}
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;
	
	cursor.X = origX;
	cursor.Y += 1;
	if (mEquippedArmor == nullptr)
	{
		message = "(" + to_string(mDefensePower) + ") No armor equipped.";
		cursor.X -= (message.size() / 2);
	}
	else
	{
		message = message = mEquippedArmor->getName() + ": (" + to_string(getDefensePower()) + ") Total Defense Power";
		cursor.X -= message.size() / 2;
	}
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;
}

int Player::getAttackPower() const
{
	if (mEquippedWeapon == nullptr)
	{
		return mAttackPower;
	}
	else
	{
		return mAttackPower + mEquippedWeapon->getPower();
	}
}

int Player::getDefensePower() const
{
	if (mEquippedArmor == nullptr)
	{
		return mDefensePower;
	}
	else
	{
		return mDefensePower + mEquippedArmor->getPower();
	}
}

void Player::addToInventory(Item* itemToAdd)
{
	if (mInventory.size() == PLAYER_MAX_INV)
	{
		return;
	}
	else
	{
		mInventory.push_back(itemToAdd);
	}
}

void Player::removeFromInventory(int indexToRemove)
{
	indexToRemove--;

	if (!(indexToRemove < mInventory.size()) || mInventory.size() == 0)
		return;

	if (mEquippedWeapon != nullptr && mInventory[indexToRemove] == mEquippedWeapon)
	{
		mEquippedWeapon = nullptr;
	}

	if (mEquippedArmor != nullptr && mInventory[indexToRemove] == mEquippedArmor)
	{
		mEquippedArmor = nullptr;
	}

	delete mInventory[indexToRemove];
	mInventory[indexToRemove] = nullptr;
	mInventory.erase(mInventory.begin() + indexToRemove);
}

void Player::setEquippedItem(Item* item)
{
	if (item == nullptr)
	{
		return;
	}

	addToInventory(item);

	switch (item->getType())
	{
	case WEAPON:
		mEquippedWeapon = item;
		break;

	case ARMOR:
		mEquippedArmor = item;
		break;
	}
}

std::string Player::saveState()
{
	string data = "";

	data += to_string(mHealth) + ";";

	if (mEquippedWeapon != nullptr)
	{
		data += mEquippedWeapon->getName() + ";";
	}
	else
	{
		data += "NULL;";
	}

	if (mEquippedArmor != nullptr)
	{
		data += mEquippedArmor->getName() + ";";
	}
	else
	{
		data += "NULL;";
	}

	//add inventory functionality
	return data;
}

void manageInventory(Player& player)
{
	int response = 0;

	do
	{
		displayInv(player);

		string message = "[1] Equip Item  --+--  [2] Discard Item  --+-- [3] Move On";
		COORD cursor = { (cInfo.srWindow.Right - message.size()) / 2, cInfo.srWindow.Bottom - 3 };
		SetConsoleCursorPosition(hConsole, cursor);
		cout << message;

		int response = (getInput("What do you do [1-3]? ", 1, 3));

		int itemSelection;
		switch (response)
		{
		case 1:
			clearRow(cursor.Y);
			itemSelection = getInput("What item do you want to equip [1-10]? ", 1, 10);
			equipItem(player, itemSelection);
			break;

		case 2:
			clearRow(cursor.Y);
			itemSelection = getInput("What item do you want to discard [1-10]? ", 1, 10);
			player.removeFromInventory(itemSelection);
			break;

		case 3:
			return;
		}
	} while (response != 3);
}

void displayInv(const Player& player)
{
	clearScreen();

	COORD cursor = { (cInfo.srWindow.Right / 2) - 25, 2 };
	for (int i = 0, y = 0; i < PLAYER_MAX_INV; i++, y++)
	{
		if (y == 5)
		{
			cursor.X += 38;
			cursor.Y = 2;
		}

		SetConsoleCursorPosition(hConsole, cursor);

		cout << "[" << to_string(i+1) << "] ";
		if (i < player.mInventory.size())
		{
			cout << player.mInventory[i]->getName();
		}
		else
		{
			cout << "Empty";
		}

		cursor.Y += 2;
	}

	cursor.X = (cInfo.srWindow.Right / 2) - 20;
	cursor.Y = cInfo.srWindow.Bottom - 8;
	string message;
	if (player.mEquippedWeapon == nullptr)
	{
		message = "(" + to_string(player.mAttackPower) + ") No weapon equipped.";
		cursor.X -= (message.size() / 2);
	}
	else
	{
		message = player.mEquippedWeapon->getName() + ": (" + to_string(player.getAttackPower()) + ") Total Attack Power";
		cursor.X -= message.size() / 2;
	}
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;

	cursor.X += 54;
	if (player.mEquippedArmor == nullptr)
	{
		message = "(" + to_string(player.mDefensePower) + ") No armor equipped.";
		cursor.X -= (message.size() / 2);
	}
	else
	{
		message = message = player.mEquippedArmor->getName() + ": (" + to_string(player.getDefensePower()) + ") Total Defense Power";
		cursor.X -= message.size() / 2;
	}
	SetConsoleCursorPosition(hConsole, cursor);
	cout << message;
}

void equipItem(Player& player, int invIndex)
{
	invIndex--;

	if (!(invIndex < player.mInventory.size()) || player.mInventory.size() == 0)
		return;

	switch (player.mInventory[invIndex]->getType())
	{
	case WEAPON:
		player.mEquippedWeapon = player.mInventory[invIndex];
		break;

	case ARMOR:
		player.mEquippedArmor = player.mInventory[invIndex];
		break;
	}
}

Player::~Player()
{
	mEquippedArmor = nullptr;
	mEquippedWeapon = nullptr;

	//deallocate all items in the inventory
	for (int i = 0; i < mInventory.size(); i++)
	{
		delete mInventory[i];
	}

	mInventory.clear();
}
#pragma endregion

#pragma region EntityManager
void EntityManager::add(Entity* entityToAdd)
{
   mEntityList.push_back(entityToAdd);
}

Entity* EntityManager::getRandomEntity()
{
	if (mEntityList.size() <= 0)
	{
		throw NullDataException("mEntityList is empty in getRandomEntity.");
	}

	srand(time(nullptr));
	Entity* pickedEntity = nullptr;

	int selection = rand() % mEntityList.size();

	for (int i = 0; i < mEntityList.size(); i++)
	{
		if (i == selection)
		{
			pickedEntity = mEntityList[i];
		}
	}

	if (pickedEntity == nullptr)
	{
		throw NullDataException("Null entity picked in getRandomEntity.");
	}

	mEntityList.erase(mEntityList.begin() + selection);
	return pickedEntity;
}

EntityManager::~EntityManager()
{
	for (int i = 0; i < mEntityList.size(); i++)
	{
		delete mEntityList[i];
	}

	mEntityList.clear();
}
#pragma endregion

EntityManager* loadEntityContent()
{
	ifstream entityFile;

	EntityManager* entityList = new EntityManager();

	entityFile.open("Content/EntityInfo.txt");

	if (!entityFile.is_open())
	{
		throw FileFailureException("Failed to open EntityInfo.txt.");
	}

	while (!entityFile.eof())
	{
		string readLine;
		getline(entityFile, readLine);
		Entity* newEntity;
		try
		{
			newEntity = new Entity(readLine);
		}
		catch (InvalidStringException invalidString)
		{
			debug(invalidString.message);
			return nullptr;
		}

		if (newEntity != nullptr)
		{
			entityList->add(newEntity);
		}
		else
		{
			debug("newEntity pointer did not get assigned a value and is null.");
		}
	}

	entityFile.close();

	debug("Successfully read all entity data.");
	return entityList;
}

EntityManager* reloadEntityData()
{
	return loadEntityContent();
}