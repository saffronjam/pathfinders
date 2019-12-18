#include "Grid.hpp"

#include "AStar.hpp"
#include "Funclib.hpp"
#include "Camera.hpp"
#include "Random.hpp"

Grid::Grid(InputUtility &iu, AStar &solver, sf::Vector2i const &boxSize)
    : m_iu(iu),
      m_solver(solver),
      m_boxSize(boxSize),
      m_size((int)((Graphics::ScreenWidth - 200) / boxSize.x), (int)((Graphics::ScreenHeight) / boxSize.y)),
      m_isPlacingObstacles(false)
{
    //Box grid
    for (int i = 0; i < m_size.x * m_size.y; i++)
    {
        sf::Vector2i traverseBoxPos = sf::Vector2i((i % m_size.x) * m_boxSize.x - Camera::OffsetX, int(i / m_size.x) * m_boxSize.y - Camera::OffsetY);
        m_boxGridLayout.push_back(std::make_shared<TraverseBox>(sf::IntRect(traverseBoxPos.x, traverseBoxPos.y, m_boxSize.x, m_boxSize.y)));
    }
    ComputeBoxGridNeighbors();

    //Polygon grid
    Random randX;
    randX.SetLowerBound(-Camera::OffsetX - 100);
    randX.SetUpperBound(Camera::OffsetX - 100);

    Random randY;
    randY.SetLowerBound(-Camera::OffsetY - 200);
    randY.SetUpperBound(Camera::OffsetY + 200);

    unsigned int nPoints = 1200;
    for (unsigned int i = 0; i < nPoints; i++)
    {
        double x = (double)randX();
        double y = (double)randY();
        m_voronoi_points.push_back(new VoronoiPoint(x, y));
    }
    m_voronoi_creator = new Voronoi();
    m_edges = m_voronoi_creator->ComputeVoronoiGraph(m_voronoi_points, -Camera::OffsetY - 600, Camera::OffsetY + 600);
    delete m_voronoi_creator;
    std::vector<sf::ConvexShape> allPolygons = Funclib::CreateShapeListFromVonoroi(m_voronoi_points, m_edges);
    Funclib::ApplyLloydsRelaxtion(m_edges, m_voronoi_points, allPolygons, -Camera::OffsetY - 600, Camera::OffsetY + 600, 2);
    for (auto &polygon : allPolygons)
    {
        m_polygonGridLayout.push_back(std::make_shared<TraversePolygon>(polygon));
    }
    ComputePolygonGridNeighbors();
}

void Grid::Update()
{
    switch (m_gridMode)
    {
    case GridMode::ModeBox:
        UpdateBoxMode();
        break;
    case GridMode::ModePolygon:
        UpdatePolygonMode();
        break;
    }
}

void Grid::DrawGrid(Graphics &gfx)
{
    switch (m_gridMode)
    {
    case GridMode::ModeBox:
        DrawBoxGrid(gfx);
        break;
    case GridMode::ModePolygon:
        DrawPolygonGrid(gfx);
        break;
    }
}
void Grid::DrawInBoxGrid(Graphics &gfx, sf::Vector2i coord)
{
    sf::IntRect toDraw(coord.x * m_boxSize.x, coord.y * m_boxSize.x, m_boxSize.x, m_boxSize.y);
    gfx.Draw(toDraw, sf::Color::Blue, false);
}

void Grid::ChangeToBoxMode()
{
    if (m_gridMode != GridMode::ModeBox)
    {
        m_solver.EnterNewMode(GridMode::ModeBox);
    }
}

void Grid::ChangeToPolygonMode()
{
    if (m_gridMode != GridMode::ModePolygon)
    {
        m_solver.EnterNewMode(GridMode::ModePolygon);
    }
}

void Grid::UpdateBoxMode()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        for (auto &traverseBox : m_boxGridLayout)
        {
            sf::FloatRect hitbox(traverseBox->GetRectangleShape().getPosition(), traverseBox->GetRectangleShape().getSize());
            if (hitbox.contains((sf::Vector2f)m_iu.GetMousePosition()) && !traverseBox->GetIsBlocked())
            {
                std::shared_ptr<Node> node = traverseBox->GetNode();
                if (m_isPlacingObstacles)
                {
                    node->SetIsObstructed(true);
                }
                else if (m_isRemovingObstacles)
                {
                    node->SetIsObstructed(false);
                }
                else if (m_isPlacingStart)
                {
                    m_solver.SetStart(node);
                }
                else if (m_isPlacingFinal)
                {
                    m_solver.SetFinal(node);
                }
            }
        }
    }
    for (auto &traverseBox : m_boxGridLayout)
    {
        std::shared_ptr<Node> node = traverseBox->GetNode();
        if (node->GetIsStart())
        {
            traverseBox->SetColor(sf::Color(150, 150, 0));
            traverseBox->SetIsBlocked(true);
        }
        else if (node->GetIsFinal())
        {
            traverseBox->SetColor(sf::Color(0, 150, 0));
            traverseBox->SetIsBlocked(true);
        }
        else if (node->GetVia())
        {
            traverseBox->SetColor(sf::Color(50, 0, 0));
            traverseBox->SetIsBlocked(true);
        }
        else if (node->GetIsObstructed())
        {
            traverseBox->SetColor(sf::Color(125, 125, 125));
        }
        else
        {
            traverseBox->SetColor(sf::Color::Transparent);
            traverseBox->SetIsBlocked(false);
        }
    }
}

void Grid::UpdatePolygonMode()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        for (auto &traversePolygon : m_polygonGridLayout)
        {
            sf::ConvexShape hitbox(traversePolygon->GetPolygon());
            if ((sfmlext::PolygonContains(hitbox, (sf::Vector2f)m_iu.GetMousePosition()) && (!traversePolygon->GetIsBlocked())))
            {
                std::shared_ptr<Node> node = traversePolygon->GetNode();
                if (m_isPlacingObstacles)
                {
                    node->SetIsObstructed(true);
                }
                else if (m_isRemovingObstacles)
                {
                    node->SetIsObstructed(false);
                }
                else if (m_isPlacingStart)
                {
                    m_solver.SetStart(node);
                }
                else if (m_isPlacingFinal)
                {
                    m_solver.SetFinal(node);
                }
            }
        }
    }
    for (auto &traversePolygon : m_polygonGridLayout)
    {
        std::shared_ptr<Node> node = traversePolygon->GetNode();
        if (node->GetIsStart())
        {
            traversePolygon->SetColor(sf::Color(150, 150, 0));
            traversePolygon->SetIsBlocked(true);
        }
        else if (node->GetIsFinal())
        {
            traversePolygon->SetColor(sf::Color(0, 150, 0));
            traversePolygon->SetIsBlocked(true);
        }
        else if (node->GetVia())
        {
            traversePolygon->SetColor(sf::Color(50, 0, 0));
            traversePolygon->SetIsBlocked(true);
        }
        else if (node->GetIsObstructed())
        {
            traversePolygon->SetColor(sf::Color(125, 125, 125));
        }
        else
        {
            traversePolygon->SetColor(sf::Color::Transparent);
            traversePolygon->SetIsBlocked(false);
        }
    }
}

void Grid::DrawBoxGrid(Graphics &gfx)
{
    for (auto &traverseBox : m_boxGridLayout)
    {
        gfx.Draw(traverseBox->GetRectangleShape());
    }
}

void Grid::DrawPolygonGrid(Graphics &gfx)
{
    for (auto &traversePolygon : m_polygonGridLayout)
    {
        gfx.Draw(traversePolygon->GetPolygon());
    }
}

void Grid::ComputeBoxGridNeighbors()
{
    for (int i = 0; i < m_size.x * m_size.y; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (((i % m_size.x == 0) && (j == 0)) ||
                ((i >= 0 && i < m_size.x) && (j == 1)) ||
                (((i + 1) % m_size.x == 0) && (j == 2)) ||
                ((i >= (m_size.x * (m_size.y - 1)) && (i <= (m_size.x * m_size.y)) && (j == 3))))
            {
                continue;
            }
            switch (j)
            {
            case 0:
                m_boxGridLayout[i]->GetNode()->AddNeighbor(m_boxGridLayout[i - 1]->GetNode());
                break;
            case 1:
                m_boxGridLayout[i]->GetNode()->AddNeighbor(m_boxGridLayout[i - m_size.x]->GetNode());
                break;
            case 2:
                m_boxGridLayout[i]->GetNode()->AddNeighbor(m_boxGridLayout[i + 1]->GetNode());
                break;
            case 3:
                m_boxGridLayout[i]->GetNode()->AddNeighbor(m_boxGridLayout[i + m_size.x]->GetNode());
                break;
            default:
                break;
            }
        }
    }
}

void Grid::ComputePolygonGridNeighbors()
{
    std::vector<std::shared_ptr<TraversePolygon>> &m_polys = m_polygonGridLayout;
    for (size_t i = 0; i < m_polys.size(); i++)
    {
        sf::Vector2f i_pos = (sf::Vector2f)m_polys[i]->GetPosition();
        for (size_t j = 0; j < m_polys.size(); j++)
        {
            sf::Vector2f j_pos = (sf::Vector2f)m_polys[j]->GetPosition();
            if (j == i)
            {
                continue;
            }
            if (vf::LengthSq(i_pos - j_pos) > 10000.0f)
            {
                continue;
            }
            sf::Vector2f closest_point = sfmlext::ClosestPolygonVertex(m_polys[j]->GetPolygon(), i_pos);
            for (size_t k = 0; k < m_polys[i]->GetPolygon().getPointCount(); k++)
            {
                sf::Vector2f first = m_polys[i]->GetPolygon().getPoint(k);
                sf::Vector2f second = closest_point;
                if (vf::LengthSq(first - second) < 5.0f)
                {
                    m_polys[i]->GetNode()->AddNeighbor(m_polys[j]->GetNode());
                    m_polys[j]->GetNode()->AddNeighbor(m_polys[i]->GetNode());
                }
            }
        }
    }
}

Grid::TraverseBase::TraverseBase(sf::Vector2i const &nodePosition)
    : m_node(std::make_shared<Node>(nodePosition)),
      m_isBlocked(false)
{
}

Grid::TraverseBox::TraverseBox(sf::IntRect const &rectangle)
    : TraverseBase((sf::Vector2i)sfmlext::GetCentroidOfRectangle((sf::FloatRect)rectangle)),
      m_rectangleShape(sf::Vector2f(rectangle.width, rectangle.height))
{
    m_rectangleShape.setPosition(rectangle.left, rectangle.top);
    m_rectangleShape.setFillColor(sf::Color::Transparent);
    m_rectangleShape.setOutlineColor(sf::Color::Blue);
    m_rectangleShape.setOutlineThickness(1);
}

Grid::TraversePolygon::TraversePolygon(sf::ConvexShape const &polygon)
    : TraverseBase((sf::Vector2i)sfmlext::GetCentroidOfPolygon(polygon)),
      m_polygon(polygon)
{
    m_polygon.setFillColor(sf::Color::Transparent);
    m_polygon.setOutlineColor(sf::Color::Blue);
    m_polygon.setOutlineThickness(1);
}