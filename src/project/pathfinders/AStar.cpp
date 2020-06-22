#include "AStar.h"

void AStar::FindPath()
{
    m_checkingQueue.push_front(&GetNodes().at(m_traverseGrid->GetStartUID()));
    m_maxCost = -1.0f;
    while (m_checkingQueue.size() && m_state != State::BeingCollected)
    {
        m_activeNode = m_checkingQueue.front();
        if (m_traverseGrid->IsGoal(m_activeNode->GetUID()))
        {
            // Algorithm is reach goal, start check other ways that could be faster
            m_checkingQueue.clear();
            m_maxCost = m_activeNode->GetGCost();
        }
        else if (m_activeNode->GetGCost() < m_maxCost || m_maxCost == -1.0f)
        {
            for (auto &neighbor : m_activeNode->GetNeighbors())
            {
                if (m_state == State::BeingCollected)
                    break;
                SleepDelay();
                if (!m_traverseGrid->IsObstacle(neighbor->GetUID()) && neighbor != m_activeNode->GetVia())
                {
                    float suggestedGCost = m_activeNode->GetGCost() + m_activeNode->GetUCost(neighbor);
                    if (suggestedGCost < neighbor->GetGCost() || neighbor->GetGCost() == -1.0f)
                    {
                        if (std::find(m_checkingQueue.begin(), m_checkingQueue.end(), neighbor) == m_checkingQueue.end())
                        {
                            m_checkingQueue.push_back(neighbor);
                        }
                        neighbor->SetVia(m_activeNode);
                        neighbor->SetGCost(suggestedGCost);
                        neighbor->SetHCost(vl::Length(neighbor->GetPosition() - GetNodes().at(m_traverseGrid->GetGoalUID()).GetPosition()));
                        neighbor->SetFCost(suggestedGCost + neighbor->GetHCost());
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
                  [](const auto &lhs, const auto &rhs) { return lhs->GetFCost() < rhs->GetFCost(); });
    };
    m_checkingQueue.clear();
}