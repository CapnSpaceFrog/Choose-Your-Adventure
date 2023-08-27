#ifndef _Gameplay_
#define _Gameplay_

#include "Entity.h"
#include <iostream>
#include <fstream>
using namespace std;

enum EncounterType
{
	GAIN_ITEM = 1,
	FIGHT,
	SHOP
};

void prologue();

//Picks a random enemy and pits the Player against them. Handles death and all functions of life loss.
bool fight(Entity& enemy, Entity& player, bool& ran);

//Opens an inventory screen, displaying items and allowing the Player to interact with their inventory.
void manageInventory(const Player& player);

void shop();

void epilogue();

//Given a string of data, return segments terminated at semi-colons.
string parseInfo(int& index, const string& info);

void debug(string message);
#endif