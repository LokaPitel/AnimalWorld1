#pragma once
#include <set>
#include "SetUtility.h"
#include "Map.h"
#include "Entity.h"
#include "Animal.h"
#include "Plant.h"
#include "Food.h"

class Model
{
	int lastId;
	Map map;
	std::set<Entity*> entities;

public:
	Model(Model& another)
	{
		lastId = another.lastId;
		map = another.map;
		entities = std::set<Entity*>(another.entities);
	}

	Model(int mapHeight, int mapWidth): map(mapHeight, mapWidth)
	{
		lastId = 0;

		// initialize entities

		const int INITIAL_PLANTEATING_MALE_COUNT = 3;
		const int INITIAL_PLANTEATING_FEMALE_COUNT = 4;
		const int INITIAL_PREDATORS_MALE_COUNT = 4;
		const int INITIAL_PREDATORS_FEMALE_COUNT = 4;
		const int INITIAL_PLANTS_COUNT = 7;
		const int INITIAL_FOOD_COUNT = 5;

		std::set<Position> free = getFreePositions();

		for (int i = 0; i < INITIAL_PLANTEATING_MALE_COUNT; i++)
		{
			Position pos = SetUtility::randomFrom(free);

			addPlantEatingMale(pos);

			free.erase(pos);
		}

		for (int i = 0; i < INITIAL_PLANTEATING_FEMALE_COUNT; i++)
		{
			Position pos = SetUtility::randomFrom(free);

			addPlantEatingFemale(pos);

			free.erase(pos);
		}

		for (int i = 0; i < INITIAL_PREDATORS_MALE_COUNT; i++)
		{
			Position pos = SetUtility::randomFrom(free);

			addPredatorMale(pos);

			free.erase(pos);
		}

		for (int i = 0; i < INITIAL_PREDATORS_FEMALE_COUNT; i++)
		{
			Position pos = SetUtility::randomFrom(free);

			addPredatorFemale(pos);

			free.erase(pos);
		}

		for (int i = 0; i < INITIAL_PLANTS_COUNT; i++)
		{
			Position pos = SetUtility::randomFrom(free);

			addPlant(pos);

			free.erase(pos);
		}

		for (int i = 0; i < INITIAL_FOOD_COUNT; i++)
		{
			Position pos = SetUtility::randomFrom(free);

			addFood(pos);

			free.erase(pos);
		}
	}

	Map* getMap() { return &map; }
	std::set<Entity*>& getEntities() { return entities; }


	std::string getSerealization()
	{
		std::string result = "";

		result += std::to_string(lastId);
		result += std::string(" ");

		result += std::to_string(map.getHeight());
		result += std::string(" ");

		result += std::to_string(map.getWidth());
		result += std::string("\n");

		for (Entity* entity : entities)
			result += getSerealizationOfEntity(entity);

		return result;
	}

	void deserealizeRepresentation(std::string representation)
	{
		std::stringstream ss(representation);

		std::string line;

		std::getline(ss, line);

		std::stringstream inner(line);

		inner >> lastId;
		
		int heightSize;
		int widthSize;

		inner >> heightSize;
		inner >> widthSize;

		map = Map(heightSize, widthSize);

		std::set<Entity*> newEntities;

		while (true)
		{
			std::getline(ss, line);

			if (line.empty())
				break;

			Entity* ent = getDeserealization(line);

			newEntities.insert(ent);
		}

		ss = std::stringstream(representation);
		
		std::string ignore;

		std::getline(ss, ignore);

		while (true)
		{
			std::getline(ss, line);

			if (line.empty())
				break;

			extractTargetAndCallee(line);
		}

		entities = newEntities;
	}

	Entity* getDeserealization(std::string serealizationLine)
	{
		std::stringstream ss(serealizationLine);

		int id;
		int old;
		int health;
		int hunger;
		bool active;
		bool male;
		bool predator;

		int state;

		int target;
		int callee;

		int xPos;
		int yPos;

		int type;

		ss >> id;
		ss >> old;
		ss >> health;
		ss >> hunger;
		ss >> active;
		ss >> male;
		ss >> predator;
		ss >> state;
		ss >> target;
		ss >> callee;
		ss >> xPos;
		ss >> yPos;
		ss >> type;

		Entity* result = nullptr;

		if (type == 0)
			result = new Animal(id, predator, old, health, hunger, active, male, Position(xPos, yPos));

		else if (type == 1)
			result = new Plant(id, old, health, hunger, active, Position(xPos, yPos));

		else if (type == 2)
			result = new Food(id, old, health, hunger, active, Position(xPos, yPos));

		return result;
	}

	void extractTargetAndCallee(std::string serealizationLine)
	{
		std::stringstream ss(serealizationLine);

		int id;
		int old;
		int health;
		int hunger;
		bool active;
		bool male;
		bool predator;

		int state;

		int target;
		int callee;

		int xPos;
		int yPos;

		int type;

		ss >> id;
		ss >> old;
		ss >> health;
		ss >> hunger;
		ss >> active;
		ss >> male;
		ss >> predator;
		ss >> state;
		ss >> target;
		ss >> callee;
		ss >> xPos;
		ss >> yPos;
		ss >> type;

		Entity* entity = getById(id);

		if (!entity)
			return;

		entity->setTarget(getById(target));
		entity->setCallee(getById(callee));
	}

	std::string getSerealizationOfEntity(Entity* ent)
	{
		std::string result = "";

		result += std::to_string(ent->getId());
		result += std::string(" ");

		result += std::to_string(ent->getOld());
		result += std::string(" ");

		result += std::to_string(ent->getHealth());
		result += std::string(" ");

		result += std::to_string(ent->getHunger());
		result += std::string(" ");

		result += std::to_string(ent->isActive());
		result += std::string(" ");

		result += std::to_string(ent->isMale());
		result += std::string(" ");

		result += std::to_string(ent->isPredator());
		result += std::string(" ");

		result += std::to_string(ent->getState());
		result += std::string(" ");

		/*std::stringstream ss;
		ss << ent->getTarget();

		result += ss.str();
		result += std::string(" ");

		ss = std::stringstream();
		ss << ent->getCallee();*/

		if (ent->getTarget())
			result += std::to_string(ent->getTarget()->getId());

		else
			result += std::to_string(-1);
		result += std::string(" ");


		if (ent->getCallee())
			result += std::to_string(ent->getCallee()->getId());

		else
			result += std::to_string(-1);

		result += std::string(" ");

		result += std::to_string(ent->getPosition().getX());
		result += std::string(" ");

		result += std::to_string(ent->getPosition().getY());
		result += std::string(" ");

		if (ent->isAnimal())
			result += std::to_string(0);

		else if (ent->isPlant())
			result += std::to_string(1);

		else if (ent->isFood())
			result += std::to_string(2);

		result += "\n";

		return result;
	}

	bool isPredatorIn(Position pos)
	{
		bool result = false;

		for (Entity* entity : entities)
		{
			if (entity->isPredator() && entity->getPosition() == pos)
				return true;
		}

		return result;
	}

	Entity* getById(int id)
	{
		Entity* result = nullptr;

		for (Entity* ent : entities)
			if (ent->getId() == id)
				result = ent;

		return result;
	}

	std::set<Entity*> getNonReproducing()
	{
		std::set<Entity*> result;
		
		for (Entity* entity : entities)
			if (entity->getState() != REPRODUCING)
				result.insert(entity);

		return result;
	}

	Entity* getClosest(std::set<Entity*> searchSet, Entity* to)
	{
		Entity* closest = nullptr;

		for (Entity* entity : searchSet)
		{
			if (entity == closest)
				continue;

			else if (!closest)
				closest = entity;

			if (closest->getPosition().difference(to->getPosition()) > entity->getPosition().difference(to->getPosition()))
				closest = entity;
		}

		return closest;
	}

	std::set<Entity*> getAnimals()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isAnimal())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getPlants()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isPlant())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getFood()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isFood())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getPlantEating()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isPlantEating())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getPredators()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isPredator())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getAlive()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (!entity->hasZeroHealth())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getReproducable()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isReproducable())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getEatable()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isPlantEating() || entity->isPlant() || entity->isFood())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getActive()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isActive())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getInactive()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (!entity->isActive())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getMale()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isAnimal() && entity->isMale())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getFemale()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->isAnimal() && !entity->isMale())
				result.insert(entity);

		return result;
	}

	std::set<Entity*> getDied()
	{
		std::set<Entity*> result;

		for (Entity* entity : entities)
			if (entity->getState() == DIED)
				result.insert(entity);

		return result;
	}

	bool isAdjacent(Entity* one, Entity* another)
	{
		return Position::isAdjacent(one->getPosition(), another->getPosition());
	}

	bool isSafe(Entity* entity)
	{
		return getDangerLevel(entity->getPosition()) == 0;
	}

	bool isFree(Position pos)
	{
		return ! SetUtility::isIn(getNonFreePositions(), pos);
	}

	Entity* getIn(Position pos)
	{
		for (Entity* entity : entities)
			if (entity->getPosition() == pos)
				return entity;

		return nullptr;
	}

	std::set<Position> getAllPositions()
	{
		return map.getAllPositions();
	}

	std::set<Position> getNonFreePositions()
	{
		std::set<Position> notFreePositions;

		for (Entity* ent : entities)
			notFreePositions.insert(ent->getPosition());

		return notFreePositions;
	}

	std::set<Position> getFreePositions()
	{
		return SetUtility::Difference(getAllPositions(), getNonFreePositions());
	}

	std::set<Position> getFreeAdjacent(Position pos)
	{
		return SetUtility::Intersection(getFreePositions(), pos.getAdjacent());
	}

	void bornNewPlantEatingFemale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, 0, 0, 15, 0, false, false, pos));
	}

	void addEntity(Entity* ent)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, ent->getPosition()))
			entities.insert(ent);
	}

	void bornNewPlantEatingMale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, 0, 0, 15, 0, false, true, pos));
	}

	void bornNewPredatorFemale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, true, 0, 15, 0, false, false, pos));
	}

	void bornNewPredatorMale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, true, 0, 15, 0, false, true, pos));
	}

	void addPlantEatingMale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, false, 0, 15, 0, true, true, pos));
	}

	void addPlantEatingFemale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, 0, 0, 15, 0, true, false, pos));
	}

	void addPredatorMale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, true, 0, 15, 0, true, true, pos));
	}

	void addPredatorFemale(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Animal(lastId++, true, 0, 15, 0, true, false, pos));
	}

	void bornNewPlant(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Plant(lastId++, 0, 15, 0, false, pos));
	}

	void addPlant(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Plant(lastId++, 0, 15, 0, true, pos));
	}

	void addFood(Position pos)
	{
		std::set<Position> nonFreePositions = getNonFreePositions();

		if (!SetUtility::isIn(nonFreePositions, pos))
			entities.insert(new Food(lastId++, 0, 15, 0, true, pos));
	}

	int getDangerLevel(Position pos)
	{
		std::set<Position> innerDomain;
		std::set<Position> outerDomain;

		innerDomain = Position::getAdjacent(pos);

		int sum = 0;

		for (Position current : innerDomain)
		{
			std::set<Position> tmp = current.getAdjacent();


			for (Position pos : tmp)
				if (isPredatorIn(pos))
					sum += 2;;

			//outerDomain.emplace(tmp);

			for (Position pos : tmp)
				outerDomain.insert(pos);
		}

		for (Position current : outerDomain)
		{
			std::set<Position> tmp = current.getAdjacent();

			for (Position pos : tmp)
				if (isPredatorIn(pos))
					sum += 1;
		}

		return sum;
	}

	Position getClosest(std::set<Position> searchSet, Position to)
	{
		Position closest = Position(-1, -1);

		for (Position pos: searchSet)
		{ 
			if (pos == to)
				continue;

			if (closest == Position(-1, -1))
			{
				closest = pos;
				continue;
			}

			if (to.difference(closest) > to.difference(pos))
				closest = pos;
		}

		return closest;
	}

	std::set<Position> getSafePlaces()
	{
		std::set<Position> all = getAllPositions();

		std::set<Position> safe;

		for (Position pos : all)
		{
			if (getDangerLevel(pos) == 0)
				safe.insert(pos);
		}

		return safe;
	}
};
