#pragma once

#include "Pathfinder.h"

namespace Se
{
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