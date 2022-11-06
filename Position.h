#pragma once
#include <set>
#include <cmath>

class Position
{
	int xPos;
	int yPos;

public:
	Position() : xPos(0), yPos(0) {}

	Position(int x, int y)
	{
		xPos = x;
		yPos = y;
	}

	int getX() const { return xPos; }
	int getY() const { return yPos; }

	int distanceToOrigin()
	{
		return std::sqrt(xPos * xPos + yPos * yPos);
	}

	static int difference(Position pos1, Position pos2)
	{
		return std::sqrt( std::pow(pos1.getX() - pos2.getX(), 2) + std::pow(pos1.getY() - pos2.getY(), 2));
	}

	int difference(Position other)
	{
		return difference(*this, other);
	}

	std::set<Position> getAdjacent()
	{
		std::set<Position> positions;

		positions.insert(Position(xPos - 1, yPos - 1));
		positions.insert(Position(xPos - 1, yPos ));
		positions.insert(Position(xPos - 1, yPos + 1));
		positions.insert(Position(xPos, yPos - 1));
		positions.insert(Position(xPos, yPos + 1));
		positions.insert(Position(xPos + 1, yPos - 1));
		positions.insert(Position(xPos + 1, yPos));
		positions.insert(Position(xPos + 1, yPos + 1));

		return positions;
	}

	static std::set<Position> getAdjacent(Position pos)
	{
		return pos.getAdjacent();
	}

	bool isAdjacent(Position another)
	{
		std::set<Position> adjacentPositions = getAdjacent();

		return adjacentPositions.find(another) != adjacentPositions.end();
	}

	static bool isAdjacent(Position one, Position another)
	{
		return one.isAdjacent(another);
	}

	bool operator==(Position another)
	{
		return xPos == another.getX() && yPos == another.getY();
	}

	bool operator!=(Position another)
	{
		return ! operator==(another);
	}

	bool operator<(Position another)
	{
		return getY() * 100 + getX() < another.getY() * 100 + another.getX();
	}

	friend bool operator<(Position one, Position another)
	{
		return one.getY() * 100 + one.getX() < another.getY() * 100 + another.getX();
	}
};