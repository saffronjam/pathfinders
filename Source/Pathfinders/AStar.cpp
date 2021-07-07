#include "Pathfinders/AStar.h"

namespace Se
{
AStar::AStar() :
	Pathfinder("A*")
{
	SetBodyColor(sf::Color::Cyan);
}

void AStar::FindPath(int startUID, int goalUID)
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
					if (!neighbor.HasCost("Heuristic"))
					{
						neighbor.SetCost("Heuristic",
						                 VecUtils::Length(neighbor.Position() - NodeByUid(goalUID).Position()));
					}
					neighbor.SetCost("Total", suggestedTentativeCost + neighbor.Cost("Heuristic"));
				}
			}
			activeNode.AddVisitedNeighbor(neighborUID);
		}
		std::ranges::sort(_checkingQueue, [this](const int& lhs, const int& rhs)
		{
			return NodeByUid(lhs).Cost("Total") < NodeByUid(rhs).Cost("Total");
		});
	}
	_checkingQueue.clear();
}
}
