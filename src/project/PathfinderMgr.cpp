#include "PathfinderMgr.h"

PathfinderMgr::PathfinderMgr()
    : m_traverseGrid(TraverseGrid::Type::Square, sf::FloatRect(-Camera::GetOffset(), sf::Vector2f(static_cast<float>(Window::GetWidth()) - 150.0f, static_cast<float>(Window::GetHeight()))))
{
    m_pathfinders.emplace_back(std::make_unique<AStar>());

    for (auto &pathfinder : m_pathfinders)
    {
        pathfinder->AssignNodes(m_traverseGrid.GetNodes());
        pathfinder->SetTraverseGrid(&m_traverseGrid);
    }
}

void PathfinderMgr::DrawGrid()
{
    m_traverseGrid.Draw();
}

void PathfinderMgr::DrawPathfinders()
{
    for (auto &pathfinder : m_pathfinders)
    {
        pathfinder->DrawProgress();
        if (pathfinder->IsDone())
            pathfinder->DrawResult();
    }
}