#include "AStar.h"

void AStar::FindPath()
{
    m_checkingQueue.push_front(m_traverseGrid->GetStartUID());
    m_maxCost = -1.0f;
    while (m_checkingQueue.size() && m_state != State::BeingCollected)
    {
        PauseCheck();
        m_activeNodeUID = m_checkingQueue.front();
        Node &activeNode = GetNodes().at(m_activeNodeUID);
        if (m_traverseGrid->IsGoal(m_activeNodeUID))
        {
            // Algorithm is done
            m_checkingQueue.clear();
            m_maxCost = activeNode.GetGCost();
        }
        else if (activeNode.GetGCost() < m_maxCost || m_maxCost == -1.0f)
        {
            for (auto &neighborUID : activeNode.GetNeighbors())
            {
                Node &neighbor = GetNodes().at(neighborUID);
                if (m_state == State::BeingCollected)
                    break;
                SleepDelay();
                PauseCheck();
                if (!m_traverseGrid->IsObstacle(neighbor.GetUID()) && neighbor.GetUID() != activeNode.GetViaUID())
                {
                    float suggestedGCost = activeNode.GetGCost() + activeNode.GetUCost(neighborUID);
                    if (suggestedGCost < neighbor.GetGCost() || neighbor.GetGCost() == -1.0f)
                    {
                        if (std::find(m_checkingQueue.begin(), m_checkingQueue.end(), neighborUID) == m_checkingQueue.end())
                        {
                            m_checkingQueue.push_back(neighborUID);
                        }
                        neighbor.SetVia(m_activeNodeUID);
                        neighbor.SetGCost(suggestedGCost);
                        neighbor.SetHCost(vl::Length(neighbor.GetPosition() - GetNodes().at(m_traverseGrid->GetGoalUID()).GetPosition()));
                        neighbor.SetFCost(suggestedGCost + neighbor.GetHCost());
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
                  [this](const auto &lhs, const auto &rhs) { return GetNode(lhs).GetFCost() < GetNode(rhs).GetFCost(); });
    };
    m_checkingQueue.clear();
}