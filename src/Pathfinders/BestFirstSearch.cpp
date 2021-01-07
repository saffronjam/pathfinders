#include "Pathfinders/BestFirstSearch.h"

namespace Se
{
void BestFirstSearch::FindPath(int startUID, int goalUID)
{
	_checkingQueue.push_front(startUID);
	GetNode(startUID).SetCost("Heuristic", VecUtils::Length(GetNode(startUID).GetPosition() - GetNode(goalUID).GetPosition()));
	while ( !_checkingQueue.empty() && _state != State::BeingCollected )
	{
		PauseCheck();
		_activeNodeUID = _checkingQueue.front();
		if ( _activeNodeUID == goalUID )
		{
			break;
		}

		Node &activeNode = GetNode(_activeNodeUID);
		_checkingQueue.pop_front();

		for ( const auto &neighborUID : activeNode.GetNeighbors() )
		{
			if ( _state == State::BeingCollected )
				break;
			PauseCheck();
			SleepDelay();

			Node &neighbor = GetNode(neighborUID);
			if ( !_traverseGrid->IsObstacle(neighborUID) && neighborUID != activeNode.GetViaUID() )
			{
				neighbor.SetCost("Heuristic",
								 VecUtils::Length(neighbor.GetPosition() - GetNode(goalUID).GetPosition()));
				if ( !neighbor.WasVisited() )
				{
					_checkingQueue.push_front(neighborUID);
					neighbor.SetVia(_activeNodeUID);
				}
			}
			activeNode.AddVisitedNeighbor(neighborUID);
		}
		std::sort(_checkingQueue.begin(),
				  _checkingQueue.end(),
				  [this](const auto &lhs, const auto &rhs)
				  {
					  return GetNode(lhs).GetCost("Heuristic") < GetNode(rhs).GetCost("Heuristic");
				  });
	}
	_checkingQueue.clear();
}
}