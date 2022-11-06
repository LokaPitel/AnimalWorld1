#pragma once
#include <random>

class MathUtility
{
public:
	static int randomInt(int from, int to)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(from, to);
		
		return dist(rd);
	}
};
