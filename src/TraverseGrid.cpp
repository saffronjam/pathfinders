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

void TraverseGrid::GenerateMaze()
{
	ClearObstacles();
	_visitedNodes.clear();

	for ( auto &[uid, node] : _nodes )
	{
		for ( const auto &neighborUid : node.GetNeighbors() )
		{
			AddObstacle(uid, neighborUid);
		}
	}

	Stack<int> checkStack;
	checkStack.push(_nodes.begin()->first);
	_visitedNodes.emplace(_nodes.begin()->first);

	auto getRandomNeighbor = [&](int uid)
	{
		int neighborFound = -1;

		const  auto &neighbors = GetNode(uid).GetNeighbors();

		ArrayList<int> shuffledNeighbors(neighbors.begin(), neighbors.end());
		std::shuffle(shuffledNeighbors.begin(), shuffledNeighbors.end(), _randomEngine);

		for ( int neighbor : shuffledNeighbors )
		{
			auto result = _visitedNodes.find(neighbor);
			if ( result == _visitedNodes.end() )
			{
				neighborFound = neighbor;
				break;
			}
		}

		return neighborFound;
	};

	sf::Time currentSleep = sf::Time::Zero;
	Clock loopClock;

	int active = GetStartUID();
	while ( !checkStack.empty() )
	{
		const auto elapsed = loopClock.Restart();
		currentSleep += elapsed;
		if ( currentSleep > sf::microseconds(400) )
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			currentSleep = sf::Time::Zero;
			loopClock.Restart();
		}

		const int neighbor = getRandomNeighbor(active);
		if ( neighbor == -1 )
		{
			active = checkStack.top();
			checkStack.pop();
		}
		else
		{
			_visitedNodes.emplace(neighbor);
			_visitedNodes.emplace(active);
			checkStack.push(active);

			RemoveObstacle(active, neighbor);

			active = neighbor;
		}
	}


	for ( int i = 0; i < _noToSmash; i++ )
	{
		const int randomIndex = Random::Integer(0, static_cast<int>(_nodes.size()) - 1);
		for ( int neighbor : GetNode(randomIndex).GetNeighbors() )
		{
			RemoveObstacle(randomIndex, neighbor);
		}
	}

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
		!IsStart(uid) &&
		!IsGoal(uid) &&
		!IsSubGoal(uid);
}

bool TraverseGrid::IsEdgeObstacle(int fromUid, int toUid) const
{
	return _obstacleUIDs.find({ fromUid, toUid }) != _obstacleUIDs.end();
}

bool TraverseGrid::IsEdgeClear(int fromUid, int toUid) const
{
	return !IsEdgeObstacle(fromUid, toUid);
}

bool TraverseGrid::HasFilledEdges(int uid)
{
	auto &node = GetNode(uid);
	bool filled = true;
	for ( int neighborUid : node.GetNeighbors() )
	{
		if ( IsEdgeClear(uid, neighborUid) )
		{
			filled = false;
			break;
		}
	}
	return filled;
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

void TraverseGrid::AddObstacle(int fromUid, int toUid)
{
	if ( IsEdgeClear(fromUid, toUid) )
	{
		_obstacleUIDs.emplace(fromUid, toUid);
		_obstacleUIDs.emplace(toUid, fromUid);
		OnObstacleChange(fromUid, toUid);
		SetNodeEdgeColor(fromUid, toUid, _obstacleColor);
	}
}

void TraverseGrid::RemoveObstacle(int fromUid, int toUid)
{
	_obstacleUIDs.erase({ fromUid, toUid });
	_obstacleUIDs.erase({ toUid, fromUid });
	OnObstacleChange(fromUid, toUid);
	ClearNodeEdgeColor(fromUid, toUid);
	ClearNodeEdgeColor(toUid, fromUid);
}

void TraverseGrid::ClearObstacles()
{
	for ( const auto &[fromUid, toUid] : _obstacleUIDs )
	{
		ClearNodeEdgeColor(fromUid, toUid);
	}

	for ( auto &[uid, node] : _nodes )
	{
		if ( IsClear(uid) )
		{
			ClearNodeColor(uid);
		}
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

void TraverseGrid::OnObstacleChange(int fromUid, int toUid)
{
	if ( IsClear(fromUid) )
	{
		if ( HasFilledEdges(fromUid) )
		{
			SetNodeColor(fromUid, _fadedObstacleColor);
		}
		else
		{
			ClearNodeColor(fromUid);
		}
	}

	if ( IsClear(toUid) )
	{
		if ( HasFilledEdges(toUid) )
		{
			SetNodeColor(toUid, _fadedObstacleColor);
		}
		else
		{
			ClearNodeColor(toUid);
		}
	}
}
}
