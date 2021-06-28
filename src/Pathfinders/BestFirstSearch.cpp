#include "Pathfinders/BestFirstSearch.h"

namespace Se
{
BestFirstSearch::BestFirstSearch() :
	Pathfinder("Best First Search")
{
	SetBodyColor(sf::Color::Magenta);
}

void BestFirstSearch::FindPath(int startUID, int goalUID)
{
	_checkingQueue.push_front(startUID);
	NodeByUid(startUID).SetCost("Heuristic",
	                            VecUtils::Length(NodeByUid(startUID).Position() - NodeByUid(goalUID).Position()));
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
			if (_state == PathfinderState::BeingCollected) return;
			PauseCheck();
			SleepDelay();

			Node& neighbor = NodeByUid(neighborUID);
			if (!_traverseGrid->IsEdgeObstacle(_activeNodeUID, neighborUID) && neighborUID != activeNode.ViaUID())
			{
				neighbor.SetCost("Heuristic", VecUtils::Length(neighbor.Position() - NodeByUid(goalUID).Position()));
				if (!neighbor.Visited())
				{
					_checkingQueue.push_front(neighborUID);
					neighbor.SetVia(_activeNodeUID);
				}
			}
			activeNode.AddVisitedNeighbor(neighborUID);
		}
		std::ranges::sort(_checkingQueue, [this](const auto& lhs, const auto& rhs)
		{
			return NodeByUid(lhs).Cost("Heuristic") < NodeByUid(rhs).Cost("Heuristic");
		});
	}
	_checkingQueue.clear();
}
}
