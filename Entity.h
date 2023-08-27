#ifndef _Entity_
#define _Entity_

#include "Item.h"
#include <windows.h>
#include <iostream>
#include <vector>

class Entity
{
public:
   Entity();

   Entity(const std::string& info);

   virtual void displayInfo(COORD cursor) const;

   void updateHealthBar(COORD lftHndCorner) const;

   void drawHealthBarBorder(COORD lftHndCorner) const;

   virtual int getAttackPower() const;

   virtual int getDefensePower() const;

   std::string getName();

   void recoverHealth(int healthRecovered);

   int takePaddedDamage(int damageTaken);

   int takeFlatDamage(int damageTaken);

   bool isDead();

   bool operator==(const Entity& comparison);

   ~Entity();

protected:
   std::string mName;
   int mHealth;
   int mAttackPower;
   int mDefensePower;
};

class Player : public Entity
{
public:
   Player();

   Player(int baseHealth, int baseAttackPower, int baseDefensePower);

   void reset(int baseHealth);

   void displayInfo(COORD cursor) const override;

   int getAttackPower() const override;

   int getDefensePower() const override;

   void addToInventory(Item* itemToAdd);

   void removeFromInventory(int indexToRemove);
   
   void setEquippedItem(Item* info);

   std::string saveState();

   friend void manageInventory(Player& player);

   friend void displayInv(const Player& player);

   friend void equipItem(Player& player, int indexToEquip);

   ~Player();

private:
   Item* mEquippedWeapon;
   Item* mEquippedArmor;
   std::vector<Item*> mInventory;
};

class EntityManager
{
public:

   void add(Entity* entityToAdd);

   //Gets a random entity from the entity list, and then removes them from possible future selections
   Entity* getRandomEntity();

   ~EntityManager();

private:
   std::vector<Entity*> mEntityList;
};

EntityManager* loadEntityContent();

EntityManager* reloadEntityData();
#endif