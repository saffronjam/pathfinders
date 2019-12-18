#include "AStar.hpp"

AStar::AStar(Grid &grid)
    : m_grid(grid),
      m_gridMode(grid.GetGridModeRef()),
      m_defaultStartAimingPosition(-1000.0f, -1000.0f),
      m_defaultFinalAimingPosition(1000.0f, 1000.0f),
      m_stepTime(0.0f),
      m_maxCost(-1.0f),
      m_isPaused(false),
      m_noPathFound(false)
{
    EnterNewMode(Grid::GridMode::ModeBox);
}

void AStar::Update(sf::Time dt)
{
    if (!m_isPaused && !m_isDone)
    {
        if (m_currentWait >= m_stepTime)
        {
            m_currentWait = 0.0f;
            Step();
        }
        else
        {
            m_currentWait += dt.asSeconds();
        }
    }
}

void AStar::DrawFinal(Graphics &gfx)
{
    if (!m_noPathFound)
    {
        for (auto &node : m_finalPath)
        {
            sf::CircleShape cs(5.0f);
            cs.setFillColor(sf::Color::Green);
            cs.setPosition(sf::Vector2f(node->GetPosition().x - cs.getRadius(), node->GetPosition().y - cs.getRadius()));
            gfx.Draw(cs);
        }
    }
    else
    {
        gfx.Draw((sf::Vector2f)m_startNode->GetPosition(), 10.0f, sf::Color::Red);
        gfx.Draw((sf::Vector2f)m_finalNode->GetPosition(), 10.0f, sf::Color::Red);
        gfx.DrawLine((sf::Vector2f)m_startNode->GetPosition(), (sf::Vector2f)m_finalNode->GetPosition(), sf::Color::Red);
    }
}

void AStar::DrawCurrentAnticipation(Graphics &gfx)
{
    if (m_active)
        for (std::shared_ptr<Node> cmp = m_active; cmp.get()->GetVia(); cmp = cmp->GetVia())
        {
            gfx.DrawLine((sf::Vector2f)cmp->GetPosition(), (sf::Vector2f)cmp->GetVia()->GetPosition(), sf::Color::Red);
        }
}

void AStar::DrawAllNodeViaConnections(Graphics &gfx)
{
    for (auto &node : m_allNodes)
    {
        if (node->GetVia())
        {
            gfx.DrawLine((sf::Vector2f)node->GetPosition(), (sf::Vector2f)node->GetVia()->GetPosition());
        }
    }
}

void AStar::DrawLineToNeighbors(Graphics &gfx)
{
    for (auto &node : m_allNodes)
    {
        sf::Vector2f _this = (sf::Vector2f)node->GetPosition();
        for (auto &neighbor : node->GetNeighbors())
        {
            sf::Vector2f _neighbor = (sf::Vector2f)neighbor->GetPosition();
            gfx.DrawLine(_this, _neighbor, sf::Color::Cyan);
        }
    }
}

void AStar::Solve()
{
    m_isPaused = false;
}

void AStar::Pause()
{
    m_isPaused = true;
}

void AStar::ResetAll()
{
    for (auto &node : m_allNodes)
    {
        node->ResetAll();
    }
    SetStart(FindClosestNode(m_defaultStartAimingPosition));
    SetFinal(FindClosestNode(m_defaultFinalAimingPosition));
    Reset();
}

void AStar::ResetSearch()
{
    for (auto &node : m_allNodes)
    {
        node->ResetSearch();
    }
    Reset();
}

void AStar::Reset()
{
    Pause();
    m_active = nullptr;
    m_isDone = false;
    m_noPathFound = false;
    m_maxCost = -1.0f;
    m_finalPath.clear();
    m_checkingQueue.clear();
    m_checkingQueue.push_front(m_startNode);
}

void AStar::Step()
{
    if (m_checkingQueue.size())
    {
        m_active = m_checkingQueue.front();
        if (m_active == m_finalNode)
        {
            m_checkingQueue.pop_front();
            m_maxCost = m_active->GetGCost();
            ComputeFinalPath();
        }
        else if (m_active->GetGCost() < m_maxCost || m_maxCost == -1.0f)
        {
            for (size_t i = 0; i < m_active->GetNeighbors().size(); i++)
            {
                std::shared_ptr<Node> neighbor = m_active->GetNeighbors()[i];
                if (!neighbor->GetIsObstructed() && neighbor != m_active->GetVia())
                {
                    float suggested_GCost = m_active->GetGCost() + m_active->GetUCost(i);
                    if (suggested_GCost < neighbor->GetGCost() || neighbor->GetGCost() == -1.0f)
                    {
                        if (std::find(m_checkingQueue.begin(), m_checkingQueue.end(), neighbor) == m_checkingQueue.end())
                        {
                            m_checkingQueue.push_back(neighbor);
                        }
                        neighbor->SetVia(m_active);
                        neighbor->SetGCost(suggested_GCost);
                        neighbor->SetHCost(CalculateHCost(neighbor));
                        neighbor->SetFCost(suggested_GCost + neighbor->GetHCost());
                    }
                }
            }
            m_checkingQueue.pop_front();
        }
        else
        {
            m_checkingQueue.pop_front();
        }
        std::sort(m_checkingQueue.begin(),
                  m_checkingQueue.end(),
                  [](const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs) { return lhs->GetFCost() < rhs->GetFCost(); });
    }
    else
    {
        m_active = m_finalNode;
        m_isDone = true;
        if (!m_active->GetVia())
        {
            m_noPathFound = true;
        }
    }
}

float AStar::CalculateHCost(std::shared_ptr<Node> to_calc)
{
    sf::Vector2f _this = (sf::Vector2f)m_allNodes.find(to_calc)->get()->GetPosition();
    sf::Vector2f _final = (sf::Vector2f)m_finalNode->GetPosition();
    float HCost = vf::Distance(_this, _final);
    return HCost;
}

void AStar::ComputeFinalPath()
{
    if (m_finalNode->GetVia())
    {
        m_finalPath.clear();
        for (std::shared_ptr<Node> last, cmp = m_finalNode; last != m_startNode; last = cmp, cmp = cmp->GetVia())
        {
            m_finalPath.emplace(cmp);
        }
    }
}

std::shared_ptr<Node> AStar::FindClosestNode(sf::Vector2f const &position)
{
    float smallest = -1.0f;
    std::shared_ptr<Node> closest = nullptr;
    sf::IntRect screen(-Camera::GetOffset(), Camera::GetOffset() * 2);
    screen.width -= 200; // leaves room for UI
    for (auto &node : m_allNodes)
    {

        float distance = vf::Distance(position, (sf::Vector2f)node->GetPosition());
        if ((distance < smallest || smallest == -1.0f) && (node->GetNeighbors().size()) && (screen.contains(node->GetPosition())))
        {
            smallest = distance;
            closest = node;
        }
    }
    return closest;
}

void AStar::SetStart(std::shared_ptr<Node> startNode)
{
    if (startNode && m_startNode != startNode)
    {
        //before
        if (m_startNode)
        {
            m_startNode->SetIsStart(false);
        }

        m_startNode = startNode;
        ResetSearch();

        //after
        m_startNode->SetIsStart(true);
        m_startNode->SetGCost(0.0f);
        m_startNode->SetHCost(CalculateHCost(m_startNode));
        m_startNode->SetFCost(m_startNode->GetGCost() + m_startNode->GetHCost());
    }
}

void AStar::SetFinal(std::shared_ptr<Node> finalNode)
{
    if (finalNode && m_finalNode != finalNode)
    {
        //before
        if (m_finalNode)
        {
            m_finalNode->SetIsFinal(false);
        }

        m_finalNode = finalNode;
        ResetSearch();

        //after
        m_finalNode->SetIsFinal(true);
        m_startNode->SetGCost(0.0f);
        m_startNode->SetHCost(CalculateHCost(m_startNode));
        m_startNode->SetFCost(m_startNode->GetGCost() + m_startNode->GetHCost());
    }
}

void AStar::EnterNewMode(Grid::GridMode gridMode)
{
    m_gridMode = gridMode;

    m_allNodes.clear();
    switch (gridMode)
    {
    case Grid::GridMode::ModeBox:
        for (size_t i = 0; i < m_grid.GetAllTraverseBoxes().size(); i++)
        {
            m_allNodes.emplace(m_grid.GetAllTraverseBoxes()[i]->GetNode());
        }
        break;
    case Grid::GridMode::ModePolygon:
        for (size_t i = 0; i < m_grid.GetAllTraversePolygons().size(); i++)
        {
            m_allNodes.emplace(m_grid.GetAllTraversePolygons()[i]->GetNode());
        }
        break;
    }
    m_startNode = FindClosestNode(m_defaultFinalAimingPosition);
    m_finalNode = FindClosestNode(m_defaultStartAimingPosition);
    SetStart(FindClosestNode(m_defaultStartAimingPosition));
    SetFinal(FindClosestNode(m_defaultFinalAimingPosition));
    ResetAll();
}