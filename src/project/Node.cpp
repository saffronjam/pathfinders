#include "Node.h"

Node::Node(int uid, const sf::Vector2f &position)
    : m_uid(uid),
      m_position(position),
      m_FCost(-1.0f),
      m_HCost(-1.0f),
      m_GCost(-1.0f),
      m_via(nullptr)
{
}

void Node::AddNeighbor(Node *node, float cost)
{
    m_neighbors.emplace(node);
    m_UCosts.emplace(std::make_pair(node, cost));
}

void Node::ResetPath() noexcept
{
    SetFCost(-1.0f);
    SetGCost(-1.0f);
    SetVia(nullptr);
}

void Node::ResetAll() noexcept
{
    ResetPath();
}