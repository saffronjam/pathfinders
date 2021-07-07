#include "Pathfinders/BFS.h"

namespace Se
{
BFS::BFS() :
	Pathfinder("BFS")
{
	SetBodyColor(sf::Color::Green);
}

void BFS::FindPath(int startUID, int goalUID)
{
	_checkingQueue.push_front(startUID);
	NodeByUid(startUID).SetCost("Tentative", 0.0f);
	while (!_checkingQueue.empty() && _state != PathfinderState::BeingCollected)
	{
		PauseCheck();
		_activeNodeUID = _checkingQueue.front();
		if (_activeNodeUID == goalUID)
		{
			break;
		}

		Node& activeNode = NodeByUid(_activeNodeUID);
		_checkingQueue.pop_front();

		for (const auto& neighborUID : activeNode.Neighbors())
		{
			if (_state == PathfinderState::BeingCollected)
			{
				break;
			}
			PauseCheck();
			SleepDelay();

			Node& neighbor = NodeByUid(neighborUID);
			if (!_traverseGrid->IsEdgeObstacle(_activeNodeUID, neighborUID) && neighborUID != activeNode.ViaUID())
			{
				const float suggestedTentativeCost = activeNode.Cost("Tentative") + activeNode.NeighborCostByUid(
					neighborUID);
				if (suggestedTentativeCost < neighbor.Cost("Tentative"))
				{
					if (std::ranges::find(_checkingQueue, neighborUID) == _checkingQueue.end())
					{
						_checkingQueue.push_back(neighborUID);
					}

					neighbor.SetVia(_activeNodeUID);
					neighbor.SetCost("Tentative", suggestedTentativeCost);
				}
			}
			activeNode.AddVisitedNeighbor(neighborUID);
		}
	}
	_checkingQueue.clear();
}
}
