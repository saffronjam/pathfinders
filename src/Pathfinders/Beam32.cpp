#include "Pathfinders/Beam32.h"

namespace Se
{
void Beam32::FindPath(int startUID, int goalUID)
{
	_checkingQueue.push_front(startUID);
	GetNode(startUID).SetCost("Tentative", 0.0f);
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
				float suggestedTentativeCost =
					activeNode.GetCost("Tentative") + activeNode.GetNeighborCost(neighborUID);
				if ( suggestedTentativeCost < neighbor.GetCost("Tentative") )
				{
					if ( std::find(_checkingQueue.begin(), _checkingQueue.end(), neighborUID) == _checkingQueue.end() )
						_checkingQueue.push_back(neighborUID);

					neighbor.SetVia(_activeNodeUID);
					neighbor.SetCost("Tentative", suggestedTentativeCost);
					neighbor.SetCost("Heuristic",
									 VecUtils::Length(neighbor.GetPosition() - GetNode(goalUID).GetPosition()));
					neighbor.SetCost("Total", suggestedTentativeCost + neighbor.GetCost("Heuristic"));
				}
			}
			activeNode.AddVisitedNeighbor(neighborUID);
		}
		std::sort(_checkingQueue.begin(), _checkingQueue.end(), [this](const auto &lhs, const auto &rhs)
				  {
					  return GetNode(lhs).GetCost("Total") < GetNode(rhs).GetCost("Total");
				  });

		if ( _checkingQueue.size() > beamWidth )
		{
			int toErase = _checkingQueue.size() - beamWidth;
			_checkingQueue.erase(_checkingQueue.end() - toErase, _checkingQueue.end());
		}
	}
	_checkingQueue.clear();
}
}