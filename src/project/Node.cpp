#include "Node.h"

Node::Node(int uid, const sf::Vector2f &position)
    : m_uid(uid),
      m_position(position),
      m_viaUID(-1)
{
}

void Node::AddNeighbor(long uid, float cost)
{
    m_neighbors.emplace(uid);
    m_neighborCosts.emplace(std::make_pair(uid, cost));
}

void Node::ResetCosts() noexcept
{
    m_costs.clear();
}

void Node::ResetPath() noexcept
{
    ResetCosts();
    SetVia(-1);
}

float Node::GetCost(const std::string &type) noexcept
{
    if (m_costs.find(type) == m_costs.end())
        m_costs.emplace(std::make_pair(type, -1));
    return m_costs.at(type);
}

void Node::SetCost(const std::string &type, float cost)
{
    if (m_costs.find(type) == m_costs.end())
        m_costs.emplace(std::make_pair(type, -1));
    m_costs.at(type) = cost;
}