#include "Dijkstra.h"

void Dijkstra::FindPath(long startUID, long goalUID)
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
                    float suggestedGCost = activeNode.GetCost("Tentative") + activeNode.GetNeighborCost(neighborUID);
                    if (suggestedGCost < neighbor.GetCost("Tentative") || neighbor.GetCost("Tentative") == -1.0f)
                    {
                        if (std::find(m_checkingQueue.begin(), m_checkingQueue.end(), neighborUID) == m_checkingQueue.end())
                        {
                            m_checkingQueue.push_back(neighborUID);
                        }
                        neighbor.SetVia(m_activeNodeUID);
                        neighbor.SetCost("Tentative", suggestedGCost);
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
                  [this](const auto &lhs, const auto &rhs) { return GetNode(lhs).GetCost("Tentative") < GetNode(rhs).GetCost("Tentative"); });
    };
    m_checkingQueue.clear();
}