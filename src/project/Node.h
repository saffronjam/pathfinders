#pragma once

#include <set>
#include <map>
#include <optional>

#include <SFML/System/Vector2.hpp>

class Node
{
    friend class TraverseGrid;

public:
    Node(int uid, const sf::Vector2f &position)
        : m_uid(uid),
          m_position(position),
          m_FCost(-1.0f),
          m_HCost(-1.0f),
          m_GCost(-1.0f)
    {
    }

    void AddNeighbor(Node *node) { m_neighbors.emplace(node); }
    void RemoveNeighbor(Node *node) { m_neighbors.erase(node); }

    bool IsObstacle() const noexcept { return m_isObstacle; }

    int GetUID() const noexcept { return m_uid; }
    const sf::Vector2f &GetPosition() const noexcept { return m_position; }
    const std::set<Node *> &GetNeighbors() const noexcept { return m_neighbors; }
    Node *GetVia() const noexcept { return m_via; }
    float GetFCost() const noexcept { return m_FCost; }
    float GetHCost() const noexcept { return m_HCost; }
    float GetGCost() const noexcept { return m_GCost; }
    float GetUCost(Node *neighbor) const noexcept { return m_UCosts.at(neighbor); }

    void SetVia(Node *via) { m_via = via; }
    void SetFCost(float FCost) { m_FCost = FCost; }
    void SetHCost(float HCost) { m_HCost = HCost; }
    void SetGCost(float GCost) { m_GCost = GCost; }
    void SetUCost(float UCost, Node *neighbor) { m_UCosts[neighbor] = UCost; }

private:
    int m_uid;
    sf::Vector2f m_position;
    std::set<Node *> m_neighbors;

    Node *m_via;

    bool m_isObstacle;

    float m_FCost;
    float m_HCost;
    float m_GCost;
    std::map<Node *, float> m_UCosts;
};