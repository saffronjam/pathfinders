#pragma once

#include "Pathfinder.h"

namespace Se
{
class BestFirstSearch : public Pathfinder
{
public:
	BestFirstSearch() : Pathfinder("Best First Search")
	{
		SetBodyColor(sf::Color::Magenta);
	}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
};
}