#pragma once
#include "Position.h"

class Map
{
	int mapHeight;
	int mapWidth;
	std::set<Position> allPositions;

public:
	Map() {}

	Map(Map& another)
	{
		mapHeight = another.mapHeight;
		mapWidth = another.mapWidth;

		allPositions = another.allPositions;
	}

	Map(int mapHeight, int mapWidth)
	{
		this->mapHeight = mapHeight;
		this->mapWidth = mapWidth;

		for (int i = 0; i < mapHeight; i++)
			for (int j = 0; j < mapWidth; j++)
				allPositions.insert(Position(i + 1, j + 1));
	}

	std::set<Position> getAllPositions() { return allPositions; }

	bool isValid(Position pos)
	{
		return pos.getX() >= 1 && pos.getX() <= mapWidth && pos.getY() >= 1 && pos.getY() <= mapHeight;
	}

	int getHeight() { return mapHeight; }
	int getWidth() { return mapWidth; }
};