#pragma once

#include <unordered_set>
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
	auto Cost(const std::string& type) -> float;
	auto HasCost(const std::string& type) -> bool;

	auto Neighbors() const -> const std::unordered_set<int>&;
	auto NeighborCostByUid(int uid) const -> float;
	void SetWeight(int uidNeighbor, float weight);

	auto Visited() const -> bool;
	auto VisitedBy(Node& node) const -> bool;

	void SetVia(int uid);
	void SetCost(const std::string &type, float cost);
	void SetNeighborCost(float cost, int uid);

	void AddVisitedNeighbor(int uid);
	void RemoveVisitedNeighbor(int uid);
	void ClearVisitedNeighbors();

private:
	int _uid;
	sf::Vector2f _position;
	std::unordered_set<int> _neighbors;
	std::unordered_set<int> _visitedNeighbors;

	int _viaUID;

	std::unordered_map<std::string, float> _costs;
	std::unordered_map<int, float> _neighborCosts;
	std::unordered_map<int, float> _resetNeighborCosts;
};
}