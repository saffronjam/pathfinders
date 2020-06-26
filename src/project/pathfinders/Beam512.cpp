#include "Beam512.h"

void Beam512::FindPath(long startUID, long goalUID)
{
    m_checkingQueue.push_front(startUID);
    GetNode(startUID).SetCost("Tentative", 0.0f);
    while (m_checkingQueue.size() && m_state != State::BeingCollected)
    {
        PauseCheck();
        m_activeNodeUID = m_checkingQueue.front();
        if (m_activeNodeUID == goalUID)
            break;

        Node &activeNode = GetNodes().at(m_activeNodeUID);
        m_checkingQueue.pop_front();

        for (auto &neighborUID : activeNode.GetNeighbors())
        {
            if (m_state == State::BeingCollected)
                break;
            PauseCheck();
            SleepDelay();

            Node &neighbor = GetNodes().at(neighborUID);
            if (!m_traverseGrid->IsObstacle(neighborUID) && neighborUID != activeNode.GetViaUID())
            {
                float suggestedTentativeCost = activeNode.GetCost("Tentative") + activeNode.GetNeighborCost(neighborUID);
                if (suggestedTentativeCost < neighbor.GetCost("Tentative"))
                {
                    if (std::find(m_checkingQueue.begin(), m_checkingQueue.end(), neighborUID) == m_checkingQueue.end())
                        m_checkingQueue.push_back(neighborUID);

                    neighbor.SetVia(m_activeNodeUID);
                    neighbor.SetCost("Tentative", suggestedTentativeCost);
                    neighbor.SetCost("Heuristic", vl::Length(neighbor.GetPosition() - GetNodes().at(goalUID).GetPosition()));
                    neighbor.SetCost("Total", suggestedTentativeCost + neighbor.GetCost("Heuristic"));
                }
            }
            activeNode.AddVisitedNeighbor(neighborUID);
        }
        std::sort(m_checkingQueue.begin(), m_checkingQueue.end(), [this](const auto &lhs, const auto &rhs) {
            return GetNode(lhs).GetCost("Total") < GetNode(rhs).GetCost("Total");
        });

        if (m_checkingQueue.size() > beamWidth)
        {
            int toErase = m_checkingQueue.size() - beamWidth;
            m_checkingQueue.erase(m_checkingQueue.end() - toErase, m_checkingQueue.end());
        }
    }
    m_checkingQueue.clear();
}