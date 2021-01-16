#pragma once

#include <Saffron.h>

#include "Node.h"

namespace Se
{
class TraverseGrid
{
public:
	typedef Uint32 DrawFlags;
	enum DrawFlag_ : Uint32
	{
		DrawFlag_None = 0u,
		DrawFlag_Grid = 1u << 4u,
		DrawFlag_Objects = 1u << 5u,
		DrawFlag_Weights = 1u << 6u,
		DrawFlag_All = 0xffffffff
	};

public:
	explicit TraverseGrid(String name);
	virtual ~TraverseGrid() = default;

	virtual void OnUpdate();
	virtual	void OnRender(Scene &scene);
	virtual void OnRenderTargetResize(const sf::Vector2f &size);

	void Reset();

	Map<int, Node> &GetNodes() { return _nodes; }
	const Map<int, Node> &GetNodes() const { return _nodes; }

	Node &GetNode(int uid) { return _nodes.at(uid); }
	const Node &GetNode(int uid) const { return _nodes.at(uid); }
	int GetClosestNeighborUID(int uid, const sf::Vector2f &position) const;

	int GetNodeUID(const sf::Vector2f &position) const;
	int GetStartUID() const { return _startUID; }
	int GetGoalUID() const { return _goalUID; }

	const Set<int> &GetObstacleUIDs() { return _obstacleUIDs; }
	const Set<int> &GetSubGoalUIDs() { return _subGoalUIDs; }
	const Set<int> &GetEditedWeightUIDs() { return _editedWeightUIDs; }

	DrawFlags GetDrawFlags() const { return _drawFlags; }
	virtual void SetDrawFlags(DrawFlags drawFlags);
	void AddDrawFlags(DrawFlags drawFlags);
	void RemoveDrawFlags(DrawFlags drawFlags);

	void SetWeight(int uidFirst, int uidSecond, float weight);
	void SetWeightColorAlpha(Uint8 alpha);

	const String &GetName() const { return _name; }

	bool IsObstacle(int uid) const { return _obstacleUIDs.find(uid) != _obstacleUIDs.end(); }
	bool IsStart(int uid) const { return _startUID == uid; }
	bool IsGoal(int uid) const { return _goalUID == uid; }
	bool IsSubGoal(int uid) const;
	bool IsClear(int uid) const;

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

	void AddObstacle(const sf::Vector2f &position);
	void AddObstacle(int uid);
	void RemoveObstacle(const sf::Vector2f &position);
	void RemoveObstacle(int uid);
	void ClearObstacles();

	virtual	void ClearNodeColor(int uid) = 0;
	virtual void SetNodeColor(int uid, const sf::Color &color) = 0;

	static sf::Color GetWeightColor(float weight);
	static constexpr float MaxWeight = 50.0f;

protected:
	virtual void GenerateGrid() = 0;
	virtual void GenerateNodes() = 0;
	virtual void CalculateNeighbors() = 0;

	void SetDefaultStartGoal();


private:
	void OnRenderWeights(Scene &scene);

protected:
	Map<int, Node> _nodes;
	sf::FloatRect _visRect;
	sf::Vector2f _renderTargetSize = { 0.0f, 0.0f };
	DrawFlags _drawFlags = DrawFlag_All;
	sf::Color _gridColor = { 50, 0, 220, 100 };

private:
	String _name;

	Set<int> _obstacleUIDs;
	Set<int> _subGoalUIDs;
	Set<int> _editedWeightUIDs;

	sf::Color _obstacleColor{ 60, 60, 60, 190 };
	sf::Color _startColor{ 0, 255, 255, 180 };
	sf::Color _goalColor{ 255, 255, 0, 180 };

	sf::VertexArray _weightLinesVA;
	bool _wantNewWeightLinesVA = true;
	// Caching VA-index with costs to minimize recreation of VA
	// < <uidFrom, uidTo>, <VA-index1, VA-index2> >
	Map<Pair<int, int>, Pair<int, int>> _weightLinesCacheVA;
	Uint8 _weightLinesColorAlpha = 255;
	bool _wantNewWeightLinesColor = true;

	// Cached
	int _startUID;
	int _goalUID;
};
}