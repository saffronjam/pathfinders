#pragma once

#include "pathfinder.h"

namespace pathfinders
{
using namespace saffron;
class BestFirstSearch : public Pathfinder
{
public:
	BestFirstSearch();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
};
}