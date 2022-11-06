#pragma once
#include "EntityState.h"
#include "Entity.h"

class Food: public Entity
{
public:
	Food(int id, int old, int health, int hunger, bool active, Position position) :
		Entity(id, false, old, health, hunger, active, false, position)
	{
	}

	virtual bool isOld()
	{
		return old == getMaxOld();
	}

	virtual bool isFood() { return true; };

	virtual int getMaxHealth() { return 15; }
	virtual int getMaxOld() { return 20; }

	virtual std::string typeName() { return "Food"; }
	virtual std::string getSymbolNotation() { return "*"; }
};