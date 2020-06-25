#include "PathfinderMgr.h"

PathfinderMgr::PathfinderMgr()
    : m_editState(EditState::None),
      m_traverseGrid(TraverseGrid::Type::Square, sf::FloatRect(-Camera::GetOffset(), sf::Vector2f(static_cast<float>(Window::GetWidth()) - 200.0f, static_cast<float>(Window::GetHeight())))),
      m_drawWorker(true),
      m_drawViaConnections(true),
      m_drawNeighbors(false),
      m_activePathFinder(nullptr)
{
    m_pathfinders.push_back(std::make_unique<AStar>());
    m_pathfinders.push_back(std::make_unique<Dijkstra>());
    m_pathfinders.push_back(std::make_unique<GreedyBFS>());
    SetActiveAlgorithm("A*");

    for (auto &pathfinder : m_pathfinders)
    {
        pathfinder->SetTraverseGrid(&m_traverseGrid);
        pathfinder->AssignNodes(m_traverseGrid.GetNodes());
    }
}

void PathfinderMgr::Update()
{
    if (Mouse::IsDown(sf::Mouse::Button::Left) && Mouse::GetPos().x < static_cast<float>(Window::GetWidth() - 200))
    {
        long nodeUID = m_traverseGrid.GetNodeUID(Camera::ScreenToWorld(Mouse::GetPos()));
        switch (m_editState)
        {
        case EditState::AddObstacles:
            m_traverseGrid.AddObstacle(nodeUID);
            break;
        case EditState::RemObstacles:
            m_traverseGrid.RemoveObstacle(nodeUID);
            break;
        case EditState::SetStart:
            m_traverseGrid.SetStart(nodeUID);
            break;
        case EditState::SetGoal:
        {
            m_traverseGrid.SetGoal(nodeUID);
            break;
        }
        case EditState::AddSubGoal:
        {
            m_traverseGrid.AddSubGoal(nodeUID);
            break;
        }
        case EditState::RemSubGoal:
        {
            m_traverseGrid.RemoveSubGoal(nodeUID);
            break;
        }
        break;
        default:
            break;
        }
    }
}

void PathfinderMgr::DrawGrid()
{
    m_traverseGrid.Draw();
}

void PathfinderMgr::DrawPathfinders()
{
    if (m_drawNeighbors)
        m_activePathFinder->DrawNeighbors();
    if (m_drawViaConnections)
        m_activePathFinder->DrawViaConnections();
    if (m_drawWorker)
    {
        if (!m_activePathFinder->IsDone())
        {
            m_activePathFinder->DrawAnticipation();
        }
        else
        {
            m_activePathFinder->DrawResult();
        }
    }
}

void PathfinderMgr::Start()
{
    m_activePathFinder->Start(m_traverseGrid.GetStartUID(), m_traverseGrid.GetGoalUID(), m_traverseGrid.GetSubGoalUIDs());
}

void PathfinderMgr::Pause()
{
    m_activePathFinder->Pause();
}

void PathfinderMgr::Resume()
{
    m_activePathFinder->Resume();
}

void PathfinderMgr::Restart()
{
    m_activePathFinder->Restart();
}

void PathfinderMgr::Reset()
{
    for (auto &pathfinder : m_pathfinders)
        pathfinder->Reset();
    m_traverseGrid.ClearObstacles();
    m_traverseGrid.ClearSubGoals();
    m_traverseGrid.ResetStartGoal();
}

void PathfinderMgr::SetSleepDelay(sf::Time delay) noexcept
{
    for (auto &pathfinder : m_pathfinders)
        pathfinder->SetSleepDelay(delay);
}

void PathfinderMgr::SetVisType(TraverseGrid::Type type)
{
    if (m_traverseGrid.GetType() != type)
    {
        for (auto &pathfinder : m_pathfinders)
        {
            pathfinder->Reset();
        }
        m_traverseGrid.ChangeGridType(type);
        for (auto &pathfinder : m_pathfinders)
        {
            pathfinder->AssignNodes(m_traverseGrid.GetNodes());
        }
    }
}

void PathfinderMgr::SetActiveAlgorithm(const std::string &name)
{
    if (m_activePathFinder)
        Restart();
    for (auto &pathfinder : m_pathfinders)
    {
        if (pathfinder->GetName() == name)
        {
            m_activePathFinder = pathfinder.get();
        }
    }
}