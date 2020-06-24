#define JC_VORONOI_IMPLEMENTATION

#include "Voronoi.h"

Voronoi::Voronoi()
    : Voronoi(sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f), std::vector<sf::Vector2f>())
{
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, const std::vector<sf::Vector2f> &points)
    : m_boundingBox(boundingBox),
      m_points(m_points),
      m_fillColors({sf::Color::Transparent})
{
    GenerateVoronoi();
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, int nRandomPoints)
    : m_boundingBox(boundingBox),
      m_fillColors({sf::Color::Transparent})
{
    for (int i = 0; i < nRandomPoints; i++)
        m_points.push_back(Random::Vec2(boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height));
    GenerateVoronoi();
}

Voronoi::~Voronoi()
{
    if (m_diagram.has_value())
        jcv_diagram_free(&m_diagram.value());
}

void Voronoi::SetPoints(const std::vector<sf::Vector2f> &points)
{
    m_points = points;
    GenerateVoronoi();
}

void Voronoi::SetBoundingBox(const sf::FloatRect &boundingBox)
{
    m_boundingBox = boundingBox;
    GenerateVoronoi();
}

void Voronoi::SetOutlineColor(const sf::Color &color) noexcept
{
    for (auto &polygon : m_polygons)
    {
        polygon.setOutlineColor(color);
    }
}

void Voronoi::SetOutlineThickness(float thickness) noexcept
{
    for (auto &polygon : m_polygons)
    {
        polygon.setOutlineThickness(thickness);
    }
}

void Voronoi::Relax(int iterations)
{
    if (m_diagram.has_value())
    {
        for (int i = 0; i < iterations; i++)
        {
            const jcv_site *sites = jcv_diagram_get_sites(&m_diagram.value());
            for (int i = 0; i < m_diagram.value().numsites; ++i)
            {
                const jcv_site *site = &sites[i];
                jcv_point sum = site->p;
                int count = 1;
                for (const jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next)
                {
                    sum.x += edge->pos[0].x;
                    sum.y += edge->pos[0].y;
                    ++count;
                }
                m_points[site->index].x = sum.x / count;
                m_points[site->index].y = sum.y / count;
            }
        }
        GenerateVoronoi();
    }
    else
    {
        throw Voronoi::Exception(__LINE__, __FILE__, "Voronoi was not created. m_diagram was not instantiated");
    }
}

Voronoi::Polygon &Voronoi::GetPolygon(const sf::Vector2f &position)
{
    float minDistance = std::numeric_limits<float>::infinity();
    Voronoi::Polygon *closest = nullptr;
    for (auto &polygon : m_polygons)
    {
        float distance = vl::LengthSq(polygon.getVoronoiPoint() - position);
        if (distance < minDistance)
        {
            minDistance = distance;
            closest = &polygon;
        }
    }

    if (closest != nullptr)
    {
        return *closest;
    }
    else
    {
        throw Voronoi::Exception(__LINE__, __FILE__, "Failed to find any polygons closer than std::numeric_limits<float>::infinity() units from given position");
    }
}

void Voronoi::GenerateVoronoi()
{
    if (m_boundingBox.width > 0.0f &&
        m_boundingBox.height > 0.0f &&
        m_points.size() > 0)
    {
        jcv_rect rect = ConvertBoundingBox(m_boundingBox);

        if (m_diagram.has_value())
        {
            jcv_diagram_free(&m_diagram.value());
            std::memset(&m_diagram.value(), 0, sizeof(jcv_diagram));
        }
        m_diagram = jcv_diagram();
        jcv_diagram_generate(m_points.size(), reinterpret_cast<const jcv_point *>(m_points.data()), &rect, nullptr, &m_diagram.value());

        // Generate polygon list from sites
        const jcv_site *sites = jcv_diagram_get_sites(&m_diagram.value());
        m_polygons.clear();
        m_polygons.reserve(m_diagram.value().numsites);
        for (int i = 0; i < m_diagram.value().numsites; ++i)
        {
            const jcv_site *site = &sites[i];

            std::vector<sf::Vector2f> polygonPoints;
            for (jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next)
            {
                polygonPoints.emplace_back(edge->pos[0].x, edge->pos[0].y);
            }
            sf::ConvexShape convexShape = Lib::CreateConvexShape(polygonPoints);
            Voronoi::Polygon polygon(convexShape, vl::ConvertTo<sf::Vector2f>(site->p));
            polygon.setFillColor(m_fillColors[i % m_fillColors.size()]);
            polygon.setOutlineColor(sf::Color::White);
            polygon.setOutlineThickness(1);

            m_polygons.emplace_back(polygon);
        }

        for (int i = 0; i < m_diagram.value().numsites; ++i)
        {
            const jcv_site *site = &sites[i];

            for (jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next)
            {
                if (edge->neighbor)
                {
                    Voronoi::Polygon &neighbor = GetPolygon(vl::ConvertTo<sf::Vector2f>(edge->neighbor->p));
                    m_polygons[i].addNeighbor(&neighbor);
                }
            }
        }
    }
}

jcv_rect Voronoi::ConvertBoundingBox(const sf::FloatRect &boundingBox)
{
    return {boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height};
}

void Voronoi::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_diagram.has_value())
    {
        for (auto &polygon : m_polygons)
        {
            target.draw(polygon, states);
        }
    }
    else
    {
        throw Voronoi::Exception(__LINE__, __FILE__, "Voronoi was not created. m_diagram was not instantiated");
    }
}

Voronoi::Exception::Exception(int line, const char *file, const char *errorString) noexcept
    : IException(line, file),
      errorString(errorString)
{
}

const char *Voronoi::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *Voronoi::Exception::GetType() const noexcept
{
    return "V-2DFramework Voronoi Exception";
}

const char *Voronoi::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}