#pragma once

#include "pathfinder.h"

namespace pathfinders
{
using namespace saffron;
class AStar : public Pathfinder
{
public:
	AStar();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
};

}