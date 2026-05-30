#pragma once

#include "pathfinder.h"

namespace pathfinders
{
using namespace saffron;
class Dijkstra : public Pathfinder
{
public:
	Dijkstra();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
};
}