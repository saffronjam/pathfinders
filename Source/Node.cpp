#include "Node.h"

namespace Se
{
Node::Node(int uid, const sf::Vector2f &position)
	: _uid(uid),
	_position(position),
	_viaUID(-1)
{
}

bool Node::operator==(int uid) const
{
	return _uid == uid;
}

void Node::RemoveNeighbor(int uid)
{
	_neighbors.erase(uid);
}

int Node::Uid() const
{
	return _uid;
}

const sf::Vector2f& Node::Position() const
{
	return _position;
}

int Node::ViaUID() const
{
	return _viaUID;
}

const std::unordered_set<int>& Node::Neighbors() const
{
	return _neighbors;
}

float Node::NeighborCostByUid(int uid) const
{
	return _neighborCosts.at(uid);
}

void Node::SetWeight(int uidNeighbor, float weight)
{
	_neighborCosts.at(uidNeighbor) = weight;
}

bool Node::Visited() const
{
	return _viaUID != -1;
}

void Node::SetVia(int uid)
{
	_viaUID = uid;
}

void Node::AddVisitedNeighbor(int uid)
{
	_visitedNeighbors.emplace(uid);
}

void Node::RemoveVisitedNeighbor(int uid)
{
	_visitedNeighbors.erase(uid);
}

void Node::ClearVisitedNeighbors()
{
	_visitedNeighbors.clear();
}

void Node::AddNeighbor(int uid, float cost)
{
	_neighbors.emplace(uid);
	_neighborCosts.emplace(std::make_pair(uid, cost));
	_resetNeighborCosts.emplace(std::make_pair(uid, cost));
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

float Node::Cost(const std::string &type)
{
	if ( !HasCost(type) )
	{
		_costs.emplace(std::make_pair(type, std::numeric_limits<float>::infinity()));
	}
	return _costs.at(type);
}

bool Node::HasCost(const std::string &type)
{
	return _costs.find(type) != _costs.end();
}

bool Node::VisitedBy(Node &node) const
{
	return node._visitedNeighbors.find(_uid) != node._visitedNeighbors.end();
}

void Node::SetCost(const std::string &type, float cost)
{
	if ( !HasCost(type) )
	{
		_costs.emplace(std::make_pair(type, cost));
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
