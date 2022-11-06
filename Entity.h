#pragma once
#include <string>
#include <sstream>
#include "EntityState.h"
#include "Position.h"

class Entity
{
protected:
	int id;
	int old;
	int health;
	int hunger;

	bool active;
	bool male;
	bool predator;

	EntityState state;
	Position pos;

	Entity* target;
	Entity* callee;

public:
	Entity(int id, bool isPredator, int old, int health, int hunger, bool active, bool isMale, Position position)
	{
		this->id = id;
		this->old = old;
		this->health = health;
		this->hunger = hunger;

		this->active = active;
		this->male = isMale;
		this->predator = isPredator;

		state = IDLE;

		target = callee = nullptr;
		pos = position;
	}

	int getId() { return id; }
	int getOld() { return old; }
	int getHealth() { return health; }
	int getHunger() { return hunger; }

	void setOld(int old) { this->old = old; }
	void setHealth(int health) { this->health = health; }
	void setHunger(int hunger) { this->hunger = hunger; }
	EntityState getState() { return state; }
	void setState(EntityState state) { this->state = state; }

	Position getPosition() { return pos; }
	void setPosition(Position position) { pos = position; }

	Entity* getTarget() { return target; }
	void setTarget(Entity* entity) { target = entity; }

	Entity* getCallee() { return callee; }
	void setCallee(Entity* entity) { callee = entity; }

	void makeActive() { active = true; }

	bool isActive() { return active; }
	bool isMale() { return male; }

	bool hasZeroHealth() { return health == 0; }

	bool hasKillingHunger() { return hunger == getMaxHunger(); }

	bool hasCall() { return callee; }

	friend bool operator<(Entity one, Entity another)
	{
		return one.getId() < another.getId();
	}

	virtual bool isReproducable() { return false; }
	virtual bool hasLowHealth() { return false; }
	virtual bool isOld() { return false; };
	virtual int getMaxOld() { return 0; }
	virtual bool isHunger() { return false; }
	virtual bool hasPair() { return false; }
	virtual int getMaxHealth() { return 0; }
	virtual int getMaxHunger() { return 0; }
	virtual bool hasAdmissibleHunger() { return false; }

	virtual bool isAnimal() { return false; };
	virtual bool isFood() { return false; };
	virtual bool isPlant() { return false; }
	virtual bool isPredator() { return false; }
	virtual bool isPlantEating() { return false; }

	virtual std::string typeName() { return ""; }
	virtual std::string getSymbolNotation() { return ""; }
	virtual std::string getSexNotation() { return "N/A"; }

	virtual void byeTarget()
	{
		if (target)
		{
			target->setTarget(nullptr);
			target = nullptr;
		}
	}

	virtual void call(Entity* entity) 
	{
		entity->setCallee(this);
	}
};