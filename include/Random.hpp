#pragma once
#include <random>

class Random
{

public:
	Random()
		: lowerBound(0),
		  upperBound(10),
		  rng(rd()),
		  randomNum(lowerBound, upperBound){};

	int operator()()
	{
		return randomNum(rng);
	}

	void SetLowerBound(int lowerBound_IN)
	{
		lowerBound = lowerBound_IN;
		randomNum = std::uniform_int_distribution<int>(lowerBound, upperBound);
	};
	void SetUpperBound(int upperBound_IN)
	{
		upperBound = upperBound_IN;
		randomNum = std::uniform_int_distribution<int>(lowerBound, upperBound);
	};

	static double Generate(double lower, double upper)
	{
		if (lower > upper)
		{
			std::swap(lower, upper);
		}
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> randomNum(lower, upper);
		return randomNum(rng);
	}

private:
	int lowerBound;
	int upperBound;

	std::random_device rd;
	std::mt19937 rng;
	std::uniform_int_distribution<int> randomNum;
};