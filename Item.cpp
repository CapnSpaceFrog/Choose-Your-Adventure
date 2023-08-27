#include "Item.h"
#include "Exceptions.h"
#include "Gameplay.h"
#include "Entity.h"
#include <iostream>
#include <fstream>
using namespace std;

#pragma region Base Item
int Item::idSeed = 0;

Item::Item()
{
	mName = "";
	mPrice = 0;
	mPowerRating = 0;
	mID = idSeed;
	idSeed++;
}

Item::Item(const string& info)
{
	if (info == "")
	{
		throw InvalidStringException("Info passed to Location constructor was invalid.");
	}

	int index = 0;

	mType = (ItemType)stoi(parseInfo(index, info));
	mName = parseInfo(index, info);
	mPrice = stoi(parseInfo(index, info));
	mPowerRating = stoi(parseInfo(index, info));
	mID = idSeed;
	idSeed++;
}

Item::Item(const Item& i)
{
	mName = i.mName;
	mPrice = i.mPrice;
	mType = i.mType;
	mPowerRating = i.mPowerRating;

	mID = idSeed;
	idSeed++;
}

string Item::getName() const
{
	return mName;
}

int Item::getPower() const
{
	return mPowerRating;
}

int Item::getPrice() const
{
	return mPrice;
}

ItemType Item::getType() const
{
	return mType;
}

int Item::getID() const
{
	return mID;
}

bool Item::operator==(const Item& rhs)
{
	if (mID == rhs.mID)
	{
		return true;
	}

	return false;
}

Item Item::operator+(const Item& rhs)
{
	Item newItem = Item();

	string newName = "";
	for (int i = 0; i < (mName.size() / 2); i++)
	{
		newName += mName[i];
	}

	for (int i = (rhs.mName.size() / 2); i < (rhs.mName.size()); i++)
	{
		newName += rhs.mName[i];
	}

	newItem.mName = newName;
	newItem.mPowerRating = mPowerRating + rhs.mPowerRating;
	newItem.mType = mType;
	newItem.mPrice = mPrice + rhs.mPrice;

	return newItem;
}
#pragma endregion

#pragma region Item Manager
Item* ItemManager::gainItem()
{
	if (mItemList.size() <= 0)
	{
		throw NullDataException("mItemList is empty in gainItem.");
	}

	srand(time(nullptr));

	int selection = rand() % mItemList.size();
	Item newItem;

	for (int i = 0; i < mItemList.size(); i++)
	{
		if (i == selection)
		{
			//Need to take a copy of this item and allocate it on the stack.
			newItem = Item(*mItemList[i]);
		}
	}

	if (rand() % 100 >= 75)
	{
		//get another item
		selection = rand() % mItemList.size();
		Item secondItem;
		for (int i = 0; i < mItemList.size(); i++)
		{
			if (i == selection)
			{
				//Need to take a copy of this item and allocate it on the stack.
				secondItem = Item(*mItemList[i]);
			}
		}

		Item* itemPointer = new Item(newItem + secondItem);
		return itemPointer;
	}
	else
	{
		Item* itemPointer = new Item(newItem);
		return itemPointer;
	}
}

void ItemManager::add(Item* itemToAdd)
{
	mItemList.push_back(itemToAdd);
}

Item* ItemManager::getItemCopy(const string& itemName)
{
	if (itemName == "NULL")
	{
		return nullptr;
	}
		
	for (int i = 0; i < mItemList.size(); i++)
	{
		if (mItemList[i]->getName() == itemName)
		{
			Item* newItem = new Item(*mItemList[i]);
			return newItem;
		}
	}

	return nullptr;
}

ItemManager::~ItemManager()
{
	for (int i = 0; i < mItemList.size(); i++)
	{
		delete mItemList[i];
	}

	mItemList.clear();
}
#pragma endregion

ItemManager* loadItemContent()
{
	ifstream itemFile;

	ItemManager* itemList = new ItemManager();

	itemFile.open("Content/ItemInfo.txt");

	if (!itemFile.is_open())
	{
		throw FileFailureException("Failed to open ItemInfo.txt.");
	}

	while (!itemFile.eof())
	{
		string readLine;
		getline(itemFile, readLine);
		Item* newItem = nullptr;

		try
		{
			newItem = new Item(readLine);
		}
		catch (InvalidStringException invalidString)
		{
			debug(invalidString.message);
			return nullptr;
		}

		if (newItem != nullptr)
		{
			itemList->add(newItem);
		}
		else
		{
			debug("NewItem pointer did not get assigned a value and is null.");
		}
	}

	itemFile.close();

	debug("Successfully read all item data.");
	return itemList;
}