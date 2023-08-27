#ifndef _Item_
#define _Item_

#include <iostream>
#include <vector>

enum ItemType
{
   WEAPON = 1,
   ARMOR
};

class Item
{
public:
   Item();

   Item(const std::string& info);

   Item(const Item& i);
   
   std::string getName() const;

   int getPower() const;

   int getPrice() const;

   ItemType getType() const;

   int getID() const;

   bool operator==(const Item& rhs);

   Item operator+(const Item& rhs);

private:
   int mID;
   int mPrice;
   std::string mName;
   ItemType mType;
   int mPowerRating;

   static int idSeed;
};

class ItemManager
{
public:
   Item* gainItem();

   void add(Item* itemToAdd);

   Item* getItemCopy(const std::string& itemName);

   ~ItemManager();

private:
   std::vector<Item*> mItemList;
};

ItemManager* loadItemContent();
#endif