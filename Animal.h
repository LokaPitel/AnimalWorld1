#pragma once
#include "EntityState.h"
#include "Entity.h"

class Animal : public Entity
{
public:
	Animal(int id, bool isPredator, int old, int health, int hunger, bool active, bool isMale, Position position) :
		Entity(id, isPredator, old, health, hunger, active, isMale, position)
	{
	}

	virtual bool isReproducable()
	{
		return old >= 8 && old <= 30;
	}

	virtual bool hasLowHealth()
	{
		return health <= 5;
	}

	virtual bool isOld()
	{
		return old == getMaxOld();
	}

	virtual bool isHunger()
	{
		if (predator)
			return hunger >= 13;

		else
			return hunger >= 7;
	}

	virtual bool hasPair()
	{
		return state == WAITINGFORPAIR && target || state == SEARCHINGFORPAIR && target;
	}

	virtual bool isAnimal() { return true; };
	virtual bool isPredator() { return predator; }
	virtual bool isPlantEating() { return !predator; }

	virtual bool hasAdmissibleHunger()
	{
		if (predator)
			return hunger <= 3;

		else
			return hunger <= 1;
	}

	virtual int getMaxHealth() { return 15; }
	virtual int getMaxHunger() { return 20; }

	virtual int getMaxOld()
	{ 
		if (predator)
			return 30;

		else
			return 45;
	}

	virtual std::string typeName() 
	{ 
		if (isPredator())
			return "Predator"; 

		return "Plant Eating";
	}
	virtual std::string getSymbolNotation()
	{
		if (predator)
			return "&";

		return "@"; 
	}

	virtual std::string getSexNotation()
	{
		if (isMale())
			return "Male";

		return "Female";
	}
};