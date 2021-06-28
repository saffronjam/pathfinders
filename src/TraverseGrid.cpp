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
	if (_wantNewWeightLinesVA)
	{
		_weightLinesCacheVA.clear();
		int noTotalNeighbors = 0;
		for (const auto& node : _nodes | std::views::values)
		{
			noTotalNeighbors += node.Neighbors().size();
		}
		_weightLinesVA.resize(noTotalNeighbors * 2);
		int i = 0;
		for (const auto& [uid, node] : _nodes)
		{
			for (const auto& neighborUID : node.Neighbors())
			{
				const auto color = WeightColor(node.NeighborCostByUid(neighborUID));
				_weightLinesVA[i] = {node.Position(), color};
				_weightLinesVA[i + 1] = {NodeByUid(neighborUID).Position(), color};
				_weightLinesCacheVA.emplace(CreatePair(uid, neighborUID), CreatePair(i, i + 1));
				i += 2;
			}
		}
		_weightLinesColorAlpha = 255;
		_wantNewWeightLinesVA = false;
	}
}

void TraverseGrid::OnRender(Scene& scene)
{
	if (_drawFlags & TraverseGridDrawFlag_Weights)
	{
		OnRenderWeights(scene);
	}
}

void TraverseGrid::OnRenderTargetResize(const sf::Vector2f& size)
{
	if (_renderTargetSize != size)
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
	for (auto& node : _nodes | std::views::values)
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

	for (auto& [uid, node] : _nodes)
	{
		for (const auto& neighborUid : node.Neighbors())
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

		const auto& neighbors = NodeByUid(uid).Neighbors();

		List<int> shuffledNeighbors(neighbors.begin(), neighbors.end());
		std::ranges::shuffle(shuffledNeighbors, _randomEngine);

		for (int neighbor : shuffledNeighbors)
		{
			auto result = _visitedNodes.find(neighbor);
			if (result == _visitedNodes.end())
			{
				neighborFound = neighbor;
				break;
			}
		}

		return neighborFound;
	};

	sf::Time currentSleep = sf::Time::Zero;
	Clock loopClock;

	int active = StartUid();
	while (!checkStack.empty())
	{
		const auto elapsed = loopClock.Restart();
		currentSleep += elapsed;
		if (currentSleep > sf::microseconds(400))
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			currentSleep = sf::Time::Zero;
			loopClock.Restart();
		}

		const int neighbor = getRandomNeighbor(active);
		if (neighbor == -1)
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


	for (int i = 0; i < _noToSmash; i++)
	{
		const int randomIndex = Random::Integer(0, static_cast<int>(_nodes.size()) - 1);
		for (int neighbor : NodeByUid(randomIndex).Neighbors())
		{
			RemoveObstacle(randomIndex, neighbor);
		}
	}
}

void TraverseGrid::SetNoWallsToSmash(int no)
{
	_noToSmash = no;
}

auto TraverseGrid::Nodes() -> TreeMap<int, Node>& { return _nodes; }

auto TraverseGrid::Nodes() const -> const TreeMap<int, Node>& { return _nodes; }

auto TraverseGrid::NodeByUid(int uid) -> Node& { return _nodes.at(uid); }

auto TraverseGrid::NodeByUid(int uid) const -> const Node& { return _nodes.at(uid); }

auto TraverseGrid::StartUid() const -> int { return _startUID; }

auto TraverseGrid::GoalUid() const -> int { return _goalUID; }

auto TraverseGrid::ObstacleUids() -> const TreeSet<Pair<int, int>>& { return _obstacleUIDs; }

auto TraverseGrid::SubGoalUids() -> const List<int>& { return _subGoalUIDs; }

auto TraverseGrid::EditedWeightUids() -> const TreeSet<int>& { return _editedWeightUIDs; }

auto TraverseGrid::DrawFlags() const -> TraverseGridDrawFlags { return _drawFlags; }

auto TraverseGrid::Name() const -> const String& { return _name; }

auto TraverseGrid::IsStart(int uid) const -> bool { return _startUID == uid; }

auto TraverseGrid::IsGoal(int uid) const -> bool { return _goalUID == uid; }

int TraverseGrid::ClosestNeighborUID(int uid, const sf::Vector2f& position) const
{
	const Node& node = NodeByUid(uid);
	const auto& neighbors = node.Neighbors();
	Debug::Assert(!node.Neighbors().empty(), "No neighbors in node. Generate neighbors before querying the closest.");

	float closestDistance = std::numeric_limits<float>::infinity();
	int closestUID = -1;

	for (const int neighborUid : neighbors)
	{
		const Node& neighbor = NodeByUid(neighborUid);
		const float candidateDistance = VecUtils::LengthSq(position - neighbor.Position());
		if (candidateDistance < closestDistance)
		{
			closestUID = neighborUid;
			closestDistance = candidateDistance;
		}
	}

	Debug::Assert(closestUID != -1, "Somehow failed to find node");
	return closestUID;
}

int TraverseGrid::NodeUidByPosition(const sf::Vector2f& position) const
{
	float closestDistance = std::numeric_limits<float>::infinity();
	int closestUID = -1;

	for (const auto& [uid, node] : Nodes())
	{
		const float candidate = VecUtils::LengthSq(node.Position() - position);
		if (candidate < closestDistance)
		{
			closestUID = uid;
			closestDistance = candidate;
		}
	}

	Debug::Assert(closestUID != -1, "Somehow failed to find node");
	return closestUID;
}

void TraverseGrid::SetDrawFlags(TraverseGridDrawFlags drawFlags)
{
	_drawFlags = drawFlags;
}

void TraverseGrid::AddDrawFlags(TraverseGridDrawFlags drawFlags)
{
	SetDrawFlags(DrawFlags() | drawFlags);
}

void TraverseGrid::RemoveDrawFlags(TraverseGridDrawFlags drawFlags)
{
	SetDrawFlags(DrawFlags() & ~drawFlags);
}

void TraverseGrid::SetWeight(int uidFirst, int uidSecond, float weight)
{
	NodeByUid(uidFirst).SetNeighborCost(weight, uidSecond);
	NodeByUid(uidSecond).SetNeighborCost(weight, uidFirst);

	const auto color = WeightColor(weight);

	_weightLinesVA[_weightLinesCacheVA[{uidFirst, uidSecond}].first].color = color;
	_weightLinesVA[_weightLinesCacheVA[{uidFirst, uidSecond}].second].color = color;

	_editedWeightUIDs.emplace(uidFirst);
	_editedWeightUIDs.emplace(uidSecond);
}

void TraverseGrid::SetWeightColorAlpha(uchar alpha)
{
	if (alpha != _weightLinesColorAlpha)
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
	return uid != -1 && !IsStart(uid) && !IsGoal(uid) && !IsSubGoal(uid);
}

bool TraverseGrid::IsEdgeObstacle(int fromUid, int toUid) const
{
	return _obstacleUIDs.find({fromUid, toUid}) != _obstacleUIDs.end();
}

bool TraverseGrid::IsEdgeClear(int fromUid, int toUid) const
{
	return !IsEdgeObstacle(fromUid, toUid);
}

bool TraverseGrid::HasFilledEdges(int uid)
{
	auto& node = NodeByUid(uid);
	bool filled = true;
	for (int neighborUid : node.Neighbors())
	{
		if (IsEdgeClear(uid, neighborUid))
		{
			filled = false;
			break;
		}
	}
	return filled;
}

void TraverseGrid::SetStart(const sf::Vector2f& position)
{
	SetStart(NodeUidByPosition(position));
}

void TraverseGrid::SetStart(int uid)
{
	if (IsClear(uid))
	{
		if (_startUID != -1)
		{
			ClearNodeColor(_startUID);
		}
		_startUID = uid;
		SetNodeColor(uid, _startColor);
	}
}

void TraverseGrid::SetGoal(const sf::Vector2f& position)
{
	SetGoal(NodeUidByPosition(position));
}

void TraverseGrid::SetGoal(int uid)
{
	if (IsClear(uid))
	{
		if (_goalUID != -1)
		{
			ClearNodeColor(_goalUID);
		}
		_goalUID = uid;
		SetNodeColor(uid, _goalColor);
	}
}

void TraverseGrid::ResetStartGoal()
{
	if (_startUID != -1)
	{
		ClearNodeColor(_startUID);
	}
	if (_goalUID != -1)
	{
		ClearNodeColor(_goalUID);
	}

	_startUID = -1;
	_goalUID = -1;

	SetDefaultStartGoal();
}

void TraverseGrid::AddSubGoal(const sf::Vector2f& position)
{
	AddSubGoal(NodeUidByPosition(position));
}

void TraverseGrid::AddSubGoal(int uid)
{
	if (IsClear(uid))
	{
		_subGoalUIDs.push_back(uid);
		SetNodeColor(uid, sf::Color::Blue);
	}
}

void TraverseGrid::RemoveSubGoal(const sf::Vector2f& position)
{
	RemoveSubGoal(NodeUidByPosition(position));
}

void TraverseGrid::RemoveSubGoal(int uid)
{
	if (IsSubGoal(uid))
	{
		_subGoalUIDs.erase(std::find(_subGoalUIDs.begin(), _subGoalUIDs.end(), uid));
		ClearNodeColor(uid);
	}
}

void TraverseGrid::ClearSubGoals()
{
	for (auto& uid : _subGoalUIDs)
	{
		ClearNodeColor(uid);
	}
	_subGoalUIDs.clear();
}

void TraverseGrid::AddObstacle(int fromUid, int toUid)
{
	if (IsEdgeClear(fromUid, toUid))
	{
		_obstacleUIDs.emplace(fromUid, toUid);
		_obstacleUIDs.emplace(toUid, fromUid);
		OnObstacleChange(fromUid, toUid);
		SetNodeEdgeColor(fromUid, toUid, _obstacleColor);
	}
}

void TraverseGrid::RemoveObstacle(int fromUid, int toUid)
{
	_obstacleUIDs.erase({fromUid, toUid});
	_obstacleUIDs.erase({toUid, fromUid});
	OnObstacleChange(fromUid, toUid);
	ClearNodeEdgeColor(fromUid, toUid);
	ClearNodeEdgeColor(toUid, fromUid);
}

void TraverseGrid::ClearObstacles()
{
	for (const auto& [fromUid, toUid] : _obstacleUIDs)
	{
		ClearNodeEdgeColor(fromUid, toUid);
	}

	for (auto& [uid, node] : _nodes)
	{
		if (IsClear(uid))
		{
			ClearNodeColor(uid);
		}
	}

	_obstacleUIDs.clear();
}

sf::Color TraverseGrid::WeightColor(float weight)
{
	const sf::Uint8 red = weight / MaxWeight * 255.0f;
	const sf::Uint8 blue = 255.0f - weight / MaxWeight * 255.0f;

	return {red, 0, blue, 170};
}

void TraverseGrid::SetDefaultStartGoal()
{
	const auto visRectTopLeft = _visRect.getPosition();
	const auto visRectSize = _visRect.getSize();

	SetStart(visRectTopLeft + sf::Vector2f{2.0f * visRectSize.x / 9.0f, visRectSize.y / 2.0f});
	SetGoal(visRectTopLeft + sf::Vector2f{7.0f * visRectSize.x / 9.0f, visRectSize.y / 2.0f});
}

void TraverseGrid::OnRenderWeights(Scene& scene)
{
	if (_wantNewWeightLinesColor)
	{
		for (int i = 0; i < _weightLinesVA.getVertexCount(); i++)
		{
			_weightLinesVA[i].color.a = _weightLinesColorAlpha;
		}
	}

	scene.Submit(_weightLinesVA);
}

void TraverseGrid::OnObstacleChange(int fromUid, int toUid)
{
	if (IsClear(fromUid))
	{
		if (HasFilledEdges(fromUid))
		{
			SetNodeColor(fromUid, _fadedObstacleColor);
		}
		else
		{
			ClearNodeColor(fromUid);
		}
	}

	if (IsClear(toUid))
	{
		if (HasFilledEdges(toUid))
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
