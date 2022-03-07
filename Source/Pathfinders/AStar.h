#pragma once

#include "Pathfinder.h"

namespace Se
{
class AStar : public Pathfinder
{
public:
	AStar();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
};

}