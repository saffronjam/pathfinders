#pragma once

#include "Pathfinder.h"

namespace Se
{
class Beam512 : public Pathfinder
{
public:
	Beam512() : Pathfinder("Beam512") {}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
	static constexpr int beamWidth = 512;
};
}