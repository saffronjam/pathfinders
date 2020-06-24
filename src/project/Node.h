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
    float GetFCost() const noexcept { return m_FCost; }
    float GetHCost() const noexcept { return m_HCost; }
    float GetGCost() const noexcept { return m_GCost; }
    float GetUCost(long uid) const { return m_UCosts.at(uid); }
    bool WasVisited() const noexcept { return m_viaUID != -1; }

    void SetVia(long uid) { m_viaUID = uid; }
    void SetFCost(float FCost) { m_FCost = FCost; }
    void SetHCost(float HCost) { m_HCost = HCost; }
    void SetGCost(float GCost) { m_GCost = GCost; }
    void SetUCost(float UCost, long uid) { m_UCosts[uid] = UCost; }

private:
    int m_uid;
    sf::Vector2f m_position;
    std::set<long> m_neighbors;

    long m_viaUID;

    float m_FCost;
    float m_HCost;
    float m_GCost;
    std::map<long, float> m_UCosts;
};