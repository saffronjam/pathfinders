#pragma once

#include <set>
#include <map>
#include <optional>

#include <SFML/System/Vector2.hpp>

class Node
{
    friend class TraverseGrid;

public:
    Node(int uid, const sf::Vector2f &position);

    void AddNeighbor(long uid, float cost);
    void RemoveNeighbor(long uid) { m_neighbors.erase(uid); }

    void ResetCosts() noexcept;
    void ResetPath() noexcept;

    int GetUID() const noexcept { return m_uid; }
    const sf::Vector2f &GetPosition() const noexcept { return m_position; }
    const std::set<long> &GetNeighbors() const noexcept { return m_neighbors; }
    long GetViaUID() const noexcept { return m_viaUID; }
    float GetCost(const std::string &type) noexcept;
    float GetNeighborCost(long uid) const { return m_neighborCosts.at(uid); }
    bool WasVisited() const noexcept { return m_viaUID != -1; }

    void SetVia(long uid) { m_viaUID = uid; }
    void SetCost(const std::string &type, float cost);
    void SetNeighborCost(float UCost, long uid) { m_neighborCosts.at(uid) = UCost; }

private:
    int m_uid;
    sf::Vector2f m_position;
    std::set<long> m_neighbors;

    long m_viaUID;

    std::map<std::string, float> m_costs;
    std::map<long, float> m_neighborCosts;
};