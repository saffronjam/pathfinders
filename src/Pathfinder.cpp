#include "Pathfinder.h"

#include <SFML/System/Sleep.hpp>

namespace Se
{
Pathfinder::Pathfinder(String name) :
	_state(PathfinderState::WaitingForStart),
	_activeNodeUID(-1),
	_name(Move(name)),
	_sleepDelay(sf::seconds(0.01f)),
	_minorDelay(false),
	_minorDelayTimer(0),
	_bodyVA(sf::PrimitiveType::LineStrip),
	_bodyFinishedVA(sf::PrimitiveType::Quads),
	_viaVA(sf::PrimitiveType::Lines)
{
}

Pathfinder::~Pathfinder()
{
	Reset();
}

void Pathfinder::OnUpdate()
{
	if (_pathWasFound)
	{
		_finalPathTimer += Global::Clock::FrameTime();

		while (_finalPathTimer > _finalPathTimerUpdateInterval)
		{
			if (_noFinalPathNodes < _finalPath.size() - 1)
			{
				_noFinalPathNodes++;
			}
			_bigCircleNodeIndex++;
			if (_bigCircleNodeIndex >= _finalPath.size() - 1)
			{
				_bigCircleNodeIndex = 0;
			}

			_finalPathTimer -= _finalPathTimerUpdateInterval;
		}
	}
}

void Pathfinder::OnRenderAnticipation(Scene& scene)
{
	if (_activeNodeUID != -1 && _state == PathfinderState::Searching || _state == PathfinderState::Paused || _state ==
		PathfinderState::Finished)
	{
		_bodyVA.clear();
		_bodyVA.append({NodeByUid(_activeNodeUID).Position(), _bodyColor});
		int viaID = NodeByUid(_activeNodeUID).ViaUID();
		for (Node* node; viaID != -1; node = &NodeByUid(viaID), viaID = node->ViaUID())
		{
			_bodyVA.append({NodeByUid(viaID).Position(), _bodyColor});
		}
		scene.Submit(_bodyVA);

		RenderFinishedBodyHelper(scene, _bodyColor, _finalPath.size());
		scene.Submit(NodeByUid(_activeNodeUID).Position(), _bodyColor, 5.0f);
	}
}

void Pathfinder::OnRenderViaConnections(Scene& scene)
{
	_viaVA.clear();
	for (auto& [uid, node] : _nodes)
	{
		const int via = node.ViaUID();
		if (via != -1)
		{
			auto color = sf::Color(150, 150, 150, 20);
			_viaVA.append({node.Position(), color});
			_viaVA.append({NodeByUid(via).Position(), color});
		}
	}
	scene.Submit(_viaVA);
}

void Pathfinder::OnRenderBody(Scene& scene)
{
	Done() ? OnRenderResult(scene) : OnRenderAnticipation(scene);
}

void Pathfinder::OnRenderResult(Scene& scene)
{
	if (_pathWasFound)
	{
		auto bc = BodyColor();
		bc.a = 140.0f;
		RenderFinishedBodyHelper(scene, bc, _noFinalPathNodes);
		bc.a = 255.0f;
		scene.Submit(_finalPath[_bigCircleNodeIndex]->Position(), bc, 5.0f);
	}
}

auto Pathfinder::State() const -> PathfinderState
{
	return _state;
}

auto Pathfinder::Name() -> const String&
{
	return _name;
}

auto Pathfinder::StateString() const -> String
{
	switch (_state)
	{
	case PathfinderState::Searching: return "Searching";
	case PathfinderState::WaitingForStart: return "Waiting";
	case PathfinderState::Paused: return "Paused";
	case PathfinderState::Finished: return _pathWasFound ? "Finished" : "Failed";
	case PathfinderState::BeingCollected: return "Collecting";
	default: return "INVALID STATE";
	}
}

auto Pathfinder::Result() -> String
{
	if (_pathWasFound)
	{
		OStringStream oss;
		oss << "Cost: " << FinalCost();
		return oss.str();
	}
	return "No path found";
}

void Pathfinder::AssignNodes(TreeMap<int, Node> nodes)
{
	_nodes = Move(nodes);
}

void Pathfinder::SetTraverseGrid(const Shared<const TraverseGrid>& traverseGrid)
{
	_activeNodeUID = -1;
	_traverseGrid = traverseGrid;
}

void Pathfinder::Start(int startUID, int goalUID, const List<int>& subGoalsUIDs)
{
	if (_state == PathfinderState::Finished)
	{
		Restart();
	}
	if (_state == PathfinderState::WaitingForStart)
	{
		CollectFinder();
		_state = PathfinderState::Searching;
		_finder = Thread(&Pathfinder::FindPathThreadFn, this, startUID, goalUID, subGoalsUIDs);
	}
}

void Pathfinder::Pause()
{
	if (_state == PathfinderState::Searching)
	{
		_state = PathfinderState::Paused;
	}
}

void Pathfinder::Resume()
{
	if (_state == PathfinderState::Paused)
	{
		_state = PathfinderState::Searching;
	}
}

void Pathfinder::Restart()
{
	if (_state == PathfinderState::Searching || _state == PathfinderState::Paused || _state == PathfinderState::Finished
	)
	{
		std::scoped_lock scopedLock(_mutex);
		_pathWasFound = false;
		_finalPathTimer = sf::Time::Zero;
		_noFinalPathNodes = 0;
		_bigCircleNodeIndex = 0;
		_finalPath.clear();
		CollectFinder();
		_state = PathfinderState::WaitingForStart;
		for (auto& [uid, node] : _nodes)
		{
			node.ResetPath();
			node.ClearVisitedNeighbors();
		}
	}
}

void Pathfinder::Reset()
{
	Restart();
	if (_state == PathfinderState::Searching || _state == PathfinderState::Paused || _state == PathfinderState::Finished
	)
	{
		std::scoped_lock scopedLock(_mutex);
		for (auto& [uid, node] : _nodes)
		{
			node.ResetNeighborsCost();
		}
	}
}

void Pathfinder::Activate()
{
	_active = true;
}

void Pathfinder::Deactivate()
{
	_active = false;
}

auto Pathfinder::Done() const -> bool
{
	return _state == PathfinderState::Finished;
}

bool Pathfinder::Active() const
{
	return _active;
}

auto Pathfinder::BodyColor() const -> sf::Color
{
	return _bodyColor;
}

void Pathfinder::SetBodyColor(sf::Color color)
{
	_bodyColor = color;
}

auto Pathfinder::Nodes() -> TreeMap<int, Node>&
{
	return _nodes;
}

auto Pathfinder::Nodes() const -> const TreeMap<int, Node>&
{
	return const_cast<Pathfinder&>(*this).Nodes();
}

auto Pathfinder::NodeByUid(int uid) -> Node&
{
	return Nodes().at(uid);
}

void Pathfinder::SetSleepDelay(sf::Time delay)
{
	_sleepDelay = delay;
	_minorDelay = _sleepDelay.asMicroseconds() < 1000;
}

void Pathfinder::SetWeight(int uidFirst, int uidSecond, float weight)
{
	NodeByUid(uidFirst).SetNeighborCost(weight, uidSecond);
	NodeByUid(uidSecond).SetNeighborCost(weight, uidFirst);
}

void Pathfinder::PauseCheck()
{
	while (_state == PathfinderState::Paused && _state != PathfinderState::BeingCollected)
	{
		sf::sleep(sf::seconds(0.01f));
	}
}

void Pathfinder::SleepDelay()
{
	if (!_minorDelay)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(_sleepDelay.asMicroseconds()));
	}
	else
	{
		_minorDelayTimer += _sleepDelay.asMicroseconds();
		while (_minorDelayTimer > 1000)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			_minorDelayTimer -= 1000;
		}
	}
}

auto Pathfinder::FinalCost() -> float
{
	std::scoped_lock scopedLock(_mutex);

	Debug::Assert(!_finalPath.empty(), "Cost can only be calculated if path is found");

	float cost = 0.0f;
	for (int i = 0; i < _finalPath.size() - 1; i++)
	{
		cost += _finalPath[i]->NeighborCostByUid(_finalPath[i + 1]->Uid());
	}
	return cost;
}

void Pathfinder::RenderFinishedBodyHelper(Scene& scene, sf::Color color, int limit)
{
	// Finished body
	_bodyFinishedVA.clear();
	_bodyFinishedVA.resize(_finalPath.size() * 4);
	const float halfSize = 1.5f;
	auto finalPathCopy = _finalPath;
	for (int i = 0; i < limit; i++)
	{
		const sf::Vector2f position = finalPathCopy[i]->Position();

		_bodyFinishedVA[i * 4] = {position - sf::Vector2f{halfSize, 0.0f}, color};
		_bodyFinishedVA[i * 4 + 1] = {position - sf::Vector2f{0.0f, halfSize}, color};
		_bodyFinishedVA[i * 4 + 2] = {position + sf::Vector2f{halfSize, 0.0f}, color};
		_bodyFinishedVA[i * 4 + 3] = {position + sf::Vector2f{0.0f, halfSize}, color};
	}
	scene.Submit(_bodyFinishedVA);
}

void Pathfinder::FindPathThreadFn(int startUID, int goalUID, const List<int>& subGoalsUIDs)
{
	_finalPath.clear();

	int fromUID = startUID;
	int toUID;

	for (int subGoalsUID : subGoalsUIDs)
	{
		toUID = subGoalsUID;
		FindPath(fromUID, toUID);
		const bool result = CheckFindPathResult(fromUID, toUID);
		if (!result)
		{
			_pathWasFound = false;
			_state = PathfinderState::Finished;
			return;
		}
		fromUID = subGoalsUID;
	};
	toUID = goalUID;
	FindPath(fromUID, toUID);
	_pathWasFound = CheckFindPathResult(fromUID, toUID);

	if (_pathWasFound)
	{
		_finalPath.push_front(&NodeByUid(startUID));
	}

	_noFinalPathNodes = 0;
	_finalPathTimer = sf::Time::Zero;

	_state = PathfinderState::Finished;
}

auto Pathfinder::CheckFindPathResult(int fromUID, int toUID) -> bool
{
	if (_state == PathfinderState::BeingCollected)
	{
		return false;
	}

	const bool foundPath = Nodes().at(toUID).Visited();
	if (foundPath)
	{
		AppendFinalPath(fromUID, toUID);
		for (auto& [uid, node] : _nodes)
		{
			node.ResetPath();
		}
		return true;
	}
	return false;
}

void Pathfinder::AppendFinalPath(int startUID, int goalUID)
{
	List<const Node*> tmp;
	for (const Node* node = &NodeByUid(goalUID); node != &NodeByUid(startUID); node = &NodeByUid(node->ViaUID()))
	{
		tmp.push_back(node);
	}
	std::reverse_copy(tmp.begin(), tmp.end(), std::back_inserter(_finalPath));
}

void Pathfinder::CollectFinder()
{
	const auto savedState = _state;
	_state = PathfinderState::BeingCollected;
	if (_finder.joinable()) _finder.join();
	_state = savedState;
}
}
