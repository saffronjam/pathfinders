#include "TraverseGrid.h"

TraverseGrid::TraverseGrid(TraverseGrid::Type type, const sf::FloatRect &visRect)
    : m_visRect(visRect),
      m_nBoxes(45, 35),
      m_currentType(type)

{
    ChangeGridType(type);
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
    m_currentType = type;
    GenerateGrid();
}

void TraverseGrid::DrawSquareGrid()
{
    sf::RectangleShape rect;
    sf::Vector2f boxSize(m_visRect.width / m_nBoxes.x, m_visRect.height / m_nBoxes.y);
    rect.setSize(sf::Vector2f(boxSize.x, boxSize.y));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Blue);
    rect.setOutlineThickness(1.0f);
    for (auto &[uid, node] : m_nodes)
    {
        rect.setPosition(node.GetPosition() - rect.getSize() / 2.0f);
        if (IsObstacle(uid))
            rect.setFillColor(sf::Color(125, 125, 125));
        else
            rect.setFillColor(sf::Color::Transparent);
        Camera::Draw(rect);
    }
}

void TraverseGrid::DrawVoronoiGrid()
{
}

void TraverseGrid::GenerateGrid()
{
    m_nodes.clear();
    switch (m_currentType)
    {
    case Type::Square:
    {
        int uid = 0;
        sf::Vector2f boxSize(m_visRect.width / m_nBoxes.x, m_visRect.height / m_nBoxes.y);
        sf::Vector2f topLeft = sf::Vector2f(m_visRect.left, m_visRect.top) + sf::Vector2f(boxSize.x, boxSize.y) / 2.0f;
        for (int i = 0; i < m_nBoxes.y; i++)
        {
            for (int j = 0; j < m_nBoxes.x; j++)
            {
                m_nodes.emplace(std::make_pair(uid++, Node(uid, sf::Vector2f(topLeft.x + j * boxSize.x, topLeft.y + i * boxSize.y))));
            }
        }
        SetStart(0);
        SetGoal(m_nodes.size() - 1);
        break;
    }
    case Type::Voronoi:
        break;
    default:
        break;
    }
}

void TraverseGrid::CalculateNeighbors(std::map<long, Node> &nodes) const
{
    switch (m_currentType)
    {
    case Type::Square:
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
                    nodes.at(i).AddNeighbor(&nodes.at(i - 1), boxSize.x);
                    break;
                case 1:
                    nodes.at(i).AddNeighbor(&nodes.at(i - 1 - m_nBoxes.x), diagonalLength);
                    break;
                case 2:
                    nodes.at(i).AddNeighbor(&nodes.at(i - m_nBoxes.x), boxSize.y);
                    break;
                case 3:
                    nodes.at(i).AddNeighbor(&nodes.at(i + 1 - m_nBoxes.x), diagonalLength);
                    break;
                case 4:
                    nodes.at(i).AddNeighbor(&nodes.at(i + 1), boxSize.x);
                    break;
                case 5:
                    nodes.at(i).AddNeighbor(&nodes.at(i + 1 + m_nBoxes.x), diagonalLength);
                    break;
                case 6:
                    nodes.at(i).AddNeighbor(&nodes.at(i + m_nBoxes.x), boxSize.y);
                    break;
                case 7:
                    nodes.at(i).AddNeighbor(&nodes.at(i - 1 + m_nBoxes.x), diagonalLength);
                    break;
                default:
                    break;
                }
            }
        }
        break;
    }
    case Type::Voronoi:
        break;
    default:
        break;
    }
}

long TraverseGrid::GetNodeUIDByPosition(const sf::Vector2f &position) const
{
    float closestDistance = std::numeric_limits<float>::infinity();
    long closestUID;
    for (auto &[uid, node] : m_nodes)
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

void TraverseGrid::SetStart(const sf::Vector2f &start)
{
    SetStart(GetNodeUIDByPosition(start));
}

void TraverseGrid::SetStart(long uid)
{
    m_startUID = uid;
}

void TraverseGrid::SetGoal(const sf::Vector2f &goal)
{
    SetGoal(GetNodeUIDByPosition(goal));
}

void TraverseGrid::SetGoal(long uid)
{
    m_goalUID = uid;
}

void TraverseGrid::SetIsObstacle(long uid, bool isObstacle)
{
    if (isObstacle)
        m_obstacles.emplace(uid);
    else
        m_obstacles.erase(uid);
}