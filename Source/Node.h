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

	auto operator==(int uid) const -> bool;

	void AddNeighbor(int uid, float cost);
	void RemoveNeighbor(int uid);

	void ResetCosts();
	void ResetNeighborsCost();
	void ResetPath();

	auto Uid() const -> int;
	auto Position() const -> const sf::Vector2f&;
	auto ViaUID() const -> int;
	auto Cost(const String& type) -> float;
	auto HasCost(const String& type) -> bool;

	auto Neighbors() const -> const HashSet<int>&;
	auto NeighborCostByUid(int uid) const -> float;
	void SetWeight(int uidNeighbor, float weight);

	auto Visited() const -> bool;
	auto VisitedBy(Node& node) const -> bool;

	void SetVia(int uid);
	void SetCost(const String &type, float cost);
	void SetNeighborCost(float cost, int uid);

	void AddVisitedNeighbor(int uid);
	void RemoveVisitedNeighbor(int uid);
	void ClearVisitedNeighbors();

private:
	int _uid;
	sf::Vector2f _position;
	HashSet<int> _neighbors;
	HashSet<int> _visitedNeighbors;

	int _viaUID;

	HashMap<String, float> _costs;
	HashMap<int, float> _neighborCosts;
	HashMap<int, float> _resetNeighborCosts;
};
}