#include "Node.h"

namespace Se
{
Node::Node(int uid, const sf::Vector2f &position)
	: _uid(uid),
	_position(position),
	_viaUID(-1)
{
}

void Node::AddNeighbor(int uid, float cost)
{
	_neighbors.emplace(uid);
	_neighborCosts.emplace(CreatePair(uid, cost));
	_resetNeighborCosts.emplace(CreatePair(uid, cost));
}

void Node::ResetCosts()
{
	_costs.clear();
}

void Node::ResetNeighborsCost()
{
	_neighborCosts = _resetNeighborCosts;
}

void Node::ResetPath()
{
	ResetCosts();
	SetVia(-1);
}

float Node::GetCost(const String &type)
{
	if ( !HasCost(type) )
	{
		_costs.emplace(CreatePair(type, std::numeric_limits<float>::infinity()));
	}
	return _costs.at(type);
}

bool Node::HasCost(const String &type)
{
	return _costs.find(type) != _costs.end();
}

bool Node::WasVisitedBy(Node &node) const
{
	return node._visitedNeighbors.find(_uid) != node._visitedNeighbors.end();
}

void Node::SetCost(const String &type, float cost)
{
	if ( !HasCost(type) )
	{
		_costs.emplace(CreatePair(type, cost));
	}
	else
	{
		_costs.at(type) = cost;
	}
}

void Node::SetNeighborCost(float cost, int uid)
{
	_neighborCosts.at(uid) = cost;
}
}
