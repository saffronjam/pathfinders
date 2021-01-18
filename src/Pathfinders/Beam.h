#pragma once

#include "Pathfinder.h"

namespace Se
{
template<int BeamWidth>
class Beam : public Pathfinder
{
public:
	Beam() : Pathfinder("Beam " + std::to_string(BeamWidth))
	{
		const float shade = static_cast<float>(_beamWidth) / 512.0f * 255.0f;
		SetBodyColor(sf::Color(255, 128, shade));
	}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
	static constexpr int _beamWidth = BeamWidth;
};

template <int BeamWidth>
void Beam<BeamWidth>::FindPath(int startUID, int goalUID)
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
			if ( !_traverseGrid->IsEdgeObstacle(_activeNodeUID, neighborUID) && neighborUID != activeNode.GetViaUID() )
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

		if ( _checkingQueue.size() > _beamWidth )
		{
			int toErase = _checkingQueue.size() - _beamWidth;
			_checkingQueue.erase(_checkingQueue.end() - toErase, _checkingQueue.end());
		}
	}
	_checkingQueue.clear();
}
}
