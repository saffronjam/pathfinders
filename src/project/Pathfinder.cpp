#include "Pathfinder.h"

Pathfinder::Pathfinder()
    : m_state(State::WaitingForStart),
      m_activeNode(nullptr),
      m_sleepDelay(sf::seconds(0.01f)),
      m_minorDelayTimer(0),
      m_minorDelay(false)
{
}

Pathfinder::~Pathfinder()
{
    CollectFinder();
}

void Pathfinder::DrawAnticipation()
{
    if (m_activeNode && m_state == State::Finding || m_state == State::Paused || m_state == State::Finished)
    {
        for (Node *node = m_activeNode; node->GetVia(); node = node->GetVia())
        {
            Camera::DrawLine(node->GetPosition(), node->GetVia()->GetPosition(), sf::Color::Red);
        }
        Camera::DrawPoint(m_activeNode->GetPosition(), sf::Color::Red);
    }
}

void Pathfinder::DrawViaConnections()
{
    for (auto &[uid, node] : m_nodes)
    {
        if (node.GetVia())
            Camera::DrawLine(node.GetPosition(), node.GetVia()->GetPosition(), sf::Color(150, 150, 150, 20));
    }
}

void Pathfinder::DrawNeighbors()
{
    for (auto &[uid, node] : m_nodes)
    {
        for (auto &neighbor : node.GetNeighbors())
        {
            Camera::DrawLine(node.GetPosition(), neighbor->GetPosition(), sf::Color(255, 0, 255, 50));
        }
    }
}

void Pathfinder::DrawResult()
{
    if (m_pathWasFound)
    {
        for (auto &node : m_finalPath)
        {
            Camera::DrawPoint(node->GetPosition(), sf::Color::Green, 5.0f);
        }
        Camera::DrawPoint(m_nodes.at(m_traverseGrid->GetStartUID()).GetPosition(), sf::Color::Green, 5.0f);
    }
    else
    {
        Camera::DrawPoint(m_nodes.at(m_traverseGrid->GetStartUID()).GetPosition(), sf::Color::Red, 10.0f);
        Camera::DrawPoint(m_nodes.at(m_traverseGrid->GetGoalUID()).GetPosition(), sf::Color::Red, 10.0f);
    }
}

void Pathfinder::AssignNodes(const std::map<long, Node> &nodes) noexcept
{
    m_nodes = nodes;
    m_traverseGrid->CalculateNeighbors(m_nodes);
}

void Pathfinder::Start()
{
    if (m_state == State::Finished)
    {
        Restart();
    }
    if (m_state == State::WaitingForStart)
    {
        CollectFinder();
        m_state = State::Finding;
        m_finder = std::thread(Pathfinder::FindPathThreadFn, this);
    }
}

void Pathfinder::Pause()
{
    if (m_state == State::Finding)
        m_state = State::Paused;
}

void Pathfinder::Resume()
{
    if (m_state == State::Paused)
        m_state = State::Finding;
}

void Pathfinder::Restart()
{
    if (m_state == State::Finding || m_state == State::Paused || m_state == State::Finished)
    {
        CollectFinder();
        m_state = State::WaitingForStart;
        for (auto &[uid, node] : m_nodes)
        {
            node.ResetPath();
        }
    }
}

void Pathfinder::Reset()
{
    if (m_state != State::WaitingForStart)
    {
        CollectFinder();
        m_state = State::WaitingForStart;
        for (auto &[uid, node] : m_nodes)
            node.ResetAll();
    }
}

void Pathfinder::SetSleepDelay(sf::Time delay)
{
    m_sleepDelay = delay;
    m_minorDelay = (m_sleepDelay.asMicroseconds() < 1000);
}

void Pathfinder::PauseCheck()
{
    while (m_state == State::Paused && m_state != State::BeingCollected)
        sf::sleep(sf::seconds(0.01f));
}

void Pathfinder::SleepDelay()
{
    if (!m_minorDelay)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(m_sleepDelay.asMicroseconds()));
    }
    else
    {
        m_minorDelayTimer += m_sleepDelay.asMicroseconds();
        while (m_minorDelayTimer > 1000)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            m_minorDelayTimer -= 1000;
        }
    }
}

void Pathfinder::FindPathThreadFn()
{
    FindPath();
    if (m_state != State::BeingCollected)
        OnFinish();
}

void Pathfinder::OnFinish()
{
    m_pathWasFound = static_cast<bool>(GetNodes().at(m_traverseGrid->GetGoalUID()).GetVia());
    if (m_pathWasFound)
    {
        m_finalPath.clear();
        for (const Node *node = &m_nodes.at(m_traverseGrid->GetGoalUID()); node != &m_nodes.at(m_traverseGrid->GetStartUID()); node = node->GetVia())
        {
            m_finalPath.emplace(node);
        }
    }
    m_state = State::Finished;
}

void Pathfinder::CollectFinder()
{
    auto savedState = m_state;
    m_state = State::BeingCollected;
    if (m_finder.joinable())
        m_finder.join();
    m_state = savedState;
}
