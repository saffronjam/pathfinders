#pragma once

#include <stack>
#include <deque>
#include <thread>
#include <unordered_set>

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
	explicit Pathfinder(std::string name);
	virtual ~Pathfinder();

	void OnUpdate();

	void OnRenderAnticipation(Scene &scene);
	void OnRenderViaConnections(Scene &scene);
	void OnRenderBody(Scene &scene);
	void OnRenderResult(Scene &scene);

	auto State() const -> PathfinderState;
	auto StateString() const -> std::string;
	auto Name() -> const std::string&;
	auto Result() -> std::string;

	void AssignNodes(std::map<int, Node> nodes);
	void SetTraverseGrid(const std::shared_ptr<const TraverseGrid> &traverseGrid);
	void SetSleepDelay(sf::Time delay);
	void SetWeight(int uidFirst, int uidSecond, float weight);

	void Start(int startUID, int goalUID, const std::vector<int> &subGoalsUIDs);
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
	auto Nodes() -> std::map<int, Node>&;
	auto Nodes() const -> const std::map<int, Node>&;

	auto NodeByUid(int uid) -> Node&;
	virtual void FindPath(int startUID, int goalUID) = 0;

	void PauseCheck();
	void SleepDelay();

	auto FinalCost() -> float;

private:
	void RenderFinishedBodyHelper(Scene &scene, sf::Color color, int limit);
	void FindPathThreadFn(int startUID, int goalUID, const std::vector<int> &subGoalsUIDs);
	bool CheckFindPathResult(int fromUID, int toUID);
	void AppendFinalPath(int startUID, int goalUID);
	void CollectFinder();

protected:
	PathfinderState _state;
	std::shared_ptr<const TraverseGrid> _traverseGrid;
	int _activeNodeUID;

private:
	std::string _name;
	bool _active = true;

	std::thread _finder;
	std::mutex _mutex;

	sf::Color _bodyColor;

	sf::Time _sleepDelay;
	bool _minorDelay;
	sf::Int64 _minorDelayTimer;

	std::map<int, Node> _nodes;
	std::deque<const Node *> _finalPath;

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