#pragma once

#include <Saffron.h>

#include "Node.h"

namespace Se
{

typedef uint TraverseGridDrawFlags;
enum TraverseGridDrawFlag_ : uint
{
	TraverseGridDrawFlag_None = 0u,
	TraverseGridDrawFlag_Grid = 1u << 4u,
	TraverseGridDrawFlag_Objects = 1u << 5u,
	TraverseGridDrawFlag_Weights = 1u << 6u,
	TraverseGridDrawFlag_All = 0xffffffff
};

class TraverseGrid
{
public:

public:
	explicit TraverseGrid(std::string name);
	virtual ~TraverseGrid() = default;

	virtual void OnUpdate();
	virtual	void OnRender(Scene &scene);
	virtual void OnRenderTargetResize(const sf::Vector2f &size);
	virtual void OnExit();
	
	void Reset();

	void GenerateMaze();
	void SetNoWallsToSmash(int no);

	auto Nodes() -> std::map<int, Node>&;
	auto Nodes() const -> const std::map<int, Node>&;

	auto NodeByUid(int uid) -> Node&;
	auto NodeByUid(int uid) const -> const Node&;

	auto ClosestNeighborUID(int uid, const sf::Vector2f &position) const -> int;
	auto NodeUidByPosition(const sf::Vector2f &position) const -> int;

	auto StartUid() const -> int;
	auto GoalUid() const -> int;

	auto ObstacleUids() -> const std::set<std::pair<int, int>>&;
	auto SubGoalUids() -> const std::vector<int>&;
	auto EditedWeightUids() -> const std::set<int>&;

	auto DrawFlags() const -> TraverseGridDrawFlags;
	virtual void SetDrawFlags(TraverseGridDrawFlags drawFlags);
	void AddDrawFlags(TraverseGridDrawFlags drawFlags);
	void RemoveDrawFlags(TraverseGridDrawFlags drawFlags);

	void SetWeight(int uidFirst, int uidSecond, float weight);
	void SetWeightColorAlpha(uchar alpha);

	auto Name() const -> const std::string&;
	
	auto IsStart(int uid) const -> bool;
	auto IsGoal(int uid) const -> bool;
	auto IsSubGoal(int uid) const -> bool;
	auto IsClear(int uid) const -> bool;

	auto IsEdgeObstacle(int fromUid, int toUid) const -> bool;
	auto IsEdgeClear(int fromUid, int toUid) const -> bool;
	auto HasFilledEdges(int uid) -> bool;

	void SetStart(const sf::Vector2f &position);
	void SetStart(int uid);
	void SetGoal(const sf::Vector2f &position);
	void SetGoal(int uid);
	void ResetStartGoal();

	void AddSubGoal(const sf::Vector2f &position);
	void AddSubGoal(int uid);
	void RemoveSubGoal(const sf::Vector2f &position);
	void RemoveSubGoal(int uid);
	void ClearSubGoals();

	void AddObstacle(int fromUid, int toUid);
	void RemoveObstacle(int fromUid, int toUid);
	void ClearObstacles();

	virtual	void ClearNodeColor(int uid) = 0;
	virtual void SetNodeColor(int uid, const sf::Color &color) = 0;

	virtual	void ClearNodeEdgeColor(int fromUid, int toUid) = 0;
	virtual void SetNodeEdgeColor(int fromUid, int toUid, const sf::Color &color) = 0;

	static auto WeightColor(float weight) -> sf::Color;
	auto GridColor() const -> sf::Color { return _gridColor; }

	static constexpr float MaxWeight = 50.0f;

protected:
	virtual void GenerateGrid() = 0;
	virtual void GenerateNodes() = 0;
	virtual void CalculateNeighbors() = 0;

	void SetDefaultStartGoal();

private:
	void OnRenderWeights(Scene &scene);
	void OnObstacleChange(int fromUid, int toUid);

protected:
	std::map<int, Node> _nodes;
	sf::FloatRect _visRect;
	sf::Vector2f _renderTargetSize = { 0.0f, 0.0f };
	TraverseGridDrawFlags _drawFlags = TraverseGridDrawFlag_All;
	sf::Color _gridColor = { 50, 0, 220, 100 };

private:
	std::string _name;

	std::set<std::pair<int, int>> _obstacleUIDs;
	std::vector<int> _subGoalUIDs;
	std::set<int> _editedWeightUIDs;

	sf::Color _obstacleColor{ 40, 40, 40 };
	sf::Color _fadedObstacleColor{ _obstacleColor.r, _obstacleColor.g, _obstacleColor.b, _obstacleColor.a * 0.6f };
	sf::Color _startColor{ 0, 255, 255, 180 };
	sf::Color _goalColor{ 255, 255, 0, 180 };

	sf::VertexArray _weightLinesVA;
	bool _wantNewWeightLinesVA = true;
	// Caching VA-index with costs to minimize recreation of VA
	// < <uidFrom, uidTo>, <VA-index1, VA-index2> >
	std::map<std::pair<int, int>, std::pair<int, int>> _weightLinesCacheVA;
	uchar _weightLinesColorAlpha = 255;
	bool _wantNewWeightLinesColor = true;

	int _startUID;
	int _goalUID;

	// Maze Generation
	std::set<int> _visitedNodes;
	int _noToSmash = 0;
	Random::Device _randomDevice;
	Random::Engine _randomEngine{ _randomDevice() };

	std::atomic<bool> _shouldExit = false;
};
}