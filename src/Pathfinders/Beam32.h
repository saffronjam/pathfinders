#pragma once

#include "Pathfinder.h"

namespace Se
{
class Beam32 : public Pathfinder
{
public:
	Beam32() : Pathfinder("Beam32") {}

	void FindPath(int startUID, int goalUID) override;

private:
	Deque<int> _checkingQueue;
	static constexpr int beamWidth = 32;
};
}