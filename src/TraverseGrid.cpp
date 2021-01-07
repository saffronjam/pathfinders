#include "TraverseGrid.h"

namespace Se
{
TraverseGrid::TraverseGrid(String name) :
	_visRect(-1.0f, -1.0f, 2.0f, 2.0f),
	_name(Move(name)),
	_obstacleColor(60, 60, 60),
	_startColor(sf::Color::Cyan),
	_goalColor(sf::Color::Yellow),
	_startUID(-1),
	_goalUID(-1)
{
}

void TraverseGrid::OnRenderTargetResize(const sf::Vector2f &size)
{
	_visRect = sf::FloatRect(-size / 2.0f, size);
	GenerateNodes();
	GenerateGrid();
}

int TraverseGrid::GetNodeUID(const sf::Vector2f &position) const
{
	float closestDistance = std::numeric_limits<float>::infinity();
	int closestUID = -1;

	for ( const auto &[uid, node] : GetNodes() )
	{
		const float candidate = VecUtils::LengthSq(node.GetPosition() - position);
		if ( candidate < closestDistance )
		{
			closestUID = uid;
			closestDistance = candidate;
		}
	}

	SE_CORE_ASSERT(closestDistance != -1, "Somehow failed to find node");
	return closestUID;
}

bool TraverseGrid::IsSubGoal(int uid) const
{
	return std::find(_subGoalUIDs.begin(), _subGoalUIDs.end(), uid) != _subGoalUIDs.end();
}

bool TraverseGrid::IsClear(int uid) const
{
	return !IsObstacle(uid) &&
		!IsStart(uid) &&
		!IsGoal(uid) &&
		!IsSubGoal(uid);
}

void TraverseGrid::SetStart(const sf::Vector2f &position)
{
	SetStart(GetNodeUID(position));
}

void TraverseGrid::SetStart(int uid)
{
	if ( IsClear(uid) )
	{
		ClearNodeColor(_startUID);
		_startUID = uid;
		SetNodeColor(uid, _startColor);
	}
}

void TraverseGrid::SetGoal(const sf::Vector2f &position)
{
	SetGoal(GetNodeUID(position));
}

void TraverseGrid::SetGoal(int uid)
{
	if ( IsClear(uid) )
	{
		ClearNodeColor(_goalUID);
		_goalUID = uid;
		SetNodeColor(uid, _goalColor);
	}
}

void TraverseGrid::ResetStartGoal()
{
	SetStart(_visRect.getPosition());
	SetGoal(_visRect.getPosition() + _visRect.getSize());
}

void TraverseGrid::AddSubGoal(const sf::Vector2f &position)
{
	AddSubGoal(GetNodeUID(position));
}

void TraverseGrid::AddSubGoal(int uid)
{
	if ( IsClear(uid) )
	{
		_subGoalUIDs.push_back(uid);
		SetNodeColor(uid, sf::Color::Blue);
	}
}

void TraverseGrid::RemoveSubGoal(const sf::Vector2f &position)
{
	RemoveSubGoal(GetNodeUID(position));
}

void TraverseGrid::RemoveSubGoal(int uid)
{
	if ( IsSubGoal(uid) )
	{
		_subGoalUIDs.erase(std::find(_subGoalUIDs.begin(), _subGoalUIDs.end(), uid));
		ClearNodeColor(uid);
	}
}

void TraverseGrid::ClearSubGoals()
{
	for ( auto &uid : _subGoalUIDs )
	{
		ClearNodeColor(uid);
	}
	_subGoalUIDs.clear();
}

void TraverseGrid::AddObstacle(const sf::Vector2f &position)
{
	AddObstacle(GetNodeUID(position));
}

void TraverseGrid::AddObstacle(int uid)
{
	if ( IsClear(uid) )
	{
		_obstacleUIDs.emplace(uid);
		SetNodeColor(uid, _obstacleColor);
	}
}

void TraverseGrid::RemoveObstacle(const sf::Vector2f &position)
{
	RemoveObstacle(GetNodeUID(position));
}

void TraverseGrid::RemoveObstacle(int uid)
{
	if ( IsObstacle(uid) )
	{
		_obstacleUIDs.erase(uid);
		ClearNodeColor(uid);
	}
}

void TraverseGrid::ClearObstacles()
{
	for ( auto &uid : _obstacleUIDs )
	{
		ClearNodeColor(uid);
	}
	_obstacleUIDs.clear();
}
}
