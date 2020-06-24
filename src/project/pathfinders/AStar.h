#pragma once

#include "Pathfinder.h"

class AStar : public Pathfinder
{
public:
    void FindPath() override;

    const std::string &GetName() override { return m_name; }

private:
    std::deque<long> m_checkingQueue;
    float m_maxCost;
    std::string m_name = "A*";
};