#pragma once
#include "Position.h"
#include "Entity.h"
#include "MathUtility.h"

class SetUtility
{
public:
	static std::set<Position> Union(std::set<Position> first, std::set<Position> second)
	{
		std::set<Position> result;

		for (Position pos : first)
			result.insert(pos);

		for (Position pos : second)
			result.insert(pos);

		return result;
	}

	static std::set<Position> Intersection(std::set<Position> first, std::set<Position> second)
	{
		std::set<Position> result;

		for (Position pos : first)
			if (isIn(second, pos))
				result.insert(pos);

		return result;
	}

	static std::set<Position> Difference(std::set<Position> first, std::set<Position> second)
	{
		std::set<Position> result;

		for (Position pos : first)
			if (!isIn(second, pos))
				result.insert(pos);

		return result;
	}

	static std::set<Entity *> Union(std::set<Entity *> first, std::set<Entity *> second)
	{
		std::set<Entity*> result;

		for (Entity* ent : first)
			result.insert(ent);

		for (Entity* ent : second)
			result.insert(ent);

		return result;
	}

	static std::set<Entity *> Intersection(std::set<Entity *> first, std::set<Entity *> second)
	{
		std::set<Entity*> result;

		for (Entity* ent : first)
			if (isIn(second, ent))
				result.insert(ent);

		return result;
	}

	static std::set<Entity*> Difference(std::set<Entity*> first, std::set<Entity*> second)
	{
		std::set<Entity*> result;

		for (Entity* ent : first)
			if (isIn(second, ent))
				result.insert(ent);

		return result;
	}

	template<typename T>
	static bool isIn(std::set<T> searchSet, T entity)
	{
		for (T item : searchSet)
			if (item == entity)
				return true;

		return false;
	}

	static Entity* randomFrom(std::set<Entity*> from)
	{
		int index = MathUtility::randomInt(0, from.size() - 1);

		int counter = 0;
		for (Entity* item : from)
		{
			if (counter == index)
				return item;

			counter++;
		}
	}

	static Position randomFrom(std::set<Position> from)
	{
		int index = MathUtility::randomInt(0, from.size() - 1);

		int counter = 0;
		for (Position item : from)
		{
			if (counter == index)
				return item;

			counter++;
		}
	}
};