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
		Searching,
		WaitingForStart,
		Paused,
		Finished,
		BeingCollected
	};

public:
	explicit Pathfinder(String name);
	virtual ~Pathfinder();

	void OnUpdate();

	void OnRenderAnticipation(Scene &scene);
	void OnRenderViaConnections(Scene &scene);
	void OnRenderBody(Scene &scene);
	void OnRenderResult(Scene &scene);

	State GetState() const { return _state; }
	String GetStateString() const;
	const String &GetName() { return _name; }
	String GetResult();

	void AssignNodes(Map<int, Node> nodes);
	void SetTraverseGrid(const Shared<const TraverseGrid> &traverseGrid);
	void SetSleepDelay(sf::Time delay);
	void SetWeight(int uidFirst, int uidSecond, float weight);

	void Start(int startUID, int goalUID, const Set<int> &subGoalsUIDs);
	void Pause();
	void Resume();
	void Restart();
	void Reset();
	void Activate();
	void Deactivate();

	bool IsDone() const { return _state == State::Finished; }
	bool IsActive() const { return _active; }

	sf::Color GetBodyColor() const { return _bodyColor; }
	void SetBodyColor(sf::Color color) { _bodyColor = color; }

protected:
	Map<int, Node> &GetNodes() { return _nodes; }
	Node &GetNode(int uid) { return GetNodes().at(uid); }
	virtual void FindPath(int startUID, int goalUID) = 0;

	void PauseCheck();
	void SleepDelay();

	float GetFinalCost();

private:
	void RenderFinishedBodyHelper(Scene &scene, sf::Color color, int limit);
	void FindPathThreadFn(int startUID, int goalUID, const Set<int> &subGoalsUIDs);
	bool CheckFindPathResult(int fromUID, int toUID);
	void AppendFinalPath(int startUID, int goalUID);
	void CollectFinder();

protected:
	State _state;
	Shared<const TraverseGrid> _traverseGrid;
	int _activeNodeUID;

private:
	String _name;
	bool _active = true;

	Thread _finder;
	Mutex _mutex;

	sf::Color _bodyColor;

	sf::Time _sleepDelay;
	bool _minorDelay;
	sf::Int64 _minorDelayTimer;

	Map<int, Node> _nodes;
	Deque<const Node *> _finalPath;

	sf::Time _finalPathTimer;
	int _noFinalPathNodes = 0;
	int _bigCircleNodeIndex = 0;
	const sf::Time _finalPathTimerUpdateInterval = sf::seconds(0.05f);

	bool _pathWasFound = false;

	sf::VertexArray _bodyVA;
	sf::VertexArray _bodyFinishedVA;
	sf::VertexArray _viaVA;
};

}