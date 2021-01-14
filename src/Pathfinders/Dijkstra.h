#pragma once

#include "Pathfinder.h"

namespace Se
{
class Dijkstra : public Pathfinder
{
public:
	Dijkstra() : Pathfinder("Dijkstra")
	{
		SetBodyColor(sf::Color::Yellow);
	}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
};
}