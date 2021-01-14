#pragma once

#include "Pathfinder.h"

namespace Se
{
class BFS : public Pathfinder
{
public:
	BFS() : Pathfinder("BFS")
	{
		SetBodyColor(sf::Color::Green);
	}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
};
}