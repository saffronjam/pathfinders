#include "Pathfinder.h"

#include <SFML/System/Sleep.hpp>

namespace Se
{
Pathfinder::Pathfinder(String name) :
	_state(State::WaitingForStart),
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
	if ( _pathWasFound )
	{
		_finalPathTimer += Global::Clock::GetFrameTime();

		while ( _finalPathTimer > _finalPathTimerUpdateInterval )
		{
			if ( _noFinalPathNodes < _finalPath.size() - 1 )
			{
				_noFinalPathNodes++;
			}
			_bigCircleNodeIndex++;
			if ( _bigCircleNodeIndex >= _finalPath.size() - 1 )
			{
				_bigCircleNodeIndex = 0;
			}

			_finalPathTimer -= _finalPathTimerUpdateInterval;
		}
	}
}

void Pathfinder::OnRenderAnticipation(Scene &scene)
{
	if ( _activeNodeUID != -1 && _state == State::Searching || _state == State::Paused || _state == State::Finished )
	{
		_bodyVA.clear();
		_bodyVA.append({ GetNode(_activeNodeUID).GetPosition(), _bodyColor });
		int viaID = GetNode(_activeNodeUID).GetViaUID();
		for ( Node *node; viaID != -1; node = &GetNode(viaID), viaID = node->GetViaUID() )
		{
			_bodyVA.append({ GetNode(viaID).GetPosition(), _bodyColor });
		}
		scene.Submit(_bodyVA);

		RenderFinishedBodyHelper(scene, _bodyColor, _finalPath.size());
		scene.Submit(GetNode(_activeNodeUID).GetPosition(), _bodyColor, 5.0f);
	}
}

void Pathfinder::OnRenderViaConnections(Scene &scene)
{
	_viaVA.clear();
	for ( auto &[uid, node] : _nodes )
	{
		const int via = node.GetViaUID();
		if ( via != -1 )
		{
			auto color = sf::Color(150, 150, 150, 20);
			_viaVA.append({ node.GetPosition(), color });
			_viaVA.append({ GetNode(via).GetPosition(), color });
		}
	}
	scene.Submit(_viaVA);
}

void Pathfinder::OnRenderBody(Scene &scene)
{
	IsDone() ? OnRenderResult(scene) : OnRenderAnticipation(scene);
}

void Pathfinder::OnRenderNeighbors(Scene &scene)
{
	for ( auto &[uid, node] : _nodes )
	{
		for ( const auto &neighborUID : node.GetNeighbors() )
		{
			scene.Submit(node.GetPosition(), GetNode(neighborUID).GetPosition(), sf::Color(255, 0, 255, 20));
		}
	}
}

void Pathfinder::OnRenderResult(Scene &scene)
{
	if ( _pathWasFound )
	{
		auto bc = GetBodyColor();
		bc.a = 140.0f;
		RenderFinishedBodyHelper(scene, bc, _noFinalPathNodes);
		bc.a = 255.0f;
		scene.Submit(_finalPath[_bigCircleNodeIndex]->GetPosition(), bc, 5.0f);
	}
}

String Pathfinder::GetStateString() const
{
	switch ( _state )
	{
	case State::Searching:
		return "Searching";
	case State::WaitingForStart:
		return "Waiting";
	case State::Paused:
		return "Paused";
	case State::Finished:
		return _pathWasFound ? "Finished" : "Failed";
	case State::BeingCollected:
		return "Collecting";
	default:
		return "INVALID STATE";
	}
}

String Pathfinder::GetResult()
{
	if ( _pathWasFound )
	{
		OutputStringStream oss;
		oss << "Cost: " << GetFinalCost();
		return oss.str();
	}
	return "No path found";
}

void Pathfinder::AssignNodes(Map<int, Node> nodes)
{
	_nodes = Move(nodes);
}

void Pathfinder::SetTraverseGrid(const Shared<const TraverseGrid> &traverseGrid)
{
	_activeNodeUID = -1;
	_traverseGrid = traverseGrid;
}

void Pathfinder::Start(int startUID, int goalUID, const Set<int> &subGoalsUIDs)
{
	if ( _state == State::Finished )
	{
		Restart();
	}
	if ( _state == State::WaitingForStart )
	{
		CollectFinder();
		_state = State::Searching;
		_finder = Thread(&Pathfinder::FindPathThreadFn, this, startUID, goalUID, subGoalsUIDs);
	}
}

void Pathfinder::Pause()
{
	if ( _state == State::Searching )
	{
		_state = State::Paused;
	}
}

void Pathfinder::Resume()
{
	if ( _state == State::Paused )
	{
		_state = State::Searching;
	}
}

void Pathfinder::Restart()
{
	if ( _state == State::Searching || _state == State::Paused || _state == State::Finished )
	{
		ScopedLock scopedLock(_mutex);
		_pathWasFound = false;
		_finalPathTimer = sf::Time::Zero;
		_noFinalPathNodes = 0;
		_bigCircleNodeIndex = 0;
		_finalPath.clear();
		CollectFinder();
		_state = State::WaitingForStart;
		for ( auto &[uid, node] : _nodes )
		{
			node.ResetPath();
			node.ClearVisitedNeighbors();
		}
	}
}

void Pathfinder::Reset()
{
	Restart();
	if ( _state == State::Searching || _state == State::Paused || _state == State::Finished )
	{
		ScopedLock scopedLock(_mutex);
		for ( auto &[uid, node] : _nodes )
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

void Pathfinder::SetSleepDelay(sf::Time delay)
{
	_sleepDelay = delay;
	_minorDelay = _sleepDelay.asMicroseconds() < 1000;
}

void Pathfinder::SetWeight(int uidFirst, int uidSecond, float weight)
{
	GetNode(uidFirst).SetNeighborCost(weight, uidSecond);
	GetNode(uidSecond).SetNeighborCost(weight, uidFirst);
}

void Pathfinder::PauseCheck()
{
	while ( _state == State::Paused && _state != State::BeingCollected )
	{
		sf::sleep(sf::seconds(0.01f));
	}
}

void Pathfinder::SleepDelay()
{
	if ( !_minorDelay )
	{
		std::this_thread::sleep_for(std::chrono::microseconds(_sleepDelay.asMicroseconds()));
	}
	else
	{
		_minorDelayTimer += _sleepDelay.asMicroseconds();
		while ( _minorDelayTimer > 1000 )
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			_minorDelayTimer -= 1000;
		}
	}
}

float Pathfinder::GetFinalCost()
{
	ScopedLock scopedLock(_mutex);

	SE_CORE_ASSERT(!_finalPath.empty(), "Cost can only be calculated if path is found");

	float cost = 0.0f;
	for ( int i = 0; i < _finalPath.size() - 1; i++ )
	{
		cost += _finalPath[i]->GetNeighborCost(_finalPath[i + 1]->GetUID());
	}
	return cost;
}

void Pathfinder::RenderFinishedBodyHelper(Scene &scene, sf::Color color, int limit)
{
	// Finished body
	_bodyFinishedVA.clear();
	_bodyFinishedVA.resize(_finalPath.size() * 4);
	const float halfSize = 1.5f;
	auto finalPathCopy = _finalPath;
	for ( int i = 0; i < limit; i++ )
	{
		const sf::Vector2f position = finalPathCopy[i]->GetPosition();

		_bodyFinishedVA[i * 4] = { position - sf::Vector2f{halfSize, 0.0f}, color };
		_bodyFinishedVA[i * 4 + 1] = { position - sf::Vector2f{0.0f, halfSize}, color };
		_bodyFinishedVA[i * 4 + 2] = { position + sf::Vector2f{halfSize, 0.0f}, color };
		_bodyFinishedVA[i * 4 + 3] = { position + sf::Vector2f{0.0f, halfSize}, color };
	}
	scene.Submit(_bodyFinishedVA);
}

void Pathfinder::FindPathThreadFn(int startUID, int goalUID, const Set<int> &subGoalsUIDs)
{
	_finalPath.clear();

	int fromUID = startUID;
	int toUID;

	for ( int subGoalsUID : subGoalsUIDs )
	{
		toUID = subGoalsUID;
		FindPath(fromUID, toUID);
		const bool result = CheckFindPathResult(fromUID, toUID);
		if ( !result )
		{
			_pathWasFound = false;
			_state = State::Finished;
			return;
		}
		fromUID = subGoalsUID;
	};
	toUID = goalUID;
	FindPath(fromUID, toUID);
	_pathWasFound = CheckFindPathResult(fromUID, toUID);

	if ( _pathWasFound )
	{
		_finalPath.push_front(&GetNode(startUID));
	}

	_noFinalPathNodes = 0;
	_finalPathTimer = sf::Time::Zero;

	_state = State::Finished;
}

bool Pathfinder::CheckFindPathResult(int fromUID, int toUID)
{
	if ( _state == State::BeingCollected )
	{
		return false;
	}

	const bool foundPath = GetNodes().at(toUID).WasVisited();
	if ( foundPath )
	{
		AppendFinalPath(fromUID, toUID);
		for ( auto &[uid, node] : _nodes )
		{
			node.ResetPath();
		}
		return true;
	}
	return false;
}

void Pathfinder::AppendFinalPath(int startUID, int goalUID)
{
	ArrayList<const Node *> tmp;
	for ( const Node *node = &GetNode(goalUID); node != &GetNode(startUID); node = &GetNode(node->GetViaUID()) )
	{
		tmp.push_back(node);
	}
	std::reverse_copy(tmp.begin(), tmp.end(), std::back_inserter(_finalPath));
}

void Pathfinder::CollectFinder()
{
	const auto savedState = _state;
	_state = State::BeingCollected;
	if ( _finder.joinable() )
		_finder.join();
	_state = savedState;
}
}