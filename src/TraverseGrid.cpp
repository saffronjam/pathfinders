#include "TraverseGrid.h"

namespace Se
{
TraverseGrid::TraverseGrid(String name) :
	_visRect(-1.0f, -1.0f, 2.0f, 2.0f),
	_name(Move(name)),
	_weightLinesVA(sf::PrimitiveType::Lines),
	_startUID(-1),
	_goalUID(-1)
{
}

void TraverseGrid::OnUpdate()
{
	if ( _wantNewWeightLinesVA )
	{
		_weightLinesCacheVA.clear();
		int noTotalNeighbors = 0;
		for ( const auto &[uid, node] : _nodes )
		{
			noTotalNeighbors += node.GetNeighbors().size();
		}
		_weightLinesVA.resize(noTotalNeighbors * 2);
		int i = 0;
		for ( const auto &[uid, node] : _nodes )
		{
			for ( const auto &neighborUID : node.GetNeighbors() )
			{
				const auto color = GetWeightColor(node.GetNeighborCost(neighborUID));
				_weightLinesVA[i] = { node.GetPosition(), color };
				_weightLinesVA[i + 1] = { GetNode(neighborUID).GetPosition(), color };
				_weightLinesCacheVA.emplace(CreatePair(uid, neighborUID), CreatePair(i, i + 1));
				i += 2;
			}
		}
		_weightLinesColorAlpha = 255;
		_wantNewWeightLinesVA = false;
	}
}

void TraverseGrid::OnRender(Scene &scene)
{
	if ( _drawFlags & DrawFlag_Weights )
	{
		OnRenderWeights(scene);
	}
}

void TraverseGrid::OnRenderTargetResize(const sf::Vector2f &size)
{
	if ( _renderTargetSize != size )
	{
		_visRect = sf::FloatRect(-size / 2.0f, size);
		ClearSubGoals();
		ClearObstacles();
		GenerateGrid();
		GenerateNodes();
		CalculateNeighbors();

		_startUID = -1;
		_goalUID = -1;
		SetDefaultStartGoal();

		_wantNewWeightLinesVA = true;
		_renderTargetSize = size;
	}
}

void TraverseGrid::Reset()
{
	for ( auto &[uid, node] : _nodes )
	{
		node.ResetPath();
		node.ResetNeighborsCost();
		node.ClearVisitedNeighbors();
	}
	_wantNewWeightLinesVA = true;
}

int TraverseGrid::GetClosestNeighborUID(int uid, const sf::Vector2f &position) const
{
	const Node &node = GetNode(uid);
	const auto &neighbors = node.GetNeighbors();
	SE_CORE_ASSERT(!node.GetNeighbors().empty(), "No neighbors in node. Generate neighbors before querying the closest.");

	float closestDistance = std::numeric_limits<float>::infinity();
	int closestUID = -1;

	for ( const int neighborUid : neighbors )
	{
		const Node &neighbor = GetNode(neighborUid);
		const float candidateDistance = VecUtils::LengthSq(position - neighbor.GetPosition());
		if ( candidateDistance < closestDistance )
		{
			closestUID = neighborUid;
			closestDistance = candidateDistance;
		}
	}

	SE_CORE_ASSERT(closestUID != -1, "Somehow failed to find node");
	return closestUID;
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

	SE_CORE_ASSERT(closestUID != -1, "Somehow failed to find node");
	return closestUID;
}

void TraverseGrid::SetDrawFlags(DrawFlags drawFlags)
{
	_drawFlags = drawFlags;
}

void TraverseGrid::AddDrawFlags(DrawFlags drawFlags)
{
	SetDrawFlags(GetDrawFlags() | drawFlags);
}

void TraverseGrid::RemoveDrawFlags(DrawFlags drawFlags)
{
	SetDrawFlags(GetDrawFlags() & ~drawFlags);
}

void TraverseGrid::SetWeight(int uidFirst, int uidSecond, float weight)
{
	GetNode(uidFirst).SetNeighborCost(weight, uidSecond);
	GetNode(uidSecond).SetNeighborCost(weight, uidFirst);

	const auto color = GetWeightColor(weight);

	_weightLinesVA[_weightLinesCacheVA[{uidFirst, uidSecond}].first].color = color;
	_weightLinesVA[_weightLinesCacheVA[{uidFirst, uidSecond}].second].color = color;

	_editedWeightUIDs.emplace(uidFirst);
	_editedWeightUIDs.emplace(uidSecond);
}

void TraverseGrid::SetWeightColorAlpha(Uint8 alpha)
{
	if ( alpha != _weightLinesColorAlpha )
	{
		_weightLinesColorAlpha = alpha;
		_wantNewWeightLinesColor = true;
	}
}

bool TraverseGrid::IsSubGoal(int uid) const
{
	return std::find(_subGoalUIDs.begin(), _subGoalUIDs.end(), uid) != _subGoalUIDs.end();
}

bool TraverseGrid::IsClear(int uid) const
{
	return
		uid != -1 &&
		!IsObstacle(uid) &&
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
		if ( _startUID != -1 )
		{
			ClearNodeColor(_startUID);
		}
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
		if ( _goalUID != -1 )
		{
			ClearNodeColor(_goalUID);
		}
		_goalUID = uid;
		SetNodeColor(uid, _goalColor);
	}
}

void TraverseGrid::ResetStartGoal()
{
	if ( _startUID != -1 )
	{
		ClearNodeColor(_startUID);
	}
	if ( _goalUID != -1 )
	{
		ClearNodeColor(_goalUID);
	}

	_startUID = -1;
	_goalUID = -1;

	SetDefaultStartGoal();
}

void TraverseGrid::AddSubGoal(const sf::Vector2f &position)
{
	AddSubGoal(GetNodeUID(position));
}

void TraverseGrid::AddSubGoal(int uid)
{
	if ( IsClear(uid) )
	{
		_subGoalUIDs.emplace(uid);
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
	for ( const auto &uid : _obstacleUIDs )
	{
		ClearNodeColor(uid);
	}
	_obstacleUIDs.clear();
}

sf::Color TraverseGrid::GetWeightColor(float weight)
{
	const sf::Uint8 red = weight / MaxWeight * 255.0f;
	const sf::Uint8 blue = 255.0f - weight / MaxWeight * 255.0f;

	return { red, 0, blue, 170 };
}

void TraverseGrid::SetDefaultStartGoal()
{
	const auto visRectTopLeft = _visRect.getPosition();
	const auto visRectSize = _visRect.getSize();

	SetStart(visRectTopLeft + sf::Vector2f{ 2.0f * visRectSize.x / 9.0f, visRectSize.y / 2.0f });
	SetGoal(visRectTopLeft + sf::Vector2f{ 7.0f * visRectSize.x / 9.0f, visRectSize.y / 2.0f });
}

void TraverseGrid::OnRenderWeights(Scene &scene)
{
	if ( _wantNewWeightLinesColor )
	{
		for ( int i = 0; i < _weightLinesVA.getVertexCount(); i++ )
		{
			_weightLinesVA[i].color.a = _weightLinesColorAlpha;
		}
	}

	scene.Submit(_weightLinesVA);
}
}
