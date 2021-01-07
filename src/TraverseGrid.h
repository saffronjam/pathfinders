#pragma once

#include <Saffron.h>

#include "Node.h"

namespace Se
{
class TraverseGrid
{
public:
	explicit TraverseGrid(String name);
	virtual ~TraverseGrid() = default;

	virtual	void OnRender(Scene &scene) = 0;
	virtual void OnRenderTargetResize(const sf::Vector2f &size);

	Map<int, Node> &GetNodes() { return _nodes; }
	Node &GetNode(int uid) { return _nodes.at(uid); }
	const Node &GetNode(int uid) const { return _nodes.at(uid); }

	int GetNodeUID(const sf::Vector2f &position) const;
	int GetStartUID() const { return _startUID; }
	int GetGoalUID() const { return _goalUID; }
	ArrayList<int> &GetSubGoalUIDs() { return _subGoalUIDs; }

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

protected:
	virtual void GenerateNodes() = 0;
	virtual void GenerateGrid() = 0;
	virtual void CalculateNeighbors() = 0;

private:
	virtual	void ClearNodeColor(int uid) = 0;
	virtual void SetNodeColor(int uid, const sf::Color &color) = 0;

	const Map<int, Node> &GetNodes() const { return _nodes; }

protected:
	Map<int, Node> _nodes;
	sf::FloatRect _visRect;

private:
	String _name;

	Set<int> _obstacleUIDs;

	sf::Color _obstacleColor;
	sf::Color _startColor;
	sf::Color _goalColor;

	// Cached
	int _startUID;
	int _goalUID;
	ArrayList<int> _subGoalUIDs;
};
}