#pragma once

#include <vector>

#include "AStar.h"
#include "Mouse.h"
#include "Camera.h"

class PathfinderMgr
{
public:
    enum class EditState
    {
        None,
        AddObstacles,
        RemObstacles,
        SetStart,
        SetGoal
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

    const auto &GetPathfinders() const noexcept { return m_pathfinders; }
    EditState GetEditState() const noexcept { return m_editState; }

    void SetSleepDelay(sf::Time delay) noexcept;
    void SetEditState(EditState editState) noexcept { m_editState = editState; }
    void SetVisType(TraverseGrid::Type type);

    void Activate(const std::string &name);
    void Deactivate(const std::string &name);

private:
    EditState m_editState;
    TraverseGrid m_traverseGrid;
    std::vector<std::unique_ptr<Pathfinder>> m_pathfinders;
};