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
	Stack<int> _checkingStack;
	HashSet<int> _visited;
};
}