#ifndef _Location_
#define _Location_

#include "Gameplay.h"
#include <string>
#include <vector>

const int MAX_LOC_CHOICES = 3;

struct Prompt
{
	std::string prompt = "";
	EncounterType outcome = GAIN_ITEM;
};

class Location
{
public:
	Location* mConnectedLocation;

	Location();
	
	Location(const std::string& info);

	std::string getName();

	//Displays the information of the location passed.
	friend void displayLocationInfo(const Location& toDisplay);

	//Handles the Players location choice and forwards execution to the matching function
	friend EncounterType handlePromptChoice(const Location& toDisplay);

	~Location();

private:
	std::string mName;
	std::string mRadiant;

	Prompt* mChoices;

	static Prompt* createChoices(const std::string& info);

	static EncounterType getEncounterType(const int type);
};

class LocationList
{
public:
	LocationList();
	
	Location* getStartLocation() const;

	void add(Location* locationToAdd);

	void linkLocations();

	Location* getLocation(const string& locationName);

	~LocationList();

	void clear();

private:
	Location* mStartLocation;
	std::vector<Location*> mStoredLocations;
};

LocationList* loadLocationContent();
#endif