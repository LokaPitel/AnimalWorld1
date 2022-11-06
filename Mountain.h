#pragma once
#include "Entity.h"

class Mountain : public Entity
{
public:
	Mountain(int id, Position position) :
		Entity(id, false, 0, 0, 0, 0, false, position)
	{
	}

	virtual std::string typeName() { return "Mountain"; }
	virtual std::string getSymbolNotation() { return "^"; }
};