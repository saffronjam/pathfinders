#pragma once

#include "Pathfinder.h"

class Dijkstra : public Pathfinder
{
public:
    void FindPath(long startUID, long goalUID) override;

    const std::string &GetName() override { return m_name; }

private:
    std::string m_name = "Dijkstra";
};
