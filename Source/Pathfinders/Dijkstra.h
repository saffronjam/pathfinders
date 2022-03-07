#pragma once

#include "Pathfinder.h"

namespace Se
{
class Dijkstra : public Pathfinder
{
public:
	Dijkstra();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
};
}