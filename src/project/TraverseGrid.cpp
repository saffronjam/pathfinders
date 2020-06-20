#include "TraverseGrid.h"

TraverseGrid::TraverseGrid(TraverseGrid::Type type, const sf::FloatRect &visRect)
    : m_visRect(visRect),
      m_nBoxes(45, 35)

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
    CalculateNeighbors();
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
        for (int i = 0; i < m_nBoxes.x; i++)
        {
            for (int j = 0; j < m_nBoxes.y; j++)
            {
                m_nodes.emplace(std::make_pair(uid++, Node(uid, sf::Vector2f(topLeft.x + i * boxSize.x, topLeft.y + j * boxSize.y))));
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

void TraverseGrid::CalculateNeighbors()
{
    switch (m_currentType)
    {
    case Type::Square:
    {
        for (int i = 0; i < m_nBoxes.x * m_nBoxes.y; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (((i % m_nBoxes.x == 0) && (j == 0)) ||
                    ((i >= 0 && i < m_nBoxes.x) && (j == 1)) ||
                    (((i + 1) % m_nBoxes.x == 0) && (j == 2)) ||
                    ((i >= (m_nBoxes.x * (m_nBoxes.y - 1)) && (i <= (m_nBoxes.x * m_nBoxes.y)) && (j == 3))))
                {
                    continue;
                }
                switch (j)
                {
                case 0:
                    m_nodes.at(i).AddNeighbor(&m_nodes.at(i - 1));
                    break;
                case 1:
                    m_nodes.at(i).AddNeighbor(&m_nodes.at(i - m_nBoxes.x));
                    break;
                case 2:
                    m_nodes.at(i).AddNeighbor(&m_nodes.at(i + 1));
                    break;
                case 3:
                    m_nodes.at(i).AddNeighbor(&m_nodes.at(i + m_nBoxes.x));
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