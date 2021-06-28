#pragma once

#include <thread>

#include <SFML/System/Time.hpp>

#include <Saffron.h>

#include "TraverseGrid.h"

namespace Se
{
enum class PathfinderState
{
	Searching,
	WaitingForStart,
	Paused,
	Finished,
	BeingCollected
};
	
class Pathfinder
{
public:
	explicit Pathfinder(String name);
	virtual ~Pathfinder();

	void OnUpdate();

	void OnRenderAnticipation(Scene &scene);
	void OnRenderViaConnections(Scene &scene);
	void OnRenderBody(Scene &scene);
	void OnRenderResult(Scene &scene);

	auto State() const -> PathfinderState;
	auto StateString() const -> String;
	auto Name() -> const String&;
	auto Result() -> String;

	void AssignNodes(TreeMap<int, Node> nodes);
	void SetTraverseGrid(const Shared<const TraverseGrid> &traverseGrid);
	void SetSleepDelay(sf::Time delay);
	void SetWeight(int uidFirst, int uidSecond, float weight);

	void Start(int startUID, int goalUID, const List<int> &subGoalsUIDs);
	void Pause();
	void Resume();
	void Restart();
	void Reset();
	void Activate();
	void Deactivate();

	bool Done() const;
	bool Active() const;

	auto BodyColor() const -> sf::Color;
	void SetBodyColor(sf::Color color);

protected:
	auto Nodes() -> TreeMap<int, Node>&;
	auto Nodes() const -> const TreeMap<int, Node>&;

	auto NodeByUid(int uid) -> Node&;
	virtual void FindPath(int startUID, int goalUID) = 0;

	void PauseCheck();
	void SleepDelay();

	auto FinalCost() -> float;

private:
	void RenderFinishedBodyHelper(Scene &scene, sf::Color color, int limit);
	void FindPathThreadFn(int startUID, int goalUID, const List<int> &subGoalsUIDs);
	bool CheckFindPathResult(int fromUID, int toUID);
	void AppendFinalPath(int startUID, int goalUID);
	void CollectFinder();

protected:
	PathfinderState _state;
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

	TreeMap<int, Node> _nodes;
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