#pragma once

#include "pathfinder.h"

namespace pathfinders
{
using namespace saffron;
class DFS : public Pathfinder
{
public:
	DFS();

	void FindPath(int startUID, int goalUID) override;

private:
	std::stack<int> _checkingStack;
	std::unordered_set<int> _visited;
};
}