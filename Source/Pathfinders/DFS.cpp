#include "Pathfinders/DFS.h"

namespace Se
{
DFS::DFS() :
	Pathfinder("DFS")
{
	SetBodyColor(sf::Color::Blue);
}

void DFS::FindPath(int startUID, int goalUID)
{
	auto getUnvisitedNeighbor = [&](int uid)
	{
		int neighborFound = -1;

		const auto& neighbors = NodeByUid(uid).Neighbors();

		for (int neighbor : neighbors)
		{
			if( _state == PathfinderState::BeingCollected)
			{
				return 0;
			}

			auto result = _visited.find(neighbor);
			if (result == _visited.end() && !_traverseGrid->IsEdgeObstacle(_activeNodeUID, neighbor) && neighbor !=
				_activeNodeUID)
			{
				neighborFound = neighbor;
				break;
			}
		}

		return neighborFound;
	};

	_activeNodeUID = startUID;
	
	_checkingStack.push(_activeNodeUID);
	_visited.emplace(_activeNodeUID);
	NodeByUid(_activeNodeUID).SetCost("Tentative", 0.0f);
	while (!_checkingStack.empty() && _state != PathfinderState::BeingCollected)
	{
		PauseCheck();
		SleepDelay();

		Node& activeNode = NodeByUid(_activeNodeUID);
		const int neighborUid = getUnvisitedNeighbor(_activeNodeUID);
		if( _state == PathfinderState::BeingCollected)
		{
			break;
		}
		
		if (neighborUid == -1)
		{
			_activeNodeUID = _checkingStack.top();
			_checkingStack.pop();
		}
		else
		{
			_visited.emplace(neighborUid);
			_visited.emplace(_activeNodeUID);
			_checkingStack.push(_activeNodeUID);

			Node& neighbor = NodeByUid(neighborUid);
			const float suggestedTentativeCost = activeNode.Cost("Tentative") + activeNode.NeighborCostByUid(
				neighborUid);
			if (suggestedTentativeCost < neighbor.Cost("Tentative"))
			{
				neighbor.SetVia(_activeNodeUID);
				neighbor.SetCost("Tentative", suggestedTentativeCost);
			}

			_activeNodeUID = neighborUid;
		}
	}
	while (!_checkingStack.empty())
	{
		_checkingStack.pop();
	}
	_visited.clear();
}
}
