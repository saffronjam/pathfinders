#pragma once

#include "Pathfinder.h"

namespace Se
{
class AStar : public Pathfinder
{
public:
	AStar() : Pathfinder("A*")
	{
		SetBodyColor(sf::Color::Cyan);
	}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
};

}