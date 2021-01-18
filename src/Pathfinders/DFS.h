#pragma once

#include "Pathfinder.h"

namespace Se
{
class DFS : public Pathfinder
{
public:
	DFS() : Pathfinder("DFS")
	{
		SetBodyColor(sf::Color::Blue);
	}

	void FindPath(int startUID, int goalUID) override;

private:
	Stack<int> _checkingStack;
	Set<int> _visited;
};
}