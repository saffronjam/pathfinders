#pragma once

#include <set>
#include <map>
#include <string>

#include <SFML/System/Vector2.hpp>

#include <Saffron.h>

namespace Se
{
class Node
{
public:
	Node(int uid, const sf::Vector2f &position);

	bool operator==(int uid) const { return _uid == uid; }

	void AddNeighbor(int uid, float cost);
	void RemoveNeighbor(int uid) { _neighbors.erase(uid); }

	void ResetCosts();
	void ResetNeighborsCost();
	void ResetPath();

	int GetUID() const { return _uid; }
	const sf::Vector2f &GetPosition() const { return _position; }
	int GetViaUID() const { return _viaUID; }
	float GetCost(const String &type);
	bool HasCost(const String &type);

	const Set<int> &GetNeighbors() const { return _neighbors; }
	float GetNeighborCost(int uid) const { return _neighborCosts.at(uid); }
	void SetWeight(int uidNeighbor, float weight) { _neighborCosts.at(uidNeighbor) = weight; }

	bool WasVisited() const { return _viaUID != -1; }
	bool WasVisitedBy(Node &node) const;

	void SetVia(int uid) { _viaUID = uid; }
	void SetCost(const String &type, float cost);
	void SetNeighborCost(float cost, int uid);

	void AddVisitedNeighbor(int uid) { _visitedNeighbors.emplace(uid); }
	void RemoveVisitedNeighbor(int uid) { _visitedNeighbors.erase(uid); }
	void ClearVisitedNeighbors() { _visitedNeighbors.clear(); }

private:
	int _uid;
	sf::Vector2f _position;
	Set<int> _neighbors;
	Set<int> _visitedNeighbors;

	int _viaUID;

	Map<String, float> _costs;
	Map<int, float> _neighborCosts;
	Map<int, float> _resetNeighborCosts;
};
}