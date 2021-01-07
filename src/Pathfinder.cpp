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
	_minorDelayTimer(0)
{
}

Pathfinder::~Pathfinder()
{
	CollectFinder();
}

void Pathfinder::OnRenderAnticipation(Scene &scene)
{
	if ( _activeNodeUID != -1 && _state == State::Finding || _state == State::Paused || _state == State::Finished )
	{
		for ( Node *node = &GetNode(_activeNodeUID); node->GetViaUID() != -1; node = &GetNode(node->GetViaUID()) )
		{
			scene.Submit(node->GetPosition(), GetNode(node->GetViaUID()).GetPosition(), sf::Color::Red);
		}
		for ( const auto &node : _finalPath )
		{
			scene.Submit(node->GetPosition(), sf::Color::Magenta, 3.0f);
		}
		scene.Submit(GetNodes().at(_activeNodeUID).GetPosition(), sf::Color::Red, 5.0f);
	}
}

void Pathfinder::OnRenderViaConnections(Scene &scene)
{
	for ( auto &[uid, node] : _nodes )
	{
		if ( node.GetViaUID() != -1 )
			scene.Submit(node.GetPosition(), GetNode(node.GetViaUID()).GetPosition(), sf::Color(150, 150, 150, 20));
	}
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
		if ( _noFinalPathNodes < _finalPath.size() - 1 )
		{
			_finalPathTimer += Global::Clock::GetFrameTime();
			if ( _finalPathTimer.asSeconds() > 0.05f )
			{
				_noFinalPathNodes++;
				_finalPathTimer = sf::Time::Zero;
			}
		}
		for ( size_t i = 0; i < _noFinalPathNodes; i++ )
		{
			scene.Submit(_finalPath[i]->GetPosition(), sf::Color(0, 150, 0), 3.0f);
		}
		scene.Submit(GetNode(_traverseGrid->GetStartUID()).GetPosition(), sf::Color(0, 150, 0), 5.0f);
		scene.Submit(_finalPath[_noFinalPathNodes]->GetPosition(), sf::Color::Green, 5.0f);
	}
	else
	{
		scene.Submit(GetNode(_traverseGrid->GetStartUID()).GetPosition(), sf::Color::Red, 10.0f);
		scene.Submit(GetNode(_traverseGrid->GetGoalUID()).GetPosition(), sf::Color::Red, 10.0f);
	}
}

void Pathfinder::AssignNodes(const Map<int, Node> &nodes)
{
	_nodes = nodes;
}

void Pathfinder::Start(int startUID, int goalUID, const ArrayList<int> &subGoalsUIDs)
{
	if ( _state == State::Finished )
	{
		Restart();
	}
	if ( _state == State::WaitingForStart )
	{
		CollectFinder();
		_state = State::Finding;
		_finder = std::thread(&Pathfinder::FindPathThreadFn, this, startUID, goalUID, subGoalsUIDs);
	}
}

void Pathfinder::Pause()
{
	if ( _state == State::Finding )
		_state = State::Paused;
}

void Pathfinder::Resume()
{
	if ( _state == State::Paused )
		_state = State::Finding;
}

void Pathfinder::Restart()
{
	if ( _state == State::Finding || _state == State::Paused || _state == State::Finished )
	{
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
	if ( _state != State::WaitingForStart )
	{
		CollectFinder();
		_state = State::WaitingForStart;
		for ( auto &[uid, node] : _nodes )
		{
			node.ResetPath();
			node.ClearVisitedNeighbors();
		}
	}
}

void Pathfinder::SetSleepDelay(sf::Time delay)
{
	_sleepDelay = delay;
	_minorDelay = (_sleepDelay.asMicroseconds() < 1000);
}

void Pathfinder::PauseCheck()
{
	while ( _state == State::Paused && _state != State::BeingCollected )
		sf::sleep(sf::seconds(0.01f));
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

void Pathfinder::FindPathThreadFn(int startUID, int goalUID, const ArrayList<int> &subGoalsUIDs)
{
	_finalPath.clear();

	int fromUID = startUID;
	int toUID;
	for ( int i = 0; i < subGoalsUIDs.size(); i++ )
	{
		toUID = subGoalsUIDs[i];
		FindPath(fromUID, toUID);
		bool result = CheckFindPathResult(fromUID, toUID);
		if ( !result )
		{
			_pathWasFound = false;
			_state = State::Finished;
			return;
		}
		fromUID = subGoalsUIDs[i];
	};
	toUID = goalUID;
	FindPath(fromUID, toUID);
	_pathWasFound = CheckFindPathResult(fromUID, toUID);
	_state = State::Finished;

	_noFinalPathNodes = 0;
	_finalPathTimer = sf::Time::Zero;
}

bool Pathfinder::CheckFindPathResult(int fromUID, int toUID)
{
	if ( _state == State::BeingCollected )
		return false;

	bool foundPath = (GetNodes().at(toUID).WasVisited());
	if ( foundPath )
	{
		AppendFinalPath(fromUID, toUID);
		for ( auto &[uid, node] : _nodes )
		{
			node.ResetPath();
		}
		return true;
	}
	else
	{
		_pathWasFound = false;
		return false;
	}
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
	auto savedState = _state;
	_state = State::BeingCollected;
	if ( _finder.joinable() )
		_finder.join();
	_state = savedState;
}
}