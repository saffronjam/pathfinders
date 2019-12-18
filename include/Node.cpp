#include "Node.hpp"

#include <iostream>

#include "Random.hpp"

Node::Node(sf::Vector2i position)
    : m_position(position),
      m_via(nullptr),
      m_FCost(-1.0f),
      m_HCost(-1.0f),
      m_GCost(-1.0f),
      m_isStart(false),
      m_isFinal(false),
      m_isObstructed(false)
{
    m_neighbors.reserve(10);
    m_UCosts.reserve(10);
}

void Node::AddNeighbor(std::shared_ptr<Node> neighbor)
{
    m_neighbors.push_back(neighbor);
    m_UCosts.push_back(11.0f); //Random::Generate(10.0f, 10.0f));
}

void Node::ResetAll()
{
    ResetSearch();
    m_isObstructed = false;
}

void Node::ResetSearch()
{
    m_via = nullptr;
    m_FCost = -1.0f;
    m_HCost = -1.0f;
    m_GCost = -1.0f;
}