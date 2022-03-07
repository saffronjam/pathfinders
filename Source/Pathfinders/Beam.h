#pragma once

#include "Pathfinder.h"

namespace Se
{
template <int BeamWidth>
class Beam : public Pathfinder
{
public:
	Beam();

	void FindPath(int startUID, int goalUID) override;

private:
	std::deque<int> _checkingQueue;
	static constexpr int _beamWidth = BeamWidth;
};

template <int BeamWidth>
Beam<BeamWidth>::Beam() :
	Pathfinder("Beam " + std::to_string(BeamWidth))
{
	const float shade = static_cast<float>(_beamWidth) / 512.0f * 255.0f;
	SetBodyColor(sf::Color(255, 128, shade));
}

template <int BeamWidth>
void Beam<BeamWidth>::FindPath(int startUID, int goalUID)
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
				const auto suggestedTentativeCost = activeNode.Cost("Tentative") + activeNode.NeighborCostByUid(
					neighborUID);
				if (suggestedTentativeCost < neighbor.Cost("Tentative"))
				{
					if (std::ranges::find(_checkingQueue, neighborUID) == _checkingQueue.end())
						_checkingQueue.push_back(neighborUID);

					neighbor.SetVia(_activeNodeUID);
					neighbor.SetCost("Tentative", suggestedTentativeCost);
					neighbor.SetCost("Heuristic",
					                 VecUtils::Length(neighbor.Position() - NodeByUid(goalUID).Position()));
					neighbor.SetCost("Total", suggestedTentativeCost + neighbor.Cost("Heuristic"));
				}
			}
			activeNode.AddVisitedNeighbor(neighborUID);
		}
		std::sort(_checkingQueue.begin(), _checkingQueue.end(), [this](const auto& lhs, const auto& rhs)
		{
			return NodeByUid(lhs).Cost("Total") < NodeByUid(rhs).Cost("Total");
		});

		if (_checkingQueue.size() > _beamWidth)
		{
			const auto toErase = _checkingQueue.size() - _beamWidth;
			_checkingQueue.erase(_checkingQueue.end() - toErase, _checkingQueue.end());
		}
	}
	_checkingQueue.clear();
}
}
