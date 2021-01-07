#pragma once

#include <thread>

#include <SFML/System/Time.hpp>

#include <Saffron.h>

#include "TraverseGrid.h"

namespace Se
{
class Pathfinder
{
public:
	enum class State
	{
		Finding,
		WaitingForStart,
		Paused,
		Finished,
		BeingCollected
	};

public:
	explicit Pathfinder(String name);
	virtual ~Pathfinder();

	void OnRenderAnticipation(Scene &scene);
	void OnRenderViaConnections(Scene &scene);
	void OnRenderNeighbors(Scene &scene);
	void OnRenderResult(Scene &scene);

	State GetState() const { return _state; }
	const String &GetName() { return _name; }

	void AssignNodes(const Map<int, Node> &nodes);
	void SetTraverseGrid(const Shared<const TraverseGrid> &traverseGrid) { _traverseGrid = traverseGrid; }
	void SetSleepDelay(sf::Time delay);

	void Start(int startUID, int goalUID, const ArrayList<int> &subGoalsUIDs);
	void Pause();
	void Resume();
	void Restart();
	void Reset();

	bool IsDone() const { return _state == State::Finished; }

protected:
	Map<int, Node> &GetNodes() { return _nodes; }
	Node &GetNode(int uid) { return GetNodes().at(uid); }
	virtual void FindPath(int startUID, int goalUID) = 0;

	void PauseCheck();
	void SleepDelay();

private:
	void FindPathThreadFn(int startUID, int goalUID, const ArrayList<int> &subGoalsUIDs);
	bool CheckFindPathResult(int fromUID, int toUID);
	void AppendFinalPath(int startUID, int goalUID);
	void CollectFinder();

protected:
	State _state;
	Shared<const TraverseGrid> _traverseGrid;
	int _activeNodeUID;

private:
	String _name;

	Thread _finder;

	sf::Time _sleepDelay;
	bool _minorDelay;
	sf::Int64 _minorDelayTimer;

	Map<int, Node> _nodes;
	ArrayList<const Node *> _finalPath;
	sf::Time _finalPathTimer;
	int _noFinalPathNodes = 0;

	bool _pathWasFound = false;
};

}