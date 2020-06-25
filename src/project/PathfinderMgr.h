#pragma once

#include <vector>
#include <bitset>

#include "Mouse.h"
#include "Camera.h"

#include "AStar.h"
#include "Dijkstra.h"
#include "GreedyBFS.h"
#include "Beam32.h"
#include "Beam512.h"

class PathfinderMgr
{
public:
    enum class EditState
    {
        None,
        AddObstacles,
        RemObstacles,
        SetStart,
        SetGoal,
        AddSubGoal,
        RemSubGoal
    };

public:
    PathfinderMgr();

    void Update();

    void DrawGrid();
    void DrawPathfinders();

    void Start();
    void Pause();
    void Resume();
    void Restart();
    void Reset();

    void SetDrawWorker(bool onoff) noexcept { m_drawWorker = onoff; }
    void SetDrawViaConnections(bool onoff) noexcept { m_drawViaConnections = onoff; }
    void SetDrawNeighbors(bool onoff) noexcept { m_drawNeighbors = onoff; }

    const auto &GetPathfinders() const noexcept { return m_pathfinders; }
    EditState GetEditState() const noexcept { return m_editState; }

    void SetSleepDelay(sf::Time delay) noexcept;
    void SetEditState(EditState editState) noexcept { m_editState = editState; }
    void SetVisType(TraverseGrid::Type type);

    void SetActiveAlgorithm(const std::string &name);

private:
    EditState m_editState;
    TraverseGrid m_traverseGrid;
    std::vector<std::unique_ptr<Pathfinder>> m_pathfinders;
    Pathfinder *m_activePathFinder;

    bool m_drawWorker;
    bool m_drawViaConnections;
    bool m_drawNeighbors;
};