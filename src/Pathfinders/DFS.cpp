#include "Pathfinders/DFS.h"

namespace Se
{
void DFS::FindPath(int startUID, int goalUID)
{

	auto getUnvisitedNeighbor = [&](int uid)
	{
		int neighborFound = -1;

		const  auto &neighbors = GetNode(uid).GetNeighbors();

		for ( int neighbor : neighbors )
		{
			auto result = _visited.find(neighbor);
			if ( result == _visited.end() && !_traverseGrid->IsEdgeObstacle(_activeNodeUID, neighbor) && neighbor != _activeNodeUID )
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
	GetNode(_activeNodeUID).SetCost("Tentative", 0.0f);
	while ( !_checkingStack.empty() && _state != State::BeingCollected )
	{

		if ( _state == State::BeingCollected )
		{
			break;
		}

		PauseCheck();
		SleepDelay();


		Node &activeNode = GetNode(_activeNodeUID);
		const int neighborUid = getUnvisitedNeighbor(_activeNodeUID);
		if ( neighborUid == -1 )
		{
			_activeNodeUID = _checkingStack.top();
			_checkingStack.pop();
		}
		else
		{
			_visited.emplace(neighborUid);
			_visited.emplace(_activeNodeUID);
			_checkingStack.push(_activeNodeUID);

			Node &neighbor = GetNode(neighborUid);
			const float suggestedTentativeCost = activeNode.GetCost("Tentative") + activeNode.GetNeighborCost(neighborUid);
			if ( suggestedTentativeCost < neighbor.GetCost("Tentative") )
			{
				neighbor.SetVia(_activeNodeUID);
				neighbor.SetCost("Tentative", suggestedTentativeCost);
			}

			_activeNodeUID = neighborUid;
		}


	}
	while ( !_checkingStack.empty() )
	{
		_checkingStack.pop();
	}
	_visited.clear();
}
}