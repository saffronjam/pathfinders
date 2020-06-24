#include "Node.h"

Node::Node(int uid, const sf::Vector2f &position)
    : m_uid(uid),
      m_position(position),
      m_FCost(-1.0f),
      m_HCost(-1.0f),
      m_GCost(-1.0f),
      m_viaUID(-1)
{
}

void Node::AddNeighbor(long uid, float cost)
{
    m_neighbors.emplace(uid);
    m_UCosts.emplace(std::make_pair(uid, cost));
}

void Node::ResetPath() noexcept
{
    SetFCost(-1.0f);
    SetGCost(-1.0f);
    SetVia(-1);
}

void Node::ResetAll() noexcept
{
    ResetPath();
}