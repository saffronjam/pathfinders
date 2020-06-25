#include "Beam32.h"

void Beam32::FindPath(long startUID, long goalUID)
{
    m_checkingQueue.push_front(startUID);
    m_maxCost = -1.0f;
    while (m_checkingQueue.size() && m_state != State::BeingCollected)
    {
        PauseCheck();
        m_activeNodeUID = m_checkingQueue.front();
        Node &activeNode = GetNodes().at(m_activeNodeUID);
        if (m_activeNodeUID == goalUID)
        {
            // Algorithm is done
            m_checkingQueue.clear();
            m_maxCost = activeNode.GetCost("Tentative");
        }
        else if (activeNode.GetCost("Tentative") < m_maxCost || m_maxCost == -1.0f)
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
                    float suggestedTentativeCost = activeNode.GetCost("Tentative") + activeNode.GetNeighborCost(neighborUID);
                    if (suggestedTentativeCost < neighbor.GetCost("Tentative") || neighbor.GetCost("Tentative") == -1.0f)
                    {
                        if (std::find(m_checkingQueue.begin(), m_checkingQueue.end(), neighborUID) == m_checkingQueue.end())
                        {
                            m_checkingQueue.push_back(neighborUID);
                        }
                        neighbor.SetVia(m_activeNodeUID);
                        neighbor.SetCost("Tentative", suggestedTentativeCost);
                        neighbor.SetCost("Heuristic", vl::Length(neighbor.GetPosition() - GetNodes().at(goalUID).GetPosition()));
                        neighbor.SetCost("Total", suggestedTentativeCost + neighbor.GetCost("Heuristic"));
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
                  [this](const auto &lhs, const auto &rhs) { return GetNode(lhs).GetCost("Total") < GetNode(rhs).GetCost("Total"); });
        if (m_checkingQueue.size() > beamWidth)
        {
            int toErase = m_checkingQueue.size() - beamWidth;
            m_checkingQueue.erase(m_checkingQueue.end() - toErase, m_checkingQueue.end());
        }
    };
    m_checkingQueue.clear();
}