#pragma once

#include "Pathfinder.h"

class AStar : public Pathfinder
{
public:
    AStar()
        : m_maxCost(-1.0f)
    {
    }

    void FindPath() override;

private:
    std::deque<Node *> m_checkingQueue;
    float m_maxCost;
};