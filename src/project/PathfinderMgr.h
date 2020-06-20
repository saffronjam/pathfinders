#pragma once

#include <vector>

#include "AStar.h"

class PathfinderMgr
{
public:
    PathfinderMgr();

    void DrawGrid();
    void DrawPathfinders();

    void Start();
    void Pause();
    void Resume();
    void Restart();
    void Reset();

private:
    TraverseGrid m_traverseGrid;
    std::vector<std::unique_ptr<Pathfinder>> m_pathfinders;
};