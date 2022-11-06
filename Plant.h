#pragma once
#pragma once
#include "EntityState.h"
#include "Entity.h"

class Plant : public Entity
{
public:
	Plant(int id, int old, int health, int hunger, bool active, Position position) :
		Entity(id, false, old, health, hunger, active, false, position)
	{
	}

	virtual bool isReproducable()
	{
		return old >= 10 && old <= 20;
	}

	virtual bool isOld()
	{
		return old == getMaxOld();
	}

	virtual int getMaxOld() { return 31; }

	virtual bool isPlant() { return true; }

	virtual int getMaxHealth() { return 30; }

	virtual std::string typeName() { return "Plant"; }
	virtual std::string getSymbolNotation() { return "X"; }
};