#include "TraverseGrid.h"

TraverseGrid::TraverseGrid(TraverseGrid::Type type, const sf::FloatRect &visRect)
    : m_visRect(visRect),
      m_nBoxes(45, 35),
      m_currentType(type),
      m_voronoiGrid(sf::FloatRect(-Camera::GetOffset(), sf::Vector2f(Window::GetWidth() - 200, Window::GetHeight())), 1600),
      m_startUID(-1),
      m_goalUID(-1),
      m_obstacleColor(60, 60, 60),
      m_startColor(sf::Color::Cyan),
      m_goalColor(sf::Color::Yellow)
{
    m_voronoiGrid.Relax(50);
    GenerateGrids();
    ChangeGridType(type);
    CalculateSquareGridNeighbors();
    CalculateVoronoiGridNeighbors();
}

void TraverseGrid::Draw()
{
    switch (m_currentType)
    {
    case Type::Square:
        DrawSquareGrid();
        break;
    case Type::Voronoi:
        DrawVoronoiGrid();
        break;
    default:
        break;
    }
}

void TraverseGrid::ChangeGridType(TraverseGrid::Type type)
{
    ClearObstacles();
    ClearSubGoals();
    m_currentType = type;
    m_startUID = -1;
    m_goalUID = -1;
    ResetStartGoal();
}

long TraverseGrid::GetNodeUID(const sf::Vector2f &position) const
{
    auto &nodeMap = GetActiveNodesConst();

    float closestDistance = std::numeric_limits<float>::infinity();
    long closestUID;

    for (auto &[uid, node] : nodeMap)
    {
        auto dist = vl::LengthSq(node.GetPosition() - position);
        if (dist < closestDistance)
        {
            closestUID = uid;
            closestDistance = dist;
        }
    }
    return closestUID;
}

long TraverseGrid::GetNodeUID(const sf::Vector2f &position, Type type) const
{
    const std::map<long, Node> *nodeMap;
    switch (type)
    {
    case Type::Square:
        nodeMap = &m_squareGridNodes;
        break;
    case Type::Voronoi:
        nodeMap = &m_voronoiGridNodes;
        break;
    }

    float closestDistance = std::numeric_limits<float>::infinity();
    long closestUID;

    for (auto &[uid, node] : *nodeMap)
    {
        auto dist = vl::LengthSq(node.GetPosition() - position);
        if (dist < closestDistance)
        {
            closestUID = uid;
            closestDistance = dist;
        }
    }
    return closestUID;
}

bool TraverseGrid::IsClear(long uid) const noexcept
{
    return !IsObstacle(uid) &&
           !IsStart(uid) &&
           !IsGoal(uid) &&
           !IsSubGoal(uid);
}

void TraverseGrid::SetStart(const sf::Vector2f &position)
{
    SetStart(GetNodeUID(position));
}

void TraverseGrid::SetStart(long uid)
{
    if (IsClear(uid))
    {
        ClearNodeColor(m_startUID);
        m_startUID = uid;
        SetNodeColor(uid, m_startColor);
    }
}

void TraverseGrid::SetGoal(const sf::Vector2f &position)
{
    SetGoal(GetNodeUID(position));
}

void TraverseGrid::SetGoal(long uid)
{
    if (IsClear(uid))
    {
        ClearNodeColor(m_goalUID);
        m_goalUID = uid;
        SetNodeColor(uid, m_goalColor);
    }
}

void TraverseGrid::ResetStartGoal()
{
    SetStart(-Camera::GetOffset());
    SetGoal(Camera::GetOffset() - sf::Vector2f(200.0f, 0.0f));
}

void TraverseGrid::AddSubGoal(const sf::Vector2f &position)
{
    AddSubGoal(GetNodeUID(position));
}

void TraverseGrid::AddSubGoal(long uid)
{
    if (IsClear(uid))
    {
        m_subGoalUIDs.push_back(uid);
        SetNodeColor(uid, sf::Color::Blue);
    }
}

void TraverseGrid::RemoveSubGoal(const sf::Vector2f &position)
{
    RemoveSubGoal(GetNodeUID(position));
}

void TraverseGrid::RemoveSubGoal(long uid)
{
    if (IsSubGoal(uid))
    {
        m_subGoalUIDs.erase(std::find(m_subGoalUIDs.begin(), m_subGoalUIDs.end(), uid));
        ClearNodeColor(uid);
    }
}

void TraverseGrid::ClearSubGoals()
{
    for (auto &uid : m_subGoalUIDs)
    {
        ClearNodeColor(uid);
    }
    m_subGoalUIDs.clear();
}

void TraverseGrid::AddObstacle(const sf::Vector2f &position)
{
    AddObstacle(GetNodeUID(position));
}

void TraverseGrid::AddObstacle(long uid)
{
    if (IsClear(uid))
    {
        m_obstacleUIDs.emplace(uid);
        SetNodeColor(uid, m_obstacleColor);
    }
}

void TraverseGrid::RemoveObstacle(const sf::Vector2f &position)
{
    RemoveObstacle(GetNodeUID(position));
}

void TraverseGrid::RemoveObstacle(long uid)
{
    if (IsObstacle(uid))
    {
        m_obstacleUIDs.erase(uid);
        ClearNodeColor(uid);
    }
}

void TraverseGrid::ClearObstacles()
{
    for (auto &uid : m_obstacleUIDs)
    {
        ClearNodeColor(uid);
    }
    m_obstacleUIDs.clear();
}

void TraverseGrid::DrawSquareGrid()
{
    for (auto &[uid, rect] : m_squareGrid)
    {
        Camera::Draw(rect);
    }
}

void TraverseGrid::DrawVoronoiGrid()
{
    Camera::Draw(m_voronoiGrid);
}

void TraverseGrid::GenerateGrids()
{
    m_squareGridNodes.clear();
    m_voronoiGridNodes.clear();

    // ---- SQUARE ----
    int uid = 0;
    sf::Vector2f boxSize(m_visRect.width / m_nBoxes.x, m_visRect.height / m_nBoxes.y);
    sf::Vector2f topLeft = sf::Vector2f(m_visRect.left, m_visRect.top) + sf::Vector2f(boxSize.x, boxSize.y) / 2.0f;

    for (int i = 0; i < m_nBoxes.y; i++)
    {
        for (int j = 0; j < m_nBoxes.x; j++)
        {
            m_squareGridNodes.emplace(std::make_pair(uid++, Node(uid, sf::Vector2f(topLeft.x + j * boxSize.x, topLeft.y + i * boxSize.y))));
        }
    }

    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(boxSize.x, boxSize.y));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Blue);
    rect.setOutlineThickness(1.0f);
    for (auto &[uid, node] : m_squareGridNodes)
    {
        rect.setPosition(node.GetPosition() - rect.getSize() / 2.0f);
        m_squareGrid.emplace(std::make_pair(uid, rect));
    }

    // ---- VORONOI ----
    uid = 0;
    auto &polygons = m_voronoiGrid.GetPolygons();
    for (size_t i = 0; i < polygons.size(); i++)
    {
        m_voronoiGridNodes.emplace(uid++, Node(uid, Lib::Mid(polygons[i])));
    }
    m_voronoiGrid.SetOutlineColor(sf::Color::Blue);

    ResetStartGoal();
}

void TraverseGrid::CalculateSquareGridNeighbors()
{
    sf::Vector2f boxSize(m_visRect.width / m_nBoxes.x, m_visRect.height / m_nBoxes.y);
    float diagonalLength = vl::Length(boxSize);
    for (int i = 0; i < m_nBoxes.x * m_nBoxes.y; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (((i % m_nBoxes.x == 0) && j == 0) ||
                ((i % m_nBoxes.x == 0 || (i >= 0 && i < m_nBoxes.x)) && j == 1) ||
                ((i >= 0 && i < m_nBoxes.x) && j == 2) ||
                (((i >= 0 && i < m_nBoxes.x) || (i + 1) % m_nBoxes.x == 0) && j == 3) ||
                (((i + 1) % m_nBoxes.x == 0) && j == 4) ||
                (((i + 1) % m_nBoxes.x == 0 || i >= m_nBoxes.x * (m_nBoxes.y - 1)) && j == 5) ||
                ((i >= m_nBoxes.x * (m_nBoxes.y - 1) && (i <= (m_nBoxes.x * m_nBoxes.y))) && j == 6) ||
                ((i >= m_nBoxes.x * (m_nBoxes.y - 1) || i % m_nBoxes.x == 0) && j == 7))
            {
                continue;
            }
            switch (j)
            {
            case 0:
                m_squareGridNodes.at(i).AddNeighbor(i - 1, boxSize.x);
                break;
            case 1:
                m_squareGridNodes.at(i).AddNeighbor(i - 1 - m_nBoxes.x, diagonalLength);
                break;
            case 2:
                m_squareGridNodes.at(i).AddNeighbor(i - m_nBoxes.x, boxSize.y);
                break;
            case 3:
                m_squareGridNodes.at(i).AddNeighbor(i + 1 - m_nBoxes.x, diagonalLength);
                break;
            case 4:
                m_squareGridNodes.at(i).AddNeighbor(i + 1, boxSize.x);
                break;
            case 5:
                m_squareGridNodes.at(i).AddNeighbor(i + 1 + m_nBoxes.x, diagonalLength);
                break;
            case 6:
                m_squareGridNodes.at(i).AddNeighbor(i + m_nBoxes.x, boxSize.y);
                break;
            case 7:
                m_squareGridNodes.at(i).AddNeighbor(i - 1 + m_nBoxes.x, diagonalLength);
                break;
            default:
                break;
            }
        }
    }
}

void TraverseGrid::CalculateVoronoiGridNeighbors()
{
    auto &polygons = m_voronoiGrid.GetPolygons();
    for (size_t i = 0; i < polygons.size(); i++)
    {
        auto &neighbors = polygons[i].getNeighbors();
        for (auto &neighbor : polygons[i].getNeighbors())
        {
            long nodeNeighborUID = GetNodeUID(Lib::Mid(*neighbor), Type::Voronoi);
            m_voronoiGridNodes.at(i).AddNeighbor(nodeNeighborUID, vl::Length(Lib::Mid(polygons[i]) - m_voronoiGridNodes.at(nodeNeighborUID).GetPosition()));
        }
    }
}

void TraverseGrid::ClearNodeColor(long uid)
{
    if (uid == -1)
        return;

    switch (m_currentType)
    {
    case Type::Square:
    {
        m_squareGrid.at(uid).setFillColor(sf::Color::Transparent);
        break;
    }
    case Type::Voronoi:
    {
        m_voronoiGrid.GetPolygon(m_voronoiGridNodes.at(uid).GetPosition()).setFillColor(sf::Color::Transparent);
        break;
    }
    }
}

void TraverseGrid::SetNodeColor(long uid, const sf::Color &color)
{
    if (uid == -1)
        return;

    switch (m_currentType)
    {
    case Type::Square:
    {
        m_squareGrid.at(uid).setFillColor(color);
        break;
    }
    case Type::Voronoi:
    {
        m_voronoiGrid.GetPolygon(m_voronoiGridNodes.at(uid).GetPosition()).setFillColor(color);
        break;
    }
    }
}

std::map<long, Node> &TraverseGrid::GetActiveNodes() noexcept
{
    switch (m_currentType)
    {
    case Type::Square:
        return m_squareGridNodes;
    case Type::Voronoi:
        return m_voronoiGridNodes;
    }
    return m_squareGridNodes;
}

const std::map<long, Node> &TraverseGrid::GetActiveNodesConst() const noexcept
{
    switch (m_currentType)
    {
    case Type::Square:
        return m_squareGridNodes;
    case Type::Voronoi:
        return m_voronoiGridNodes;
    }
    return m_squareGridNodes;
}
