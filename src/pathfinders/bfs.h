#pragma once

#include "pathfinder.h"

namespace pathfinders
{
using namespace saffron;
class BFS : public Pathfinder
{
public:
	BFS();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
};
}