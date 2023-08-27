#include "Exceptions.h"
#include "Location.h"
#include "ConsoleDisplay.h"
#include <iostream>
using namespace std;

#pragma region Location
Location::Location()
{
	mName = "";
	mRadiant = "";
	mChoices = nullptr;
	mConnectedLocation = nullptr;
}

Location::Location(const string& info)
{
	int index = 0;

	if (info == "")
	{
		throw InvalidStringException("Info passed to Location constructor was invalid.");
	}

	mName = parseInfo(index, info);
	mRadiant = parseInfo(index, info);

	mChoices = createChoices(parseInfo(index, info));

	mConnectedLocation = nullptr;
}

string Location::getName()
{
	return mName;
}

void displayLocationInfo(const Location& toDisplay)
{
	clearScreen();

	string title = "You arrive at " + toDisplay.mName;
	COORD cursor = { (cInfo.srWindow.Right - title.size()) / 2, 3 };
	SetConsoleCursorPosition(hConsole, cursor);
	cout << title;

	string radiant = toDisplay.mRadiant;
	cursor.X = (cInfo.srWindow.Right - radiant.size()) / 2;
	cursor.Y += 4;
	SetConsoleCursorPosition(hConsole, cursor);
	cout << radiant;

	string prompt1 = "[1] " + toDisplay.mChoices[0].prompt;
	cursor.X = (cInfo.srWindow.Right - prompt1.size()) / 2;
	cursor.Y += 13;
	SetConsoleCursorPosition(hConsole, cursor);
	cout << prompt1;

	string prompt2 = "[2] " + toDisplay.mChoices[1].prompt;
	cursor.X = (cInfo.srWindow.Right - prompt2.size()) / 2;
	cursor.Y += 2;
	SetConsoleCursorPosition(hConsole, cursor);
	cout << prompt2;

	string prompt3 = "[3] " + toDisplay.mChoices[2].prompt;
	cursor.X = (cInfo.srWindow.Right - prompt3.size()) / 2;
	cursor.Y += 2;
	SetConsoleCursorPosition(hConsole, cursor);
	cout << prompt3;
}

EncounterType handlePromptChoice(const Location& toDisplay)
{
	return toDisplay.mChoices[getInput("What do you do [1-3]? ", 1, 3) - 1].outcome;
}

Prompt* Location::createChoices(const string& info)
{
	Prompt* choices = new Prompt[MAX_LOC_CHOICES];
	string temp = "";
	int index = 0;

	for (int i = 0; i < info.length(); i++)
	{
		if (info[i] == '-')
		{
			choices[index].outcome = getEncounterType(stoi(temp));
			index++;
			temp = "";
			continue;
		}

		if (info[i] == ':')
		{
			choices[index].prompt = temp;
			temp = "";
			continue;
		}

		temp += info[i];
	}

	return choices;
}

EncounterType Location::getEncounterType(const int type)
{
	switch (type)
	{
	case 1:
		return GAIN_ITEM;

	case 2:
		return FIGHT;

	case 3:
		return SHOP;

	default:
		debug("defaulted in getEncounterType");
		return GAIN_ITEM;
	}
}

Location::~Location()
{
	mConnectedLocation = nullptr;

	delete[] mChoices;
	mChoices = nullptr;
}
#pragma endregion

#pragma region Location List
LocationList::LocationList()
{
	mStartLocation = nullptr;
}

Location* LocationList::getStartLocation() const
{
	return mStartLocation;
}

void LocationList::linkLocations()
{
	mStartLocation = mStoredLocations[0];
	Location* currentLocation = mStartLocation;

	for (int i = 1; i < mStoredLocations.size(); i++)
	{
		currentLocation->mConnectedLocation = mStoredLocations[i];
		currentLocation = currentLocation->mConnectedLocation;
	}
}

void LocationList::add(Location* locationToAdd)
{
	mStoredLocations.push_back(locationToAdd);
}

Location* LocationList::getLocation(const string& locationName)
{
	for (int i = 0; i < mStoredLocations.size(); i++)
	{
		if (mStoredLocations[i]->getName() == locationName)
		{
			return mStoredLocations[i];
		}
	}

	return nullptr;
}

LocationList::~LocationList()
{
	clear();
}

void LocationList::clear()
{
	mStartLocation = nullptr;

	for (int i = 0; i < mStoredLocations.size(); i++)
	{
		delete mStoredLocations[i];
	}

	mStoredLocations.clear();
}
#pragma endregion

LocationList* loadLocationContent()
{
	ifstream locationFile;

	LocationList* locationList = new LocationList();

	locationFile.open("Content/LocationInfo.txt");

	if (!locationFile.is_open())
	{
		throw FileFailureException("Failed to open LocationInfo.txt.");
	}

	while (!locationFile.eof())
	{
		string readLine;
		getline(locationFile, readLine);
		Location* newLocation;
		try
		{
			newLocation = new Location(readLine);
		}
		catch (InvalidStringException invalidString)
		{
			debug(invalidString.message);
			return nullptr;
		}

		locationList->add(newLocation);
	}

	locationFile.close();

	debug("Successfully read all location data.");
	locationList->linkLocations();
	return locationList;
}