#pragma once

#include <vector>
#include <memory>

#include <SFML/System/Vector2.hpp>

class Node
{
public:
    Node(sf::Vector2i position);

    void AddNeighbor(std::shared_ptr<Node> neighbor);

    void ResetAll();
    void ResetSearch();

    void SetVia(std::shared_ptr<Node> via) { m_via = via; }
    void SetFCost(float const &FCost) { m_FCost = FCost; }
    void SetHCost(float const &HCost) { m_HCost = HCost; }
    void SetGCost(float const &GCost) { m_GCost = GCost; }
    void SetUCost(float const &UCost, int const &index) { m_UCosts[index] = UCost; }
    void SetIsStart(bool const &isStart) { m_isStart = isStart; }
    void SetIsFinal(bool const &isFinal) { m_isFinal = isFinal; }
    void SetIsObstructed(bool const &isObstructed) { m_isObstructed = isObstructed; }

    sf::Vector2i GetPosition() { return m_position; }
    std::shared_ptr<Node> GetVia() { return m_via; }
    float GetFCost() { return m_FCost; }
    float GetHCost() { return m_HCost; }
    float GetGCost() { return m_GCost; }
    float GetUCost(int const &index) { return m_UCosts[index]; }
    std::vector<std::shared_ptr<Node>> GetNeighbors() { return m_neighbors; }
    bool GetIsStart() { return m_isStart; }
    bool GetIsFinal() { return m_isFinal; }
    bool GetIsObstructed() { return m_isObstructed; }

private:
    sf::Vector2i m_position;
    std::shared_ptr<Node> m_via;
    float m_FCost;
    float m_HCost;
    float m_GCost;
    std::vector<std::shared_ptr<Node>> m_neighbors;
    std::vector<float> m_UCosts;

    bool m_isStart;
    bool m_isFinal;
    bool m_isObstructed;
};